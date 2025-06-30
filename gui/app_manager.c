#include "app_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    APP_STATE_UNKNOWN,
    APP_STATE_LOADED,
    APP_STATE_RUNNING,
    APP_STATE_STOPPED, // Explicitly stopped
    APP_STATE_ERROR
} AppState;

// Internal structure for a loaded/running application instance
struct AppInstance {
    AppManifest* manifest; // The application's manifest
    AppState current_state;
    // int process_id; // Conceptual: OS-level process ID if running
    // void* container_handle; // Conceptual: Handle to the container
};

// --- Global state for the App Manager ---
static AppInstance* G_loaded_apps[MAX_LOADED_APPS];
static int G_loaded_apps_count = 0;

// For simplicity, running apps are just a subset of loaded_apps with RUNNING state.
// A more complex system might have a separate list for running apps.
// static AppInstance* G_running_apps[MAX_RUNNING_APPS];
// static int G_running_apps_count = 0;


int AppManager_Initialize() {
    G_loaded_apps_count = 0;
    memset(G_loaded_apps, 0, sizeof(G_loaded_apps));
    printf("AppManager: Initialized.\n");
    return 0;
}

void AppManager_Shutdown() {
    printf("AppManager: Shutting down...\n");
    for (int i = 0; i < G_loaded_apps_count; ++i) {
        if (G_loaded_apps[i]) {
            if (G_loaded_apps[i]->current_state == APP_STATE_RUNNING) {
                AppManager_StopApp(G_loaded_apps[i]); // Attempt to stop running apps
            }
            AppManifest_Destroy(G_loaded_apps[i]->manifest); // AppManager owns the manifest after loading
            free(G_loaded_apps[i]);
            G_loaded_apps[i] = NULL;
        }
    }
    G_loaded_apps_count = 0;
    printf("AppManager: Shutdown complete.\n");
}

// Takes ownership of the manifest if successful
AppInstance* AppManager_LoadAppFromManifest(AppManifest* manifest) {
    if (!manifest) {
        fprintf(stderr, "AppManager Error: Cannot load app from null manifest.\n");
        return NULL;
    }
    if (G_loaded_apps_count >= MAX_LOADED_APPS) {
        fprintf(stderr, "AppManager Error: Max loaded apps reached. Cannot load %s.\n", manifest->app_id);
        // Important: If AppManager doesn't take ownership on failure here, caller must free manifest.
        // For this design, let's say the caller still owns manifest if load fails.
        return NULL;
    }

    // Check for duplicates by app_id (optional, but good practice)
    for (int i = 0; i < G_loaded_apps_count; ++i) {
        if (G_loaded_apps[i] && G_loaded_apps[i]->manifest &&
            strcmp(G_loaded_apps[i]->manifest->app_id, manifest->app_id) == 0) {
            fprintf(stderr, "AppManager Warning: App %s already loaded.\n", manifest->app_id);
            // Caller still owns manifest.
            return G_loaded_apps[i]; // Return existing instance
        }
    }

    AppInstance* app_instance = (AppInstance*)malloc(sizeof(AppInstance));
    if (!app_instance) {
        fprintf(stderr, "AppManager Error: Failed to allocate memory for AppInstance for %s.\n", manifest->app_id);
        // Caller still owns manifest.
        return NULL;
    }

    app_instance->manifest = manifest; // AppManager now owns this manifest
    app_instance->current_state = APP_STATE_LOADED;
    // app_instance->process_id = 0; // Not running yet

    G_loaded_apps[G_loaded_apps_count++] = app_instance;
    printf("AppManager: App '%s' (%s) loaded successfully.\n", manifest->app_name, manifest->app_id);
    return app_instance;
}

int AppManager_StartApp(AppInstance* app) {
    if (!app) {
        fprintf(stderr, "AppManager Error: Cannot start a null AppInstance.\n");
        return -1;
    }
    if (app->current_state == APP_STATE_RUNNING) {
        printf("AppManager Info: App '%s' is already running.\n", AppInstance_GetName(app));
        return 0; // Not an error, already running
    }
    if (app->current_state != APP_STATE_LOADED && app->current_state != APP_STATE_STOPPED) {
        fprintf(stderr, "AppManager Error: App '%s' is not in a state to be started (current state: %d).\n", AppInstance_GetName(app), app->current_state);
        return -1;
    }

    // --- Conceptual Containerization & Process Creation ---
    printf("AppManager: Starting app '%s'...\n", AppInstance_GetName(app));
    printf("  (Conceptual) Allocating container resources...\n");
    printf("  (Conceptual) Setting up sandbox with permissions for %s...\n", AppInstance_GetName(app));
    // for (int i = 0; i < app->manifest->permission_count; ++i) {
    //    printf("    - Granting permission: %d\n", app->manifest->required_permissions[i]);
    // }
    printf("  (Conceptual) Launching entry point: %s\n", app->manifest->entry_point);
    // app->process_id = conceptual_os_create_process(app->manifest->entry_point);
    // if (app->process_id == 0) { app->current_state = APP_STATE_ERROR; return -1; }
    // --- End Conceptual ---

    app->current_state = APP_STATE_RUNNING;
    printf("AppManager: App '%s' started (conceptually).\n", AppInstance_GetName(app));
    return 0;
}

int AppManager_StopApp(AppInstance* app) {
    if (!app) {
        fprintf(stderr, "AppManager Error: Cannot stop a null AppInstance.\n");
        return -1;
    }
    if (app->current_state != APP_STATE_RUNNING) {
        printf("AppManager Info: App '%s' is not currently running.\n", AppInstance_GetName(app));
        return 0; // Not an error, already stopped or not started
    }

    // --- Conceptual Process Termination & Container Cleanup ---
    printf("AppManager: Stopping app '%s'...\n", AppInstance_GetName(app));
    // conceptual_os_terminate_process(app->process_id);
    // app->process_id = 0;
    printf("  (Conceptual) Releasing container resources for %s...\n", AppInstance_GetName(app));
    // --- End Conceptual ---

    app->current_state = APP_STATE_STOPPED;
    printf("AppManager: App '%s' stopped (conceptually).\n", AppInstance_GetName(app));
    return 0;
}

const AppManifest* AppManager_GetManifest(const AppInstance* app) {
    if (!app) return NULL;
    return app->manifest;
}

// AppState AppManager_GetAppState(const AppInstance* app) {
//     if (!app) return APP_STATE_UNKNOWN;
//     return app->current_state;
// }

int AppManager_GetLoadedApps(AppInstance* loaded_apps_array[], int array_size) {
    int count = 0;
    for (int i = 0; i < G_loaded_apps_count && count < array_size; ++i) {
        if (G_loaded_apps[i]) { // Should always be true if count is managed properly
            loaded_apps_array[count++] = G_loaded_apps[i];
        }
    }
    return count;
}


// --- Helper functions for AppInstance ---
const char* AppInstance_GetName(const AppInstance* app) {
    if (!app || !app->manifest) return "UnknownApp";
    return app->manifest->app_name ? app->manifest->app_name : "UnnamedApp";
}

const char* AppInstance_GetIconPath(const AppInstance* app) {
    if (!app || !app->manifest) return "gui/assets/icons/default.png"; // Default icon
    return app->manifest->icon_path ? app->manifest->icon_path : "gui/assets/icons/default.png";
}

const char* AppInstance_GetAppID(const AppInstance* app) {
    if (!app || !app->manifest) return "unknown.app.id";
    return app->manifest->app_id ? app->manifest->app_id : "unnamed.app.id";
}

AppInstance* AppManager_FindAppByID(const char* app_id) {
    if (!app_id) return NULL;
    for (int i = 0; i < G_loaded_apps_count; ++i) {
        if (G_loaded_apps[i] && G_loaded_apps[i]->manifest && G_loaded_apps[i]->manifest->app_id &&
            strcmp(G_loaded_apps[i]->manifest->app_id, app_id) == 0) {
            return G_loaded_apps[i];
        }
    }
    return NULL; // Not found
}

int AppManager_UnloadApp(const char* app_id) {
    if (!app_id) {
        fprintf(stderr, "AppManager Error: Cannot unload app with null ID.\n");
        return -1;
    }

    int found_index = -1;
    AppInstance* app_to_unload = NULL;

    for (int i = 0; i < G_loaded_apps_count; ++i) {
        if (G_loaded_apps[i] && G_loaded_apps[i]->manifest && G_loaded_apps[i]->manifest->app_id &&
            strcmp(G_loaded_apps[i]->manifest->app_id, app_id) == 0) {
            app_to_unload = G_loaded_apps[i];
            found_index = i;
            break;
        }
    }

    if (!app_to_unload) {
        fprintf(stderr, "AppManager Info: App with ID '%s' not found for unloading.\n", app_id);
        return -1; // Or 0 if "not found" isn't an error for unload
    }

    printf("AppManager: Unloading app '%s' (%s)...\n", AppInstance_GetName(app_to_unload), app_id);

    // Stop the app if it's running
    if (app_to_unload->current_state == APP_STATE_RUNNING) {
        AppManager_StopApp(app_to_unload); // This will change its state to STOPPED
    }

    // Destroy the manifest (owned by AppInstance which is owned by AppManager)
    AppManifest_Destroy(app_to_unload->manifest);
    app_to_unload->manifest = NULL;

    // Free the AppInstance itself
    free(app_to_unload);
    G_loaded_apps[found_index] = NULL; // Mark as NULL in the array

    // Compact the array: shift elements down to fill the gap
    if (found_index < G_loaded_apps_count - 1) {
        // memmove is safer for overlapping regions if we were shifting more complex structs
        // For an array of pointers, a simple loop is fine.
        for (int i = found_index; i < G_loaded_apps_count - 1; ++i) {
            G_loaded_apps[i] = G_loaded_apps[i + 1];
        }
    }
    G_loaded_apps_count--;
    if (G_loaded_apps_count > 0 || found_index == G_loaded_apps_count ) { // if it was not the last element or array is now empty
         G_loaded_apps[G_loaded_apps_count] = NULL; // Null out the new last potentially duplicated pointer
    }


    printf("AppManager: App '%s' unloaded successfully.\n", app_id);
    return 0;
}
