#ifndef SANDBOX_H
#define SANDBOX_H

#include "app_manifest.h" // For AppManifest, as policies might be derived from it.
#include <stdbool.h>    // For bool type

// Conceptual Process ID type. In a real OS, this would be a more specific type.
typedef int ProcessID;

// --- Resource Types for Sandboxing ---
// Defines the types of resources or actions that can be controlled by a sandbox rule.
typedef enum {
    RESOURCE_TYPE_UNKNOWN,
    RESOURCE_TYPE_FILESYSTEM_READ,    // Access to read a file/directory path
    RESOURCE_TYPE_FILESYSTEM_WRITE,   // Access to write/modify a file/directory path
    RESOURCE_TYPE_FILESYSTEM_EXECUTE, // Access to execute a file
    // RESOURCE_TYPE_FILESYSTEM_METADATA_READ, // Access to read file metadata
    // RESOURCE_TYPE_FILESYSTEM_METADATA_WRITE, // Access to change file metadata (owner, perms)

    RESOURCE_TYPE_NETWORK_SOCKET_CREATE, // Permission to create a network socket
    RESOURCE_TYPE_NETWORK_CONNECT,    // Permission to connect to a specific address/port
    RESOURCE_TYPE_NETWORK_LISTEN,     // Permission to listen on a specific port
    // RESOURCE_TYPE_NETWORK_BIND,       // Permission to bind to a specific local address/port

    RESOURCE_TYPE_SYSTEM_CALL,        // Control over specific system calls (e.g., by syscall number or name)
    RESOURCE_TYPE_DEVICE_ACCESS,      // Access to a specific hardware device (e.g., "/dev/camera0")
    RESOURCE_TYPE_PROCESS_INTERACTION,// e.g., ptrace, send signal to other processes
    RESOURCE_TYPE_IPC,                // Inter-Process Communication (e.g., specific named pipes, shared memory)

    // Higher-level abstract permissions that might map to multiple low-level resources
    RESOURCE_TYPE_CAMERA_ACCESS,
    RESOURCE_TYPE_MICROPHONE_ACCESS,
    RESOURCE_TYPE_LOCATION_ACCESS,

    // Add more resource types as needed
} ResourceType;

// --- Sandbox Rule Actions ---
typedef enum {
    SANDBOX_ACTION_ALLOW,
    SANDBOX_ACTION_DENY,
    SANDBOX_ACTION_LOG,          // Allow the action but log it
    SANDBOX_ACTION_PROMPT_USER   // (Conceptual) Pause and ask the user for permission
} SandboxAction;

// --- Sandbox Rule ---
// A single rule in a sandbox policy.
typedef struct {
    ResourceType type;      // The type of resource or action this rule applies to
    SandboxAction action;   // What to do if this rule matches
    char resource_identifier[256]; // Specific identifier for the resource
                                   // e.g., file path pattern, network address "ip:port", syscall name/number, device name.
                                   // Wildcards like "*" or path prefixes might be supported.
    // uint32_t flags; // Additional flags for the rule, e.g., case-insensitivity for paths
} SandboxRule;

#define MAX_SANDBOX_RULES 100

// --- Sandbox Policy ---
// A collection of sandbox rules that define the boundaries for a process.
typedef struct {
    char policy_name[128]; // e.g., "WebApp_StandardPolicy", "SystemDaemon_PrivilegedPolicy"
    SandboxRule rules[MAX_SANDBOX_RULES];
    int rule_count;
    SandboxAction default_action_filesystem; // Default if no filesystem rule matches
    SandboxAction default_action_network;    // Default if no network rule matches
    SandboxAction default_action_generic;    // Default for other types
} SandboxPolicy;


// --- Sandbox Manager API Signatures ---

// Initialize the Sandbox Manager system
int SandboxManager_Initialize();
void SandboxManager_Shutdown();

// Create or load a sandbox policy.
// This might load from a predefined template or a configuration file.
// For this conceptual API, we can imagine creating one programmatically.
SandboxPolicy* SandboxPolicy_Create(const char* policy_name);
void SandboxPolicy_Destroy(SandboxPolicy* policy);
int SandboxPolicy_AddRule(SandboxPolicy* policy, SandboxRule rule);
// void SandboxPolicy_SetDefaultAction(SandboxPolicy* policy, ResourceType general_type, SandboxAction action);

// Load a sandbox policy tailored for a specific application, possibly based on its manifest.
// This function would interpret app_manifest->required_permissions and other metadata
// to construct or select an appropriate SandboxPolicy.
SandboxPolicy* SandboxManager_GetPolicyForApp(const AppManifest* manifest);

// Apply a sandbox policy to a given process (identified by ProcessID).
// This is a highly conceptual function. In a real OS, this involves deep kernel integration.
// Returns 0 on success, -1 on failure.
int SandboxManager_ApplyPolicyToProcess(ProcessID pid, const SandboxPolicy* policy);

// Remove or detach a sandbox policy from a process.
// int SandboxManager_RemovePolicyFromProcess(ProcessID pid);

// Check if a specific action on a resource is allowed for a given process under its current sandbox.
// This would be called by kernel hooks or system call wrappers when a process attempts an action.
// Returns true if allowed, false if denied.
bool SandboxManager_IsAccessAllowed(ProcessID pid, ResourceType type, const char* resource_identifier);

// Log a sandbox event (e.g., a denied action or a logged action).
// void SandboxManager_LogEvent(ProcessID pid, const SandboxRule* matched_rule, const char* resource_identifier, bool was_denied);

#endif // SANDBOX_H
