#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *menu;
    lv_obj_t *medir;
    lv_obj_t *historial;
    lv_obj_t *configuracion;
    lv_obj_t *info;
    lv_obj_t *filtros;
    lv_obj_t *obj0;
    lv_obj_t *obj0__obj0;
    lv_obj_t *obj0__obj1;
    lv_obj_t *obj0__fecha_1;
    lv_obj_t *obj0__lugar_1;
    lv_obj_t *obj0__bateria_1;
    lv_obj_t *obj0__obj2;
    lv_obj_t *obj1;
    lv_obj_t *obj1__obj0;
    lv_obj_t *obj1__obj1;
    lv_obj_t *obj1__fecha_1;
    lv_obj_t *obj1__lugar_1;
    lv_obj_t *obj1__bateria_1;
    lv_obj_t *obj1__obj2;
    lv_obj_t *obj2;
    lv_obj_t *obj2__obj3;
    lv_obj_t *obj3;
    lv_obj_t *obj3__obj0;
    lv_obj_t *obj3__obj1;
    lv_obj_t *obj3__fecha_1;
    lv_obj_t *obj3__lugar_1;
    lv_obj_t *obj3__bateria_1;
    lv_obj_t *obj3__obj2;
    lv_obj_t *obj4;
    lv_obj_t *obj4__obj3;
    lv_obj_t *obj5;
    lv_obj_t *obj5__obj3;
    lv_obj_t *obj6;
    lv_obj_t *obj6__obj3;
    lv_obj_t *obj7;
    lv_obj_t *obj7__obj3;
    lv_obj_t *btn_medir;
    lv_obj_t *btn_historial;
    lv_obj_t *btn_config;
    lv_obj_t *btn_info;
    lv_obj_t *comenzar_medir;
    lv_obj_t *comenzar_medir_2;
    lv_obj_t *actualizar_data;
    lv_obj_t *comenzar_medir_3;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *comenzar_rutina;
    lv_obj_t *obj13;
    lv_obj_t *temperatura_ag;
    lv_obj_t *temp_agua_value;
    lv_obj_t *obj14;
    lv_obj_t *temperatura_amb;
    lv_obj_t *temp_amb_value;
    lv_obj_t *obj15;
    lv_obj_t *humedad;
    lv_obj_t *humedad_value;
    lv_obj_t *obj16;
    lv_obj_t *ph;
    lv_obj_t *ph_value;
    lv_obj_t *obj17;
    lv_obj_t *ec;
    lv_obj_t *ec_value;
    lv_obj_t *obj18;
    lv_obj_t *nitrogeno;
    lv_obj_t *nitrogeno_value;
    lv_obj_t *obj19;
    lv_obj_t *fosforo;
    lv_obj_t *fosforo_value;
    lv_obj_t *obj20;
    lv_obj_t *potasio;
    lv_obj_t *potasio_value;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *select_crops;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *select_time;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *temperatura_ag_1;
    lv_obj_t *t_agua_1;
    lv_obj_t *t_agua_2;
    lv_obj_t *temperatura_ag_4;
    lv_obj_t *t_agua_7;
    lv_obj_t *t_agua_8;
    lv_obj_t *temperatura_ag_2;
    lv_obj_t *t_agua_3;
    lv_obj_t *t_agua_4;
    lv_obj_t *temperatura_ag_3;
    lv_obj_t *t_agua_5;
    lv_obj_t *t_agua_6;
    lv_obj_t *temperatura_ag_6;
    lv_obj_t *t_agua_11;
    lv_obj_t *t_agua_12;
    lv_obj_t *temperatura_ag_7;
    lv_obj_t *t_agua_13;
    lv_obj_t *t_agua_14;
    lv_obj_t *temperatura_ag_5;
    lv_obj_t *t_agua_9;
    lv_obj_t *t_agua_10;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *dia_desde;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *mes_desde;
    lv_obj_t *dia_hasta;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *mes_hasta;
    lv_obj_t *age;
    lv_obj_t *anio;
    lv_obj_t *obj42;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MENU = 1,
    SCREEN_ID_MEDIR = 2,
    SCREEN_ID_HISTORIAL = 3,
    SCREEN_ID_CONFIGURACION = 4,
    SCREEN_ID_INFO = 5,
    SCREEN_ID_FILTROS = 6,
};

void create_screen_menu();
void tick_screen_menu();

void create_screen_medir();
void tick_screen_medir();

void create_screen_historial();
void tick_screen_historial();

void create_screen_configuracion();
void tick_screen_configuracion();

void create_screen_info();
void tick_screen_info();

void create_screen_filtros();
void tick_screen_filtros();

void create_user_widget_panel(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_panel(void *flowState, int startWidgetIndex);

void create_user_widget_btn_atras(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_btn_atras(void *flowState, int startWidgetIndex);

enum Themes {
    THEME_ID_DEFAULT,
};
enum Colors {
    COLOR_ID_FONDO,
    COLOR_ID_INFO,
    COLOR_ID_ERROR,
    COLOR_ID_WARNING,
    COLOR_ID_NORMAL,
    COLOR_ID_TEXTO,
    COLOR_ID_FONDO2,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[1][7];

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/