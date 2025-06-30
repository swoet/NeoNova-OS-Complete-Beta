#ifndef KERNEL_MODULE_H
#define KERNEL_MODULE_H

// Forward declaration for configuration data type (if needed)
// typedef struct ModuleConfig ModuleConfig;

// Return status for module operations
typedef enum {
    MODULE_STATUS_OK,
    MODULE_STATUS_ERROR,
    MODULE_STATUS_NOT_IMPLEMENTED,
    MODULE_STATUS_ALREADY_INITIALIZED,
    MODULE_STATUS_NOT_INITIALIZED,
    MODULE_STATUS_DEPENDENCY_MISSING
} ModuleStatus;

// Generic Kernel Module Structure
typedef struct KernelModule {
    const char* name;         // Unique name of the module
    const char* description;  // Brief description of the module's purpose

    // Initialization function for the module.
    // Called once during kernel startup.
    // Should return MODULE_STATUS_OK on success.
    ModuleStatus (*init)(void);

    // Shutdown function for the module.
    // Called once during kernel shutdown.
    // Should return MODULE_STATUS_OK on success.
    ModuleStatus (*shutdown)(void);

    // (Conceptual) Load configuration for the module.
    // void (*load_config)(const ModuleConfig* config);

    // (Conceptual) Get current status or health of the module.
    // ModuleStatus (*get_status)(void);

    // (Conceptual) Pointer to module-specific API, if any, to be exposed.
    // This would require casting and careful management.
    // const void* api_interface;

    // (Conceptual) List of dependencies (other module names)
    // const char** dependencies;
    // int dependency_count;

} KernelModule;


// --- API Signatures for Kernel Module Management ---

// Register a kernel module with the kernel's module manager.
// The kernel does not take ownership of the KernelModule pointer itself,
// so it should typically point to a statically allocated struct.
// Returns MODULE_STATUS_OK on success.
ModuleStatus Kernel_RegisterModule(KernelModule* module);

// Unregister a kernel module by its name.
// Returns MODULE_STATUS_OK on success.
ModuleStatus Kernel_UnregisterModule(const char* name);

// Initialize all registered kernel modules in an order that respects dependencies (conceptual).
// Returns MODULE_STATUS_OK if all modules initialize successfully.
ModuleStatus Kernel_InitializeAllModules();

// Shutdown all registered kernel modules in reverse order (conceptual).
// Returns MODULE_STATUS_OK if all modules shut down successfully.
ModuleStatus Kernel_ShutdownAllModules();

// Get a specific module by name (e.g., for inter-module communication or to access its API)
// KernelModule* Kernel_GetModule(const char* name);


#endif // KERNEL_MODULE_H
