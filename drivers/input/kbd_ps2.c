#include "kbd_ps2.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Hardware I/O (to be provided by kernel/arch)
uint8_t inb(uint16_t port); // extern
void outb(uint16_t port, uint8_t value); // extern

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64

static int current_scancode_set = 2;
static kbd_event_t event_queue[KBD_EVENT_QUEUE_SIZE];
static int queue_head = 0, queue_tail = 0;
static uint8_t modifiers = 0;

// Modifier bitmasks
#define MOD_SHIFT 0x01
#define MOD_CTRL  0x02
#define MOD_ALT   0x04

// Example scan code to keycode mapping (expand as needed)
static uint16_t scancode_to_keycode_set2[256] = {
    [0x1C] = 'A', [0x32] = 'B', [0x21] = 'C', [0x23] = 'D', [0x24] = 'E',
    [0x2B] = 'F', [0x34] = 'G', [0x33] = 'H', [0x43] = 'I', [0x3B] = 'J',
    [0x42] = 'K', [0x4B] = 'L', [0x3A] = 'M', [0x31] = 'N', [0x44] = 'O',
    [0x4D] = 'P', [0x15] = 'Q', [0x2D] = 'R', [0x1B] = 'S', [0x2C] = 'T',
    [0x3C] = 'U', [0x2A] = 'V', [0x1D] = 'W', [0x22] = 'X', [0x35] = 'Y',
    [0x1A] = 'Z', [0x45] = '0', [0x16] = '1', [0x1E] = '2', [0x26] = '3',
    [0x25] = '4', [0x2E] = '5', [0x36] = '6', [0x3D] = '7', [0x3E] = '8',
    [0x46] = '9',
    // ... add more mappings as needed ...
};

void kbd_ps2_set_scancode_set(int set) {
    current_scancode_set = set;
    // Send command to keyboard if needed
}

static void enqueue_event(const kbd_event_t* ev) {
    event_queue[queue_head] = *ev;
    queue_head = (queue_head + 1) % KBD_EVENT_QUEUE_SIZE;
    if (queue_head == queue_tail) queue_tail = (queue_tail + 1) % KBD_EVENT_QUEUE_SIZE; // Overwrite oldest
}

bool kbd_ps2_poll_event(kbd_event_t* out_event) {
    // Poll hardware for new scancode
    if ((inb(PS2_STATUS_PORT) & 1) != 0) {
        uint8_t sc = inb(PS2_DATA_PORT);
        kbd_event_t ev = {0};
        ev.scancode = sc;
        ev.keycode = scancode_to_keycode_set2[sc & 0x7F];
        ev.modifiers = modifiers;
        if (sc == 0x12 || sc == 0x59) modifiers |= MOD_SHIFT; // Shift press
        if (sc == 0xF0) modifiers &= ~MOD_SHIFT; // Shift release (simplified)
        // ... handle other modifiers ...
        ev.type = (sc & 0x80) ? KBD_EVENT_RELEASE : KBD_EVENT_PRESS;
        enqueue_event(&ev);
    }
    // Return next event from queue
    if (queue_tail != queue_head) {
        *out_event = event_queue[queue_tail];
        queue_tail = (queue_tail + 1) % KBD_EVENT_QUEUE_SIZE;
        return true;
    }
    return false;
}

void kbd_ps2_init(void) {
    queue_head = queue_tail = 0;
    modifiers = 0;
    kbd_ps2_set_scancode_set(2);
    printf("[KBD] PS/2 keyboard initialized (scan code set 2, advanced mode).\n");
} 