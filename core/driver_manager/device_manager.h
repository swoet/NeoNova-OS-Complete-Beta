#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../drivers/unified_driver_framework/driver_framework.h"

typedef struct {
    hw_fingerprint_t fingerprint;
    int driver_status; // 0=ok, -1=missing, 1=cloud, 2=ai, etc.
    char driver_name[64];
} device_info_t;

void device_manager_init(void);
int device_manager_enumerate(device_info_t* out, int max_count);
const device_info_t* device_manager_list(int* count);
const char* device_manager_get_status(int device_index);
void device_manager_rescan(void); 