#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h> // For standard integer types
#include <stddef.h> // For size_t

// Forward declaration of Driver struct
struct Driver;

// --- Device Types (Conceptual and Expandable) ---
// This enum helps categorize devices and match them with suitable drivers.
typedef enum {
    DEVICE_TYPE_UNKNOWN,
    DEVICE_TYPE_SYSTEM_BUS,   // e.g., PCI, USB root hub
    DEVICE_TYPE_PROCESSOR,    // CPU core
    DEVICE_TYPE_MEMORY_CONTROLLER,
    DEVICE_TYPE_STORAGE,      // HDD, SSD, NVMe
    DEVICE_TYPE_NETWORK,      // Ethernet, Wi-Fi
    DEVICE_TYPE_GRAPHICS,     // GPU
    DEVICE_TYPE_AUDIO,
    DEVICE_TYPE_INPUT,        // Keyboard, Mouse, Touchscreen
    DEVICE_TYPE_SERIAL_PORT,
    DEVICE_TYPE_USB_DEVICE,   // Generic USB device (could be further specialized)
    DEVICE_TYPE_PLATFORM      // Other platform-specific devices
    // ... add more specific types as needed
} DeviceType;

// --- Device Status ---
typedef enum {
    DEVICE_STATUS_UNKNOWN,      // Initial state
    DEVICE_STATUS_DETECTED,     // Hardware detected, no driver attached
    DEVICE_STATUS_DRIVER_FOUND, // A suitable driver has been identified
    DEVICE_STATUS_ATTACHING,    // Driver is in the process of attaching
    DEVICE_STATUS_ATTACHED,     // Driver successfully attached and device is operational
    DEVICE_STATUS_DETACHED,     // Driver detached or device unplugged
    DEVICE_STATUS_ERROR,        // Device is in an error state
    DEVICE_STATUS_DISABLED      // Device is disabled by user or system policy
} DeviceStatus;

// Generic Device Structure
// Represents a piece of hardware in the system.
typedef struct Device {
    char name[128];             // Unique system name, e.g., "eth0", "sda", "gpu0", "/dev/ttyS0"
    char description[256];      // Human-readable description, e.g., "Intel Gigabit Ethernet"

    DeviceType type;            // General type of the device
    DeviceStatus status;        // Current status of the device

    uintptr_t unique_hw_id;     // A unique hardware identifier (e.g., hash of PCI BDF, USB VID/PID+Serial)
    struct Device* parent_bus;  // Device this device is connected to (e.g., a PCI device on a PCI bridge)
    // List of child devices (e.g., USB devices connected to a USB hub)
    // struct Device* children[MAX_CHILD_DEVICES];
    // int child_count;

    // Hardware-specific information (e.g., PCI config space, USB descriptors)
    // This would be a union or a void* pointer to type-specific data.
    void* hw_specific_info;

    // Pointer to the driver currently attached to this device (if any).
    struct Driver* driver;

    // Resources allocated to this device (e.g., I/O ports, memory regions, IRQ lines)
    // struct ResourceList* resources;

    // Power management state for the device
    // DevicePowerState power_state;

    // Reference count for resource management
    // int ref_count;

} Device;

// --- Device API (Conceptual - these might be part of DeviceManager or internal helpers) ---

// Create a new device structure (typically called by bus drivers or device detection logic).
// Device* Device_Create(DeviceType type, const char* name_hint, void* hw_specific_info);
// void Device_Destroy(Device* dev);

// Set the status of a device.
// void Device_SetStatus(Device* dev, DeviceStatus status);

// Add a hardware resource used by the device (e.g., memory mapped I/O range).
// int Device_AddResource(Device* dev, ResourceType type, uintptr_t start, size_t length, uint32_t flags);

#endif // DEVICE_H
