#include <kernel/include/scheduler.h>
#include <kernel/include/kernel.h>  // For g_kernel_state, kernel_memset
#include <kernel/include/thread.h>  // For thread_t (though already in scheduler.h)

// Initializes the scheduler.
void scheduler_init(void) {
    scheduler_t* sched = &g_kernel_state.scheduler_state;
    kernel_memset(sched, 0, sizeof(scheduler_t)); // Clears all queues to NULL, current_thread to NULL, ticks to 0

    // No threads to schedule initially.
    // The first thread (e.g., from init process) will be added by thread_create.
    // kprintf("Scheduler initialized.\n");
}

// Adds a thread to the appropriate ready queue based on its priority.
void scheduler_add_ready_thread(thread_t* thread) {
    if (!thread || thread->state != THREAD_STATE_READY) {
        // kprintf("scheduler_add_ready_thread: Invalid thread or state for TID %u\n", thread ? thread->tid : 0);
        return;
    }

    scheduler_t* sched = &g_kernel_state.scheduler_state;
    int priority_level = (int)thread->priority;

    if (priority_level < 0 || priority_level >= NUM_PRIORITY_LEVELS) {
        // kprintf("scheduler_add_ready_thread: Invalid priority level %d for TID %u\n", priority_level, thread->tid);
        priority_level = (int)PRIORITY_NORMAL; // Default to normal
    }

    // Add to the tail of the corresponding priority queue (simple append to head for now)
    // For fair round-robin within a priority, need to append to tail.
    // This simple version prepends, making it LIFO for same-priority.
    thread->next = sched->ready_queues[priority_level];
    // if (sched->ready_queues[priority_level]) {
    //    sched->ready_queues[priority_level]->prev = thread; // If doubly linked
    // }
    // thread->prev = NULL; // If doubly linked
    sched->ready_queues[priority_level] = thread;

    // kprintf("Scheduler: Added TID %u (Prio %d) to ready queue.\n", thread->tid, priority_level);
}

// The main scheduling function.
void schedule(void) {
    // This is where the context switch magic happens.
    // 1. Disable interrupts (critical section).
    // 2. If current_thread is still RUNNING (e.g., preempted by timer or yielded),
    //    - Save its CPU context (registers) into current_thread->cpu_state.
    //    - Change its state to THREAD_STATE_READY.
    //    - Add it back to the appropriate ready queue (scheduler_add_ready_thread).
    // 3. Select the next thread to run:
    //    - Iterate through ready_queues from highest priority to lowest.
    //    - Pick the first thread from the highest-priority non-empty queue.
    //    - If no ready threads, pick an "idle thread" (or loop with HLT).
    // 4. If a new thread is selected (next_thread != current_thread):
    //    - Update scheduler_state.current_thread = next_thread.
    //    - Change next_thread->state to THREAD_STATE_RUNNING.
    //    - Load its CPU context (registers) from next_thread->cpu_state.
    //    - (VMM) Switch to next_thread's address space if different.
    //    - Reset its time_slice_ticks.
    // 5. Re-enable interrupts.
    // 6. "Return" to the new thread's instruction pointer (often via IRET or special jump).

    // --- Simplified Conceptual Implementation (matching original code's logic) ---
    scheduler_t* sched = &g_kernel_state.scheduler_state;
    thread_t* next_thread_to_run = NULL;
    thread_t* previously_current_thread = sched->current_thread;

    // Atomically:
    // __asm__ volatile("cli"); // Disable interrupts

    // Find highest priority ready thread
    for (int prio = PRIORITY_CRITICAL; prio >= PRIORITY_IDLE; prio--) {
        if (sched->ready_queues[prio]) {
            next_thread_to_run = sched->ready_queues[prio];

            // Remove from ready queue (becomes the new head)
            sched->ready_queues[prio] = next_thread_to_run->next;
            next_thread_to_run->next = NULL; // Detach it
            // next_thread_to_run->prev = NULL; // If doubly linked
            break;
        }
    }

    if (next_thread_to_run) {
        sched->current_thread = next_thread_to_run;
        sched->current_thread->state = THREAD_STATE_RUNNING;
        // sched->current_thread->time_slice_ticks = 10; // Reset time slice (example)

        // If there was a previously running thread, and it wasn't the one just chosen
        // (e.g. it yielded or was preempted), put it back in the ready queue.
        if (previously_current_thread && previously_current_thread != sched->current_thread) {
            if (previously_current_thread->state == THREAD_STATE_RUNNING) { // Check if it was running (not blocked/terminated)
                previously_current_thread->state = THREAD_STATE_READY;
                scheduler_add_ready_thread(previously_current_thread);
            }
            // If it was blocked or terminated, it shouldn't be added back here.
        }

        // kprintf("Scheduler: Switching to TID %u (Prio %d). Prev TID %u.\n",
        //        sched->current_thread->tid, sched->current_thread->priority,
        //        previously_current_thread ? previously_current_thread->tid : 0);

        // TODO: Actual context switch using assembly.
        // This involves:
        //  - Saving registers of 'previously_current_thread' (if any, and if it's being switched out).
        //  - Switching page directory (CR3) if address spaces are different.
        //  - Loading registers of 'sched->current_thread'.
        //  - Jumping to sched->current_thread->cpu_state.rip.
        // For now, this is a placeholder. In a cooperative system without timer preemption yet,
        // this function might just set current_thread and expect the caller (yield) or
        // the next "return from interrupt" to handle the actual switch.
        // The original code structure implies this `schedule` function might be called
        // and then the system continues until a context switch is forced by an interrupt return.

    } else if (previously_current_thread && previously_current_thread->state == THREAD_STATE_RUNNING) {
        // No other thread to run, so the current one continues if it's still running.
        // This case might happen if schedule() is called but current thread is the only one ready.
        sched->current_thread = previously_current_thread;
        // kprintf("Scheduler: No other ready threads. Continuing TID %u.\n", sched->current_thread->tid);
    } else {
        // No threads ready to run, not even the previous one. This is an idle state.
        // A dedicated idle thread (PID 0's thread) should be scheduled here.
        // For now, if current_thread becomes NULL, kernel_main's HLT loop takes over.
        sched->current_thread = NULL; // Should point to idle thread
        // kprintf("Scheduler: No ready threads. System idle.\n");
    }

    // sched->quantum_ticks was in original global struct, moved to thread->time_slice_ticks
    sched->total_scheduler_ticks++;

    // __asm__ volatile("sti"); // Re-enable interrupts
}
