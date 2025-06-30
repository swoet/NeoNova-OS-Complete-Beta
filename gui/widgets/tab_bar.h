#ifndef TAB_BAR_H
#define TAB_BAR_H

#define MAX_TAB_ITEMS 5 // Typical max for iOS tab bars

typedef struct {
    char* title;
    // char* icon_path; // Path to an icon image, for future use
    // void (*action_callback)(); // Function to call when tab is selected
} TabBarItem;

typedef struct {
    TabBarItem items[MAX_TAB_ITEMS];
    int item_count;
    int active_item_index;
} TabBar;

// Function to create a new tab bar
TabBar* TabBar_Create();

// Function to add an item to the tab bar
// Returns 0 on success, -1 on failure (e.g., max items reached)
int TabBar_AddItem(TabBar* bar, const char* title /*, const char* icon_path, void (*action_callback)() */);

// Function to set the active tab
// Returns 0 on success, -1 if index is out of bounds
int TabBar_SetActiveItem(TabBar* bar, int item_index);

// Function to render the tab bar
void TabBar_Render(TabBar* bar);

// Function to destroy the tab bar and free memory
void TabBar_Destroy(TabBar* bar);

#endif // TAB_BAR_H
