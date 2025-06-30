#include "cli.h"
#include "app_manager.h" // For list_apps command
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> // For variadic functions

#define MAX_REGISTERED_COMMANDS 50
static CLI_Command G_command_registry[MAX_REGISTERED_COMMANDS];
static int G_command_count = 0;

// --- Standard Output/Error Functions ---
void CLI_DisplayOutput(const char* format, ...) {
    char buffer[MAX_OUTPUT_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, MAX_OUTPUT_LENGTH, format, args);
    va_end(args);
    // In a real OS, this would print to the active console/terminal widget
    printf("CLI: %s\n", buffer);
}

void CLI_DisplayError(const char* format, ...) {
    char buffer[MAX_OUTPUT_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, MAX_OUTPUT_LENGTH, format, args);
    va_end(args);
    // Would print to stderr or a special error console area
    fprintf(stderr, "CLI Error: %s\n", buffer);
}


// Helper to convert AppPermission enum to string
const char* AppPermissionToString(AppPermission perm) {
    switch (perm) {
        case PERMISSION_NONE: return "None";
        case PERMISSION_FILESYSTEM_READ: return "Filesystem Read";
        case PERMISSION_FILESYSTEM_WRITE: return "Filesystem Write";
        case PERMISSION_NETWORK_ACCESS: return "Network Access";
        case PERMISSION_DEVICE_CAMERA: return "Device Camera";
        case PERMISSION_DEVICE_MICROPHONE: return "Device Microphone";
        default: return "Unknown Permission";
    }
}

// --- Command Handlers ---
int cmd_help_handler(int argc, char* argv[]) {
    CLI_DisplayOutput("Available commands:");
    for (int i = 0; i < G_command_count; ++i) {
        CLI_DisplayOutput("  %s - %s", G_command_registry[i].name, G_command_registry[i].description ? G_command_registry[i].description : "No description");
    }
    return 0;
}

int cmd_echo_handler(int argc, char* argv[]) {
    if (argc < 2) {
        CLI_DisplayOutput(""); // Echo nothing if no args
        return 0;
    }
    // Concatenate all arguments after "echo"
    char echo_buffer[MAX_COMMAND_LENGTH] = {0};
    for (int i = 1; i < argc; ++i) {
        strncat(echo_buffer, argv[i], MAX_COMMAND_LENGTH - strlen(echo_buffer) - 1);
        if (i < argc - 1) {
            strncat(echo_buffer, " ", MAX_COMMAND_LENGTH - strlen(echo_buffer) - 1);
        }
    }
    CLI_DisplayOutput("%s", echo_buffer);
    return 0;
}

int cmd_list_apps_handler(int argc, char* argv[]) {
    AppInstance* loaded_apps[MAX_LOADED_APPS];
    int app_count = AppManager_GetLoadedApps(loaded_apps, MAX_LOADED_APPS);

    if (app_count == 0) {
        CLI_DisplayOutput("No applications loaded.");
        return 0;
    }

    CLI_DisplayOutput("Loaded Applications (%d):", app_count);
    CLI_DisplayOutput("  ID                               | Name             | Version");
    CLI_DisplayOutput("-----------------------------------|------------------|----------");
    for (int i = 0; i < app_count; ++i) {
        const AppManifest* manifest = AppManager_GetManifest(loaded_apps[i]);
        if (manifest) {
            CLI_DisplayOutput("  %-32.32s | %-16.16s | %s",
                manifest->app_id ? manifest->app_id : "N/A",
                manifest->app_name ? manifest->app_name : "N/A",
                manifest->version ? manifest->version : "N/A");
        }
    }
    return 0;
}

int cmd_start_app_handler(int argc, char* argv[]) {
    if (argc < 2) {
        CLI_DisplayError("Usage: start_app <app_id>");
        return 1;
    }
    const char* app_id_to_start = argv[1];
    AppInstance* app_to_start = AppManager_FindAppByID(app_id_to_start);

    if (app_to_start) {
        if (AppManager_StartApp(app_to_start) == 0) {
            CLI_DisplayOutput("App '%s' started.", app_id_to_start);
        } else {
            CLI_DisplayError("Failed to start app '%s'. It might be already running or in an error state.", app_id_to_start);
        }
    } else {
        CLI_DisplayError("App with ID '%s' not found.", app_id_to_start);
    }
    return 0;
}

int cmd_stop_app_handler(int argc, char* argv[]) {
    if (argc < 2) {
        CLI_DisplayError("Usage: stop_app <app_id>");
        return 1;
    }
    const char* app_id_to_stop = argv[1];
    AppInstance* app_to_stop = AppManager_FindAppByID(app_id_to_stop);

    if (app_to_stop) {
        // AppManager_StopApp is designed to be idempotent (safe to call on already stopped app)
        if (AppManager_StopApp(app_to_stop) == 0) {
            CLI_DisplayOutput("App '%s' stopped (or was not running).", app_id_to_stop);
        } else {
            // This path might not be easily reachable if StopApp is simple
            CLI_DisplayError("Failed to issue stop for app '%s'.", app_id_to_stop);
        }
    } else {
        CLI_DisplayError("App with ID '%s' not found.", app_id_to_stop);
    }
    return 0;
}

int cmd_app_info_handler(int argc, char* argv[]) {
    if (argc < 2) {
        CLI_DisplayError("Usage: app_info <app_id>");
        return 1;
    }
    const char* app_id_to_info = argv[1];
    AppInstance* app_to_info = AppManager_FindAppByID(app_id_to_info);

    if (app_to_info) {
        const AppManifest* manifest = AppManager_GetManifest(app_to_info);
        if (manifest) {
            CLI_DisplayOutput("App Information:");
            CLI_DisplayOutput("  ID: %s", manifest->app_id ? manifest->app_id : "N/A");
            CLI_DisplayOutput("  Name: %s", manifest->app_name ? manifest->app_name : "N/A");
            CLI_DisplayOutput("  Version: %s", manifest->version ? manifest->version : "N/A");
            CLI_DisplayOutput("  Entry Point: %s", manifest->entry_point ? manifest->entry_point : "N/A");
            CLI_DisplayOutput("  Icon Path: %s", manifest->icon_path ? manifest->icon_path : "N/A");

            CLI_DisplayOutput("  Permissions (%d):", manifest->permission_count);
            if (manifest->permission_count > 0) {
                for (int i = 0; i < manifest->permission_count; ++i) {
                    CLI_DisplayOutput("    - %s (Code: %d)",
                                      AppPermissionToString(manifest->required_permissions[i]),
                                      manifest->required_permissions[i]);
                }
            } else {
                CLI_DisplayOutput("    (No specific permissions requested)");
            }

            CLI_DisplayOutput("  Metadata (%d):", manifest->metadata_count);
            if (manifest->metadata_count > 0) {
                for (int i = 0; i < manifest->metadata_count; ++i) {
                    CLI_DisplayOutput("    - %s: %s",
                                   manifest->metadata[i].key ? manifest->metadata[i].key : "N/A",
                                   manifest->metadata[i].value ? manifest->metadata[i].value : "N/A");
                }
            } else {
                CLI_DisplayOutput("    (No metadata)");
            }
        } else {
            CLI_DisplayError("Could not retrieve manifest for app '%s'.", app_id_to_info);
        }
    } else {
        CLI_DisplayError("App with ID '%s' not found.", app_id_to_info);
    }
    return 0;
}


// --- CLI System Functions ---
void CLI_Initialize() {
    G_command_count = 0;
    memset(G_command_registry, 0, sizeof(G_command_registry));

    CLI_Command help_cmd = {"help", "Show this help message.", cmd_help_handler};
    CLI_RegisterCommand(help_cmd);

    CLI_Command echo_cmd = {"echo", "Display a line of text.", cmd_echo_handler};
    CLI_RegisterCommand(echo_cmd);

    CLI_Command list_apps_cmd = {"list_apps", "List all loaded applications.", cmd_list_apps_handler};
    CLI_RegisterCommand(list_apps_cmd);

    CLI_Command start_app_cmd = {"start_app", "Start an application by its ID.", cmd_start_app_handler};
    CLI_RegisterCommand(start_app_cmd);

    CLI_Command stop_app_cmd = {"stop_app", "Stop a running application by its ID.", cmd_stop_app_handler};
    CLI_RegisterCommand(stop_app_cmd);

    CLI_Command app_info_cmd = {"app_info", "Display detailed information about an application by its ID.", cmd_app_info_handler};
    CLI_RegisterCommand(app_info_cmd);

    CLI_DisplayOutput("CLI Initialized. Type 'help' for commands.");
}

int CLI_RegisterCommand(CLI_Command command) {
    if (G_command_count >= MAX_REGISTERED_COMMANDS) {
        CLI_DisplayError("Cannot register command '%s': Command registry full.", command.name);
        return -1;
    }
    // Check for duplicates (optional, but good practice)
    for (int i = 0; i < G_command_count; ++i) {
        if (strcmp(G_command_registry[i].name, command.name) == 0) {
            CLI_DisplayError("Cannot register command '%s': Already exists.", command.name);
            return -1;
        }
    }

    G_command_registry[G_command_count++] = command;
    return 0;
}

void CLI_ProcessInput(const char* input_string) {
    if (!input_string || strlen(input_string) == 0) {
        return;
    }

    char input_copy[MAX_COMMAND_LENGTH];
    strncpy(input_copy, input_string, MAX_COMMAND_LENGTH -1);
    input_copy[MAX_COMMAND_LENGTH-1] = '\0'; // Ensure null termination

    char* argv[MAX_COMMAND_ARGS];
    int argc = 0;

    char* token = strtok(input_copy, " \t\n\r");
    while (token != NULL && argc < MAX_COMMAND_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n\r");
    }

    if (argc == 0) {
        return; // No command entered
    }

    // Find and execute the command
    for (int i = 0; i < G_command_count; ++i) {
        if (strcmp(G_command_registry[i].name, argv[0]) == 0) {
            if (G_command_registry[i].handler) {
                G_command_registry[i].handler(argc, argv);
            } else {
                CLI_DisplayError("Command '%s' has no handler.", argv[0]);
            }
            return;
        }
    }
    CLI_DisplayError("Command not found: %s. Type 'help'.", argv[0]);
}
