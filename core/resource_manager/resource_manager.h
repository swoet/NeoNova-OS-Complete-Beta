#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <stdint.h>
#include <stddef.h>

// Resource usage structure
typedef struct {
    float cpu_usage;
    float ram_usage;
    float gpu_usage;
    float io_usage;
} resource_usage_t;

// API
void resource_manager_init(void);
void resource_manager_update(void);
resource_usage_t resource_manager_get_usage(void);
void resource_manager_scale(void);
void resource_manager_prioritize(void);
void resource_manager_power_adjust(void);

#endif // RESOURCE_MANAGER_H 