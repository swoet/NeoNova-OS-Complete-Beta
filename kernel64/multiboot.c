#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
} multiboot_info_t;

void parse_multiboot_info(const void* mb_addr, multiboot_info_t* out) {
    const uint32_t* mb = (const uint32_t*)mb_addr;
    out->mem_lower = mb[1];
    out->mem_upper = mb[2];
    out->boot_device = mb[3];
    printf("[Multiboot] mem_lower=%uKB, mem_upper=%uKB, boot_device=0x%08X\n", out->mem_lower, out->mem_upper, out->boot_device);
}
