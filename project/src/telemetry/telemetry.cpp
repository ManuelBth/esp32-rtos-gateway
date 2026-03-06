/**
 * @file telemetry.cpp
 * @brief Implementación de envío de telemetría MQTT
 * @details Construye payloads JSON y publica en broker MQTT.
 */

#include "telemetry.h"
#include "../core/config_data.h"
#include "../core/debug.h"
#include "../connectivity/connectivity.h"
#include "../storage/sd_logger.h"
#include "../../lib/custom_json/SimpleJson.h"
#include <WiFi.h>
#include <esp_system.h>
#include <PubSubClient.h>

// ============================================
// INICIALIZACIÓN
// ============================================

/**
 * @brief Inicializa el módulo de telemetría
 */
void telemetry_init() {
    DEBUG_INFO("TELE: Init");
}

// ============================================
// FUNCIONES AUXILIARES
// ============================================

/**
 * @brief Verifica condiciones para enviar telemetría
 */
static bool puede_enviar_telemetria() {
    if (!sistema.estados.wifiConectado) return false;
    if (!sistema.estados.mqttConectado) return false;
    if (!sistema.estados.internetDisponible) return false;
    if (!sistema.controlUsuario.envioTelemetriaHabilitado) return false;
    if (!sistema.controlUsuario.rutinaMQTTActiva) return false;
    return true;
}

/**
 * @brief Verifica condiciones para enviar heartbeat
 */
static bool puede_enviar_heartbeat() {
    if (!sistema.estados.wifiConectado) return false;
    if (!sistema.estados.mqttConectado) return false;
    if (!sistema.estados.internetDisponible) return false;
    return true;
}

/**
 * @brief Verifica si variable está en cultivo seleccionado
 */
static bool variable_en_cultivo(const char* var_name) {
    uint8_t idx = sistema.controlUsuario.cultivoSeleccionado;
    if (idx >= sistema.cultivos.numCultivos) return false;
    
    for (int i = 0; i < 16; i++) {
        if (sistema.cultivos.cultivos[idx].variables[i] == var_name) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Construye payload JSON para telemetría
 */
static String construir_payload_telemetria() {
    SimpleJson json;
    json.create_object();

    json.add_string("type", "TELEMETRY");
    
    uint64_t ts_millis = (uint64_t)sistema.tiempo.epoch * 1000ULL;
    json.add_uint64("ts", ts_millis);
    
    uint8_t crop_idx = sistema.controlUsuario.cultivoSeleccionado;
    if (crop_idx < sistema.cultivos.numCultivos) {
        json.add_string("cultivo", sistema.cultivos.cultivos[crop_idx].nombre);
    } else {
        json.add_string("cultivo", "Desconocido");
    }
    
    JsonValue* readings = json.add_array("readings");
    
    if (readings) {
        if (sistema.sensores.rs485.valido) {
            if (variable_en_cultivo("ph")) {
                JsonValue* reading = new JsonValue(JsonType::OBJECT);
                reading->add_string("var", "ph");
                reading->add_number("val", sistema.sensores.rs485.ph);
                readings->add_array_value(reading);
            }
            if (variable_en_cultivo("ec")) {
                JsonValue* reading = new JsonValue(JsonType::OBJECT);
                reading->add_string("var", "ec");
                reading->add_number("val", sistema.sensores.rs485.ec);
                readings->add_array_value(reading);
            }
            if (variable_en_cultivo("nitrogeno")) {
                JsonValue* reading = new JsonValue(JsonType::OBJECT);
                reading->add_string("var", "nitrogeno");
                reading->add_number("val", sistema.sensores.rs485.nitrogen);
                readings->add_array_value(reading);
            }
            if (variable_en_cultivo("potasio")) {
                JsonValue* reading = new JsonValue(JsonType::OBJECT);
                reading->add_string("var", "potasio");
                reading->add_number("val", sistema.sensores.rs485.potassium);
                readings->add_array_value(reading);
            }
            if (variable_en_cultivo("fosforo")) {
                JsonValue* reading = new JsonValue(JsonType::OBJECT);
                reading->add_string("var", "fosforo");
                reading->add_number("val", sistema.sensores.rs485.phosphorus);
                readings->add_array_value(reading);
            }
        }
        
        if (sistema.sensores.ds18b20.valido && variable_en_cultivo("temp_agua")) {
            JsonValue* reading = new JsonValue(JsonType::OBJECT);
            reading->add_string("var", "temp_agua");
            reading->add_number("val", sistema.sensores.ds18b20.temperatura);
            readings->add_array_value(reading);
        }
        
        if (sistema.sensores.dht22.valido && variable_en_cultivo("temp_amb")) {
            JsonValue* reading = new JsonValue(JsonType::OBJECT);
            reading->add_string("var", "temp_amb");
            reading->add_number("val", sistema.sensores.dht22.temperatura);
            readings->add_array_value(reading);
        }
        
        if (sistema.sensores.dht22.valido && variable_en_cultivo("humedad")) {
            JsonValue* reading = new JsonValue(JsonType::OBJECT);
            reading->add_string("var", "humedad");
            reading->add_number("val", sistema.sensores.dht22.humedad);
            readings->add_array_value(reading);
        }
    }

    return json.to_string();
}

/**
 * @brief Construye payload JSON para heartbeat
 */
static String construir_payload_heartbeat() {
    SimpleJson json;
    json.create_object();

    json.add_string("type", "HEARTBEAT");
    
    uint64_t ts_millis = (uint64_t)sistema.tiempo.epoch * 1000ULL;
    json.add_uint64("ts", ts_millis);
    
    json.add_string("fw", sistema.dispositivo.vFirmware);
    json.add_string("hw", sistema.dispositivo.vHardware);
    
    JsonValue* bat = json.add_object("bat");
    if (bat) {
        char nivelStr[8];
        snprintf(nivelStr, sizeof(nivelStr), "%d", sistema.dispositivo.bateria.nivel);
        bat->add_string("lvl", nivelStr);
    }
    
    JsonValue* wifi = json.add_object("wifi");
    if (wifi) {
        wifi->add_number("rssi", WiFi.RSSI());
    }
    
    json.add_number("heap", ESP.getFreeHeap());
    json.add_uint64("uptime", millis() / 1000);

    return json.to_string();
}

/**
 * @brief Publica mensaje en MQTT
 */
static bool publicar_mqtt(const char* tipo, const String& payload, int qos) {
    extern PubSubClient mqttClient;
    
    bool resultado = mqttClient.publish(sistema.mqtt.topic, payload.c_str(), false);
    
    if (resultado) {
        DEBUG_INFOF("TELE: %s %db", tipo, payload.length());
    } else {
        DEBUG_ERRORF("TELE: %s fail", tipo);
    }
    
    return resultado;
}

// ============================================
// FUNCIONES PÚBLICAS
// ============================================

/**
 * @brief Envía telemetría al broker MQTT
 */
void telemetry_enviar_telemetria() {
    if (!puede_enviar_telemetria()) return;

    sistema.ultimaEjecucion.telemetria = millis();
    
    sd_logger_guardar_medicion();
    
    String payload = construir_payload_telemetria();
    publicar_mqtt("TEL", payload, 2);
}

/**
 * @brief Envía heartbeat al broker MQTT
 */
void telemetry_enviar_heartbeat() {
    if (!puede_enviar_heartbeat()) return;

    sistema.ultimaEjecucion.heartbeat = millis();
    String payload = construir_payload_heartbeat();
    publicar_mqtt("HB", payload, 0);
}

/**
 * @brief Habilita/deshabilita envío de telemetría
 */
void telemetry_habilitar_envio(bool habilitar) {
    sistema.controlUsuario.envioTelemetriaHabilitado = habilitar;
    DEBUG_INFOF("TELE: %s", habilitar ? "ON" : "OFF");
}

/**
 * @brief Consulta estado de telemetría
 */
bool telemetry_esta_habilitado() {
    return sistema.controlUsuario.envioTelemetriaHabilitado;
}