// Support for Matter, Thread, Home Assistant

#include <stdio.h>
#include <string.h>

typedef struct {
    char name[64];
    char protocol[32];
    int status;
} iot_device_t;

void iot_register_device(iot_device_t* dev, const char* name, const char* protocol) {
    strncpy(dev->name, name, 63); dev->name[63] = '\0';
    strncpy(dev->protocol, protocol, 31); dev->protocol[31] = '\0';
    dev->status = 1;
    printf("[IoT] Registered device '%s' with protocol %s\n", dev->name, dev->protocol);
}

void iot_device_status(const iot_device_t* dev) {
    printf("[IoT] Device '%s' protocol %s status: %s\n", dev->name, dev->protocol, dev->status ? "online" : "offline");
}
