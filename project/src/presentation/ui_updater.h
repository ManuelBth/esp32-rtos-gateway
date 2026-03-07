#ifndef UI_UPDATER_H
#define UI_UPDATER_H

#include <stdint.h>

/**
 * @brief Inicializa el módulo de actualización de UI
 */
void ui_updater_init();

/**
 * @brief Actualiza las variables LVGL desde la estructura SistemaConfig
 * @note Debe ser llamado periódicamente desde presentation_loop()
 */
void ui_updater_update();

/**
 * @brief Actualiza la variable lugar (cultivo seleccionado) en LVGL
 * @param cropIndex Índice del cultivo seleccionado (0-7)
 */
void ui_updater_set_crop(uint8_t cropIndex);

/**
 * @brief Actualiza el intervalo de telemetría desde UI
 * @param minutes Tiempo en minutos (1-10)
 */
void ui_updater_set_telemetry_interval(uint8_t minutes);

/**
 * @brief Actualiza roller de cultivos con datos de sistema.cultivos
 * @note Llamar después de recibir CONFIG
 */
void ui_updater_refresh_crops_roller();

#endif // UI_UPDATER_H
