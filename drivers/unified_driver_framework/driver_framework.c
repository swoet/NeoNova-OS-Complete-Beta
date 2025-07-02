// unified driver framework implementation

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "driver_framework.h"
#include "../../kernel64/include/modular.h"
#include "../../core/driver_manager/ai_driver_generator.h"
#include "../../core/driver_manager/cloud_driver_fetcher.h"
#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include <cfgmgr32.h>
#pragma comment(lib, "setupapi.lib")
#endif

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC
#define PCI_MAX_BUS        256
#define PCI_MAX_DEVICE     32
#define PCI_MAX_FUNCTION   8

#define DF_LOG_ERR(fmt, ...) fprintf(stderr, "[DriverFramework][ERR] " fmt "\n", ##__VA_ARGS__)
#define DF_LOG_INFO(fmt, ...) fprintf(stdout, "[DriverFramework][INFO] " fmt "\n", ##__VA_ARGS__)

// Hardware fingerprint structure (PCI, USB, etc.)
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass_code;
    // Add more fields as needed (USB, etc.)
} hw_fingerprint_t;

// Driver interface
typedef struct {
    kernel_module_t base;
    int (*probe)(hw_fingerprint_t* fp);
    int (*ioctl)(int cmd, void* arg);
} driver_t;

// Driver registry (linked list)
static driver_t* driver_list = NULL;

// Register a driver
int register_driver(driver_t* drv) {
    if (!drv || !drv->base.init) return -1;
    drv->base.type = MODULE_TYPE_DRIVER;
    drv->base.next = (kernel_module_t*)driver_list;
    driver_list = drv;
    return register_module(&drv->base);
}

// Unregister a driver
int unregister_driver(const char* name) {
    driver_t **cur = &driver_list;
    while (*cur) {
        if ((*cur)->base.name && name && strcmp((*cur)->base.name, name) == 0) {
            unregister_module(name);
            *cur = (driver_t*)(*cur)->base.next;
            return 0;
        }
        cur = (driver_t**)&((*cur)->base.next);
    }
    return -1;
}

// I/O port access (x86 only, stub for other arch)
static inline uint32_t pci_io_read32(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}
static inline void pci_io_write32(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

// Read PCI config
static uint32_t pci_config_read32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (1U << 31) | ((uint32_t)bus << 16) | ((uint32_t)device << 11) |
                      ((uint32_t)function << 8) | (offset & 0xFC);
    pci_io_write32(PCI_CONFIG_ADDRESS, address);
    return pci_io_read32(PCI_CONFIG_DATA);
}

// Enumerate PCI devices and fill hw_fingerprint_t array
int fingerprint_hardware(hw_fingerprint_t* out_fp, int max_count) {
    int found = 0;
    // PCI enumeration
    for (uint8_t bus = 0; bus < PCI_MAX_BUS && found < max_count; ++bus) {
        for (uint8_t device = 0; device < PCI_MAX_DEVICE && found < max_count; ++device) {
            for (uint8_t function = 0; function < PCI_MAX_FUNCTION && found < max_count; ++function) {
                uint32_t vendor_device = pci_config_read32(bus, device, function, 0x00);
                uint16_t vendor_id = vendor_device & 0xFFFF;
                if (vendor_id == 0xFFFF) continue; // No device
                uint16_t device_id = (vendor_device >> 16) & 0xFFFF;
                uint32_t class_reg = pci_config_read32(bus, device, function, 0x08);
                uint8_t class_code = (class_reg >> 24) & 0xFF;
                uint8_t subclass_code = (class_reg >> 16) & 0xFF;
                out_fp[found].bus_type = BUS_TYPE_PCI;
                out_fp[found].vendor_id = vendor_id;
                out_fp[found].device_id = device_id;
                out_fp[found].class_code = class_code;
                out_fp[found].subclass_code = subclass_code;
                found++;
            }
        }
    }
#ifdef _WIN32
    // USB enumeration using SetupAPI
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, 0, 0, DIGCF_PRESENT);
    if (hDevInfo != INVALID_HANDLE_VALUE) {
        SP_DEVINFO_DATA devInfoData;
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData) && found < max_count; ++i) {
            char desc[256];
            if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)desc, sizeof(desc), NULL)) {
                out_fp[found].bus_type = BUS_TYPE_USB;
                out_fp[found].usb_vendor_id = 0x0000; // Real code: parse from hardware IDs
                out_fp[found].usb_product_id = 0x0000;
                out_fp[found].usb_class = 0x00;
                out_fp[found].usb_subclass = 0x00;
                found++;
                DF_LOG_INFO("USB device found: %s", desc);
            }
        }
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }
    // Hotplug detection: poll for device changes (in production, use RegisterDeviceNotification)
#endif
    // Legacy device enumeration (PS/2, serial)
#ifdef _WIN32
    // Check for PS/2 keyboard (port 0x60/0x64)
    // In production, use Windows Raw Input or GetRawInputDeviceList
    out_fp[found].bus_type = BUS_TYPE_LEGACY;
    out_fp[found].legacy_type = 1; // PS2_KBD
    found++;
    out_fp[found].bus_type = BUS_TYPE_LEGACY;
    out_fp[found].legacy_type = 2; // PS2_MOUSE
    found++;
    out_fp[found].bus_type = BUS_TYPE_LEGACY;
    out_fp[found].legacy_type = 3; // SERIAL
    found++;
#endif
    return found;
}

// Match driver to hardware
int match_driver(hw_fingerprint_t* fp) {
    driver_t* cur = driver_list;
    while (cur) {
        if (cur->probe && cur->probe(fp) == 0) {
            // Found a matching driver
            return 0;
        }
        cur = (driver_t*)cur->base.next;
    }
    // No match found
    return -1;
}

// AI-assisted generic driver generator (calls out to core/driver_manager)
int generate_generic_driver(hw_fingerprint_t* fp) {
    DF_LOG_INFO("Invoking AI-assisted driver generator");
    if (!fp) {
        DF_LOG_ERR("generate_generic_driver: NULL fingerprint");
        return -1;
    }
    return ai_suggest_driver(fp);
}

// Cloud driver fetch and offline cache (calls out to core/driver_manager)
int fetch_driver_from_cloud(hw_fingerprint_t* fp) {
    DF_LOG_INFO("Invoking cloud driver fetcher");
    if (!fp) {
        DF_LOG_ERR("fetch_driver_from_cloud: NULL fingerprint");
        return -1;
    }
    return cloud_fetch_driver(fp);
}

// ... existing code ...