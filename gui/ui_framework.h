#ifndef UI_FRAMEWORK_H
#define UI_FRAMEWORK_H
#include "window_manager.h"
void ui_framework_init(void);
void ui_framework_tick(void);
void ui_framework_shutdown(void);
void ui_framework_set_window_manager(window_manager_t* wm);
void ui_framework_render(void);
#endif // UI_FRAMEWORK_H 