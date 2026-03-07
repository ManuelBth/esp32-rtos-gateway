#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "UiHandler.h"
#include "ui/ui.h" // Mantener referencia a la UI generada
#include "ui_updater.h" // Módulo de actualización de variables LVGL
#include "../core/debug.h"
#include "../core/hardware_config.h"

// Buffer 1/10 de pantalla (Seguro y Rápido)
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10)

// Instancias privadas de hardware
static TFT_eSPI tft = TFT_eSPI(); 
static XPT2046_Touchscreen ts(TOUCH_PIN_CS, TOUCH_PIN_IRQ);

// Punteros para LVGL
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf; 

/* FLUSH CALLBACK: Envía los pixeles renderizados a la pantalla */
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/* TOUCH READ CALLBACK: Lee el estado del hardware touch para LVGL */
static void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        
        // --- LOGS DE DEPURACIÓN DEL TOUCH ---
        // Serial.printf("DEBUG TOUCH: RAW -> X:%d | Y:%d | Z:%d\n", p.x, p.y, p.z);

        // Mapeo de coordenadas CORREGIDO (Calibración Real)
        // Datos obtenidos de logs:
        // Izquierda (X=0): RAW X ~3850
        // Derecha (X=320): RAW X ~390
        // Arriba (Y=0): RAW Y ~3665
        // Abajo (Y=240): RAW Y ~390
        
        int16_t touchX = map(p.x, 3850, 390, 0, SCREEN_WIDTH); 
        int16_t touchY = map(p.y, 3665, 390, 0, SCREEN_HEIGHT); 

        // Limitar coordenadas para evitar errores de LVGL
        if(touchX < 0) touchX = 0;
        if(touchX >= SCREEN_WIDTH) touchX = SCREEN_WIDTH - 1;
        if(touchY < 0) touchY = 0;
        if(touchY >= SCREEN_HEIGHT) touchY = SCREEN_HEIGHT - 1;
        
        // Comentamos logs para máximo rendimiento en producción
        // Serial.printf("DEBUG TOUCH: MAP -> X:%d | Y:%d\n", touchX, touchY);
        
        data->point.x = touchX;
        data->point.y = touchY;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void presentation_init() {
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // 2. Hardware: Touch
    ts.begin();
    ts.setRotation(1);

    // 3. LVGL: Inicialización del core
    lv_init();

    // REGISTRAR CALLBACK DE LOGS (Ya registrado en main, pero por seguridad)
    // lv_log_register_print_cb(...); 

    buf = (lv_color_t*) malloc(DRAW_BUF_SIZE * sizeof(lv_color_t));
    if(buf == NULL) while(1);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, DRAW_BUF_SIZE);

    // 5. Driver Display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 6. Driver Touch
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // 7. Initialize EEZ Flow UI (creates screens internally)
    ui_init();
    
    // 8. Initialize UI updater
    ui_updater_init();
}

void presentation_loop() {
    // 1. Calcular delta de tiempo real para mantener la velocidad correcta
    static uint32_t last_ms = 0;
    uint32_t now = millis();
    if(last_ms == 0) last_ms = now;
    
    uint32_t delta_ms = now - last_ms;
    last_ms = now;

    // 2. Incrementar el tiempo interno de LVGL con el tiempo REAL transcurrido
    lv_tick_inc(delta_ms); 

    // 3. Procesar tareas de LVGL (Eventos, timers, renderizado)
    lv_timer_handler(); 

    // 4. Lógica específica de la UI (Refresh de variables, etc)
    ui_tick();
    
    // 5. Actualizar variables LVGL desde la estructura de datos global
    ui_updater_update();
    
    // Pequeño delay para liberar CPU - reducido a 1ms para mejor rendimiento
    delay(1);
}
