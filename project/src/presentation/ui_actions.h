#ifndef UI_ACTIONS_H
#define UI_ACTIONS_H

#include <lvgl.h>

/**
 * @brief Inicializar callbacks de botones LVGL
 * @note Llamar después de presentation_init() en main.cpp
 * @note Registra eventos para "comenzar_medir" y "comenzar_rutina"
 */
void ui_actions_init();

/**
 * @brief Handler para botón "comenzar_medir"
 * @param e Evento LVGL (LV_EVENT_CLICKED)
 * @note Toggle de lectura de sensores + cambio de color del botón
 * @note Color verde cuando activo, color original (RGB 0,64,128) cuando inactivo
 */
void ui_actions_handle_comenzar_medir(lv_event_t * e);

/**
 * @brief Handler para botón "comenzar_rutina"
 * @param e Evento LVGL (LV_EVENT_CLICKED)
 * @note Toggle de telemetría MQTT + guardado SD (futuro) + cambio de color
 * @note Color verde cuando activo, color original (RGB 0,64,128) cuando inactivo
 * @note Valida que haya datos de sensores válidos antes de activar
 */
void ui_actions_handle_comenzar_rutina(lv_event_t * e);

#endif // UI_ACTIONS_H
