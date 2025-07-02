#include "label.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

label_widget_t* label_create(int x, int y, const char* text) {
    return label_create_a11y(x, y, text, text);
}

label_widget_t* label_create_a11y(int x, int y, const char* text, const char* a11y) {
    label_widget_t* lbl = (label_widget_t*)malloc(sizeof(label_widget_t));
    lbl->x = x; lbl->y = y;
    lbl->text = strdup(text);
    lbl->focused = 0;
    lbl->accessibility_label = a11y ? strdup(a11y) : NULL;
    return lbl;
}

static int high_contrast_mode = 0;
void label_set_high_contrast(int enabled) { high_contrast_mode = enabled; }

void label_render(const label_widget_t* lbl) {
    const char* border = high_contrast_mode ? "====" : "----";
    const char* bg = high_contrast_mode ? "\033[47;30m" : "\033[44;37m";
    const char* focus = lbl->focused ? "\033[7m" : "";
    const char* reset = "\033[0m";
    printf("%s%s%s%s\n", bg, focus, border, reset);
    printf("%s%s| %s |%s at (%d,%d)\n", bg, focus, lbl->text, reset, lbl->x, lbl->y);
    printf("%s%s%s%s\n", bg, focus, border, reset);
    if (lbl->focused && lbl->accessibility_label) {
        printf("[ScreenReader] Focused: %s\n", lbl->accessibility_label);
    }
} 