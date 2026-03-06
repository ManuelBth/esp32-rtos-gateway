#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_BATERIA = 0,
    FLOW_GLOBAL_VARIABLE_FECHA = 1,
    FLOW_GLOBAL_VARIABLE_LUGAR = 2,
    FLOW_GLOBAL_VARIABLE_MEDIR = 3,
    FLOW_GLOBAL_VARIABLE_GUARDAR = 4,
    FLOW_GLOBAL_VARIABLE_MAC = 5,
    FLOW_GLOBAL_VARIABLE_FIRMWARE_VERSION = 6,
    FLOW_GLOBAL_VARIABLE_HARDWARE_VERSION = 7,
    FLOW_GLOBAL_VARIABLE_N_SERIE = 8,
    FLOW_GLOBAL_VARIABLE_MODEL = 9,
    FLOW_GLOBAL_VARIABLE_SSID = 10,
    FLOW_GLOBAL_VARIABLE_PASS = 11
};

// Native global variables



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/