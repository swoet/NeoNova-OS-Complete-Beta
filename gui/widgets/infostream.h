#pragma once
#include <stdbool.h>

typedef enum {
    INFOSTREAM_ORIENT_TOP,
    INFOSTREAM_ORIENT_BOTTOM,
    INFOSTREAM_ORIENT_LEFT,
    INFOSTREAM_ORIENT_RIGHT
} infostream_orientation_t;

typedef struct infostream_widget {
    int x, y, length, thickness;
    infostream_orientation_t orientation;
    unsigned int color;
    const char* label;
    const char* data;
    bool animating;
    float anim_phase;
} infostream_widget_t;

infostream_widget_t* infostream_create(int x, int y, int length, int thickness, infostream_orientation_t orientation, unsigned int color, const char* label);
void infostream_render(const infostream_widget_t* stream);
void infostream_set_data(infostream_widget_t* stream, const char* data);
void infostream_animate(infostream_widget_t* stream, float phase); 