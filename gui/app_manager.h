#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include "app_manifest.h" // Uses AppManifest structure

#define MAX_LOADED_APPS 20
#define MAX_RUNNING_APPS 10 // Example limits

// Forward declaration for App structure, defined in app_manager.c
// This represents a loaded (and possibly running) application instance.
typedef struct AppInstance AppInstance;

// --- Application Manager API ---

// Initialize the Application Manager
// Returns 0 on success, -1 on failure
int AppManager_Initialize();

// Shuts down the Application Manager and cleans up resources
void AppManager_Shutdown();

// Load an application from its manifest path.
// This would typically involve parsing the manifest file (e.g., JSON, XML)
// and storing its details. Here, we might conceptually take a pre-created manifest.
// Returns a pointer to the loaded AppInstance or NULL on failure.
// The AppManager takes ownership of the passed manifest if successful (or a copy).
AppInstance* AppManager_LoadAppFromManifest(AppManifest* manifest);

// For a system where manifests are files:
// AppInstance* AppManager_LoadApp(const char* manifest_path);

// Start a loaded application instance.
// Returns 0 on success, -1 on failure (e.g., already running, permission issues).
// This is highly conceptual as it would involve process creation, sandboxing, etc.
int AppManager_StartApp(AppInstance* app);

// Stop a running application instance.
// Returns 0 on success, -1 on failure.
int AppManager_StopApp(AppInstance* app);

// Get the manifest associated with an application instance.
const AppManifest* AppManager_GetManifest(const AppInstance* app);

// Get the current state of an application (e.g., LOADED, RUNNING, STOPPED)
// typedef enum { APP_STATE_UNKNOWN, APP_STATE_LOADED, APP_STATE_RUNNING, APP_STATE_STOPPED, APP_STATE_ERROR } AppState;
// AppState AppManager_GetAppState(const AppInstance* app);

// Get a list of all loaded applications (caller should not free AppInstances)
// Returns the number of loaded apps and fills the provided array.
int AppManager_GetLoadedApps(AppInstance* loaded_apps_array[], int array_size);

// Get a list of all running applications
// int AppManager_GetRunningApps(AppInstance* running_apps_array[], int array_size);


// --- Helper functions for AppInstance (implementation detail, but exposed if needed) ---
const char* AppInstance_GetName(const AppInstance* app);
const char* AppInstance_GetIconPath(const AppInstance* app);
const char* AppInstance_GetAppID(const AppInstance* app);

// Find a loaded application instance by its ID.
// Returns a pointer to the AppInstance or NULL if not found.
AppInstance* AppManager_FindAppByID(const char* app_id);

// Unload an application by its ID.
// This will stop the app if it's running, then remove it from the loaded list,
// freeing its manifest and instance data.
// Returns 0 on success, -1 if app not found or error.
int AppManager_UnloadApp(const char* app_id);


#endif // APP_MANAGER_H
