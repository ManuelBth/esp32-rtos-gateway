/**
 * @file telemetry.h
 * @brief Envío de datos al broker MQTT
 * @details Proporciona funciones para enviar telemetría y heartbeat
 *          con datos de sensores al servidor remoto.
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <Arduino.h>

// ============================================
// INICIALIZACIÓN
// ============================================

/**
 * @brief Inicializa el módulo de telemetría
 */
void telemetry_init();

// ============================================
// ENVÍO DE DATOS
// ============================================

/**
 * @brief Envía mensaje de telemetría con datos de sensores
 * @details Construye JSON con readings filtrados por cultivo seleccionado
 */
void telemetry_enviar_telemetria();

/**
 * @brief Envía mensaje heartbeat con estado del sistema
 * @details Incluye firmware, hardware, batería, WiFi RSSI, heap y uptime
 */
void telemetry_enviar_heartbeat();

// ============================================
// CONTROL
// ============================================

/**
 * @brief Habilita o deshabilita el envío de telemetría
 * @param habilitar true para activar
 */
void telemetry_habilitar_envio(bool habilitar);

/**
 * @brief Consulta si el envío de telemetría está habilitado
 * @return true si está habilitado
 */
bool telemetry_esta_habilitado();

#endif