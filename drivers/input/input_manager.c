#include "input_manager.h"
#include <stdio.h>
#include <string.h>

static kbd_event_handler_t kbd_handler = NULL;
static mouse_event_handler_t mouse_handler = NULL;

void input_manager_init(void) {
    kbd_ps2_init();
    mouse_ps2_init();
    kbd_handler = NULL;
    mouse_handler = NULL;
    printf("[InputManager] Initialized.\n");
}

void input_manager_register_kbd_handler(kbd_event_handler_t handler) {
    kbd_handler = handler;
}

void input_manager_register_mouse_handler(mouse_event_handler_t handler) {
    mouse_handler = handler;
}

void input_manager_poll(void) {
    kbd_event_t kev;
    while (kbd_ps2_poll_event(&kev)) {
        if (kbd_handler) kbd_handler(&kev);
    }
    mouse_event_t mev;
    while (mouse_ps2_poll_event(&mev)) {
        if (mouse_handler) mouse_handler(&mev);
    }
} 