#include "fb_device.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define TEXT_MODE_COLS 80
#define TEXT_MODE_ROWS 25
#define TEXT_MODE_FB   ((volatile uint16_t*)0xB8000)

static int text_set_mode(fb_device_t* dev, fb_mode_t* mode) {
    mode->width = TEXT_MODE_COLS;
    mode->height = TEXT_MODE_ROWS;
    mode->bpp = 16;
    mode->pitch = TEXT_MODE_COLS * 2;
    mode->framebuffer = (void*)TEXT_MODE_FB;
    dev->mode = *mode;
    return 0;
}

static int text_draw_pixel(fb_device_t* dev, int x, int y, uint32_t color) {
    if (x < 0 || x >= TEXT_MODE_COLS || y < 0 || y >= TEXT_MODE_ROWS) return -1;
    volatile uint16_t* fb = (volatile uint16_t*)dev->mode.framebuffer;
    // Color: lower 8 bits = char, upper 8 bits = attr
    uint8_t ch = (color & 0xFF);
    uint8_t attr = (color >> 8) & 0xFF;
    fb[y * TEXT_MODE_COLS + x] = (attr << 8) | ch;
    return 0;
}

static int text_blit(fb_device_t* dev, int x, int y, const void* buf, size_t w, size_t h) {
    const char* chars = (const char*)buf;
    for (size_t row = 0; row < h; ++row) {
        for (size_t col = 0; col < w; ++col) {
            int idx = row * w + col;
            text_draw_pixel(dev, x + col, y + row, 0x0F00 | chars[idx]); // White on black
        }
    }
    return 0;
}

static void text_shutdown(fb_device_t* dev) {
    (void)dev;
    // No-op for now
}

static int text_init(fb_device_t* dev) {
    dev->name = "text_mode";
    dev->init = text_init;
    dev->set_mode = text_set_mode;
    dev->draw_pixel = text_draw_pixel;
    dev->blit = text_blit;
    dev->shutdown = text_shutdown;
    fb_mode_t mode = {TEXT_MODE_COLS, TEXT_MODE_ROWS, 16, TEXT_MODE_COLS * 2, (void*)TEXT_MODE_FB};
    return text_set_mode(dev, &mode);
}

// Register for auto-detection
int fb_text_probe(fb_device_t* out_dev) {
    return text_init(out_dev);
} 