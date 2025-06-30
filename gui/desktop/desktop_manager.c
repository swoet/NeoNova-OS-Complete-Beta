#include "desktop_manager.h"
#include "ios_home_screen.h"
#include "app_manager.h"    // Include AppManager
#include "app_manifest.h"   // Include AppManifest for creating manifests
#include <stdio.h>
#include <string.h>

// --- UI Mode static variable ---
static UIMode G_current_ui_mode = UI_MODE_UNKNOWN;

void DesktopManager_SetCurrentUIMode(UIMode mode) {
    if (G_current_ui_mode != mode) {
        G_current_ui_mode = mode;
        printf("DesktopManager: UI Mode changed to %d\n", mode);
        // In a real system, this might trigger a UI refresh or event.
        DesktopManager_NotifyUIModeChanged();
    }
}

UIMode DesktopManager_GetCurrentUIMode() {
    return G_current_ui_mode;
}
// --- End UI Mode static variable ---


// Placeholder for the GUI system's theme loading function
void GUISystem_LoadTheme(const char* css_file_path) {
    printf("GUI System: Attempting to load theme from %s\n", css_file_path);
    // Actual implementation would parse and apply CSS
}

// Global instance of the home screen for now, or manage its lifecycle appropriately
static IOSHomeScreen* current_home_screen = NULL;
// We might also want a TabBar instance if it's managed globally for the desktop mode
// static TabBar* global_sidebar_tab_bar = NULL;


void DesktopManager_Initialize(UIMode initial_mode) {
    DesktopManager_SetCurrentUIMode(initial_mode);
    printf("DesktopManager: Initializing in mode %d...\n", initial_mode);

    // 1. Initialize AppManager
    if (AppManager_Initialize() != 0) {
        fprintf(stderr, "DesktopManager: Failed to initialize AppManager!\n");
        // Handle error: perhaps don't continue full desktop init
        return;
    }

    // 2. Load the theme
    GUISystem_LoadTheme("gui/theme/ios_theme.css"); // Or an adaptive theme name
    printf("DesktopManager: Theme selected.\n");

    // 3. Create and configure the Home Screen
    current_home_screen = IOSHomeScreen_Create();
    if (!current_home_screen) {
        printf("DesktopManager: Failed to create Home Screen!\n");
        AppManager_Shutdown(); // Clean up AppManager if home screen fails
        return;
    }
    printf("DesktopManager: Home Screen created.\n");

    IOSHomeScreen_SetWallpaper(current_home_screen, "gui/assets/wallpapers/default_ios_wallpaper.png");
    printf("DesktopManager: Default wallpaper set.\n");

    // 4. Create and load sample applications into AppManager
    AppManifest* app1_manifest = AppManifest_Create("com.neonovos.messages", "Messages", "1.0", "messages_exec", "gui/assets/icons/messages.png");
    if (app1_manifest) {
        AppManifest_AddPermission(app1_manifest, PERMISSION_NETWORK_ACCESS);
        AppManager_LoadAppFromManifest(app1_manifest); // AppManager takes ownership
    }
    AppManifest* app2_manifest = AppManifest_Create("com.neonovos.photos", "Photos", "1.1", "photos_exec", "gui/assets/icons/photos.png");
    if (app2_manifest) {
        AppManifest_AddPermission(app2_manifest, PERMISSION_FILESYSTEM_READ);
        // To mark as a dock app (conceptual - AppManager/HomeScreen needs to use this)
        // AppManifest_AddMetadata(app2_manifest, "isDockApp", "true");
        AppManager_LoadAppFromManifest(app2_manifest);
    }
    AppManifest* app3_manifest = AppManifest_Create("com.neonovos.settings", "Settings", "1.0", "settings_exec", "gui/assets/icons/settings.png");
    AppManager_LoadAppFromManifest(app3_manifest); // AppManager takes ownership even if NULL check is skipped for brevity

    // Example of a "dock" app - this distinction needs to be handled by AppManager or HomeScreen rendering logic.
    AppManifest* dock_app1_manifest = AppManifest_Create("com.neonovos.phone", "Phone", "1.2", "phone_exec", "gui/assets/icons/phone.png");
    if (dock_app1_manifest) {
        // AppManifest_AddMetadata(dock_app1_manifest, "isDockApp", "true");
        AppManager_LoadAppFromManifest(dock_app1_manifest);
    }
    printf("DesktopManager: Sample apps loaded into AppManager.\n");

    // 5. Conceptual: Initialize other global UI elements based on mode
    // For example, if in Desktop mode, we might create a persistent sidebar TabBar here.
    // if (DesktopManager_GetCurrentUIMode() == UI_MODE_DESKTOP) {
    //    global_sidebar_tab_bar = TabBar_Create();
    //    TabBar_AddItem(global_sidebar_tab_bar, "Files");
    //    TabBar_AddItem(global_sidebar_tab_bar, "Apps");
    //    TabBar_AddItem(global_sidebar_tab_bar, "Settings");
    // }

    // 4. Initial Render of the entire desktop environment
    DesktopManager_NotifyUIModeChanged(); // This will call the main render function

    printf("DesktopManager: Initialization complete.\n");
}

// This function would be responsible for rendering the entire desktop based on the current mode
void DesktopManager_RenderFullDesktop() {
    UIMode mode = DesktopManager_GetCurrentUIMode();
    printf("DesktopManager: --- Conceptual Full Desktop Render Start (Mode: %d) ---\n", mode);

    // Example of how the layout could change:
    // The CSS with media queries or body classes (.mode-desktop) handles most of this.
    // The C code's role is to ensure the correct components are rendered.

    // If desktop mode, and we have a global sidebar, render it.
    // The home screen's CSS is now set to flex-direction: row, so it will sit next to it.
    // if (mode == UI_MODE_DESKTOP && global_sidebar_tab_bar) {
    //    TabBar_Render(global_sidebar_tab_bar);
    // }

    // Render the main content (home screen in this case)
    // The home screen's internal rendering doesn't need to change much,
    // as CSS handles its position within the larger desktop flex container.
    if (current_home_screen) {
        // Pass the mode if individual components inside home screen need to adapt in C
        // IOSHomeScreen_Render(current_home_screen, mode);
        IOSHomeScreen_Render(current_home_screen); // Assuming its render doesn't take mode yet
    } else {
        printf("<p>Error: Home screen not available.</p>\n");
    }

    // Potentially render other desktop elements like a global status bar, etc.
    // Example: if (mode == UI_MODE_DESKTOP) printf("<div class='desktop-status-bar'>...</div>\n");

    printf("DesktopManager: --- Conceptual Full Desktop Render End ---\n");
}


void DesktopManager_NotifyUIModeChanged() {
    // This is the function that should be called to refresh the UI.
    // In a real system, it would trigger a redraw event.
    // Here, we just call our main conceptual render function.
    DesktopManager_RenderFullDesktop();
}


void DesktopManager_Shutdown() {
    printf("DesktopManager: Shutting down...\n");
    if (current_home_screen) {
        IOSHomeScreen_Destroy(current_home_screen);
        current_home_screen = NULL;
        printf("DesktopManager: Home Screen destroyed.\n");
    }
    // if (global_sidebar_tab_bar) {
    //    TabBar_Destroy(global_sidebar_tab_bar);
    //    global_sidebar_tab_bar = NULL;
    // }
    AppManager_Shutdown(); // Shutdown the AppManager
    printf("DesktopManager: Shutdown complete.\n");
}


// Example of how it might be called if there's a main loop or startup sequence
// in the test harness or a future OS main loop.
/*
int main_os_loop() {
    DesktopManager_Initialize(UI_MODE_DESKTOP); // Start in desktop mode for example

    // Main OS event loop
    // while (system_is_running) {
    //    handle_input(); // This could also lead to UIMode changes
    //    update_ui_state();
    //    DesktopManager_RenderFullDesktop(); // Or called on demand via NotifyUIModeChanged
    //    Display_FlipBuffer();
    // }

    DesktopManager_Shutdown();
    return 0;
}

// To simulate a mode change:
// DesktopManager_SetCurrentUIMode(UI_MODE_MOBILE);
// DesktopManager_NotifyUIModeChanged(); // This would then call RenderFullDesktop
*/
