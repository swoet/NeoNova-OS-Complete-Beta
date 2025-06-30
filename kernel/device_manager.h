#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "drivers/device.h" // For Device and Driver structures (will be created in next plan step)
                           // This creates a circular dependency if not careful.
                           // For now, assume these are forward-declarable or defined minimally.
                           // Let's forward declare for now to break strict build order.
struct Device;
struct Driver;

// --- Device Management API (Conceptual) ---

// Initialize the Device Manager.
// This might involve scanning hardware buses (PCI, USB) if not done by specialized bus drivers.
// void DeviceManager_Initialize();

// Register a device with the system.
// This might be called by bus drivers when they detect new hardware.
// `hw_info` would be specific data about the device (e.g., PCI vendor/device ID, USB VID/PID).
// struct Device* DeviceManager_RegisterDevice(void* hw_info, DeviceType type);

// Unregister a device (e.g., on hot-unplug).
// void DeviceManager_UnregisterDevice(struct Device* device);

// Find a driver that is suitable for a given device and attach it.
// This is a key part of the PnP (Plug and Play) process.
// int DeviceManager_AttachDriver(struct Device* device);

// Detach the current driver from a device.
// void DeviceManager_DetachDriver(struct Device* device);

// Get a list of all devices known to the system, optionally filtered by type.
// struct Device** DeviceManager_ListDevices(DeviceType filter_type, int* count);

// Get a device by a unique identifier (e.g., path like "/dev/sda", or a handle).
// struct Device* DeviceManager_GetDeviceById(const char* id_string);


// --- (Conceptual) Interaction with Driver Manager ---
// The DeviceManager would work closely with a DriverManager (defined in drivers/driver_manager.h)
// DriverManager_ScanForDrivers(); // To find all available drivers
// Driver* DriverManager_FindSuitableDriver(Device* device); // Ask DM for a driver


// --- Device Manager Module API (as a KernelModule) ---
// extern KernelModule DeviceManagerModule; // Defined in device_manager.c

#endif // DEVICE_MANAGER_H
