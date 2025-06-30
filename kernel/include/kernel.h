#ifndef KERNEL_H
#define KERNEL_H

#include <kernel/include/types.h>     // For base types used by other kernel headers
#include <kernel/include/process.h>   // For process_t
#include <kernel/include/scheduler.h> // For scheduler_t
#include <kernel/include/memory.h>    // For kernel_heap_manager_t (was memory_manager_t)
#include <stdint.h>

// Forward declaration for Multiboot info structure
// This is used by kernel_main and kernel_init.
// It's better to include the actual "multiboot.h" if it's stable and available globally.
// For now, using a forward declaration as the original code did.
struct multiboot_info;


// Kernel Global State Structure
// This structure holds the global state of the entire kernel.
// A single instance of this will be defined globally (e.g., g_kernel).
typedef struct {
    // Process Management
    process_t* process_list_head; // Head of the linked list of all processes
    uint32_t next_available_pid;  // Next PID to assign
    uint32_t next_available_tid;  // Next TID to assign (can be managed per-process or globally)

    // Scheduler State
    scheduler_t scheduler_state;

    // Memory Management State (specifically for the early kernel heap)
    kernel_heap_manager_t kernel_heap; // Manages the simple kmalloc/kfree heap

    // PMM and VMM states would be separate, complex structures, likely managed by their respective modules
    // void* pmm_state_ptr;
    // void* vmm_state_ptr;

    uint8_t is_initialized;       // Flag: 1 if kernel initialization is complete, 0 otherwise

    // System uptime ticks (could be managed by timer module and accessed via scheduler_state.total_scheduler_ticks)
    // uint64_t system_uptime_ticks;

    // Information from bootloader
    // const struct multiboot_info* boot_info;

    // Add other global kernel states as needed (e.g., interrupt manager state, device manager state)

} kernel_state_t;


// --- Global Kernel State Variable ---
// This will be defined in a .c file (e.g., kernel/kernel.c or kernel/kmain.c)
extern kernel_state_t g_kernel_state; // Changed name from g_kernel to avoid conflict if g_kernel is a function


// --- Core Kernel Function Declarations ---

// Main kernel entry point, called by the bootloader.
// `magic`: Multiboot magic number (to verify bootloader compatibility).
// `mboot_info_addr`: Physical address of the Multiboot information structure.
void kernel_main(uint32_t magic, const struct multiboot_info* mboot_info_addr);

// Initializes core kernel subsystems (except those with their own dedicated init, like scheduler, memory).
// This might set up basic kernel data structures.
void kernel_core_init(const struct multiboot_info* mboot_info); // Renamed from kernel_init to be more specific

// Placeholder for interrupt system initialization (IDT, PIC/APIC, handlers).
void interrupt_system_init(void);


// --- Utility Function Declarations (Often implemented in a utils.c or string.c) ---
// These were in the original global scope, good to have them declared.

// Sets 'size' bytes of memory starting at 'ptr' to 'value'.
void kernel_memset(void* ptr, int value, size_t size);

// Copies 'size' bytes from memory area 'src' to memory area 'dest'.
void kernel_memcpy(void* dest, const void* src, size_t size);

// (Conceptual) Kernel panic function. Halts the system in an unrecoverable error state.
// void kernel_panic(const char* message_format, ...);

// (Conceptual) Kernel logging function.
// void kprintf(const char* format, ...); // Or similar, for printing debug messages


#endif // KERNEL_H
