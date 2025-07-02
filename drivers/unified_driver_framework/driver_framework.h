#ifndef DRIVER_FRAMEWORK_H
#define DRIVER_FRAMEWORK_H

#include <stdint.h>
#include <stddef.h>
#include "../../kernel64/include/modular.h"

// Hardware fingerprint structure
typedef enum {
    BUS_TYPE_PCI = 0,
    BUS_TYPE_USB = 1,
    BUS_TYPE_LEGACY = 2
} bus_type_t;

typedef struct {
    bus_type_t bus_type;
    // PCI
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass_code;
    // USB
    uint16_t usb_vendor_id;
    uint16_t usb_product_id;
    uint8_t usb_class;
    uint8_t usb_subclass;
    // Legacy
    uint8_t legacy_type; // e.g., 0=none, 1=PS2_KBD, 2=PS2_MOUSE, 3=SERIAL, etc.
} hw_fingerprint_t;

// Driver interface
typedef struct driver {
    kernel_module_t base;
    int (*probe)(hw_fingerprint_t* fp);
    int (*ioctl)(int cmd, void* arg);
} driver_t;

// API
int register_driver(driver_t* drv);
int unregister_driver(const char* name);
int fingerprint_hardware(hw_fingerprint_t* out_fp, int max_count);
int match_driver(hw_fingerprint_t* fp);
int generate_generic_driver(hw_fingerprint_t* fp);
int fetch_driver_from_cloud(hw_fingerprint_t* fp);

#endif // DRIVER_FRAMEWORK_H 