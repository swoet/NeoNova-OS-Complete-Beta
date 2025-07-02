#ifndef KBD_PS2_H
#define KBD_PS2_H
#include <stdint.h>
#include <stdbool.h>
#define KBD_EVENT_QUEUE_SIZE 64

typedef enum {
    KBD_EVENT_PRESS,
    KBD_EVENT_RELEASE
} kbd_event_type_t;

typedef struct {
    kbd_event_type_t type;
    uint8_t scancode;
    uint16_t keycode;
    uint8_t modifiers; // bitmask: shift, ctrl, alt, etc.
} kbd_event_t;

void kbd_ps2_init(void);
bool kbd_ps2_poll_event(kbd_event_t* out_event);
void kbd_ps2_set_scancode_set(int set);

#endif // KBD_PS2_H 