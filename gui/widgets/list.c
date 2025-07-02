#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int high_contrast_mode = 0;
void list_set_high_contrast(int enabled) { high_contrast_mode = enabled; }

list_widget_t* list_create(int x, int y, int w, int h) {
    return list_create_a11y(x, y, w, h, "List");
}

list_widget_t* list_create_a11y(int x, int y, int w, int h, const char* a11y) {
    list_widget_t* list = (list_widget_t*)malloc(sizeof(list_widget_t));
    list->x = x; list->y = y; list->width = w; list->height = h;
    list->item_count = 0;
    list->selected_index = -1;
    list->focused = 0;
    list->accessibility_label = a11y ? strdup(a11y) : NULL;
    memset(list->items, 0, sizeof(list->items));
    memset(list->icons, 0, sizeof(list->icons));
    memset(list->colors, 0, sizeof(list->colors));
    return list;
}

void list_add_item(list_widget_t* list, const char* item, const char* icon, const char* color) {
    if (list->item_count < MAX_LIST_ITEMS) {
        list->items[list->item_count] = strdup(item);
        list->icons[list->item_count] = icon ? strdup(icon) : NULL;
        list->colors[list->item_count] = color ? strdup(color) : NULL;
        list->item_count++;
    }
}

void list_render(const list_widget_t* list) {
    const char* border = high_contrast_mode ? "====" : "----";
    const char* bg = high_contrast_mode ? "\033[47;30m" : "\033[46;37m";
    const char* sel = high_contrast_mode ? "\033[7m" : "\033[1m";
    const char* focus = list->focused ? "\033[7m" : "";
    const char* reset = "\033[0m";
    printf("%s%s%s%s\n", bg, focus, border, reset);
    printf("%s%s[List] at (%d,%d) size %dx%d%s\n", bg, focus, list->x, list->y, list->width, list->height, reset);
    for (int i = 0; i < list->item_count; ++i) {
        const char* color = list->colors[i] ? list->colors[i] : "";
        const char* icon = list->icons[i] ? list->icons[i] : " ";
        int selected = (i == list->selected_index);
        printf("%s%s  %s%s%s %s%s%s\n", selected ? sel : "", color, icon, reset, color, list->items[i], reset, selected ? reset : "");
    }
    printf("%s%s%s%s\n", bg, focus, border, reset);
    if (list->focused && list->accessibility_label) {
        printf("[ScreenReader] Focused: %s\n", list->accessibility_label);
    }
}

void list_select_next(list_widget_t* list) {
    if (list->item_count == 0) return;
    if (list->selected_index < 0) list->selected_index = 0;
    else list->selected_index = (list->selected_index + 1) % list->item_count;
}

void list_select_prev(list_widget_t* list) {
    if (list->item_count == 0) return;
    if (list->selected_index < 0) list->selected_index = 0;
    else list->selected_index = (list->selected_index - 1 + list->item_count) % list->item_count;
}

void list_select_at(list_widget_t* list, int index) {
    if (index >= 0 && index < list->item_count) list->selected_index = index;
} 