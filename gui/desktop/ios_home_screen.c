#include "ios_home_screen.h"
#include "app_manager.h" // For AppManager_GetLoadedApps, AppInstance_GetName, AppInstance_GetIconPath
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper function for string duplication (if not already in a common utils file)
static char* StrDup_Safe_HomeScreen(const char* s) {
    if (!s) return NULL;
    char* dup = strdup(s);
    if (!dup) fprintf(stderr, "Error: strdup failed in HomeScreen. Out of memory?\n");
    return dup;
}

IOSHomeScreen* IOSHomeScreen_Create() {
    IOSHomeScreen* screen = (IOSHomeScreen*)malloc(sizeof(IOSHomeScreen));
    if (!screen) {
        fprintf(stderr, "Error: Failed to allocate memory for IOSHomeScreen.\n");
        return NULL;
    }
    screen->wallpaper_path = NULL;
    // screen->current_page = 0;
    // IOSHomeScreen_SetWallpaper(screen, "gui/assets/default_wallpaper.png");
    return screen;
}

void IOSHomeScreen_SetWallpaper(IOSHomeScreen* screen, const char* wallpaper_path) {
    if (!screen) return;
    if (screen->wallpaper_path) {
        free(screen->wallpaper_path);
    }
    screen->wallpaper_path = StrDup_Safe_HomeScreen(wallpaper_path);
}

void IOSHomeScreen_Render(IOSHomeScreen* screen) {
    if (!screen) return;

    printf("<div class=\"home-screen\" style=\"background-image: url('%s');\">\n",
           screen->wallpaper_path ? screen->wallpaper_path : "");

    // App Grid - Fetches apps from AppManager
    printf("  <div class=\"app-grid\">\n");
    AppInstance* loaded_apps[MAX_LOADED_APPS]; // MAX_LOADED_APPS from app_manager.h
    int app_count = AppManager_GetLoadedApps(loaded_apps, MAX_LOADED_APPS);

    for (int i = 0; i < app_count; ++i) {
        AppInstance* app = loaded_apps[i];
        const char* app_name = AppInstance_GetName(app);
        const char* icon_path = AppInstance_GetIconPath(app);
        // const char* app_id = AppInstance_GetAppID(app); // For click handlers etc.

        // Conceptual: Add a click handler to start the app
        // printf("    <div class=\"app-icon\" onclick=\"AppManager_StartAppById('%s')\">\n", app_id);
        printf("    <div class=\"app-icon\">\n"); // Simplified for now
        if (icon_path) {
            printf("      <div class=\"app-icon-image\" style=\"background-image: url('%s');\"></div>\n", icon_path);
        }
        if (app_name) {
            printf("      <span class=\"app-icon-name\">%s</span>\n", app_name);
        }
        printf("    </div>\n");
    }
    printf("  </div>\n");

    // Dock - Simplified: For now, the dock might be populated by apps marked with specific metadata
    // or by a separate list from AppManager. Here, we'll render a conceptual dock area
    // but won't populate it from the main app list directly without more logic.
    // The CSS media query already hides `.dock` on desktop.
    printf("  <div class=\"dock\">\n");
    // To properly populate the dock, AppManager would need a way to identify dock apps.
    // Example: iterate through `loaded_apps` and check manifest metadata:
    // if (AppManager_AppHasMetadata(app, "isDockApp", "true")) { /* render in dock */ }
    // For now, leave it empty or with placeholders.
    printf("    <!-- Dock apps would be rendered here based on AppManager logic -->\n");
    printf("  </div>\n");

    printf("</div>\n"); // End of home-screen
}

void IOSHomeScreen_Destroy(IOSHomeScreen* screen) {
    if (!screen) return;
    if (screen->wallpaper_path) {
        free(screen->wallpaper_path);
    }
    free(screen);
}
