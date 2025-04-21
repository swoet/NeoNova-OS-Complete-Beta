; 32-bit Multiboot2 loader stub for NeoNovaOS
BITS 32
SECTION .multiboot
    ALIGN 4
    dd 0x1BADB002
    dd 0x00
    dd -(0x1BADB002)

SECTION .text
global start
start:
    cli
    mov esp, 0x9FB00
    call kmain32
    hlt
