#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <kernel/include/types.h> // For process_state_t, priority_t
#include <kernel/include/thread.h>  // For thread_t (as a process contains threads)
#include <stddef.h>               // For size_t, NULL

// Process Control Block (PCB)
typedef struct process {
    uint32_t pid;               // Unique Process ID
    uint32_t parent_pid;        // PID of the parent process (0 for init or orphaned)

    process_state_t state;      // Current state of the process
    priority_t priority;        // Base priority for the process (threads may have their own)

    char name[64];              // Process name (null-terminated)

    // Memory Management Information (Conceptual - VMM would manage details)
    // uint64_t page_directory_phys_addr; // Physical address of the page directory base (e.g., CR3 for x86)
    void* address_space_handle; // Opaque handle to the VMM's representation of this process's address space
    // uint64_t memory_base_virtual; // Start of process's primary virtual memory region
    // uint64_t memory_size_virtual; // Size of this region

    // Thread Management
    thread_t* main_thread;      // Pointer to the main thread of this process
    thread_t* threads_list_head;// Head of a linked list of all threads belonging to this process
    uint32_t thread_count;      // Number of active threads in this process

    // Process Hierarchy (Conceptual - for parent/child relationships)
    struct process* parent;     // Pointer to parent process PCB
    // struct process* children_list_head; // Head of a linked list of child processes
    // struct process* next_sibling;       // For linking siblings under a parent

    // For linking processes in system-wide lists (e.g., all processes, zombie processes)
    struct process* next_in_global_list;
    // struct process* prev_in_global_list;

    // Exit status (if terminated or zombie)
    // int exit_status;

    // User and Group IDs
    // uint32_t uid;
    // uint32_t gid;

    // Working directory (conceptual path or inode pointer)
    // char current_working_directory[256];

    // Open file descriptor table (conceptual)
    // void* fd_table[MAX_PROCESS_OPEN_FILES];

} process_t;


// --- Process Management Function Declarations ---

// Initializes the process management subsystem.
// Called once during kernel initialization.
// This might create the initial kernel process or idle process.
void process_system_init(void);

// Creates a new process.
// name: A descriptive name for the process.
// entry_point: The starting address of the function this process's main thread will execute.
// priority: Initial base priority for the process.
// Returns a pointer to the newly created process_t, or NULL on failure.
process_t* process_create(const char* name, void (*entry_point)(void*), priority_t priority, uint32_t parent_pid_val);

// Terminates the specified process by its PID.
// This involves terminating all its threads, releasing its resources, etc.
// exit_code: The exit code for the process.
// Returns 0 on success, error code otherwise.
int process_terminate(uint32_t pid, int exit_code);

// The current process exits.
// This function would not return.
// void process_exit(int exit_code);

// Find a process by its PID.
// Returns a pointer to the process_t or NULL if not found.
// process_t* process_find_by_pid(uint32_t pid);

// Get the PCB of the currently running process.
// process_t* process_get_current(void);

// Get the PID of the currently running process.
// uint32_t process_get_current_pid(void);

// (Conceptual) Wait for a child process to change state (e.g., terminate).
// Similar to POSIX waitpid().
// int process_wait(uint32_t child_pid, int* status_location, int options);


#endif // KERNEL_PROCESS_H
