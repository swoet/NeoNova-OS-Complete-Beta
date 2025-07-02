#include "button.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

button_widget_t* button_create(int x, int y, int w, int h, const char* label, void (*on_click)(void*), void* user_data) {
    return button_create_a11y(x, y, w, h, label, on_click, user_data, label);
}

button_widget_t* button_create_a11y(int x, int y, int w, int h, const char* label, void (*on_click)(void*), void* user_data, const char* a11y) {
    button_widget_t* btn = (button_widget_t*)malloc(sizeof(button_widget_t));
    btn->x = x; btn->y = y; btn->width = w; btn->height = h;
    btn->label = strdup(label);
    btn->on_click = on_click;
    btn->user_data = user_data;
    btn->pressed = false;
    btn->focused = 0;
    btn->accessibility_label = a11y ? strdup(a11y) : NULL;
    return btn;
}

static int high_contrast_mode = 0;
void button_set_high_contrast(int enabled) { high_contrast_mode = enabled; }

void button_render(const button_widget_t* btn) {
    const char* border = high_contrast_mode ? "====" : "----";
    const char* bg = high_contrast_mode ? "\033[47;30m" : "\033[45;37m";
    const char* sel = btn->pressed ? (high_contrast_mode ? "\033[7m" : "\033[1m") : "";
    const char* focus = btn->focused ? "\033[7m" : "";
    const char* reset = "\033[0m";
    printf("%s%s%s%s\n", bg, focus, border, reset);
    printf("%s%s[Button] '%s'%s at (%d,%d) size %dx%d %s\n", bg, focus, btn->label, reset, btn->x, btn->y, btn->width, btn->height, btn->pressed ? "[PRESSED]" : "");
    printf("%s%s%s%s\n", bg, focus, border, reset);
    if (btn->focused && btn->accessibility_label) {
        printf("[ScreenReader] Focused: %s\n", btn->accessibility_label);
    }
    if (btn->pressed && btn->accessibility_label) {
        printf("[ScreenReader] Activated: %s\n", btn->accessibility_label);
    }
}

void button_handle_click(button_widget_t* btn, int mouse_x, int mouse_y) {
    if (mouse_x >= btn->x && mouse_x < btn->x + btn->width && mouse_y >= btn->y && mouse_y < btn->y + btn->height) {
        btn->pressed = true;
        if (btn->on_click) btn->on_click(btn->user_data);
    } else {
        btn->pressed = false;
    }
} 