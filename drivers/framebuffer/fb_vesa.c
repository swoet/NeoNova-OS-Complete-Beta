#include "fb_device.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// VESA BIOS mode info structure (simplified)
typedef struct {
    uint16_t mode_attributes;
    uint8_t win_a, win_b;
    uint16_t granularity, winsize, segment_a, segment_b;
    uint32_t phys_base_ptr;
    uint16_t pitch;
    uint16_t width, height, bpp;
} __attribute__((packed)) vesa_mode_info_t;

// BIOS interrupt call (to be provided by kernel/arch)
int bios_int10_vesa_set_mode(uint16_t mode, vesa_mode_info_t* out_info); // extern

static int vesa_set_mode(fb_device_t* dev, fb_mode_t* mode) {
    vesa_mode_info_t info = {0};
    // Example: mode 0x118 = 1024x768x32bpp
    if (bios_int10_vesa_set_mode(0x118, &info) != 0) return -1;
    mode->width = info.width;
    mode->height = info.height;
    mode->bpp = info.bpp;
    mode->pitch = info.pitch;
    mode->framebuffer = (void*)(uintptr_t)info.phys_base_ptr;
    dev->mode = *mode;
    return 0;
}

static int vesa_draw_pixel(fb_device_t* dev, int x, int y, uint32_t color) {
    if (!dev->mode.framebuffer) return -1;
    uint32_t* fb = (uint32_t*)dev->mode.framebuffer;
    fb[y * dev->mode.width + x] = color;
    return 0;
}

static int vesa_blit(fb_device_t* dev, int x, int y, const void* buf, size_t w, size_t h) {
    if (!dev->mode.framebuffer) return -1;
    for (size_t row = 0; row < h; ++row) {
        for (size_t col = 0; col < w; ++col) {
            vesa_draw_pixel(dev, x + col, y + row, ((uint32_t*)buf)[row * w + col]);
        }
    }
    return 0;
}

static void vesa_shutdown(fb_device_t* dev) {
    (void)dev;
    // No-op for now
}

static int vesa_init(fb_device_t* dev) {
    dev->name = "vesa";
    dev->init = vesa_init;
    dev->set_mode = vesa_set_mode;
    dev->draw_pixel = vesa_draw_pixel;
    dev->blit = vesa_blit;
    dev->shutdown = vesa_shutdown;
    // Default mode: 1024x768x32
    fb_mode_t mode = {1024, 768, 32, 0, NULL};
    return vesa_set_mode(dev, &mode);
}

// Register for auto-detection
int fb_vesa_probe(fb_device_t* out_dev) {
    return vesa_init(out_dev);
} 