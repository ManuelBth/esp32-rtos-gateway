/**
 * @file debug.h
 * @brief Sistema de logging condicional para reducir uso de flash
 * @details Define macros para logging que se pueden deshabilitar en tiempo
 *          de compilación para reducir el uso de memoria flash.
 * 
 * Niveles disponibles:
 * - DEBUG_LEVEL_NONE (0): Sin logging
 * - DEBUG_LEVEL_ERROR (1): Solo errores críticos
 * - DEBUG_LEVEL_WARN (2): Errores + advertencias
 * - DEBUG_LEVEL_INFO (3): Todos los mensajes (desarrollo)
 * 
 * Para habilitar debug en desarrollo, definir en platformio.ini:
 * build_flags = -DDEBUG_LEVEL=3
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

/**
 * @brief Nivel de debug deshabilitado
 */
#define DEBUG_LEVEL_NONE 0

/**
 * @brief Nivel de debug: solo errores críticos
 */
#define DEBUG_LEVEL_ERROR 1

/**
 * @brief Nivel de debug: errores y advertencias
 */
#define DEBUG_LEVEL_WARN 2

/**
 * @brief Nivel de debug: todos los mensajes (desarrollo)
 */
#define DEBUG_LEVEL_INFO 3

/**
 * @brief Nivel de debug por defecto (solo errores)
 */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_ERROR
#endif

// ============================================
// MACROS DE ERROR (nivel 1)
// ============================================

#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
#define DEBUG_ERROR(x) Serial.print("[ERROR] "); Serial.println(x)
#define DEBUG_ERRORF(fmt, ...) Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_ERROR(x)
#define DEBUG_ERRORF(fmt, ...)
#endif

// ============================================
// MACROS DE WARNING (nivel 2)
// ============================================

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
#define DEBUG_WARN(x) Serial.print("[WARN] "); Serial.println(x)
#define DEBUG_WARNF(fmt, ...) Serial.printf("[WARN] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_WARN(x)
#define DEBUG_WARNF(fmt, ...)
#endif

// ============================================
// MACROS DE INFO (nivel 3)
// ============================================

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
#define DEBUG_INFO(x) Serial.print("[INFO] "); Serial.println(x)
#define DEBUG_INFOF(fmt, ...) Serial.printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_INFO(x)
#define DEBUG_INFOF(fmt, ...)
#endif

#endif // DEBUG_H
