#include "ui_updater.h"
#include "core/config_data.h"
#include "core/debug.h"
#include "presentation/ui/vars.h"
#include "presentation/ui/eez-flow.h"
#include "presentation/ui/screens.h"
#include "../sensors/sensor_stats.h"
#include "../storage/config_manager.h"
#include <lvgl.h>

// Usar el namespace flow para acceder a las funciones globales
using namespace eez::flow;

// Variable local para controlar el intervalo de actualización
static unsigned long last_update_ms = 0;
static const unsigned long UPDATE_INTERVAL_MS = 5000; // Actualizar cada 5 segundos

// Índice del cultivo seleccionado actualmente
static uint8_t selected_crop_index = 0;

// Buffer para formatear strings
static char format_buffer[32];

// Función helper para formatear float con 1 decimal
static const char* format_float_1_decimal(float value) {
    snprintf(format_buffer, sizeof(format_buffer), "%.1f", value);
    return format_buffer;
}

// Buffer para formatear con unidades
static char format_buffer_with_unit[48];

/**
 * @brief Formatear float con 1 decimal y unidad de medida
 * @param value Valor numérico
 * @param unit Unidad de medida (ej: "°C", "%", "mS/cm", "ppm")
 * @return String formateado "valor unidad"
 */
static const char* format_with_unit(float value, const char* unit) {
    snprintf(format_buffer_with_unit, sizeof(format_buffer_with_unit), 
             "%.1f %s", value, unit);
    return format_buffer_with_unit;
}

// Forward declarations de callbacks
static void on_crop_changed(lv_event_t * e);
static void on_time_changed(lv_event_t * e);
static void ui_updater_request_config_button_handler(lv_event_t* e);

// Callback para cuando se cambia la selección de cultivo
static void on_crop_changed(lv_event_t * e) {
    lv_obj_t * roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(roller);
    
    // Actualizar el cultivo seleccionado
    ui_updater_set_crop((uint8_t)selected);
}

void ui_updater_init() {
    // ===================================================================
    // Inicializar variables globales (fecha, batería, lugar, info dispositivo)
    // ===================================================================
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_BATERIA, eez::Value(100));
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_FECHA, eez::Value("--:--:--"));
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_LUGAR, eez::Value("Sin cultivo"));
    
    // Inicializar info del dispositivo (usa variables globales)
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_MAC, eez::Value("00:00:00:00:00:00"));
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_FIRMWARE_VERSION, eez::Value("1.0.0"));
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_HARDWARE_VERSION, eez::Value("1.0"));
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_N_SERIE, eez::Value("0000"));
    
    // ===================================================================
    // Inicializar labels de sensores directamente (sin variables EEZ)
    // ===================================================================
    if (objects.temp_agua_value != NULL) {
        lv_label_set_text(objects.temp_agua_value, "-- °C");
    }
    if (objects.temp_amb_value != NULL) {
        lv_label_set_text(objects.temp_amb_value, "-- °C");
    }
    if (objects.humedad_value != NULL) {
        lv_label_set_text(objects.humedad_value, "-- %");
    }
    if (objects.ph_value != NULL) {
        lv_label_set_text(objects.ph_value, "--");
    }
    if (objects.ec_value != NULL) {
        lv_label_set_text(objects.ec_value, "-- mS/cm");
    }
    if (objects.nitrogeno_value != NULL) {
        lv_label_set_text(objects.nitrogeno_value, "-- ppm");
    }
    if (objects.fosforo_value != NULL) {
        lv_label_set_text(objects.fosforo_value, "-- ppm");
    }
    if (objects.potasio_value != NULL) {
        lv_label_set_text(objects.potasio_value, "-- ppm");
    }
    
    // Configurar el componente select-crops con los cultivos disponibles
    if (sistema.cultivos.numCultivos > 0 && objects.select_crops != NULL) {
        // Construir string con los nombres de cultivos separados por \n
        String crop_options = "";
        for (uint8_t i = 0; i < sistema.cultivos.numCultivos; i++) {
            if (sistema.cultivos.cultivos[i].nombre[0] != '\0') {
                crop_options += sistema.cultivos.cultivos[i].nombre;
                if (i < sistema.cultivos.numCultivos - 1) {
                    crop_options += "\n";
                }
            }
        }
        
        // Configurar opciones del roller
        lv_roller_set_options(objects.select_crops, crop_options.c_str(), LV_ROLLER_MODE_NORMAL);
        
        // Agregar evento de cambio
        lv_obj_add_event_cb(objects.select_crops, on_crop_changed, LV_EVENT_VALUE_CHANGED, NULL);
        
        ui_updater_set_crop(0);
    }
    
    // NUEVO: Configurar roller select-time
    if (objects.select_time != NULL) {
        // Opciones: 1 min, 2 min, ..., 10 min
        lv_roller_set_options(objects.select_time, 
            "1 min\n2 min\n3 min\n4 min\n5 min\n6 min\n7 min\n8 min\n9 min\n10 min",
            LV_ROLLER_MODE_NORMAL);
        
        // Valor por defecto: 1 minuto (índice 0)
        lv_roller_set_selected(objects.select_time, 0, LV_ANIM_OFF);
        
        // Agregar evento de cambio
        lv_obj_add_event_cb(objects.select_time, on_time_changed, LV_EVENT_VALUE_CHANGED, NULL);
        
        // Inicializar valor
        ui_updater_set_telemetry_interval(1);
    }
    
    // NUEVO: Configurar botón actualizar_data
    if (objects.actualizar_data != NULL) {
        // Agregar nuestro handler
        lv_obj_add_event_cb(objects.actualizar_data, 
                           ui_updater_request_config_button_handler, 
                           LV_EVENT_CLICKED, 
                           NULL);
    }
}

void ui_updater_update() {
    unsigned long now = millis();
    
    // Control de intervalo - actualizar cada 5 segundos
    if (now - last_update_ms < UPDATE_INTERVAL_MS) {
        return;
    }
    last_update_ms = now;
    
    // ===================================================================
    // NUEVO: Verificar si se recibió CONFIG y actualizar UI
    // ===================================================================
    static bool esperando_anterior = false;
    
    if (esperando_anterior && !sistema.controlUsuario.esperandoConfig) {
        // Se recibió CONFIG, actualizar roller
        ui_updater_refresh_crops_roller();
        
        // Restaurar color del botón actualizar_data (verde)
        if (objects.actualizar_data != NULL) {
            lv_obj_set_style_bg_color(objects.actualizar_data, 
                                      lv_color_hex(0x00FF00), // Verde
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    esperando_anterior = sistema.controlUsuario.esperandoConfig;
    
    // ===================================================================
    // CALCULAR MEDIANAS Y GUARDAR EN sistema.sensores.*
    // ===================================================================
    sensor_stats_calcular_medianas();

    
    // ===================================================================
    // ACTUALIZAR VARIABLES LVGL CON MEDIANAS + UNIDADES
    // ===================================================================
    
    // 1. Batería (valor fijo) - Usa setGlobalVariable (funciona bien)
    setGlobalVariable(FLOW_GLOBAL_VARIABLE_BATERIA, eez::Value(100));
    
    // 2. Fecha/Hora - Usa setGlobalVariable (funciona bien)
    if (sistema.tiempo.formato != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_FECHA, eez::Value(sistema.tiempo.formato));
    }
    
    // ===================================================================
    // ACTUALIZAR LABELS DE SENSORES DIRECTAMENTE (sin variables EEZ)
    // ===================================================================
    
    // 3. Temperatura del Agua (DS18B20) con unidad °C
    if (sistema.sensores.ds18b20.valido && objects.temp_agua_value != NULL) {
        lv_label_set_text(objects.temp_agua_value, 
                         format_with_unit(sistema.sensores.ds18b20.temperatura, "C"));
    }
    
    // 4. Temperatura Ambiente (DHT22) con unidad °C
    if (sistema.sensores.dht22.valido && objects.temp_amb_value != NULL) {
        lv_label_set_text(objects.temp_amb_value, 
                         format_with_unit(sistema.sensores.dht22.temperatura, "C"));
    }
    
    // 5. Humedad Ambiente (DHT22) con unidad %
    if (sistema.sensores.dht22.valido && objects.humedad_value != NULL) {
        lv_label_set_text(objects.humedad_value, 
                         format_with_unit(sistema.sensores.dht22.humedad, "%"));
    }
    
    // 6. pH (SIN unidad, solo valor)
    if (sistema.sensores.rs485.valido && objects.ph_value != NULL) {
        lv_label_set_text(objects.ph_value, 
                         format_float_1_decimal(sistema.sensores.rs485.ph));
    }
    
    // 7. Conductividad Eléctrica con unidad mS/cm
    if (sistema.sensores.rs485.valido && objects.ec_value != NULL) {
        lv_label_set_text(objects.ec_value, 
                         format_with_unit(sistema.sensores.rs485.ec, "mS"));
    }
    
    // 8. Nitrógeno con unidad ppm
    if (sistema.sensores.rs485.valido && objects.nitrogeno_value != NULL) {
        lv_label_set_text(objects.nitrogeno_value, 
                         format_with_unit(sistema.sensores.rs485.nitrogen, "ppm"));
    }
    
    // 9. Fósforo con unidad ppm
    if (sistema.sensores.rs485.valido && objects.fosforo_value != NULL) {
        lv_label_set_text(objects.fosforo_value, 
                         format_with_unit(sistema.sensores.rs485.phosphorus, "ppm"));
    }
    
    // 10. Potasio con unidad ppm
    if (sistema.sensores.rs485.valido && objects.potasio_value != NULL) {
        lv_label_set_text(objects.potasio_value, 
                         format_with_unit(sistema.sensores.rs485.potassium, "ppm"));
    }
    
    // 11. Información del dispositivo (sin cambios)
    if (sistema.dispositivo.mac != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_MAC, eez::Value(sistema.dispositivo.mac));
    }
    if (sistema.dispositivo.vFirmware != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_FIRMWARE_VERSION, eez::Value(sistema.dispositivo.vFirmware));
    }
    if (sistema.dispositivo.vHardware != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_HARDWARE_VERSION, eez::Value(sistema.dispositivo.vHardware));
    }
    if (sistema.dispositivo.serie != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_N_SERIE, eez::Value(sistema.dispositivo.serie));
    }
    if (sistema.dispositivo.modelo != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_MODEL, eez::Value(sistema.dispositivo.modelo));
    }
    if (sistema.wifi.ssid != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_SSID, eez::Value(sistema.wifi.ssid));
    }
    if (sistema.wifi.password != nullptr) {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_PASS, eez::Value(sistema.wifi.password));
    }
}

void ui_updater_set_crop(uint8_t cropIndex) {
    if (cropIndex >= sistema.cultivos.numCultivos) {
        return; // Índice fuera de rango
    }
    
    selected_crop_index = cropIndex;
    
    // NUEVO: Guardar en estructura global para telemetría
    sistema.controlUsuario.cultivoSeleccionado = cropIndex;
    
    if (sistema.cultivos.cultivos[cropIndex].nombre[0] != '\0') {
        setGlobalVariable(FLOW_GLOBAL_VARIABLE_LUGAR, 
                         eez::Value(sistema.cultivos.cultivos[cropIndex].nombre));
    }
    
    // ⭐ NUEVO: Persistir cambio en SD
    if (config_save_to_sd()) {
        DEBUG_INFOF("UI: Cultivo [%d] %s (guardado en SD)", 
                    cropIndex, sistema.cultivos.cultivos[cropIndex].nombre);
    } else {
        DEBUG_INFOF("UI: Cultivo [%d] %s (no guardado)", 
                    cropIndex, sistema.cultivos.cultivos[cropIndex].nombre);
    }
}

// ============================================
// ACTUALIZAR ROLLER DE CULTIVOS
// ============================================

void ui_updater_refresh_crops_roller() {
    if (objects.select_crops == NULL) return;
    
    if (sistema.cultivos.numCultivos > 0) {
        // Construir string con nombres separados por \n
        String crop_options = "";
        for (uint8_t i = 0; i < sistema.cultivos.numCultivos; i++) {
            if (sistema.cultivos.cultivos[i].nombre[0] != '\0') {
                crop_options += sistema.cultivos.cultivos[i].nombre;
                if (i < sistema.cultivos.numCultivos - 1) {
                    crop_options += "\n";
                }
            }
        }
        
        // Actualizar opciones del roller
        lv_roller_set_options(objects.select_crops, crop_options.c_str(), LV_ROLLER_MODE_NORMAL);
        
        // Resetear selección al primero
        lv_roller_set_selected(objects.select_crops, 0, LV_ANIM_OFF);
        ui_updater_set_crop(0);
        
        DEBUG_INFOF("UI: Roller actualizado: %d cultivos", sistema.cultivos.numCultivos);
    }
}

// ============================================
// CONFIGURAR INTERVALO DE TELEMETRÍA
// ============================================

// Callback para roller select-time
static void on_time_changed(lv_event_t * e) {
    lv_obj_t * roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(roller);
    
    // Convertir índice (0-9) a minutos (1-10)
    uint8_t minutes = selected + 1;
    ui_updater_set_telemetry_interval(minutes);
}

void ui_updater_set_telemetry_interval(uint8_t minutes) {
    if (minutes < 1) minutes = 1;
    if (minutes > 10) minutes = 10;
    
    sistema.controlUsuario.tiempoTelemetriaMin = minutes;
    sistema.intervalos.telemetria = minutes * 60000UL;  // Convertir a ms
    
    // ⭐ NUEVO: Persistir cambio en SD
    if (config_save_to_sd()) {
        DEBUG_INFOF("UI: Intervalo telemetría: %d min (guardado en SD)", minutes);
    } else {
        DEBUG_INFOF("UI: Intervalo telemetría: %d min (no guardado)", minutes);
    }
}

// ============================================
// BOTÓN ACTUALIZAR_DATA
// ============================================

void ui_updater_request_config_button_handler(lv_event_t* e) {
    lv_event_code_t event = lv_event_get_code(e);
    
    if (event == LV_EVENT_CLICKED) {
        DEBUG_INFO("UI: Botón actualizar_data presionado");
        
        // Cambiar color del botón a "cargando" (naranja)
        if (objects.actualizar_data != NULL) {
            lv_obj_set_style_bg_color(objects.actualizar_data, 
                                      lv_color_hex(0xFFA500), // Naranja
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        
        // Enviar REQUEST_CONFIG
        extern bool connectivity_request_config();
        connectivity_request_config();
        
        // El color se restaurará cuando llegue CONFIG en mqtt_callback
    }
}
