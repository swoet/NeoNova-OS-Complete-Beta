#ifndef PROCESS_SCHEDULER_H
#define PROCESS_SCHEDULER_H

#include <stdint.h> // For int types, uint64_t for time
#include "kernel/memory_manager.h" // For VirtualAddress (e.g., instruction pointer)

// --- Conceptual Types ---

typedef int32_t ProcessID; // PID
typedef int32_t ThreadID;  // TID

#define KERNEL_STACK_SIZE (PAGE_SIZE * 2) // Example kernel stack size for a thread

typedef enum {
    PROCESS_STATE_NEW,      // Just created, not yet ready to run
    PROCESS_STATE_READY,    // Ready to run, waiting for CPU
    PROCESS_STATE_RUNNING,  // Currently executing on a CPU
    PROCESS_STATE_WAITING,  // Waiting for an event (e.g., I/O, semaphore)
    PROCESS_STATE_TERMINATED // Execution finished
} ProcessState;

// Basic Process Control Block (PCB) / Thread Control Block (TCB) - Highly Simplified
typedef struct ProcessControlBlock {
    ProcessID pid;
    ThreadID main_tid; // Each process has at least one thread
    ProcessState state;

    // void* address_space_handle; // Handle from VMM for this process's virtual address space
    // uintptr_t kernel_stack_ptr; // Pointer to top of kernel stack for this process/thread
    // uintptr_t user_stack_ptr;   // Pointer to top of user stack

    // Registers context (architecture-specific, usually a struct)
    // void* registers_context;
    // VirtualAddress instruction_pointer; // Next instruction to execute

    uint64_t creation_time;
    uint64_t total_cpu_time_ns; // Total CPU time consumed

    int priority; // Scheduling priority

    // struct ProcessControlBlock* parent;
    // struct ProcessControlBlock* children[MAX_CHILDREN];
    // int child_count;

    // Open file descriptors, IPC handles, etc.
    // FileHandle* open_files[MAX_OPEN_FILES];

    char name[64]; // Process name for debugging/identification
} Process; // Using Process as a shorthand for PCB/main TCB


// --- Process & Thread Management API (Conceptual) ---

// Initialize the Process Manager and Scheduler.
// void ProcessManager_Initialize();

// Create a new process.
// entry_point: virtual address of the first instruction to run in user mode.
// name: descriptive name for the process.
// Returns PID of the new process, or <0 on error.
// ProcessID Process_Create(const char* name, VirtualAddress entry_point, void* parent_address_space_handle /* or NULL for new */);

// Terminate the current process.
// void Process_Exit(int exit_code);

// Terminate a specific process by PID.
// int Process_Terminate(ProcessID pid, int exit_code);

// Wait for a child process to terminate (similar to waitpid).
// ProcessID Process_Wait(ProcessID pid, int* status_ptr, int options);

// Get the PID of the currently running process.
// ProcessID Process_GetCurrentPID();

// Get a pointer to the PCB of the current process.
// Process* Process_GetCurrent();

// Yield the CPU to another process/thread (voluntary context switch).
// void Process_Yield();

// Put the current process to sleep for a duration (in milliseconds).
// void Process_Sleep(uint32_t milliseconds);

// Change the priority of a process.
// int Process_SetPriority(ProcessID pid, int priority);


// --- Scheduler API (Conceptual - often internal to Process Manager) ---

// This function is called by the timer interrupt to trigger a context switch if needed.
// void Scheduler_Tick();

// Explicitly invoke the scheduler to pick the next process to run.
// (Might save current context and load next context).
// void Scheduler_Schedule();

// Add a process to the ready queue.
// void Scheduler_AddReadyProcess(Process* pcb);

// Remove a process from scheduling (e.g., if waiting or terminated).
// void Scheduler_RemoveProcess(Process* pcb);


// --- Thread Management API (Conceptual - if supporting kernel/user threads) ---
// ThreadID Thread_Create(ProcessID pid, VirtualAddress entry_point, uintptr_t stack_ptr, uint32_t flags);
// void Thread_Exit(int exit_code);
// ThreadID Thread_GetCurrentTID();
// void Thread_Join(ThreadID tid, int** retval_ptr);
// void Thread_Detach(ThreadID tid);
// void Thread_Yield(); // Yield within the same process to other threads


// --- Process Manager Module API (as a KernelModule) ---
// extern KernelModule ProcessManagerModule; // Defined in process_manager.c

#endif // PROCESS_SCHEDULER_H
