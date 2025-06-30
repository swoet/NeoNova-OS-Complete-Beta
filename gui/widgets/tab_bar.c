#include "tab_bar.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strdup, memset
#include <stdio.h>  // For printf (used for conceptual rendering)

// Function to create a new tab bar
TabBar* TabBar_Create() {
    TabBar* bar = (TabBar*)malloc(sizeof(TabBar));
    if (!bar) {
        return NULL;
    }
    bar->item_count = 0;
    bar->active_item_index = 0; // Default to the first item if any
    memset(bar->items, 0, sizeof(bar->items)); // Clear items array
    return bar;
}

// Function to add an item to the tab bar
int TabBar_AddItem(TabBar* bar, const char* title) {
    if (!bar || bar->item_count >= MAX_TAB_ITEMS) {
        return -1; // Error: bar is null or max items reached
    }

    bar->items[bar->item_count].title = title ? strdup(title) : NULL;
    // bar->items[bar->item_count].icon_path = icon_path ? strdup(icon_path) : NULL;
    // bar->items[bar->item_count].action_callback = action_callback;

    bar->item_count++;
    return 0; // Success
}

// Function to set the active tab
int TabBar_SetActiveItem(TabBar* bar, int item_index) {
    if (!bar || item_index < 0 || item_index >= bar->item_count) {
        return -1; // Error: index out of bounds
    }
    bar->active_item_index = item_index;
    return 0; // Success
}

// Function to render the tab bar
void TabBar_Render(TabBar* bar) {
    if (!bar) return;

    // Conceptual: Output HTML-like structure with classes from ios_theme.css
    printf("<div class=\"tab-bar\">\n");
    for (int i = 0; i < bar->item_count; ++i) {
        const char* active_class = (i == bar->active_item_index) ? " active" : "";
        printf("  <div class=\"tab-bar-item%s\">\n", active_class);
        // In a real app, you'd render an icon here too
        // if (bar->items[i].icon_path) {
        //    printf("    <img src=\"%s\" alt=\"%s icon\">\n", bar->items[i].icon_path, bar->items[i].title);
        // }
        if (bar->items[i].title) {
            printf("    <span>%s</span>\n", bar->items[i].title);
        }
        printf("  </div>\n");
    }
    printf("</div>\n");
}

// Function to destroy the tab bar and free memory
void TabBar_Destroy(TabBar* bar) {
    if (!bar) return;
    for (int i = 0; i < bar->item_count; ++i) {
        if (bar->items[i].title) {
            free(bar->items[i].title);
        }
        // if (bar->items[i].icon_path) {
        //    free(bar->items[i].icon_path);
        // }
    }
    free(bar);
}
