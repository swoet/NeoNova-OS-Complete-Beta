#ifndef FB_DEVICE_H
#define FB_DEVICE_H
#include <stdint.h>
#include <stddef.h>

typedef struct fb_mode {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
    void* framebuffer;
} fb_mode_t;

typedef struct fb_device {
    const char* name;
    int (*init)(struct fb_device* dev);
    int (*set_mode)(struct fb_device* dev, fb_mode_t* mode);
    int (*draw_pixel)(struct fb_device* dev, int x, int y, uint32_t color);
    int (*blit)(struct fb_device* dev, int x, int y, const void* buf, size_t w, size_t h);
    void (*shutdown)(struct fb_device* dev);
    fb_mode_t mode;
    void* private_data;
} fb_device_t;

int fb_device_auto_init(fb_device_t* out_dev);

#endif // FB_DEVICE_H 