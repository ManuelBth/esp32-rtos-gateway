#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: btn_midiendo
lv_style_t *get_style_btn_midiendo_MAIN_DEFAULT();
void add_style_btn_midiendo(lv_obj_t *obj);
void remove_style_btn_midiendo(lv_obj_t *obj);

// Style: btn_normal
lv_style_t *get_style_btn_normal_MAIN_DEFAULT();
void add_style_btn_normal(lv_obj_t *obj);
void remove_style_btn_normal(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/