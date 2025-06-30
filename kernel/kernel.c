#include <kernel/include/kernel.h>
#include <kernel/include/kdefs.h> // For HEAP_START_ADDRESS, HEAP_INITIAL_SIZE, INIT_PROCESS_ENTRY_POINT
#include <kernel/include/types.h> // For priority_t for init process
// #include <multiboot.h> // Assuming this path is correct for the multiboot header from original code
                         // If it's "include/multiboot.h" relative to repo root, then that path.
                         // For now, I'll use a generic placeholder for the actual include path.
#include "multiboot.h" // Placeholder - this needs to be the correct path to your multiboot.h

// Define the global kernel state instance
kernel_state_t g_kernel_state;

// --- Utility Functions ---
void kernel_memset(void* ptr, int value, size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = (uint8_t)value;
    }
}

void kernel_memcpy(void* dest, const void* src, size_t size) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

// --- Core Kernel Initialization ---
void kernel_core_init(const struct multiboot_info* mboot_info) {
    // Clear the main kernel state structure
    kernel_memset(&g_kernel_state, 0, sizeof(kernel_state_t));

    // Initialize PIDs and TIDs
    g_kernel_state.next_available_pid = 1; // PID 0 might be reserved for kernel/idle
    g_kernel_state.next_available_tid = 1;

    g_kernel_state.process_list_head = NULL;
    g_kernel_state.is_initialized = 0; // Will be set to 1 after all systems are up

    // Store boot info if needed (conceptual)
    // g_kernel_state.boot_info = mboot_info;

    // Further very early core setup can go here.
    // kprintf("Kernel core initialized.\n"); // Conceptual kprintf
}

// Placeholder interrupt system initialization
void interrupt_system_init(void) {
    // TODO: Set up IDT (Interrupt Descriptor Table)
    // TODO: Configure PIC (Programmable Interrupt Controller) or APIC (Advanced PIC)
    // TODO: Install handlers for hardware interrupts (timer, keyboard, etc.)
    // TODO: Install handler for software interrupts (system calls)
    // TODO: Enable interrupts (e.g., sti instruction on x86)
    // kprintf("Interrupt system initialized (stub).\n");
}


// Main kernel entry point
// The 'magic' parameter is for Multiboot1. Multiboot2 has a different entry convention.
// Assuming Multiboot1 for now based on typical mboot_info usage.
void kernel_main(uint32_t magic, const struct multiboot_info* mboot_info_addr) {
    // --- Early Boot Output (VGA direct access - x86 specific) ---
    // This should be replaced by a proper console driver or HAL graphics output ASAP.
    volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000; // x86 VGA text mode buffer
    const char* boot_msg = "NeoNovaOS Kernel - Initializing Core Systems...";
    int vga_x = 0, vga_y = 0;

    // Clear screen (light grey on black)
    for (vga_y = 0; vga_y < 25; vga_y++) {
        for (vga_x = 0; vga_x < 80; vga_x++) {
            vga_buffer[vga_y * 80 + vga_x] = ((uint16_t)0x07 << 8) | ' ';
        }
    }
    vga_x = 0; vga_y = 0;
    // Print boot message
    for (int i = 0; boot_msg[i] != '\0' && vga_y < 25; ++i) {
        if (boot_msg[i] == '\n') {
            vga_y++;
            vga_x = 0;
            continue;
        }
        if (vga_x >= 80) {
            vga_y++;
            vga_x = 0;
            if (vga_y >= 25) break;
        }
        vga_buffer[vga_y * 80 + vga_x] = ((uint16_t)0x0F << 8) | boot_msg[i]; // White on black
        vga_x++;
    }
    // --- End Early Boot Output ---

    // Verify Multiboot magic (optional but good practice)
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // Panic or display error: Not booted by a compliant Multiboot loader
        const char* err_msg = "Error: Invalid Multiboot magic! System Halted.";
        vga_x = 0; vga_y = 2; // Position error message
        for (int i = 0; err_msg[i] != '\0' && vga_y < 25; ++i) { /* ... print error ... */
             if (err_msg[i] == '\n') { vga_y++; vga_x = 0; continue; }
             if (vga_x >= 80) { vga_y++; vga_x = 0; if (vga_y >= 25) break; }
             vga_buffer[vga_y * 80 + vga_x] = ((uint16_t)0x0C << 8) | err_msg[i]; // Light Red on black
             vga_x++;
        }
        while(1) __asm__ volatile("cli; hlt"); // Halt
    }

    // Initialize kernel subsystems
    kernel_core_init(mboot_info_addr);    // Basic kernel state
    kernel_heap_init(mboot_info_addr);  // Early kernel heap (kmalloc/kfree)
    // pmm_init(mboot_info_addr);       // Physical Memory Manager (Conceptual)
    // vmm_init();                      // Virtual Memory Manager (Conceptual)
    scheduler_init();                 // Scheduler
    process_system_init();            // Process management (creates kernel process)
    interrupt_system_init();          // Interrupts (critical for scheduler timer, syscalls)

    // DeviceManager_Initialize();      // Conceptual
    // Filesystem_Initialize();         // Conceptual VFS layer

    g_kernel_state.is_initialized = 1;
    // kprintf("NeoNovaOS Kernel Initialized and Ready.\n");

    // Update VGA message after init
    vga_x = 0; vga_y = 1;
    const char* ready_msg = "NeoNovaOS Ready - Kernel Active.";
    for (int i = 0; ready_msg[i] != '\0' && vga_y < 25; ++i) { /* ... print ready ... */
        if (ready_msg[i] == '\n') { vga_y++; vga_x = 0; continue; }
        if (vga_x >= 80) { vga_y++; vga_x = 0; if (vga_y >= 25) break; }
        vga_buffer[vga_y * 80 + vga_x] = ((uint16_t)0x0A << 8) | ready_msg[i]; // Light Green on black
        vga_x++;
    }

    // Create the first user-space process (init process)
    // Note: entry point is conceptual. It needs to be actual code loaded somewhere.
    process_t* init_proc = process_create("init", (void (*)(void*))INIT_PROCESS_ENTRY_POINT, PRIORITY_NORMAL, 0);
    if (!init_proc) {
        // Panic: Failed to create init process
        // kernel_panic("Failed to create init process!");
        const char* err_msg_init = "PANIC: Failed to create init process! System Halted.";
        vga_x = 0; vga_y = 3; // Position error message
        for (int i = 0; err_msg_init[i] != '\0' && vga_y < 25; ++i) { /* ... print error ... */
             if (err_msg_init[i] == '\n') { vga_y++; vga_x = 0; continue; }
             if (vga_x >= 80) { vga_y++; vga_x = 0; if (vga_y >= 25) break; }
             vga_buffer[vga_y * 80 + vga_x] = ((uint16_t)0x0C << 8) | err_msg_init[i];
             vga_x++;
        }
        while(1) __asm__ volatile("cli; hlt");
    }

    // Start scheduling (this call might not return here if it switches to init_proc)
    // The scheduler should be started by the timer interrupt enabling,
    // or an explicit call to switch to the first process.
    // For now, an explicit schedule() call.
    // Ensure interrupts are enabled before this if schedule relies on timer interrupts for preemption.
    // kprintf("Starting scheduler...\n");
    // __asm__ volatile ("sti"); // Enable interrupts if interrupt_system_init didn't
    schedule();

    // Kernel main loop (should ideally not be reached if scheduler is running user processes)
    // Or, this becomes the idle loop if no processes are ready.
    // kprintf("Kernel entering idle loop.\n");
    while (1) {
        // TODO: Implement idle task logic, power saving, etc.
        __asm__ volatile("hlt"); // Halt CPU until next interrupt
    }
}
