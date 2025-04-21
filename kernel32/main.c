#include "types.h"
void kmain32(void) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    const char* msg = "NeoNovaOS 32-bit Kernel Initialized\n";
    for (int i = 0; msg[i]; ++i) {
        vga[i] = (0x1F << 8) | msg[i];
    }
    while (1) __asm__("hlt");
}
