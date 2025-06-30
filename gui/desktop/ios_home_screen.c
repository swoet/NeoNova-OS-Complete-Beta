#include "ios_home_screen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IOSHomeScreen* IOSHomeScreen_Create() {
    IOSHomeScreen* screen = (IOSHomeScreen*)malloc(sizeof(IOSHomeScreen));
    if (!screen) return NULL;

    screen->app_count = 0;
    screen->dock_app_count = 0;
    screen->wallpaper_path = NULL;
    memset(screen->apps, 0, sizeof(screen->apps));
    memset(screen->dock_apps, 0, sizeof(screen->dock_apps));

    // Default wallpaper or leave NULL for CSS to handle
    // IOSHomeScreen_SetWallpaper(screen, "gui/assets/default_wallpaper.png");

    return screen;
}

static int AddAppToList(AppIcon* list, int* count, int max_items, const char* name, const char* icon_path) {
    if (*count >= max_items) return -1; // List full

    list[*count].name = name ? strdup(name) : NULL;
    list[*count].icon_path = icon_path ? strdup(icon_path) : NULL;
    // list[*count].launch_action = NULL; // Assign later if needed

    if ((name && !list[*count].name) || (icon_path && !list[*count].icon_path)) {
        // strdup failed, cleanup
        if (list[*count].name) free(list[*count].name);
        if (list[*count].icon_path) free(list[*count].icon_path);
        return -1;
    }
    (*count)++;
    return 0;
}

int IOSHomeScreen_AddApp(IOSHomeScreen* screen, const char* name, const char* icon_path) {
    if (!screen) return -1;
    return AddAppToList(screen->apps, &screen->app_count, MAX_APPS_ON_HOME_SCREEN, name, icon_path);
}

int IOSHomeScreen_AddAppToDock(IOSHomeScreen* screen, const char* name, const char* icon_path) {
    if (!screen) return -1;
    return AddAppToList(screen->dock_apps, &screen->dock_app_count, MAX_APPS_IN_DOCK, name, icon_path);
}

void IOSHomeScreen_SetWallpaper(IOSHomeScreen* screen, const char* wallpaper_path) {
    if (!screen) return;
    if (screen->wallpaper_path) {
        free(screen->wallpaper_path);
    }
    screen->wallpaper_path = wallpaper_path ? strdup(wallpaper_path) : NULL;
}

void IOSHomeScreen_Render(IOSHomeScreen* screen) {
    if (!screen) return;

    // Conceptual rendering
    // The outer div would be styled by CSS to show the wallpaper
    printf("<div class=\"home-screen\" style=\"background-image: url('%s');\">\n",
           screen->wallpaper_path ? screen->wallpaper_path : "");

    // App Grid
    printf("  <div class=\"app-grid\">\n");
    for (int i = 0; i < screen->app_count; ++i) {
        printf("    <div class=\"app-icon\">\n");
        if (screen->apps[i].icon_path) {
            // In a real system, this would be an <img> tag or similar
            printf("      <div class=\"app-icon-image\" style=\"background-image: url('%s');\"></div>\n", screen->apps[i].icon_path);
        }
        if (screen->apps[i].name) {
            printf("      <span class=\"app-icon-name\">%s</span>\n", screen->apps[i].name);
        }
        printf("    </div>\n");
    }
    printf("  </div>\n");

    // Dock
    printf("  <div class=\"dock\">\n");
    for (int i = 0; i < screen->dock_app_count; ++i) {
        printf("    <div class=\"app-icon dock-app-icon\">\n"); // dock-app-icon for specific styling if needed
        if (screen->dock_apps[i].icon_path) {
            printf("      <div class=\"app-icon-image\" style=\"background-image: url('%s');\"></div>\n", screen->dock_apps[i].icon_path);
        }
        // iOS dock apps usually don't show names, but we can include if desired
        // if (screen->dock_apps[i].name) {
        //    printf("      <span class=\"app-icon-name\">%s</span>\n", screen->dock_apps[i].name);
        // }
        printf("    </div>\n");
    }
    printf("  </div>\n");

    printf("</div>\n"); // End of home-screen
}

static void FreeAppList(AppIcon* list, int count) {
    for (int i = 0; i < count; ++i) {
        if (list[i].name) free(list[i].name);
        if (list[i].icon_path) free(list[i].icon_path);
    }
}

void IOSHomeScreen_Destroy(IOSHomeScreen* screen) {
    if (!screen) return;

    FreeAppList(screen->apps, screen->app_count);
    FreeAppList(screen->dock_apps, screen->dock_app_count);

    if (screen->wallpaper_path) {
        free(screen->wallpaper_path);
    }
    free(screen);
}
