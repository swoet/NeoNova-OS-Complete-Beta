#include <kernel/include/thread.h>
#include <kernel/include/process.h> // For process_t argument to thread_create
#include <kernel/include/kernel.h>  // For g_kernel_state, kernel_memset, kmalloc, kfree
#include <kernel/include/kdefs.h>   // For KERNEL_STACK_SIZE
#include <kernel/include/scheduler.h> // For scheduler_add_ready_thread

// Creates a new thread within the given process.
thread_t* thread_create(struct process* proc, void (*entry_point)(void*), void* args, priority_t priority, const char* name, size_t stack_size) {
    if (!proc) return NULL; // Must have a parent process

    thread_t* thread = (thread_t*)kmalloc(sizeof(thread_t));
    if (!thread) {
        // kprintf("thread_create: Failed to allocate TCB for thread in PID %d\n", proc->pid);
        return NULL;
    }
    kernel_memset(thread, 0, sizeof(thread_t));

    thread->tid = g_kernel_state.next_available_tid++;
    thread->pid = proc->pid;
    thread->state = THREAD_STATE_NEW; // Will be set to READY by scheduler
    thread->priority = priority;
    thread->time_slice_ticks = 10; // Default time slice (e.g., 10 scheduler ticks)
    thread->total_cpu_time_ns = 0;

    thread->kernel_stack_size = (stack_size == 0) ? KERNEL_STACK_SIZE : stack_size;
    thread->kernel_stack_base = kmalloc(thread->kernel_stack_size);
    if (!thread->kernel_stack_base) {
        // kprintf("thread_create: Failed to allocate kernel stack for TID %d (PID %d)\n", thread->tid, proc->pid);
        kfree(thread);
        return NULL;
    }
    kernel_memset(thread->kernel_stack_base, 0, thread->kernel_stack_size); // Zero the stack for security/predictability

    // Initialize CPU state for the new thread
    kernel_memset(&thread->cpu_state, 0, sizeof(cpu_state_t));

    // Setup stack for initial context switch:
    // The stack pointer (RSP) should point to the top of the allocated kernel stack.
    // The entry_point (RIP) is where the thread will start execution.
    // Arguments to entry_point are often passed via registers (e.g., RDI, RSI for x86-64 SysV ABI)
    // or pushed onto the stack before the "first call".
    // For simplicity, we're setting RIP and RSP. A real context switch frame is more complex.

    thread->cpu_state.rip = (uint64_t)entry_point;
    // Stack grows downwards. RSP should be at the top of the stack.
    // Some space might be reserved at the very top for arguments or return address from a stub.
    thread->cpu_state.rsp = (uint64_t)thread->kernel_stack_base + thread->kernel_stack_size - sizeof(uintptr_t); // Point to top, leave space for one word

    // Set RDI for the first argument (args). This is x86-64 SysV ABI specific.
    thread->cpu_state.rdi = (uint64_t)args;

    // Set default RFLAGS (e.g., interrupts enabled IF_FLAG = 0x200).
    // The original code used 0x202.
    thread->cpu_state.rflags = 0x202;

    // Set CS and SS (Code Segment, Stack Segment). For x86-64 kernel mode, these are usually
    // fixed selectors defined by GDT (Global Descriptor Table).
    // These would be set by architecture-specific code. Example values:
    // thread->cpu_state.cs = KERNEL_CODE_SELECTOR; // e.g., 0x08
    // thread->cpu_state.ss = KERNEL_DATA_SELECTOR; // e.g., 0x10
    // For now, leave them 0, assuming they are set up correctly by context switch or default.

    // Add to parent process's thread list
    thread->next = proc->threads_list_head;
    // thread->prev = NULL; // If using doubly linked list for process threads
    // if (proc->threads_list_head) proc->threads_list_head->prev = thread;
    proc->threads_list_head = thread;
    proc->thread_count++;

    // Add to scheduler's ready queue
    thread->state = THREAD_STATE_READY; // Now ready to be scheduled
    scheduler_add_ready_thread(thread);

    // kprintf("Thread created: TID %d (PID %d, Name: %s) Entry: 0x%llx Stack: 0x%llx\n",
    //         thread->tid, thread->pid, name ? name : "unnamed", thread->cpu_state.rip, thread->cpu_state.rsp);
    return thread;
}

void thread_terminate(uint32_t tid) {
    // TODO: Implement thread termination
    // 1. Find thread by TID.
    // 2. If terminating current thread, need to switch to another thread first (cannot return).
    // 3. Remove from scheduler queues.
    // 4. Change state to THREAD_STATE_TERMINATED.
    // 5. Free kernel stack (kfree(thread->kernel_stack_base)).
    // 6. Free TCB (kfree(thread)).
    // 7. Update parent process's thread list and count.
    // 8. If it's the main thread of a process, the process might need to terminate.
    // kprintf("thread_terminate: STUB TID=%u\n", tid);
    (void)tid;
}

void thread_block(void /* const char* reason, void* wait_object */) {
    // TODO: Implement thread blocking
    // 1. Get current thread.
    // 2. Set state to THREAD_STATE_BLOCKED.
    // 3. (Optional) Store reason/wait_object in TCB.
    // 4. Add to scheduler's blocked queue (or a specific wait queue).
    // 5. Call schedule() to switch to another thread.
    // kprintf("thread_block: STUB CurrentTID=%u\n", thread_get_current_tid());
}

int thread_unblock(thread_t* thread) {
    if (!thread) return -1;
    // TODO: Implement thread unblocking
    // 1. Check if thread is actually blocked.
    // 2. Remove from blocked queue.
    // 3. Set state to THREAD_STATE_READY.
    // 4. Add to scheduler's ready queue (scheduler_add_ready_thread).
    // kprintf("thread_unblock: STUB TID=%u\n", thread->tid);
    return -1; // Not implemented
}

int thread_set_priority(uint32_t tid, priority_t new_priority) {
    // TODO: Implement priority change
    // 1. Find thread by TID.
    // 2. Update thread->priority.
    // 3. If thread is in a ready queue, might need to move it to a different priority queue.
    // kprintf("thread_set_priority: STUB TID=%u, NewPrio=%d\n", tid, new_priority);
    (void)tid; (void)new_priority;
    return -1; // Not implemented
}

void thread_yield(void) {
    // If kernel is initialized, call the scheduler.
    // This allows other threads of the same or higher priority to run.
    if (g_kernel_state.is_initialized) {
        // The current thread is still READY, just giving up its current time slice.
        // The scheduler should handle putting it back into the ready queue.
        // kprintf("thread_yield: CurrentTID=%u yielding.\n", thread_get_current_tid());
        schedule();
    }
}
