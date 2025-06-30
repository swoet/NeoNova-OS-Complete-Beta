#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include <kernel/include/kdefs.h> // For PAGE_SIZE, HEAP_START_ADDRESS, HEAP_INITIAL_SIZE
#include <stdint.h>
#include <stddef.h>

// Forward declaration for Multiboot info, if passed directly to memory_init
// A better approach would be to pass a processed memory map.
struct multiboot_info;

// --- Basic Block Allocator (Kernel Heap) Structures ---
// This is for a very simple early kernel heap (kmalloc/kfree).
// A more advanced system would use a slab allocator or buddy allocator on top of PMM.

typedef struct memory_block {
    uint64_t base_address; // Start address of this block
    uint64_t size;         // Size of this block in bytes
    uint8_t is_available;  // 1 if free, 0 if allocated
    struct memory_block* next_block; // Pointer to the next block in the free or allocated list (optional)
} memory_block_t;

// Manages the simple kernel heap.
typedef struct {
    memory_block_t* block_list_head; // Head of the list of all memory blocks
    uint32_t block_count;            // Total number of blocks managed
    uint64_t total_heap_memory;      // Total size of the managed heap area
    uint64_t used_heap_memory;       // Memory currently allocated from the heap
    // void* heap_start_addr;        // Start address of the heap region
    // void* heap_end_addr;          // End address of the heap region
    // Lock for heap operations (e.g., k_spinlock_t heap_lock);
} kernel_heap_manager_t;


// --- Physical Memory Manager (PMM) API (Conceptual) ---
// Manages physical page frames.
// void pmm_init(const struct multiboot_info* mboot_info); // Or pass a processed memory map
// uintptr_t pmm_alloc_page(void);           // Allocate a single physical page frame
// uintptr_t pmm_alloc_pages(size_t count);  // Allocate contiguous physical page frames
// void pmm_free_page(uintptr_t page_addr);
// void pmm_free_pages(uintptr_t start_addr, size_t count);
// size_t pmm_get_total_memory_kb(void);
// size_t pmm_get_free_memory_kb(void);


// --- Virtual Memory Manager (VMM) API (Conceptual) ---
// Manages address spaces, page tables, and mappings.
// typedef void* address_space_t; // Opaque type for an address space / page directory
//
// void vmm_init(void);
// address_space_t vmm_create_address_space(void);
// void vmm_destroy_address_space(address_space_t as);
// void vmm_switch_address_space(address_space_t as); // e.g., load CR3
// address_space_t vmm_get_kernel_address_space(void);
// address_space_t vmm_get_current_address_space(void);
//
// int vmm_map_page(address_space_t as, uintptr_t virt_addr, uintptr_t phys_addr, uint32_t flags);
// void vmm_unmap_page(address_space_t as, uintptr_t virt_addr);
// uintptr_t vmm_get_physical_address(address_space_t as, uintptr_t virt_addr);
//
// void* vmm_alloc_region(address_space_t as, size_t size, uint32_t flags); // Allocate virtual pages
// void vmm_free_region(address_space_t as, void* virt_addr, size_t size);


// --- Kernel Heap (kmalloc/kfree) Function Declarations ---

// Initializes the basic kernel heap manager.
// `heap_start`: physical address where the heap should begin.
// `initial_size`: size of the heap.
// This will be called after PMM is initialized if kmalloc needs to allocate its own metadata.
// The provided code has a simpler global memory_manager_t.
void kernel_heap_init(const struct multiboot_info* mboot_info); // Adapting original memory_init

// Allocate a block of memory from the kernel heap.
// Size is in bytes. Returns NULL if allocation fails.
void* kmalloc(size_t size);

// Free a previously allocated block of memory.
// `ptr` must be a pointer returned by `kmalloc`.
void kfree(void* ptr);

// (Conceptual) More advanced heap functions
// void* krealloc(void* ptr, size_t new_size);
// void* kcalloc(size_t num_elements, size_t element_size);


// --- Page Allocation Functions (Placeholders from original code, would use PMM/VMM) ---
// These are high-level and would typically be implemented using the PMM and VMM.
// For now, they are just declarations as in the original code.
// void* allocate_pages(size_t page_count); // Should map to pmm_alloc_pages & vmm_map_pages
// void free_pages(void* address, size_t page_count); // Should map to vmm_unmap_pages & pmm_free_pages

// (Conceptual) Page table management functions from original code
// These are highly architecture-specific (x86-64 assumed in original).
// uint64_t create_page_directory(void); // Creates a new top-level page table structure (e.g., PML4)
// void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags); // Maps a virtual to physical page


#endif // KERNEL_MEMORY_H
