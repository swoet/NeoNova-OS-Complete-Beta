#ifndef KERNEL_DEFS_H
#define KERNEL_DEFS_H

// Kernel constants
#define KERNEL_STACK_SIZE 0x4000        // Default stack size for kernel threads
#define MAX_PROCESSES     256           // Maximum number of processes
#define MAX_THREADS       1024          // Maximum number of threads system-wide (example)
#define PAGE_SIZE         0x1000        // 4KB page size (common, but can be arch-dependent)

// Initial Heap definitions (very basic, for early kmalloc)
#define HEAP_START_ADDRESS ((void*)0x100000) // Example physical address for kernel heap start
#define HEAP_INITIAL_SIZE  0x1000000         // 16MB example initial heap size

// Default entry point for init process (conceptual)
#define INIT_PROCESS_ENTRY_POINT ((void*)0x400000)

#endif // KERNEL_DEFS_H
