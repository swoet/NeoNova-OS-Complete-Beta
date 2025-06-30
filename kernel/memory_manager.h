#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h> // For size_t
#include <stdint.h> // For uintptr_t, uint64_t

// --- Conceptual Types ---
typedef uintptr_t PhysicalAddress;
typedef uintptr_t VirtualAddress;

#define PAGE_SIZE 4096 // Example page size (could be architecture-dependent or configurable)

// Flags for memory allocation
#define MEM_FLAG_KERNEL   0x01 // Allocate from kernel space
#define MEM_FLAG_USER     0x02 // Allocate from user space (for a process)
#define MEM_FLAG_ZEROED   0x04 // Ensure allocated memory is zeroed
#define MEM_FLAG_WRITABLE 0x08
#define MEM_FLAG_EXECUTABLE 0x10
// ... other flags like DMA-able, cache policy, etc.


// --- Physical Memory Management API (Conceptual) ---

// Initialize the physical memory manager (e.g., with memory map from bootloader).
// void PMM_Initialize(const MultibootMemoryMap* memory_map); // Example dependency

// Allocate a physical page (or contiguous pages).
// Returns physical address or 0 (or NULL_PHYS_ADDR) on failure.
PhysicalAddress PMM_AllocatePage();
PhysicalAddress PMM_AllocatePages(size_t num_pages);

// Free a physical page (or pages).
void PMM_FreePage(PhysicalAddress page_addr);
void PMM_FreePages(PhysicalAddress start_addr, size_t num_pages);

// Get total and available physical memory.
// size_t PMM_GetTotalMemory();
// size_t PMM_GetFreeMemory();


// --- Virtual Memory Management API (Conceptual) ---
// This would typically involve managing page tables and address spaces for processes.
// typedef struct PageTable PageTable; // Opaque structure for a page table/address space context

// Initialize the virtual memory manager.
// void VMM_Initialize();

// Create a new address space (e.g., for a new process).
// Returns a handle to the new address space (e.g., pointer to root page table).
// void* VMM_CreateAddressSpace();
// void VMM_DestroyAddressSpace(void* address_space_handle);
// void VMM_SwitchAddressSpace(void* address_space_handle); // Switch current CPU context

// Map a physical page to a virtual address in the current (or specified) address space.
// int VMM_MapPage(VirtualAddress vaddr, PhysicalAddress paddr, uint3_t flags);
// void VMM_UnmapPage(VirtualAddress vaddr);

// Allocate virtual memory region (finds free virtual range and maps physical pages to it).
// VirtualAddress VMM_AllocateVirtualRegion(size_t size, uint3_t flags);
// void VMM_FreeVirtualRegion(VirtualAddress vaddr, size_t size);

// Handle a page fault.
// void VMM_HandlePageFault(VirtualAddress fault_address, uint3_t error_code);


// --- General Kernel Heap Allocation (Conceptual - builds on PMM/VMM) ---
// Similar to malloc/free but for kernel use.
void* kalloc(size_t size, uint3_t flags); // flags might indicate alignment, etc.
void kfree(void* ptr);


// --- Memory Manager Module API (as a KernelModule) ---
// This is how the memory manager would register itself.
// extern KernelModule MemoryManagerModule; // Defined in memory_manager.c


#endif // MEMORY_MANAGER_H
