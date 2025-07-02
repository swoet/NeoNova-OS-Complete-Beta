#include "fb_device.h"
#include <stdio.h>

// Probes for each framebuffer driver
int fb_bochs_probe(fb_device_t* out_dev);
int fb_vesa_probe(fb_device_t* out_dev);
int fb_text_probe(fb_device_t* out_dev);

int fb_device_auto_init(fb_device_t* out_dev) {
    printf("[FB] Probing for Bochs/QEMU VBE framebuffer...\n");
    if (fb_bochs_probe(out_dev) == 0) {
        printf("[FB] Using Bochs/QEMU VBE framebuffer.\n");
        return 0;
    }
    printf("[FB] Probing for VESA framebuffer...\n");
    if (fb_vesa_probe(out_dev) == 0) {
        printf("[FB] Using VESA framebuffer.\n");
        return 0;
    }
    printf("[FB] Falling back to text mode framebuffer...\n");
    if (fb_text_probe(out_dev) == 0) {
        printf("[FB] Using text mode framebuffer.\n");
        return 0;
    }
    printf("[FB] No framebuffer device available!\n");
    return -1;
} 