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

// Helper to convert permission string to AppPermission enum
static AppPermission StringToAppPermission(const char* perm_str) {
    if (!perm_str) return PERMISSION_NONE;
    if (strcmp(perm_str, "FS_READ") == 0) return PERMISSION_FILESYSTEM_READ;
    if (strcmp(perm_str, "FS_WRITE") == 0) return PERMISSION_FILESYSTEM_WRITE;
    if (strcmp(perm_str, "NET") == 0) return PERMISSION_NETWORK_ACCESS;
    if (strcmp(perm_str, "CAM") == 0) return PERMISSION_DEVICE_CAMERA;
    if (strcmp(perm_str, "MIC") == 0) return PERMISSION_DEVICE_MICROPHONE;
    // Add other permission strings here
    fprintf(stderr, "Warning: Unknown permission string '%s'\n", perm_str);
    return PERMISSION_NONE;
}

AppManifest* AppManifest_ParseFromString(const char* manifest_string) {
    if (!manifest_string) return NULL;

    char* str_copy = StrDup_Safe(manifest_string); // Work on a copy
    if (!str_copy) return NULL;

    // Temp variables to hold parsed values before creating the manifest
    char* temp_id = NULL;
    char* temp_name = NULL;
    char* temp_version = NULL;
    char* temp_entry = NULL;
    char* temp_icon = NULL;

    AppPermission temp_perms[MAX_PERMISSIONS];
    int temp_perm_count = 0;

    MetadataEntry temp_meta[MAX_METADATA_KEY_VALUE_PAIRS];
    int temp_meta_count = 0;

    char* token = strtok(str_copy, ";"); // Tokenize by semicolon for key-value pairs
    while (token != NULL) {
        char* value = strchr(token, ':');
        if (value) {
            *value = '\0'; // Null-terminate the key
            value++;       // Move pointer to the start of the value

            if (strcmp(token, "id") == 0) temp_id = value;
            else if (strcmp(token, "name") == 0) temp_name = value;
            else if (strcmp(token, "version") == 0) temp_version = value;
            else if (strcmp(token, "entry") == 0) temp_entry = value;
            else if (strcmp(token, "icon") == 0) temp_icon = value;
            else if (strcmp(token, "perms") == 0) {
                // Parse comma-separated permissions
                char* perm_token = strtok(value, ",");
                while (perm_token != NULL && temp_perm_count < MAX_PERMISSIONS) {
                    AppPermission p = StringToAppPermission(perm_token);
                    if (p != PERMISSION_NONE) { // Only add valid known permissions
                        temp_perms[temp_perm_count++] = p;
                    }
                    perm_token = strtok(NULL, ",");
                }
            } else if (strcmp(token, "meta") == 0) {
                // Parse comma-separated metadata key=value pairs
                char* meta_pair_token = strtok(value, ",");
                while (meta_pair_token != NULL && temp_meta_count < MAX_METADATA_KEY_VALUE_PAIRS) {
                    char* meta_value = strchr(meta_pair_token, '=');
                    if (meta_value) {
                        *meta_value = '\0'; // Null-terminate meta key
                        meta_value++;       // Move to meta value
                        temp_meta[temp_meta_count].key = StrDup_Safe(meta_pair_token); // strdup needed as original string is modified
                        temp_meta[temp_meta_count].value = StrDup_Safe(meta_value);
                        if (temp_meta[temp_meta_count].key && temp_meta[temp_meta_count].value) {
                           temp_meta_count++;
                        } else { // strdup failed for key or value
                            if(temp_meta[temp_meta_count].key) free(temp_meta[temp_meta_count].key);
                            if(temp_meta[temp_meta_count].value) free(temp_meta[temp_meta_count].value);
                        }
                    }
                    meta_pair_token = strtok(NULL, ",");
                }
            }
        }
        token = strtok(NULL, ";");
    }

    // Basic validation: check if essential fields were found
    if (!temp_id || !temp_name || !temp_entry) {
        fprintf(stderr, "AppManifest_ParseFromString: Error - essential fields (id, name, entry) not found in string.\n");
        // Free any strduped metadata parts before returning NULL
        for(int i=0; i < temp_meta_count; ++i) {
            if(temp_meta[i].key) free(temp_meta[i].key);
            if(temp_meta[i].value) free(temp_meta[i].value);
        }
        free(str_copy);
        return NULL;
    }

    // Create the manifest using the parsed (but still pointing into str_copy for non-meta) values
    AppManifest* manifest = AppManifest_Create(temp_id, temp_name, temp_version, temp_entry, temp_icon);
    if (!manifest) {
        for(int i=0; i < temp_meta_count; ++i) { // Free any strduped metadata if manifest creation fails
            if(temp_meta[i].key) free(temp_meta[i].key);
            if(temp_meta[i].value) free(temp_meta[i].value);
        }
        free(str_copy);
        return NULL;
    }

    // Copy permissions
    for (int i = 0; i < temp_perm_count; ++i) {
        AppManifest_AddPermission(manifest, temp_perms[i]);
    }

    // Copy metadata (which were already strduped)
    for (int i = 0; i < temp_meta_count; ++i) {
        // AppManifest_AddMetadata will strdup again if we pass temp_meta[i].key, temp_meta[i].value
        // So we directly assign the already duplicated strings and manage their lifetime with AppManifest_Destroy
        if (manifest->metadata_count < MAX_METADATA_KEY_VALUE_PAIRS) {
            manifest->metadata[manifest->metadata_count].key = temp_meta[i].key; // Transfer ownership
            manifest->metadata[manifest->metadata_count].value = temp_meta[i].value; // Transfer ownership
            manifest->metadata_count++;
        } else { // Should not happen if MAX_METADATA_KEY_VALUE_PAIRS is consistent
            free(temp_meta[i].key); // Free if cannot be added
            free(temp_meta[i].value);
        }
    }

    free(str_copy); // Free the initial duplicated manifest string
    return manifest;
}
