#include "ui_framework.h"
#include <stdio.h>
static window_manager_t* g_wm = NULL;
void ui_framework_init(void) { printf("[UIFramework] Initialized.\n"); }
void ui_framework_tick(void) {
    printf("[UIFramework] Tick.\n");
    if (g_wm) wm_render(g_wm);
}
void ui_framework_shutdown(void) { printf("[UIFramework] Shutdown.\n"); }
void ui_framework_set_window_manager(window_manager_t* wm) { g_wm = wm; }
void ui_framework_render(void) { if (g_wm) wm_render(g_wm); } 