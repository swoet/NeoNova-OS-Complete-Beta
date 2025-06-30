#include <kernel/include/process.h>
#include <kernel/include/kernel.h>  // For g_kernel_state, kernel_memset, kmalloc, kfree
#include <kernel/include/kdefs.h>   // For KERNEL_STACK_SIZE etc.
#include <kernel/include/memory.h>  // For kmalloc, kfree (though already in kernel.h effectively)
// #include <string.h> // For strncpy, strlen if used (kernel_memcpy/memset are custom)

// Initializes the process management subsystem.
// Creates the initial kernel process.
void process_system_init(void) {
    // The original code created a "kernel" process with kernel_main as entry.
    // This is unusual as kernel_main IS the kernel.
    // Typically, PID 0 is the idle process or a special kernel task/scheduler process.
    // The first "real" process is often "init" (PID 1).
    // Let's adapt: process_system_init will prepare for process creation.
    // The kernel_main will later create the 'init' process.
    // For now, let's just initialize the process list head.
    g_kernel_state.process_list_head = NULL;

    // Create a "kernel" or "idle" process with PID 0.
    // This process might not have a traditional entry point or user space.
    // It could be what the CPU falls back to when no other threads are ready.
    process_t* kernel_proc = (process_t*)kmalloc(sizeof(process_t));
    if (!kernel_proc) {
        // kernel_panic("Failed to allocate kernel process PCB!");
        return; // Critical failure
    }
    kernel_memset(kernel_proc, 0, sizeof(process_t));

    kernel_proc->pid = 0; // Kernel/Idle process is PID 0
    kernel_proc->parent_pid = 0; // No parent
    kernel_proc->state = PROCESS_STATE_RUNNING; // Conceptually always running or ready
    kernel_proc->priority = PRIORITY_IDLE; // Lowest priority
    // kernel_strncpy(kernel_proc->name, "kernel_idle", sizeof(kernel_proc->name) - 1);
    const char* kname = "kernel_idle";
    int i = 0;
    while (kname[i] && i < (sizeof(kernel_proc->name) - 1)) {
        kernel_proc->name[i] = kname[i];
        i++;
    }
    kernel_proc->name[i] = '\0';


    // Kernel process has its own address space (kernel space)
    // kernel_proc->address_space_handle = vmm_get_kernel_address_space(); // Conceptual

    kernel_proc->thread_count = 0; // No user-level threads initially, kernel might have internal threads
    kernel_proc->threads_list_head = NULL;
    kernel_proc->main_thread = NULL; // Kernel process doesn't have a "main" user thread like others.

    // Add to global process list
    kernel_proc->next_in_global_list = g_kernel_state.process_list_head;
    g_kernel_state.process_list_head = kernel_proc;

    // The original process_init also created the main_thread for the kernel_proc.
    // This is complex as the kernel is already running.
    // For PID 0, it's usually special. Let's assume it doesn't need a thread_create call here.
    // The scheduler will need an "idle_thread" TCB to switch to.

    // kprintf("Process system initialized. PID 0 (kernel_idle) created.\n");
}


process_t* process_create(const char* name, void (*entry_point)(void*), priority_t priority, uint32_t parent_pid_val) {
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if (!proc) {
        // kprintf("process_create: Failed to allocate memory for new process PCB.\n");
        return NULL;
    }
    kernel_memset(proc, 0, sizeof(process_t));

    proc->pid = g_kernel_state.next_available_pid++;
    proc->parent_pid = parent_pid_val; // TODO: Link to actual parent_t struct
    proc->state = PROCESS_STATE_NEW; // Will be set to READY by scheduler once main thread is added
    proc->priority = priority;

    // proc->address_space_handle = vmm_create_address_space(); // Conceptual VMM call
    // if (!proc->address_space_handle) { kfree(proc); return NULL; }
    // TODO: Setup new address space: copy kernel mappings, create user stack, map executable code etc.

    // Copy name
    if (name) {
        int i = 0;
        while (name[i] && i < (sizeof(proc->name) - 1)) {
            proc->name[i] = name[i];
            i++;
        }
        proc->name[i] = '\0';
    } else {
        // snprintf(proc->name, sizeof(proc->name), "proc_%d", proc->pid); // Conceptual
        const char* defname = "unnamed_proc";
        int i = 0;
        while (defname[i] && i < (sizeof(proc->name) - 1)) {
            proc->name[i] = defname[i];
            i++;
        }
        proc->name[i] = '\0';
    }

    proc->thread_count = 0;
    proc->threads_list_head = NULL;

    // Create the main thread for this process
    // The 'name' for the thread could be derived from process name, e.g., "init_main"
    char main_thread_name[sizeof(proc->name) + 6]; // "name_main\0"
    // snprintf(main_thread_name, sizeof(main_thread_name), "%s_main", proc->name);
    // Simple concatenation for now as no snprintf
    int k=0; while(proc->name[k] && k < sizeof(proc->name)-1) { main_thread_name[k] = proc->name[k]; k++; }
    const char* suffix = "_main"; int j=0; while(suffix[j] && k < sizeof(main_thread_name)-1) {main_thread_name[k++] = suffix[j++];}
    main_thread_name[k] = '\0';


    proc->main_thread = thread_create(proc, entry_point, NULL /* args for main thread? */, priority, main_thread_name, KERNEL_STACK_SIZE);

    if (!proc->main_thread) {
        // kprintf("process_create: Failed to create main thread for PID %d (%s).\n", proc->pid, proc->name);
        // vmm_destroy_address_space(proc->address_space_handle); // Conceptual VMM cleanup
        kfree(proc);
        return NULL;
    }

    // Add to global process list
    proc->next_in_global_list = g_kernel_state.process_list_head;
    g_kernel_state.process_list_head = proc;

    proc->state = PROCESS_STATE_READY; // Now it's ready to be scheduled

    // kprintf("Process created: PID %d (%s), Main TID %d.\n", proc->pid, proc->name, proc->main_thread->tid);
    return proc;
}

// Terminates the specified process by its PID.
int process_terminate(uint32_t pid, int exit_code) {
    // TODO: Implement process termination
    // 1. Find process by PID.
    // 2. Check permissions (can current process terminate target_pid?).
    // 3. Terminate all threads in the process (call thread_terminate for each).
    // 4. Change process state to PROCESS_STATE_TERMINATED or PROCESS_STATE_ZOMBIE.
    // 5. Store exit_code.
    // 6. Release process resources:
    //    - VMM: Destroy address space (vmm_destroy_address_space).
    //    - Free PCB and other process-specific kernel memory.
    //    - Close open file descriptors, IPC handles, etc.
    // 7. Notify parent process (if waiting) - involves semaphores or signals.
    // 8. Handle children (re-parent to init or terminate them - OS design choice).
    // kprintf("process_terminate: STUB PID=%u, exit_code=%d\n", pid, exit_code);
    (void)pid; (void)exit_code; // Suppress unused warnings for stub
    return -1; // Not implemented
}
