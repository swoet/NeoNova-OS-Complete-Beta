#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include <kernel/include/thread.h> // For thread_t structure
#include <kernel/include/types.h>  // For priority_t (PRIORITY_CRITICAL is max index + 1 for array size)
#include <stdint.h>

#define NUM_PRIORITY_LEVELS (PRIORITY_CRITICAL + 1) // Number of priority queues

// Scheduler Queues and State
// This structure holds all data relevant to the scheduler's operation.
typedef struct {
    // Ready Queues: An array of linked lists, one for each priority level.
    // Threads in these queues are ready to run.
    thread_t* ready_queues[NUM_PRIORITY_LEVELS];

    // Blocked Queue: A single linked list for all threads that are currently blocked
    // (e.g., waiting for I/O, a mutex, or explicitly slept).
    // A more advanced scheduler might have multiple blocked queues based on wait reasons.
    thread_t* blocked_queue_head;
    // thread_t* blocked_queue_tail; // Optional for faster append

    thread_t* current_thread;   // Pointer to the TCB of the currently executing thread

    // uint64_t quantum_ticks_remaining; // Ticks remaining for the current thread's time slice
                                     // (Managed by thread_t->time_slice_ticks now)
    uint64_t total_scheduler_ticks; // Total number of scheduler ticks since boot (system uptime)

    // Lock for scheduler data structures (e.g., k_spinlock_t scheduler_lock);
    // int scheduling_disabled_count; // To disable/enable scheduling (e.g., for critical sections)

} scheduler_t;


// --- Scheduler Function Declarations ---

// Initializes the scheduler.
// Sets up ready queues, blocked queue, and initial scheduler state.
// Called once during kernel initialization.
void scheduler_init(void);

// The main scheduling function.
// This function is called to select the next thread to run and perform a context switch.
// It's typically invoked by a timer interrupt (for preemption) or when a thread yields or blocks.
// This function will not return in the context of the calling thread if a switch occurs.
void schedule(void);

// Adds a thread to the appropriate ready queue based on its priority.
void scheduler_add_ready_thread(thread_t* thread);

// Removes a thread from whichever scheduler queue it might be in.
// Used when a thread is terminated or moved between states.
// void scheduler_remove_thread(thread_t* thread);

// (Conceptual) Called by the system timer interrupt.
// Decrements current thread's time slice, and if expired, calls schedule().
// void scheduler_timer_tick(void);

// (Conceptual) Disable preemptive scheduling.
// void scheduler_disable_preemption(void);

// (Conceptual) Enable preemptive scheduling.
// void scheduler_enable_preemption(void);


#endif // KERNEL_SCHEDULER_H
