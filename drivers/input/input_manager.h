#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "kbd_ps2.h"
#include "mouse_ps2.h"
#include <stdbool.h>

typedef void (*kbd_event_handler_t)(const kbd_event_t*);
typedef void (*mouse_event_handler_t)(const mouse_event_t*);

void input_manager_init(void);
void input_manager_register_kbd_handler(kbd_event_handler_t handler);
void input_manager_register_mouse_handler(mouse_event_handler_t handler);
void input_manager_poll(void);

#endif // INPUT_MANAGER_H 