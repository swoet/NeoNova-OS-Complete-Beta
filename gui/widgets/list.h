#pragma once

#define MAX_LIST_ITEMS 64

typedef struct list_widget {
    int x, y, width, height;
    int item_count;
    const char* items[MAX_LIST_ITEMS];
    const char* icons[MAX_LIST_ITEMS];
    const char* colors[MAX_LIST_ITEMS];
    int selected_index;
    int focused;
    const char* accessibility_label;
} list_widget_t;

list_widget_t* list_create(int x, int y, int w, int h);
void list_add_item(list_widget_t* list, const char* item, const char* icon, const char* color);
void list_render(const list_widget_t* list);
void list_select_next(list_widget_t* list);
void list_select_prev(list_widget_t* list);
void list_select_at(list_widget_t* list, int index);
void list_set_high_contrast(int enabled);
list_widget_t* list_create_a11y(int x, int y, int w, int h, const char* a11y); 