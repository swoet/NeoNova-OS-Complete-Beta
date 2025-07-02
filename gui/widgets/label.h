#pragma once

typedef struct label_widget {
    int x, y;
    const char* text;
    int focused;
    const char* accessibility_label;
} label_widget_t;

label_widget_t* label_create(int x, int y, const char* text);
label_widget_t* label_create_a11y(int x, int y, const char* text, const char* a11y);
void label_render(const label_widget_t* lbl);
void label_set_high_contrast(int enabled); 