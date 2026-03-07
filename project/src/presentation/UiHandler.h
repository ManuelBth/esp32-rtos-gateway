#ifndef UI_HANDLER_FIXED_H
#define UI_HANDLER_FIXED_H

/**
 * @brief Configura el hardware (TFT, Touch) e inicializa LVGL y la UI.
 */
void presentation_init();

/**
 * @brief Ejecuta el ciclo de vida de LVGL y las animaciones de la UI.
 */
void presentation_loop();

#endif
