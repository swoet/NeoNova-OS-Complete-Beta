#ifndef DRIVER_H
#define DRIVER_H

#include "device.h" // For Device and DeviceType (which includes Driver fwd decl)
#include <stddef.h> // For size_t
#include <stdint.h> // For int types

// Forward declare Device again, just in case (harmless)
struct Device;

// --- Driver Interface Structure ---
// This structure defines a standard set of functions that all device drivers must implement.
// Specific driver types (e.g., block driver, network driver) might extend this
// with additional function pointers for type-specific operations.
typedef struct Driver {
    const char* name;        // Driver name, e.g., "e1000_net_driver", "nvme_block_driver"
    const char* version;     // Driver version string
    const char* description; // Brief description of the driver

    // Type of device this driver primarily supports.
    // A driver might support multiple specific models under a general type.
    DeviceType supported_device_type;

    // Probe function:
    // Called by the DriverManager to see if this driver can handle the given device.
    // `device_hw_info` is a pointer to the raw hardware information (e.g., PCI config).
    // Returns a score (higher is better match) or 0 if not supported.
    // A positive score indicates the driver *believes* it can handle the device.
    int (*probe)(const struct Device* device_template); // device_template contains hw_specific_info

    // Attach function:
    // Called if probe was successful and this driver is chosen for the device.
    // The driver should initialize the hardware, allocate resources, and make the device operational.
    // The `dev` pointer is the system's Device structure for this hardware.
    // Returns 0 on success, negative error code on failure.
    int (*attach)(struct Device* dev);

    // Detach function:
    // Called when the driver is being unloaded or the device is removed.
    // The driver should release resources, reset hardware to a safe state.
    void (*detach)(struct Device* dev);

    // Shutdown function (for system shutdown/reboot):
    // Prepare the device for system power-off.
    // void (*shutdown)(struct Device* dev);

    // --- Common Device Operations (can be NULL if not applicable) ---
    // These are generic; specific device types would have more tailored functions
    // (e.g., network_send_packet, block_read_sector).

    // Read data from the device.
    // `offset` is device-specific (e.g., LBA for block, register for some).
    // `size` is number of bytes to read.
    // `buffer` is where data is stored.
    // Returns bytes read or negative error.
    // ssize_t (*read)(struct Device* dev, uint64_t offset, void* buffer, size_t size);

    // Write data to the device.
    // Returns bytes written or negative error.
    // ssize_t (*write)(struct Device* dev, uint64_t offset, const void* buffer, size_t size);

    // Device control operations (similar to ioctl).
    // `command` is a driver-specific command code.
    // `arg` is an argument for the command (can be a pointer).
    // Returns 0 on success, negative error code.
    // int (*ioctl)(struct Device* dev, int command, void* arg);

    // Power management functions for the device
    // int (*suspend)(struct Device* dev); // Device specific suspend
    // int (*resume)(struct Device* dev);  // Device specific resume

    // Pointer to driver-specific private data (allocated by driver during attach)
    // void* private_data;

    // List of specific hardware IDs this driver supports (e.g., array of PCI VID/PID pairs)
    // const HardwareID* supported_ids;
    // int num_supported_ids;

    // Next driver in a global list (for DriverManager internal use)
    // struct Driver* next;
} Driver;

#endif // DRIVER_H
