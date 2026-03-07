#include "ui_actions.h"
#include "ui/screens.h"
#include "../core/config_data.h"
#include "../core/debug.h"
#include "../sensors/sensor_stats.h"
#include "../telemetry/telemetry.h"
#include <lvgl.h>

// ============================================
// DEFINICIÓN DE COLORES
// ============================================
#define COLOR_ACTIVE_GREEN   lv_color_hex(0x00C851)  // Verde Material Design (activo)
#define COLOR_NORMAL_BG      lv_color_hex(0x004080)  // RGB(0, 64, 128) - Color original de botones

// ============================================
// VARIABLES ESTÁTICAS PARA GUARDAR ESTILOS ORIGINALES
// ============================================
static bool estilos_guardados = false;
static lv_color_t color_original_comenzar_medir;
static lv_color_t color_original_comenzar_rutina;

// ============================================
// FUNCIÓN HELPER: Verificar si hay datos válidos de sensores
// ============================================
static bool hay_datos_sensores_validos() {
    // Verificar si al menos uno de los sensores tiene datos válidos
    return sistema.sensores.dht22.valido || 
           sistema.sensores.ds18b20.valido || 
           sistema.sensores.rs485.valido;
}

// ============================================
// HANDLER: Botón "comenzar_medir"
// ============================================
void ui_actions_handle_comenzar_medir(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    
    // Toggle estado de lectura
    sistema.controlUsuario.lecturaActivaUI = !sistema.controlUsuario.lecturaActivaUI;
    
    if (sistema.controlUsuario.lecturaActivaUI) {
        // ===================================================================
        // ACTIVAR: Cambiar a verde y limpiar buffers
        // ===================================================================
        lv_obj_set_style_bg_color(btn, COLOR_ACTIVE_GREEN, LV_PART_MAIN);
        
        // Limpiar buffers circulares para empezar con datos frescos
        sensor_stats_limpiar_buffers();
        
    } else {
        lv_obj_set_style_bg_color(btn, COLOR_NORMAL_BG, LV_PART_MAIN);
    }
}

// ============================================
// HANDLER: Botón "comenzar_rutina"
// ============================================
void ui_actions_handle_comenzar_rutina(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    
    // Toggle estado de rutina MQTT
    bool nuevo_estado = !sistema.controlUsuario.rutinaMQTTActiva;
    
    if (nuevo_estado) {
        // ===================================================================
        // INTENTO DE ACTIVAR: Validar que haya datos de sensores
        // ===================================================================
        if (!hay_datos_sensores_validos()) {
            return;
        }
        
        // ACTIVAR: Cambiar a verde
        sistema.controlUsuario.rutinaMQTTActiva = true;
        lv_obj_set_style_bg_color(btn, COLOR_ACTIVE_GREEN, LV_PART_MAIN);
        
        // Habilitar telemetría MQTT
        telemetry_habilitar_envio(true);
        
    } else {
        sistema.controlUsuario.rutinaMQTTActiva = false;
        lv_obj_set_style_bg_color(btn, COLOR_NORMAL_BG, LV_PART_MAIN);
        telemetry_habilitar_envio(false);
    }
}

// ============================================
// INICIALIZACIÓN: Registrar callbacks de botones
// ============================================
void ui_actions_init() {
    if (objects.comenzar_medir == NULL || objects.comenzar_rutina == NULL) {
        return;
    }
    
    if (!estilos_guardados) {
        color_original_comenzar_medir = lv_obj_get_style_bg_color(objects.comenzar_medir, LV_PART_MAIN);
        color_original_comenzar_rutina = lv_obj_get_style_bg_color(objects.comenzar_rutina, LV_PART_MAIN);
        estilos_guardados = true;
    }
    
    lv_obj_set_style_bg_color(objects.comenzar_medir, COLOR_NORMAL_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_color(objects.comenzar_rutina, COLOR_NORMAL_BG, LV_PART_MAIN);
    
    lv_obj_add_event_cb(objects.comenzar_medir, ui_actions_handle_comenzar_medir, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.comenzar_rutina, ui_actions_handle_comenzar_rutina, LV_EVENT_CLICKED, NULL);
}
