#include "navigation_bar.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strdup
#include <stdio.h>  // For printf (used for conceptual rendering)

// Helper for strdup with NULL check, to avoid clutter
static char* StrDup_Safe_Nav(const char* s) {
    if (!s) return NULL;
    char*d = strdup(s);
    if (!d) fprintf(stderr, "strdup failed in NavigationBar\n");
    return d;
}


// --- NavigationBarActionItem Implementation ---

NavigationBarActionItem NavigationBarActionItem_CreateText(const char* action_id, const char* text_label, void (*callback)(struct NavigationBar*, const char*)) {
    NavigationBarActionItem item;
    item.type = NAV_ACTION_ITEM_TYPE_TEXT;
    item.action_id = StrDup_Safe_Nav(action_id);
    item.text_label = StrDup_Safe_Nav(text_label);
    item.icon_path = NULL;
    item.callback = callback;
    return item;
}

NavigationBarActionItem NavigationBarActionItem_CreateIcon(const char* action_id, const char* icon_path, void (*callback)(struct NavigationBar*, const char*)) {
    NavigationBarActionItem item;
    item.type = NAV_ACTION_ITEM_TYPE_ICON;
    item.action_id = StrDup_Safe_Nav(action_id);
    item.text_label = NULL;
    item.icon_path = StrDup_Safe_Nav(icon_path); // Conceptual, icon loading/rendering not implemented
    item.callback = callback;
    return item;
}

void NavigationBarActionItem_Destroy(NavigationBarActionItem* item) {
    if (!item) return;
    if (item->action_id) free(item->action_id);
    if (item->text_label) free(item->text_label);
    if (item->icon_path) free(item->icon_path);
    // Set to NULL to prevent double free if struct itself is not immediately out of scope
    item->action_id = NULL;
    item->text_label = NULL;
    item->icon_path = NULL;
    item->callback = NULL;
}


// --- NavigationBar Implementation ---

// Function to create a new navigation bar
NavigationBar* NavigationBar_Create(const char* title) {
    NavigationBar* bar = (NavigationBar*)malloc(sizeof(NavigationBar));
    if (!bar) {
        fprintf(stderr, "Failed to allocate NavigationBar\n");
        return NULL;
    }
    bar->title = StrDup_Safe_Nav(title);
    bar->left_item_count = 0;
    bar->right_item_count = 0;
    bar->user_data = NULL;
    // No need to memset fixed-size arrays of structs if counts manage access
    return bar;
}

// Function to set the title of the navigation bar
void NavigationBar_SetTitle(NavigationBar* bar, const char* title) {
    if (!bar) return;
    if (bar->title) {
        free(bar->title);
    }
    // Use the safe strdup helper
    bar->title = StrDup_Safe_Nav(title);
}

// Function to render the navigation bar
// This is a conceptual rendering. In a real system, this would generate
// UI elements that the theming engine (using ios_theme.css) would style.
void NavigationBar_Render(NavigationBar* bar) {
    if (!bar) return;

    printf("<div class=\"navigation-bar\">\n");

    // Render Left Action Items
    printf("  <div class=\"nav-actions-left\">\n");
    for (int i = 0; i < bar->left_item_count; ++i) {
        NavigationBarActionItem* item = &(bar->left_items[i]);
        // Conceptual: onclick would call a JS bridge to item->callback(bar, item->action_id)
        // Or, in a C-based GUI, a click event on this rendered element would trigger the callback.
        printf("    <button class=\"nav-action-item\" data-action-id=\"%s\">\n", item->action_id ? item->action_id : "");
        if (item->type == NAV_ACTION_ITEM_TYPE_TEXT && item->text_label) {
            printf("      %s\n", item->text_label);
        } else if (item->type == NAV_ACTION_ITEM_TYPE_ICON && item->icon_path) {
            // Conceptual icon rendering
            printf("      <img src=\"%s\" alt=\"%s\" class=\"nav-action-icon\" />\n", item->icon_path, item->action_id ? item->action_id : "icon");
        } else {
            printf("      (Action)\n"); // Fallback
        }
        printf("    </button>\n");
    }
    printf("  </div>\n");

    // Render Title
    if (bar->title) {
        printf("  <span class=\"navigation-bar-title\">%s</span>\n", bar->title);
    } else {
        printf("  <span class=\"navigation-bar-title\"></span>\n"); // Empty span to maintain structure
    }

    // Render Right Action Items
    printf("  <div class=\"nav-actions-right\">\n");
    for (int i = 0; i < bar->right_item_count; ++i) {
        NavigationBarActionItem* item = &(bar->right_items[i]);
        printf("    <button class=\"nav-action-item\" data-action-id=\"%s\">\n", item->action_id ? item->action_id : "");
        if (item->type == NAV_ACTION_ITEM_TYPE_TEXT && item->text_label) {
            printf("      %s\n", item->text_label);
        } else if (item->type == NAV_ACTION_ITEM_TYPE_ICON && item->icon_path) {
            printf("      <img src=\"%s\" alt=\"%s\" class=\"nav-action-icon\" />\n", item->icon_path, item->action_id ? item->action_id : "icon");
        } else {
            printf("      (Action)\n");
        }
        printf("    </button>\n");
    }
    printf("  </div>\n");

    printf("</div>\n"); // End of navigation-bar
}

// Function to destroy the navigation bar and free memory
void NavigationBar_Destroy(NavigationBar* bar) {
    if (!bar) return;

    if (bar->title) {
        free(bar->title);
        bar->title = NULL;
    }

    // Destroy all action items to free their internal strings
    for (int i = 0; i < bar->left_item_count; ++i) {
        NavigationBarActionItem_Destroy(&(bar->left_items[i]));
    }
    bar->left_item_count = 0;

    for (int i = 0; i < bar->right_item_count; ++i) {
        NavigationBarActionItem_Destroy(&(bar->right_items[i]));
    }
    bar->right_item_count = 0;

    // bar->user_data is not owned by NavigationBar, so not freed here

    free(bar);
}


// --- Functions to manage action items ---

int NavigationBar_AddLeftItem(NavigationBar* bar, NavigationBarActionItem item) {
    if (!bar) {
        fprintf(stderr, "NavigationBar_AddLeftItem: bar is NULL\n");
        // If item's strings were allocated, they might leak if not handled by caller
        // or if item is not subsequently destroyed. For items created by our helpers,
        // they are value types and their contents are freed when the bar is destroyed.
        return -1;
    }
    if (bar->left_item_count >= MAX_NAV_ACTION_ITEMS_PER_SIDE) {
        fprintf(stderr, "NavigationBar_AddLeftItem: Max left items reached for bar '%s'\n", bar->title ? bar->title : "");
        // As above, potential leak of item's contents if caller doesn't manage.
        // However, our NavigationBarActionItem_Destroy will be called on existing items during bar destruction.
        // If this new item isn't added, its strings would need to be freed by the caller if dynamically allocated for this call.
        // Items from CreateText/Icon are value types; if passed by value and not stored, no leak from *this* function.
        NavigationBarActionItem_Destroy(&item); // Destroy the passed-in item if it's not added and owns resources.
        return -1;
    }
    // The item is copied. Strings inside item are assumed to be already duplicated by CreateText/Icon.
    bar->left_items[bar->left_item_count++] = item;
    return 0;
}

int NavigationBar_AddRightItem(NavigationBar* bar, NavigationBarActionItem item) {
    if (!bar) {
        fprintf(stderr, "NavigationBar_AddRightItem: bar is NULL\n");
        NavigationBarActionItem_Destroy(&item); // Cleanup passed item
        return -1;
    }
    if (bar->right_item_count >= MAX_NAV_ACTION_ITEMS_PER_SIDE) {
        fprintf(stderr, "NavigationBar_AddRightItem: Max right items reached for bar '%s'\n", bar->title ? bar->title : "");
        NavigationBarActionItem_Destroy(&item); // Cleanup passed item
        return -1;
    }
    bar->right_items[bar->right_item_count++] = item;
    return 0;
}

/* Conceptual:
void NavigationBar_SetUserData(NavigationBar* bar, void* user_data) {
    if (bar) {
        bar->user_data = user_data;
    }
}

void* NavigationBar_GetUserData(const NavigationBar* bar) {
    if (bar) {
        return bar->user_data;
    }
    return NULL;
}
*/
