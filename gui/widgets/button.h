#pragma once
#include <stdbool.h>

typedef struct button_widget {
    int x, y, width, height;
    const char* label;
    void (*on_click)(void* user_data);
    void* user_data;
    bool pressed;
    int focused;
    const char* accessibility_label;
} button_widget_t;

button_widget_t* button_create(int x, int y, int w, int h, const char* label, void (*on_click)(void*), void* user_data);
button_widget_t* button_create_a11y(int x, int y, int w, int h, const char* label, void (*on_click)(void*), void* user_data, const char* a11y);
void button_render(const button_widget_t* btn);
void button_handle_click(button_widget_t* btn, int mouse_x, int mouse_y);
void button_set_high_contrast(int enabled); 