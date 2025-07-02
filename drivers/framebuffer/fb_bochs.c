#include "fb_device.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF
#define VBE_DISPI_INDEX_ENABLE 0x04
#define VBE_DISPI_INDEX_XRES   0x01
#define VBE_DISPI_INDEX_YRES   0x02
#define VBE_DISPI_INDEX_BPP    0x03
#define VBE_DISPI_INDEX_FB     0x05
#define VBE_DISPI_ENABLED      0x41
#define VBE_DISPI_DISABLED     0x00
#define VBE_FB_PHYS_ADDR       0xE0000000

static int bochs_set_mode(fb_device_t* dev, fb_mode_t* mode) {
    // Write VBE registers
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE); outw(VBE_DISPI_IOPORT_DATA, VBE_DISPI_DISABLED);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES);   outw(VBE_DISPI_IOPORT_DATA, mode->width);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES);   outw(VBE_DISPI_IOPORT_DATA, mode->height);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP);    outw(VBE_DISPI_IOPORT_DATA, mode->bpp);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE); outw(VBE_DISPI_IOPORT_DATA, VBE_DISPI_ENABLED);
    // Set framebuffer pointer
    mode->framebuffer = (void*)VBE_FB_PHYS_ADDR;
    mode->pitch = mode->width * (mode->bpp / 8);
    dev->mode = *mode;
    return 0;
}

static int bochs_draw_pixel(fb_device_t* dev, int x, int y, uint32_t color) {
    if (!dev->mode.framebuffer) return -1;
    uint32_t* fb = (uint32_t*)dev->mode.framebuffer;
    fb[y * dev->mode.width + x] = color;
    return 0;
}

static int bochs_blit(fb_device_t* dev, int x, int y, const void* buf, size_t w, size_t h) {
    if (!dev->mode.framebuffer) return -1;
    for (size_t row = 0; row < h; ++row) {
        for (size_t col = 0; col < w; ++col) {
            bochs_draw_pixel(dev, x + col, y + row, ((uint32_t*)buf)[row * w + col]);
        }
    }
    return 0;
}

static void bochs_shutdown(fb_device_t* dev) {
    (void)dev;
    // No-op for now
}

static int bochs_init(fb_device_t* dev) {
    dev->name = "bochs_vbe";
    dev->init = bochs_init;
    dev->set_mode = bochs_set_mode;
    dev->draw_pixel = bochs_draw_pixel;
    dev->blit = bochs_blit;
    dev->shutdown = bochs_shutdown;
    // Default mode: 1024x768x32
    fb_mode_t mode = {1024, 768, 32, 0, NULL};
    return bochs_set_mode(dev, &mode);
}

// Register for auto-detection
int fb_bochs_probe(fb_device_t* out_dev) {
    return bochs_init(out_dev);
}

// Platform-specific I/O (to be provided by kernel/arch)
void outw(uint16_t port, uint16_t value); // extern 