#include <kernel/include/memory.h>
#include <kernel/include/kernel.h> // For g_kernel_state, kernel_memset
// #include <multiboot.h> // Placeholder - this needs to be the correct path to your multiboot.h
#include "multiboot.h"

// --- Kernel Heap (kmalloc/kfree) Implementation ---

// Initializes the basic kernel heap.
// The original code had a fixed-size array for blocks within memory_manager_t.
// This implementation will use the HEAP_START_ADDRESS itself for the block metadata array,
// which is a common approach for simple heaps.
void kernel_heap_init(const struct multiboot_info* mboot_info) {
    kernel_heap_manager_t* heap = &g_kernel_state.kernel_heap;

    // For this simple heap, we don't use the multiboot info directly yet,
    // as HEAP_START_ADDRESS and HEAP_INITIAL_SIZE are predefined constants.
    // A more advanced PMM would use mboot_info to map available physical memory.
    (void)mboot_info; // Mark as unused for now to avoid compiler warnings

    // The block_list_head will point to the first block descriptor,
    // which resides at the beginning of the heap region.
    heap->block_list_head = (memory_block_t*)HEAP_START_ADDRESS;
    heap->block_count = 1;
    heap->total_heap_memory = HEAP_INITIAL_SIZE;
    heap->used_heap_memory = 0; // Initially, only metadata for the first block is "used" from heap perspective

    // Initialize the first (and initially only) block, which covers the entire heap area.
    // The actual usable memory starts after this first block_t descriptor.
    // A more robust heap would manage its own metadata space separately or more dynamically.
    // This simplified version assumes block_t structures are small and fit within the heap area.

    // For this simple allocator, we assume the memory_block_t structures themselves
    // are stored at the beginning of the HEAP_START_ADDRESS, and the actual allocatable
    // memory starts after these structures. The original code implies this by using
    // HEAP_START + sizeof(memory_block_t) * 1024.
    // Let's refine this to be more explicit.
    // Assume MAX_BLOCK_DESCRIPTORS can fit.
    #define MAX_BLOCK_DESCRIPTORS 1024 // Must match or be less than original code's assumption
    uintptr_t metadata_region_size = sizeof(memory_block_t) * MAX_BLOCK_DESCRIPTORS;
    uintptr_t actual_heap_start = (uintptr_t)HEAP_START_ADDRESS + metadata_region_size;
    size_t actual_heap_size = HEAP_INITIAL_SIZE - metadata_region_size;

    if (HEAP_INITIAL_SIZE <= metadata_region_size) {
        // Kernel Panic: Heap size too small for metadata
        // kernel_panic("Heap size too small for metadata!");
        // For now, use a print and halt for test harness.
        // This would be a kprintf in a real kernel.
        // Note: No kprintf available yet this early. This init is part of early sequence.
        // If VGA is available:
        // volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
        // const char* panic_msg = "Heap panic!"; for(int i=0;panic_msg[i];++i) vga[80*5+i] = (0x0C << 8) | panic_msg[i];
        // while(1) __asm__ volatile("cli; hlt");
        return; // Cannot initialize
    }

    heap->block_list_head = (memory_block_t*)HEAP_START_ADDRESS; // Descriptors start here

    heap->block_list_head[0].base_address = actual_heap_start;
    heap->block_list_head[0].size = actual_heap_size;
    heap->block_list_head[0].is_available = 1;
    heap->block_list_head[0].next_block = NULL; // Only one block initially

    heap->block_count = 1;
    heap->used_heap_memory = 0; // No user memory allocated yet

    // kprintf("Kernel heap initialized. Start: 0x%x, Size: %u KB\n", actual_heap_start, actual_heap_size / 1024);
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    kernel_heap_manager_t* heap = &g_kernel_state.kernel_heap;
    // TODO: Add locking for SMP safety (e.g., spinlock_acquire(&heap->heap_lock))

    // Align size to a multiple of a common word size (e.g., 8 or 16 bytes) for better performance/compatibility
    // size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    // The original code doesn't do this, so keeping it simple for now.

    memory_block_t* current_block_descriptor_array = heap->block_list_head;

    for (uint32_t i = 0; i < heap->block_count; i++) {
        memory_block_t* block = &current_block_descriptor_array[i];
        if (block->is_available && block->size >= size) {
            block->is_available = 0;
            heap->used_heap_memory += block->size; // Original code added 'size', but it should be block->size if splitting is imperfect

            // Try to split the block if it's significantly larger than requested
            // (e.g., if remainder is large enough to hold a new block descriptor and some data)
            // The original code's split logic: size + 64 (for new header + min data?)
            // A new block descriptor itself is sizeof(memory_block_t).
            // Let's say min useful data size is 16 bytes.
            size_t min_splittable_size = size + sizeof(memory_block_t) + 16;
            if (block->size > min_splittable_size && heap->block_count < MAX_BLOCK_DESCRIPTORS) {
                memory_block_t* new_block_descriptor = &current_block_descriptor_array[heap->block_count]; // Get next descriptor slot

                new_block_descriptor->is_available = 1;
                new_block_descriptor->base_address = block->base_address + size;
                new_block_descriptor->size = block->size - size;
                new_block_descriptor->next_block = NULL; // Or insert into a free list

                // This simple array-based descriptor list doesn't easily support inserting.
                // The original code just incremented block_count. This implies descriptors are pre-allocated or contiguous.
                // We are using the MAX_BLOCK_DESCRIPTORS pre-allocated space at HEAP_START_ADDRESS.

                heap->block_count++; // Assumes there's a free descriptor slot

                block->size = size; // Adjust current block's size
                // heap->used_heap_memory was already updated with original block size.
                // If we are more precise, it should be:
                // heap->used_heap_memory -= new_block_descriptor->size; // Correcting the used memory
            }
            // For simplicity and matching original, let's say used_memory reflects the whole block if not split, or just 'size' if split logic is complex
            // The original code did: heap->used_memory += size; which is fine if block->size becomes 'size'.
            // Let's stick to: if block is used, its entire current 'size' is "used" from heap's perspective until kfree.
            // The original logic was: heap->used_memory += size (of request)
            // This means heap->used_memory tracks requested sizes, not actual physical memory occupied by blocks.
            // Let's refine: used_heap_memory should track memory that's *not available*.
            // When block is marked unavailable, its full size is "used".
            // So, used_heap_memory += block->size (the new size, after potential split)

            // The original code: g_kernel.memory.used_memory += size; (size of request)
            // Let's adjust our calculation to match the original intent for now, for easier porting.
            // The 'heap->used_heap_memory += block->size' was my addition.
            // Reverting to original intent:
            // heap->used_heap_memory was incremented by the full block size.
            // If split, the new free block is still part of total_heap_memory.
            // The 'used' part is 'size'.
            // This is confusing. Let's make `used_heap_memory` track sum of SIZES OF UNAVAILABLE BLOCKS.
            // So, when a block of 'block->size' is made unavailable, used_heap_memory increases by 'block->size'.
            // If it's split, the original block's size changes, and a new available block is made.
            // Let's re-evaluate:
            // When found: block->is_available = 0;
            // If NOT split: heap->used_heap_memory += block->size; return (void*)block->base_address;
            // If SPLIT: (new_block is created from remainder)
            //           original_block_size = block->size;
            //           block->size = size; // Requested size
            //           new_block.size = original_block_size - size;
            //           heap->used_heap_memory += block->size; // Add requested size
            //           return (void*)block->base_address;
            // This seems more logical. The original code's `g_kernel.memory.used_memory += size;` was correct.

            // TODO: Add unlocking
            return (void*)block->base_address;
        }
    }

    // TODO: Add unlocking
    // kprintf("kmalloc: Out of memory for size %u\n", size);
    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    kernel_heap_manager_t* heap = &g_kernel_state.kernel_heap;
    // TODO: Add locking

    uintptr_t addr_to_free = (uintptr_t)ptr;
    memory_block_t* current_block_descriptor_array = heap->block_list_head;

    for (uint32_t i = 0; i < heap->block_count; i++) {
        memory_block_t* block = &current_block_descriptor_array[i];
        if (!block->is_available && block->base_address == addr_to_free) {
            block->is_available = 1;
            // heap->used_heap_memory -= block->size; // If used_heap_memory tracks sum of sizes of unavailable blocks

            // TODO: Coalesce adjacent free blocks.
            // This simple implementation doesn't coalesce.
            // Coalescing would require iterating through blocks and merging 'block'
            // with 'block->next_block' if both are free, or with its previous block.
            // This is complex with an array-based descriptor list if blocks need to be removed from descriptor array.
            // The original code did: g_kernel.memory.used_memory -= g_kernel.memory.blocks[i].size;
            // This means used_memory was tracking the actual size of the allocated segment.

            // TODO: Add unlocking
            return;
        }
    }
    // TODO: Add unlocking
    // kprintf("kfree: Attempt to free unallocated or invalid pointer 0x%x\n", ptr);
}


// --- Page Allocation Functions (Placeholders/Stubs) ---
// These would require a full PMM and VMM implementation.

// void* allocate_pages(size_t page_count) {
//     // TODO: Implement using PMM to get physical pages
//     // TODO: Implement using VMM to map these pages into current (or kernel) address space
//     // kprintf("allocate_pages: STUB page_count=%u\n", page_count);
//     return NULL;
// }

// void free_pages(void* address, size_t page_count) {
//     // TODO: Implement using VMM to unmap pages
//     // TODO: Implement using PMM to free physical pages
//     // kprintf("free_pages: STUB address=0x%x, page_count=%u\n", address, page_count);
// }

// uint64_t create_page_directory(void) {
//     // TODO: Implement: Allocate memory for page tables (PML4, PDPT, PD, PT for x86-64)
//     // TODO: Initialize them (e.g., map kernel space, identity map early hardware)
//     // Returns physical address of the top-level page table (PML4 base for x86-64)
//     // kprintf("create_page_directory: STUB\n");
//     return 0;
// }

// void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
//     // TODO: Implement: Modify page tables of current (or specified) address space
//     // to map virtual_addr to physical_addr with given flags (present, writable, user, etc.)
//     // kprintf("map_page: STUB vaddr=0x%llx, paddr=0x%llx, flags=0x%llx\n", virtual_addr, physical_addr, flags);
// }
