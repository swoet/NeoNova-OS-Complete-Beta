#ifndef MOUSE_PS2_H
#define MOUSE_PS2_H
#include <stdint.h>
#include <stdbool.h>
#define MOUSE_EVENT_QUEUE_SIZE 64

typedef enum {
    MOUSE_EVENT_MOVE,
    MOUSE_EVENT_BUTTON,
    MOUSE_EVENT_WHEEL
} mouse_event_type_t;

typedef struct {
    mouse_event_type_t type;
    int dx, dy; // movement
    int dz;     // wheel
    uint8_t buttons; // bitmask: left, right, middle, extra
} mouse_event_t;

void mouse_ps2_init(void);
bool mouse_ps2_poll_event(mouse_event_t* out_event);

#endif // MOUSE_PS2_H 