#include "device_manager.h"
#include "../../drivers/unified_driver_framework/driver_framework.h"
#include "../../network/net_stack.h"
#include <string.h>

#define MAX_DEVICES 128
static device_info_t g_devices[MAX_DEVICES];
static int g_device_count = 0;
static device_info_t prev_net_devices[MAX_DEVICES];
static int prev_net_count = 0;

static int is_network_device(const hw_fingerprint_t* fp) {
    if (fp->bus_type == BUS_TYPE_PCI && fp->class_code == 0x02) return 1; // PCI network controller
    if (fp->bus_type == BUS_TYPE_USB && (fp->usb_class == 0xE0 || fp->usb_class == 0x02)) return 1; // USB wireless or comm
    return 0;
}

void device_manager_rescan(void) {
    // Save previous network devices
    int old_count = 0;
    for (int i = 0; i < g_device_count; ++i) {
        if (is_network_device(&g_devices[i].fingerprint)) {
            prev_net_devices[old_count++] = g_devices[i];
        }
    }
    prev_net_count = old_count;
    // Rescan
    memset(g_devices, 0, sizeof(g_devices));
    g_device_count = fingerprint_hardware(&g_devices[0].fingerprint, MAX_DEVICES);
    for (int i = 0; i < g_device_count; ++i) {
        int status = match_driver(&g_devices[i].fingerprint);
        if (status == 0) {
            g_devices[i].driver_status = 0;
            strncpy(g_devices[i].driver_name, "native", sizeof(g_devices[i].driver_name));
        } else if (generate_generic_driver(&g_devices[i].fingerprint) == 0) {
            g_devices[i].driver_status = 2;
            strncpy(g_devices[i].driver_name, "ai-generated", sizeof(g_devices[i].driver_name));
        } else if (fetch_driver_from_cloud(&g_devices[i].fingerprint) == 0) {
            g_devices[i].driver_status = 1;
            strncpy(g_devices[i].driver_name, "cloud", sizeof(g_devices[i].driver_name));
        } else {
            g_devices[i].driver_status = -1;
            strncpy(g_devices[i].driver_name, "missing", sizeof(g_devices[i].driver_name));
        }
    }
    // Detect added/removed network devices
    // Add new
    for (int i = 0; i < g_device_count; ++i) {
        if (is_network_device(&g_devices[i].fingerprint)) {
            int found = 0;
            for (int j = 0; j < prev_net_count; ++j) {
                if (memcmp(&g_devices[i].fingerprint, &prev_net_devices[j].fingerprint, sizeof(hw_fingerprint_t)) == 0) {
                    found = 1; break;
                }
            }
            if (!found) {
                uint8_t mac[6] = {0xDE,0xAD,0xBE,0xEF,0x00,(uint8_t)i};
                char name[16]; snprintf(name, sizeof(name), "net%d", i);
                net_if_hotplug_add(name, mac);
            }
        }
    }
    // Remove missing
    for (int j = 0; j < prev_net_count; ++j) {
        int found = 0;
        for (int i = 0; i < g_device_count; ++i) {
            if (is_network_device(&g_devices[i].fingerprint) &&
                memcmp(&g_devices[i].fingerprint, &prev_net_devices[j].fingerprint, sizeof(hw_fingerprint_t)) == 0) {
                found = 1; break;
            }
        }
        if (!found) {
            char name[16]; snprintf(name, sizeof(name), "net%d", j);
            net_if_hotplug_remove(name);
        }
    }
}

void device_manager_init(void) {
    device_manager_rescan();
}

int device_manager_enumerate(device_info_t* out, int max_count) {
    int n = g_device_count < max_count ? g_device_count : max_count;
    memcpy(out, g_devices, n * sizeof(device_info_t));
    return n;
}

const device_info_t* device_manager_list(int* count) {
    if (count) *count = g_device_count;
    return g_devices;
}

const char* device_manager_get_status(int device_index) {
    if (device_index < 0 || device_index >= g_device_count) return "invalid";
    return g_devices[device_index].driver_name;
} 