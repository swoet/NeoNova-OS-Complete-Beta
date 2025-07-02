#include "infostream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

infostream_widget_t* infostream_create(int x, int y, int length, int thickness, infostream_orientation_t orientation, unsigned int color, const char* label) {
    infostream_widget_t* s = (infostream_widget_t*)malloc(sizeof(infostream_widget_t));
    s->x = x; s->y = y; s->length = length; s->thickness = thickness;
    s->orientation = orientation;
    s->color = color;
    s->label = label ? strdup(label) : NULL;
    s->data = NULL;
    s->animating = false;
    s->anim_phase = 0.0f;
    return s;
}

void infostream_render(const infostream_widget_t* stream) {
    const char* orient = "?";
    switch (stream->orientation) {
        case INFOSTREAM_ORIENT_TOP: orient = "top"; break;
        case INFOSTREAM_ORIENT_BOTTOM: orient = "bottom"; break;
        case INFOSTREAM_ORIENT_LEFT: orient = "left"; break;
        case INFOSTREAM_ORIENT_RIGHT: orient = "right"; break;
    }
    printf("[InfoStream] %s at (%d,%d) len=%d th=%d color=#%06X orient=%s phase=%.2f | %s: %s\n",
        stream->label ? stream->label : "(no label)", stream->x, stream->y, stream->length, stream->thickness, stream->color, orient, stream->anim_phase,
        stream->label ? stream->label : "", stream->data ? stream->data : "");
}

void infostream_set_data(infostream_widget_t* stream, const char* data) {
    if (stream->data) free((void*)stream->data);
    stream->data = data ? strdup(data) : NULL;
}

void infostream_animate(infostream_widget_t* stream, float phase) {
    stream->animating = true;
    stream->anim_phase = phase;
    printf("[InfoStream] Animating phase to %.2f\n", phase);
    stream->animating = false;
} 