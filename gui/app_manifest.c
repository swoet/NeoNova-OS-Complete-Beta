#include "app_manifest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function for string duplication
static char* StrDup_Safe(const char* s) {
    if (!s) return NULL;
    char* dup = strdup(s);
    if (!dup) {
        fprintf(stderr, "Error: strdup failed. Out of memory?\n");
        // In a real OS, this might trigger more robust error handling
    }
    return dup;
}

AppManifest* AppManifest_Create(const char* app_id, const char* app_name, const char* version, const char* entry_point, const char* icon_path) {
    AppManifest* manifest = (AppManifest*)malloc(sizeof(AppManifest));
    if (!manifest) {
        fprintf(stderr, "Error: Failed to allocate memory for AppManifest.\n");
        return NULL;
    }

    manifest->app_id = StrDup_Safe(app_id);
    manifest->app_name = StrDup_Safe(app_name);
    manifest->version = StrDup_Safe(version);
    manifest->entry_point = StrDup_Safe(entry_point);
    manifest->icon_path = StrDup_Safe(icon_path);

    manifest->permission_count = 0;
    manifest->metadata_count = 0;

    // Check if any essential duplications failed
    if ((app_id && !manifest->app_id) ||
        (app_name && !manifest->app_name) ||
        (version && !manifest->version) ||
        (entry_point && !manifest->entry_point) ||
        (icon_path && !manifest->icon_path)) {
        AppManifest_Destroy(manifest); // Clean up partially created manifest
        return NULL;
    }

    return manifest;
}

void AppManifest_AddPermission(AppManifest* manifest, AppPermission permission) {
    if (!manifest) return;
    if (manifest->permission_count < MAX_PERMISSIONS) {
        manifest->required_permissions[manifest->permission_count++] = permission;
    } else {
        fprintf(stderr, "Warning: Max permissions reached for app %s. Cannot add more.\n", manifest->app_id ? manifest->app_id : "UNKNOWN");
    }
}

void AppManifest_AddMetadata(AppManifest* manifest, const char* key, const char* value) {
    if (!manifest) return;
    if (manifest->metadata_count < MAX_METADATA_KEY_VALUE_PAIRS) {
        manifest->metadata[manifest->metadata_count].key = StrDup_Safe(key);
        manifest->metadata[manifest->metadata_count].value = StrDup_Safe(value);

        if ((key && !manifest->metadata[manifest->metadata_count].key) ||
            (value && !manifest->metadata[manifest->metadata_count].value)) {
            // strdup failed for key or value
            if (manifest->metadata[manifest->metadata_count].key) {
                free(manifest->metadata[manifest->metadata_count].key);
                manifest->metadata[manifest->metadata_count].key = NULL;
            }
            if (manifest->metadata[manifest->metadata_count].value) {
                free(manifest->metadata[manifest->metadata_count].value);
                manifest->metadata[manifest->metadata_count].value = NULL;
            }
            fprintf(stderr, "Warning: Failed to add metadata entry for app %s due to allocation failure.\n", manifest->app_id ? manifest->app_id : "UNKNOWN");
            return; // Don't increment count
        }
        manifest->metadata_count++;
    } else {
        fprintf(stderr, "Warning: Max metadata entries reached for app %s. Cannot add more.\n", manifest->app_id ? manifest->app_id : "UNKNOWN");
    }
}

void AppManifest_Destroy(AppManifest* manifest) {
    if (!manifest) return;

    if (manifest->app_id) free(manifest->app_id);
    if (manifest->app_name) free(manifest->app_name);
    if (manifest->version) free(manifest->version);
    if (manifest->entry_point) free(manifest->entry_point);
    if (manifest->icon_path) free(manifest->icon_path);

    for (int i = 0; i < manifest->metadata_count; ++i) {
        if (manifest->metadata[i].key) free(manifest->metadata[i].key);
        if (manifest->metadata[i].value) free(manifest->metadata[i].value);
    }

    free(manifest);
}

void AppManifest_Print(const AppManifest* manifest) {
    if (!manifest) {
        printf("AppManifest: (null)\n");
        return;
    }
    printf("AppManifest for: %s (%s)\n", manifest->app_name ? manifest->app_name : "N/A", manifest->app_id ? manifest->app_id : "N/A");
    printf("  Version: %s\n", manifest->version ? manifest->version : "N/A");
    printf("  Entry Point: %s\n", manifest->entry_point ? manifest->entry_point : "N/A");
    printf("  Icon Path: %s\n", manifest->icon_path ? manifest->icon_path : "N/A");

    printf("  Permissions (%d):\n", manifest->permission_count);
    for (int i = 0; i < manifest->permission_count; ++i) {
        // In a real system, you'd convert enum to string
        printf("    - Permission Code: %d\n", manifest->required_permissions[i]);
    }

    printf("  Metadata (%d):\n", manifest->metadata_count);
    for (int i = 0; i < manifest->metadata_count; ++i) {
        printf("    - %s: %s\n",
               manifest->metadata[i].key ? manifest->metadata[i].key : "N/A",
               manifest->metadata[i].value ? manifest->metadata[i].value : "N/A");
    }
    printf("------------------------------------\n");
}
