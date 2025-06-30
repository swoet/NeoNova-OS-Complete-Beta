#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

// --- Power States ---
// These represent high-level OS-managed power states.
// Actual hardware power states (ACPI states like S0, S3, S4, S5 on x86)
// would be managed internally by the implementation of these functions.
typedef enum {
    POWER_STATE_UNKNOWN,      // Initial or indeterminate state
    POWER_STATE_FULL_PERFORMANCE, // All components powered, maximum performance
    POWER_STATE_BALANCED,       // Performance balanced with power saving
    POWER_STATE_POWER_SAVER,    // Aggressive power saving, reduced performance/features
    POWER_STATE_SLEEP,          // Low-power sleep state (e.g., RAM powered, CPU low power)
    POWER_STATE_HIBERNATE,      // System state saved to disk, system powered off
    POWER_STATE_SHUTDOWN,       // System fully powered off
    POWER_STATE_REBOOTING       // System is in the process of rebooting
} PowerState;

// --- Power Events ---
// Events that the Power Manager can notify registered components about.
typedef enum {
    POWER_EVENT_NONE,
    POWER_EVENT_BATTERY_LOW,         // Battery level has dropped below a critical threshold
    POWER_EVENT_BATTERY_CRITICAL,    // Battery level is critically low, shutdown imminent
    POWER_EVENT_BATTERY_CHARGING,    // Battery has started charging
    POWER_EVENT_BATTERY_DISCHARGING, // Battery has started discharging (on battery power)
    POWER_EVENT_BATTERY_FULLY_CHARGED, // Battery has reached full charge
    POWER_EVENT_AC_POWER_CONNECTED,  // External AC power connected
    POWER_EVENT_AC_POWER_DISCONNECTED, // External AC power disconnected
    POWER_EVENT_ENTERING_SLEEP,      // System is about to enter a sleep state
    POWER_EVENT_RESUMING_FROM_SLEEP, // System is resuming from a sleep state
    POWER_EVENT_THERMAL_WARNING,     // System temperature is high
    POWER_EVENT_THERMAL_CRITICAL,    // System temperature is critical, performance may be throttled or shutdown may occur
    POWER_EVENT_POWER_STATE_CHANGED  // Generic event indicating a change in PowerState
} PowerEvent;

// Callback function type for power event notifications
// The callback might receive additional context or data depending on the event.
typedef void (*PowerEventCallback)(PowerEvent event, void* user_data);


// --- Power Manager API Signatures ---

// Initialize the Power Manager
// Returns 0 on success, -1 on failure.
int PowerManager_Initialize();

// Shutdown the Power Manager
void PowerManager_Shutdown();

// Request a change to a new power state.
// The transition might not be immediate or could be denied by policy/hardware.
// Returns 0 on success (request accepted), -1 on failure.
int PowerManager_RequestState(PowerState requested_state);

// Get the current power state of the system.
PowerState PowerManager_GetCurrentState();

// Get the current battery level as a percentage (0-100).
// Returns -1 if no battery is present or level cannot be determined.
int PowerManager_GetBatteryLevel();

// Check if the system is currently on AC power.
// Returns 1 if on AC power, 0 if on battery, -1 if status unknown.
int PowerManager_IsOnAcPower();

// Register a callback function to receive power event notifications.
// `user_data` is passed back to the callback when an event occurs.
// Returns a handle for unregistration, or NULL on failure.
void* PowerManager_RegisterForPowerEvents(PowerEventCallback callback, void* user_data);

// Unregister a previously registered power event callback.
void PowerManager_UnregisterForPowerEvents(void* registration_handle);

// (Conceptual) Functions for intelligent throttling & boosting
// These would likely be managed internally but could have high-level controls.

// Set a general power profile preference (influences automatic state transitions).
// typedef enum { POWER_PROFILE_MAX_PERFORMANCE, POWER_PROFILE_BALANCED, POWER_PROFILE_MAX_SAVINGS } PowerProfile;
// int PowerManager_SetPowerProfile(PowerProfile profile);
// PowerProfile PowerManager_GetPowerProfile();

// Request a temporary performance boost (e.g., for a demanding task).
// Duration_ms might be a hint; actual boost duration depends on system conditions (thermal, power).
// int PowerManager_RequestPerformanceBoost(int duration_ms);

// Check if a specific power feature is supported/enabled (e.g., hibernation, specific sleep states).
// bool PowerManager_IsFeatureSupported(const char* feature_name);


#endif // POWER_MANAGER_H
