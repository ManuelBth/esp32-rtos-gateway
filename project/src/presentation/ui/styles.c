#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: btn_midiendo
//

void init_style_btn_midiendo_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][4]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][5]));
    lv_style_set_text_font(style, &ui_font_roboto_btn);
};

lv_style_t *get_style_btn_midiendo_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_btn_midiendo_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_btn_midiendo(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_btn_midiendo_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_btn_midiendo(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_btn_midiendo_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: btn_normal
//

void init_style_btn_normal_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][1]));
    lv_style_set_text_font(style, &ui_font_roboto_btn);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][5]));
};

lv_style_t *get_style_btn_normal_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_btn_normal_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_btn_normal(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_btn_normal_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_btn_normal(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_btn_normal_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_btn_midiendo,
        add_style_btn_normal,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_btn_midiendo,
        remove_style_btn_normal,
    };
    remove_style_funcs[styleIndex](obj);
}

