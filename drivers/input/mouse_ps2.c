#include "mouse_ps2.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Hardware I/O (to be provided by kernel/arch)
uint8_t inb(uint16_t port); // extern
void outb(uint16_t port, uint8_t value); // extern

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64

static mouse_event_t event_queue[MOUSE_EVENT_QUEUE_SIZE];
static int queue_head = 0, queue_tail = 0;
static int last_x = 0, last_y = 0;

// Button bitmasks
#define BTN_LEFT   0x01
#define BTN_RIGHT  0x02
#define BTN_MIDDLE 0x04
#define BTN_EXTRA1 0x08
#define BTN_EXTRA2 0x10

static void enqueue_event(const mouse_event_t* ev) {
    event_queue[queue_head] = *ev;
    queue_head = (queue_head + 1) % MOUSE_EVENT_QUEUE_SIZE;
    if (queue_head == queue_tail) queue_tail = (queue_tail + 1) % MOUSE_EVENT_QUEUE_SIZE; // Overwrite oldest
}

bool mouse_ps2_poll_event(mouse_event_t* out_event) {
    // Poll hardware for new mouse packet (3 or 4 bytes)
    if ((inb(PS2_STATUS_PORT) & 1) != 0) {
        uint8_t packet[4];
        for (int i = 0; i < 4; ++i) packet[i] = inb(PS2_DATA_PORT);
        mouse_event_t ev = {0};
        // Decode packet: standard PS/2 mouse (3 bytes), IntelliMouse (4 bytes)
        ev.dx = (int8_t)packet[1];
        ev.dy = (int8_t)packet[2];
        ev.buttons = 0;
        if (packet[0] & 0x01) ev.buttons |= BTN_LEFT;
        if (packet[0] & 0x02) ev.buttons |= BTN_RIGHT;
        if (packet[0] & 0x04) ev.buttons |= BTN_MIDDLE;
        if (packet[3] & 0x10) ev.buttons |= BTN_EXTRA1;
        if (packet[3] & 0x20) ev.buttons |= BTN_EXTRA2;
        ev.dz = (int8_t)packet[3]; // Scroll wheel
        ev.type = (ev.dx || ev.dy) ? MOUSE_EVENT_MOVE : (ev.dz ? MOUSE_EVENT_WHEEL : MOUSE_EVENT_BUTTON);
        enqueue_event(&ev);
    }
    // Return next event from queue
    if (queue_tail != queue_head) {
        *out_event = event_queue[queue_tail];
        queue_tail = (queue_tail + 1) % MOUSE_EVENT_QUEUE_SIZE;
        return true;
    }
    return false;
}

void mouse_ps2_init(void) {
    queue_head = queue_tail = 0;
    last_x = last_y = 0;
    printf("[MOUSE] PS/2 mouse initialized (advanced mode, wheel, extra buttons).\n");
} 