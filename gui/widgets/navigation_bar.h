#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <stddef.h> // For NULL

// Forward declare NavigationBar for the callback
struct NavigationBar;

// --- NavigationBarActionItem Structure ---
typedef enum {
    NAV_ACTION_ITEM_TYPE_TEXT,
    NAV_ACTION_ITEM_TYPE_ICON // Icon rendering is conceptual for now
} NavigationBarActionItemType;

typedef struct {
    NavigationBarActionItemType type;
    char* text_label;     // Used if type is NAV_ACTION_ITEM_TYPE_TEXT
    char* icon_path;      // Used if type is NAV_ACTION_ITEM_TYPE_ICON (conceptual path)
    char* action_id;      // Unique ID for this action item (e.g., "back", "save", "menu")

    // Callback function when the item is activated (clicked)
    // It receives the parent navigation bar and the action_id of the item.
    void (*callback)(struct NavigationBar* bar, const char* action_id);

    // void* user_data_for_callback; // Optional: if callback needs specific data beyond action_id
} NavigationBarActionItem;


#define MAX_NAV_ACTION_ITEMS_PER_SIDE 2 // Max items on each side for simplicity

// Structure for the Navigation Bar
typedef struct NavigationBar {
    char* title;

    NavigationBarActionItem left_items[MAX_NAV_ACTION_ITEMS_PER_SIDE];
    int left_item_count;

    NavigationBarActionItem right_items[MAX_NAV_ACTION_ITEMS_PER_SIDE];
    int right_item_count;

    void* user_data; // Optional: to pass to callbacks or associate data with the bar
} NavigationBar;

// Function to create a new navigation bar
NavigationBar* NavigationBar_Create(const char* title);

// Function to set the title of the navigation bar
void NavigationBar_SetTitle(NavigationBar* bar, const char* title);

// Function to render the navigation bar
// This would typically involve generating HTML/markup with classes from ios_theme.css
void NavigationBar_Render(NavigationBar* bar);

// Function to destroy the navigation bar and free memory
void NavigationBar_Destroy(NavigationBar* bar);

// --- Functions to manage action items ---

// Add an action item to the left side of the navigation bar.
// The NavigationBar takes ownership of the item's duplicated strings if not already.
// If the item was created by NavigationBarActionItem_CreateText/Icon, its strings are already duplicated.
// Returns 0 on success, -1 if max items reached or error.
int NavigationBar_AddLeftItem(NavigationBar* bar, NavigationBarActionItem item);

// Add an action item to the right side of the navigation bar.
// Returns 0 on success, -1 if max items reached or error.
int NavigationBar_AddRightItem(NavigationBar* bar, NavigationBarActionItem item);

// (Optional future enhancements)
// void NavigationBar_ClearLeftItems(NavigationBar* bar);
// void NavigationBar_ClearRightItems(NavigationBar* bar);
// void NavigationBar_SetUserData(NavigationBar* bar, void* user_data);
// void* NavigationBar_GetUserData(const NavigationBar* bar);


// Helper to create an action item (implementation in .c)
NavigationBarActionItem NavigationBarActionItem_CreateText(const char* action_id, const char* text_label, void (*callback)(struct NavigationBar*, const char*));
NavigationBarActionItem NavigationBarActionItem_CreateIcon(const char* action_id, const char* icon_path, void (*callback)(struct NavigationBar*, const char*)); // Conceptual
void NavigationBarActionItem_Destroy(NavigationBarActionItem* item); // To free strduped text/icon_path/action_id


#endif // NAVIGATION_BAR_H
