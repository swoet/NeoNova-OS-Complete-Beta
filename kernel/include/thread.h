#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include <kernel/include/types.h>     // For thread_state_t, priority_t
#include <kernel/arch/x86_64/include/cpu_context.h> // For cpu_state_t (currently x86-64 specific)
                                     // In a more abstract system, this might be <kernel/include/cpu_context_arch.h>
                                     // which then includes the specific arch version.
#include <stddef.h>                   // For size_t

// Forward declaration for process_t to avoid circular dependency with process.h
struct process;

// Thread Control Block (TCB)
typedef struct thread {
    uint32_t tid;               // Unique Thread ID
    uint32_t pid;               // Process ID of the parent process

    thread_state_t state;       // Current state of the thread
    priority_t priority;        // Scheduling priority

    uint64_t time_slice_ticks;  // Remaining time slice in scheduler ticks (if applicable)
    uint64_t total_cpu_time_ns; // Total CPU time consumed by this thread (nanoseconds)

    cpu_state_t cpu_state;      // Saved CPU context (registers, instruction pointer, stack pointer, etc.)
                                // This is loaded when the thread is scheduled to run.

    void* kernel_stack_base;    // Base address of the kernel stack allocated for this thread
    size_t kernel_stack_size;   // Size of the kernel stack
    // void* user_stack_base;   // Base of user-mode stack (if applicable, managed by process VMM)
    // size_t user_stack_size;

    // For linking threads in scheduler queues (e.g., ready queue, blocked queue)
    struct thread* next;        // Next thread in a queue
    struct thread* prev;        // Previous thread in a queue (for doubly linked lists)

    // struct process* parent_process; // Pointer to the parent process PCB

    // Synchronization primitives this thread might be waiting on
    // void* waiting_on_object;
    // const char* wait_reason;

    // Thread-Specific Data (TSD) or Thread-Local Storage (TLS) pointer (conceptual)
    // void* tls_area;

} thread_t;


// --- Thread Management Function Declarations ---

// Creates a new thread within the given process.
// entry_point: The starting address of the function this thread will execute.
// args: A pointer to arguments to be passed to the thread's entry function (conceptual).
// stack_size: Size of the stack to be allocated for this thread (if 0, use default).
// priority: Initial priority of the thread.
// name: A descriptive name for the thread (for debugging).
// Returns a pointer to the newly created thread_t, or NULL on failure.
thread_t* thread_create(struct process* proc, void (*entry_point)(void*), void* args, priority_t priority, const char* name, size_t stack_size);

// Terminates the specified thread.
// If tid is 0 or corresponds to the calling thread, it terminates the calling thread.
// This function might not return if it terminates the calling thread.
void thread_terminate(uint32_t tid); // Or thread_exit() for current thread

// Blocks the current thread, optionally associating it with a wait_queue or reason.
// The scheduler will not run this thread until it's unblocked.
void thread_block(void /* const char* reason, void* wait_object */);

// Unblocks a specified thread, moving it from a blocked state to ready.
// Returns 0 on success, error code otherwise.
int thread_unblock(thread_t* thread);

// Changes the priority of the specified thread.
// Returns 0 on success, error code otherwise.
int thread_set_priority(uint32_t tid, priority_t new_priority);

// The currently running thread yields the CPU.
// The scheduler will select the next thread to run from the ready queue.
void thread_yield(void);

// Get the TCB of the currently running thread.
// thread_t* thread_get_current(void);

// Get the TID of the currently running thread.
// uint32_t thread_get_current_tid(void);

// (Conceptual) Clean up resources associated with a terminated thread.
// This might be called by a reaper task or when a process exits.
// void thread_cleanup(thread_t* thread);


#endif // KERNEL_THREAD_H
