/**
 * @file connectivity.h
 * @brief WiFi y MQTT connectivity management
 * @details Maneja conexiones WiFi, MQTT con TLS y procesamiento de mensajes.
 *          Proporciona verificación de estado de conectividad y gestión de
 *          configuración remota de cultivos vía MQTT.
 */

#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <Arduino.h>
#include <PubSubClient.h>

// ============================================
// INICIALIZACIÓN
// ============================================

/**
 * @brief Inicializa el módulo de conectividad
 * @details Configura WiFi en modo estación, TLS para MQTT y callbacks.
 *          Debe llamarse una vez en setup().
 */
void connectivity_init();

// ============================================
// CONEXIONES
// ============================================

/**
 * @brief Establece conexión WiFi
 * @return true si la conexión fue exitosa
 */
bool connectivity_conectar_wifi();

/**
 * @brief Establece conexión MQTT con broker
 * @details Requiere WiFi conectado. Usa TLS sin verificación de certificado.
 * @return true si la conexión fue exitosa
 */
bool connectivity_conectar_mqtt();

/**
 * @brief Verifica disponibilidad de internet
 * @details Actualmente retorna el estado de WiFi conectado.
 * @return true si hay acceso a internet
 */
bool connectivity_verificar_internet();

// ============================================
// GESTIÓN DE ESTADO
// ============================================

/**
 * @brief Verifica y mantiene conexiones activas
 * @details Reconecta WiFi y MQTT si es necesario. Ejecuta mqttClient.loop().
 */
void connectivity_verificar();

/**
 * @brief Consulta si hay conectividad disponible para envío
 * @return true si WiFi, MQTT e internet están activos
 */
bool connectivity_puede_enviar();

/**
 * @brief Solicita configuración al backend (cultivos y variables)
 * @return true si se envió REQUEST_CONFIG correctamente
 */
bool connectivity_request_config();

// ============================================
// CALLBACKS Y GLOBALES
// ============================================

/**
 * @brief Callback para mensajes MQTT entrantes
 * @param topic Topic MQTT del mensaje
 * @param payload Datos del mensaje
 * @param length Longitud del payload
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length);

/**
 * @brief Cliente MQTT global con TLS
 * @details Exportado para uso en módulos de telemetría
 */
extern PubSubClient mqttClient;

#endif // CONNECTIVITY_H

