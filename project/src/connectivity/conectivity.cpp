/**
 * @file connectivity.cpp
 * @brief Implementación de conectividad WiFi y MQTT
 * @details Maneja conexiones, callbacks y procesamiento de mensajes MQTT.
 *          Proporciona funciones para WiFi, MQTT con TLS, verificación de
 *          estado de conexión y solicitud de configuración remota.
 */

#include "connectivity.h"
#include "../core/config_data.h"
#include "../core/debug.h"
#include "../rtc/rtc_handler.h"
#include "../storage/config_manager.h"
#include "../../lib/custom_json/SimpleJson.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// ============================================
// INSTANCIAS GLOBALES
// ============================================

/**
 * @brief Cliente WiFi con soporte TLS
 */
WiFiClientSecure secureClient;

/**
 * @brief Cliente MQTT configurado con WiFiClientSecure
 */
PubSubClient mqttClient(secureClient);

// ============================================
// EXPONENTIAL BACKOFF STATE
// ============================================

/**
 * @brief Current MQTT reconnect delay (milliseconds)
 * @details Starts at 1 second, doubles each failure up to MAX_DELAY
 */
static uint32_t mqttReconnectDelay = 1000;

/**
 * @brief Maximum reconnect delay cap (30 seconds)
 */
static const uint32_t MQTT_RECONNECT_MAX_DELAY = 30000;

/**
 * @brief Flag indicating if backoff is active (waiting before retry)
 */
static bool mqttBackoffActive = false;

/**
 * @brief Last MQTT connection attempt timestamp
 */
static unsigned long lastMqttAttempt = 0;

// ============================================
// PROCESAMIENTO DE CONFIG JSON
// ============================================

/**
 * @brief Procesa JSON tipo CONFIG y actualiza cultivos en sistema
 * @param json Objeto SimpleJson con datos parseados
 * @return true si se procesó correctamente al menos un cultivo
 * @note Limita a MAX_CULTIVOS (8) y 16 variables por cultivo
 */
static bool procesar_config_json(SimpleJson& json) {
    DEBUG_INFO("CONN: Iniciando procesamiento de CONFIG");
    
    // Obtener array de cultivos
    JsonValue* cultivos_array = json.get_value("cultivos");
    if (!cultivos_array || cultivos_array->get_type() != JsonType::ARRAY) {
        DEBUG_ERROR("CONN: CONFIG sin array 'cultivos'");
        return false;
    }
    
    // Obtener número de cultivos (máximo 8)
    int num_cultivos = cultivos_array->get_array_size();
    DEBUG_INFOF("CONN: Array cultivos detectado: %d elementos", num_cultivos);
    
    if (num_cultivos > MAX_CULTIVOS) {
        DEBUG_WARNF("CONN: >%d cultivos detectados, recortando a %d", num_cultivos, MAX_CULTIVOS);
        num_cultivos = MAX_CULTIVOS;
    }
    
    // Resetear cultivos actuales
    sistema.cultivos.numCultivos = 0;
    
    // Iterar sobre cada cultivo
    for (int i = 0; i < num_cultivos; i++) {
        DEBUG_INFOF("CONN: Procesando cultivo[%d]...", i);
        
        JsonValue* cultivo_obj = cultivos_array->get_array_value(i);
        if (!cultivo_obj || cultivo_obj->get_type() != JsonType::OBJECT) {
            DEBUG_WARNF("CONN: Cultivo[%d] no es objeto válido", i);
            continue;
        }
        
        // Extraer nombre del cultivo
        JsonValue* nombre_val = cultivo_obj->get_object_value("nombre");
        if (!nombre_val || nombre_val->get_type() != JsonType::STRING) {
            DEBUG_WARNF("CONN: Cultivo[%d] sin campo 'nombre'", i);
            continue;
        }
        String nombre = nombre_val->as_string();
        DEBUG_INFOF("CONN: Cultivo[%d] nombre: '%s'", i, nombre.c_str());
        
        snprintf(sistema.cultivos.cultivos[i].nombre, 
                 sizeof(sistema.cultivos.cultivos[i].nombre), 
                 "%s", nombre.c_str());
        
        // Extraer array de variables
        JsonValue* variables_array = cultivo_obj->get_object_value("variables");
        if (!variables_array || variables_array->get_type() != JsonType::ARRAY) {
            DEBUG_WARNF("CONN: Cultivo[%d] sin array 'variables'", i);
            continue;
        }
        
        int num_vars = variables_array->get_array_size();
        DEBUG_INFOF("CONN: Cultivo[%d] tiene %d variables", i, num_vars);
        
        if (num_vars > 16) num_vars = 16;
        
        // Iterar sobre variables y extraer campo "var"
        int var_count = 0;
        for (int j = 0; j < num_vars; j++) {
            JsonValue* var_obj = variables_array->get_array_value(j);
            if (!var_obj || var_obj->get_type() != JsonType::OBJECT) continue;
            
            JsonValue* var_name_val = var_obj->get_object_value("var");
            if (!var_name_val || var_name_val->get_type() != JsonType::STRING) continue;
            
            String var_name = var_name_val->as_string();
            
            // Guardar nombre de variable en minúsculas
            sistema.cultivos.cultivos[i].variables[var_count] = var_name;
            DEBUG_INFOF("CONN:   Variable[%d]: %s", var_count, var_name.c_str());
            var_count++;
        }
        
        DEBUG_INFOF("CONN: Cultivo[%d]: '%s' con %d variables OK", 
                    i, nombre.c_str(), var_count);
        
        sistema.cultivos.numCultivos++;
    }
    
    // Validar que haya al menos un cultivo válido
    if (sistema.cultivos.numCultivos == 0) {
        DEBUG_WARN("CONN: Sin cultivos válidos procesados");
        snprintf(sistema.cultivos.cultivos[0].nombre, 
                 sizeof(sistema.cultivos.cultivos[0].nombre), 
                 "Sin cultivos");
        sistema.cultivos.numCultivos = 1;
        return false;
    }
    
    DEBUG_INFOF("CONN: CONFIG procesado exitosamente: %d cultivos cargados", 
                sistema.cultivos.numCultivos);
    return true;
}

// ============================================
// CALLBACK MQTT: Procesar mensajes entrantes
// ============================================

/**
 * @brief Callback invocado cuando llega un mensaje MQTT
 * @param topic Topic del mensaje recibido
 * @param payload Datos binarios del mensaje
 * @param length Longitud del payload en bytes
 * @note Procesa mensajes tipo CONFIG y actualiza cultivos en sistema
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    // Detectar si el mensaje fue truncado
    if (length >= 1024) {
        DEBUG_WARN("CONN: Mensaje posiblemente truncado (>=1024b)");
    }
    
    // Convertir payload a String
    String mensaje = "";
    for (unsigned int i = 0; i < length; i++) {
        mensaje += (char)payload[i];
    }
    
    DEBUG_INFOF("CONN: RX [%s] %db", topic, length);
    
    // Log del JSON completo (solo DEBUG_LEVEL >= 3)
    DEBUG_INFOF("CONN: JSON: %s", mensaje.c_str());
    
    // Parsear JSON
    SimpleJson json;
    if (json.parse(mensaje)) {
        String tipo = json.get_string("type", "");
        DEBUG_INFOF("CONN: Tipo mensaje: %s", tipo.c_str());
        
        if (tipo == "CONFIG") {
            DEBUG_INFO("CONN: Procesando CONFIG");
            
            if (procesar_config_json(json)) {
                DEBUG_INFO("CONN: CONFIG OK");
                sistema.controlUsuario.esperandoConfig = false;
                
                // Persistir configuración en SD
                time_t now;
                time(&now);
                sistema.sistema.ultima_config_servidor = now;
                
                if (config_save_to_sd()) {
                    DEBUG_INFO("CONN: CONFIG guardado en SD");
                } else {
                    DEBUG_WARN("CONN: No se pudo guardar CONFIG en SD");
                }
            } else {
                DEBUG_ERROR("CONN: CONFIG fail");
                sistema.controlUsuario.esperandoConfig = false;
            }
        } else {
            DEBUG_WARNF("CONN: tipo?: %s", tipo.c_str());
        }
    } else {
        DEBUG_ERROR("CONN: JSON parse err");
        DEBUG_ERRORF("CONN: Mensaje recibido: %s", mensaje.substring(0, 100).c_str());
    }
}

// ============================================
// INICIALIZACIÓN
// ============================================

/**
 * @brief Inicializa el módulo de conectividad
 * @details Configura WiFi en modo estación, TLS sin verificación de certificado,
 *          servidor MQTT, callback y buffer. Debe llamarse en setup().
 */
void connectivity_init() {
    DEBUG_INFO("CONN: Init");
    WiFi.mode(WIFI_STA);
    secureClient.setInsecure();
    
    mqttClient.setServer(sistema.mqtt.broker, sistema.mqtt.port);
    mqttClient.setCallback(mqtt_callback);  
    mqttClient.setBufferSize(1024);
    mqttClient.setKeepAlive(60);
    
    DEBUG_INFOF("CONN: MQTT %s:%d", sistema.mqtt.broker, sistema.mqtt.port);
}

// ============================================
// CONEXIONES
// ============================================

/**
 * @brief Establece conexión WiFi con credenciales configuradas
 * @return true si la conexión fue exitosa
 * @note Timeout de 10 intentos (~2 segundos)
 */
bool connectivity_conectar_wifi() {
    if (sistema.wifi.ssid == nullptr || strlen(sistema.wifi.ssid) == 0) {
        DEBUG_ERROR("CONN: No SSID");
        return false;
    }

    DEBUG_INFOF("CONN: WiFi->%s", sistema.wifi.ssid);
    WiFi.begin(sistema.wifi.ssid, sistema.wifi.password);

    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 10) {
        delay(200);
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        sistema.estados.wifiConectado = true;
        DEBUG_INFOF("CONN: IP:%s", WiFi.localIP().toString().c_str());
        return true;
    }

    sistema.estados.wifiConectado = false;
    DEBUG_ERROR("CONN: WiFi fail");
    return false;
}

/**
 * @brief Establishes MQTT connection with exponential backoff
 * @return true if connection successful
 * @details Implements exponential backoff: starts at 1s, doubles on each
 *          failure up to 30s cap. Resets delay on successful connection.
 */
bool connectivity_conectar_mqtt() {
    if (!sistema.estados.wifiConectado) {
        return false;
    }

    if (!mqttClient.connected()) {
        unsigned long currentTime = millis();

        if (mqttBackoffActive) {
            if (currentTime - lastMqttAttempt < mqttReconnectDelay) {
                return false;
            }
            mqttBackoffActive = false;
        }

        DEBUG_INFOF("CONN: MQTT->%s (delay=%lums)", sistema.mqtt.broker, mqttReconnectDelay);

        if (mqttClient.connect(sistema.dispositivo.serie,
                               sistema.mqtt.username,
                               sistema.mqtt.password)) {
            sistema.estados.mqttConectado = true;
            mqttClient.subscribe(sistema.mqtt.topic, 1);
            mqttReconnectDelay = 1000;
            DEBUG_INFO("CONN: MQTT OK");
            return true;
        } else {
            sistema.estados.mqttConectado = false;
            DEBUG_ERRORF("CONN: MQTT err=%d", mqttClient.state());

            lastMqttAttempt = currentTime;
            mqttBackoffActive = true;

            mqttReconnectDelay = min(mqttReconnectDelay * 2, MQTT_RECONNECT_MAX_DELAY);
            DEBUG_INFOF("CONN: MQTT backoff %lu ms", mqttReconnectDelay);

            return false;
        }
    }
    return sistema.estados.mqttConectado;
}

/**
 * @brief Verifica disponibilidad de internet
 * @return true si hay acceso a internet (basado en estado WiFi)
 * @note Implementación simple: retorna estado WiFi conectado
 */
bool connectivity_verificar_internet() {
    sistema.estados.internetDisponible = sistema.estados.wifiConectado;
    return sistema.estados.internetDisponible;
}

// ============================================
// GESTIÓN DE ESTADO
// ============================================

/**
 * @brief Verifica y mantiene conexiones activas
 * @details Reconecta WiFi si es necesario, verifica MQTT y ejecuta loop().
 *          Debe llamarse periódicamente desde el scheduler.
 */
void connectivity_verificar() {
    sistema.ultimaEjecucion.verificacionConexion = millis();
    
    bool wifiAnterior = sistema.estados.wifiConectado;
    sistema.estados.wifiConectado = (WiFi.status() == WL_CONNECTED);

    if (wifiAnterior != sistema.estados.wifiConectado) {
        DEBUG_INFOF("CONN: WiFi %s", sistema.estados.wifiConectado ? "ON" : "OFF");
    }
    
    if (!sistema.estados.wifiConectado) {
        sistema.estados.mqttConectado = false;
        sistema.estados.internetDisponible = false;
        connectivity_conectar_wifi();
        return;
    }

    bool internetAnterior = sistema.estados.internetDisponible;
    connectivity_verificar_internet();

    if (!internetAnterior && sistema.estados.internetDisponible) {
        DEBUG_INFO("CONN: Internet recuperado");
    }
    
    sistema.estados.mqttConectado = mqttClient.connected();
    
    if (!sistema.estados.mqttConectado) {
        connectivity_conectar_mqtt();
    } else {
        mqttClient.loop();
    }
}

/**
 * @brief Consulta si hay conectividad disponible para envío de datos
 * @return true si WiFi, MQTT e internet están activos
 */
bool connectivity_puede_enviar() {
    return sistema.estados.wifiConectado && 
           sistema.estados.mqttConectado && 
           sistema.estados.internetDisponible;
}

// ============================================
// SOLICITAR CONFIGURACIÓN AL BACKEND
// ============================================

/**
 * @brief Envía mensaje REQUEST_CONFIG por MQTT
 * @return true si se publicó correctamente en el topic
 * @note Activa flag esperandoConfig hasta recibir respuesta
 */
bool connectivity_request_config() {
    if (!sistema.estados.mqttConectado) {
        DEBUG_ERROR("CONN: MQTT no conectado");
        return false;
    }
    
    SimpleJson json;
    json.create_object();
    json.add_string("type", "REQUEST_CONFIG");
    
    String payload = json.to_string();
    bool resultado = mqttClient.publish(sistema.mqtt.topic, payload.c_str(), false);
    
    if (resultado) {
        DEBUG_INFOF("CONN: REQ_CFG %db", payload.length());
        sistema.controlUsuario.esperandoConfig = true;
    } else {
        DEBUG_ERROR("CONN: REQ_CFG fail");
    }
    
    return resultado;
}