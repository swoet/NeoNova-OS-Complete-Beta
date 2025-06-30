#ifndef APP_MANIFEST_H
#define APP_MANIFEST_H

#define MAX_PERMISSIONS 10
#define MAX_METADATA_KEY_VALUE_PAIRS 10

// Conceptual application permissions
typedef enum {
    PERMISSION_NONE,
    PERMISSION_FILESYSTEM_READ,
    PERMISSION_FILESYSTEM_WRITE,
    PERMISSION_NETWORK_ACCESS,
    PERMISSION_DEVICE_CAMERA,
    PERMISSION_DEVICE_MICROPHONE
    // ... other permissions
} AppPermission;

typedef struct {
    char* key;
    char* value;
} MetadataEntry;

// Application Manifest Structure
typedef struct {
    char* app_id;           // Unique identifier, e.g., "com.example.mycoolapp"
    char* app_name;         // Display name, e.g., "My Cool App"
    char* version;          // e.g., "1.0.0"
    char* entry_point;      // Path to the main executable or script within the app package
    char* icon_path;        // Path to the app icon (relative to app package or global)

    AppPermission required_permissions[MAX_PERMISSIONS];
    int permission_count;

    MetadataEntry metadata[MAX_METADATA_KEY_VALUE_PAIRS];
    int metadata_count;

    // char* app_package_path; // Path to the root of the application package/bundle
    // This would be set by the AppManager when loading the manifest.
} AppManifest;

// Functions to create, parse (conceptually), and destroy a manifest
// In a real system, Parse would read from a file. Here it might just populate from args.
AppManifest* AppManifest_Create(const char* app_id, const char* app_name, const char* version, const char* entry_point, const char* icon_path);
void AppManifest_AddPermission(AppManifest* manifest, AppPermission permission);
void AppManifest_AddMetadata(AppManifest* manifest, const char* key, const char* value);
void AppManifest_Destroy(AppManifest* manifest);

// Helper to print manifest details (for debugging/logging)
void AppManifest_Print(const AppManifest* manifest);

#endif // APP_MANIFEST_H
