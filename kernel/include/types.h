#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <stdint.h> // For sized integer types
#include <stddef.h> // For size_t, NULL

// Process states
typedef enum {
    PROCESS_STATE_NEW = 0,   // Newly created, not yet managed by scheduler
    PROCESS_STATE_READY,     // Ready to run, waiting for CPU
    PROCESS_STATE_RUNNING,   // Currently executing
    PROCESS_STATE_BLOCKED,   // Waiting for an event (e.g., I/O, semaphore)
    PROCESS_STATE_TERMINATED,// Execution finished, resources pending cleanup
    PROCESS_STATE_ZOMBIE     // Execution finished, resources cleaned, waiting for parent to collect status
} process_state_t;

// Thread states
typedef enum {
    THREAD_STATE_NEW = 0,    // Newly created, not yet managed by scheduler
    THREAD_STATE_READY,      // Ready to run, waiting for CPU
    THREAD_STATE_RUNNING,    // Currently executing
    THREAD_STATE_BLOCKED,    // Waiting for an event
    THREAD_STATE_TERMINATED  // Execution finished
} thread_state_t;

// Priority levels for scheduling
typedef enum {
    PRIORITY_IDLE = 0,       // For idle thread/process when nothing else is ready
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_REALTIME = 4,   // Higher than normal, for time-sensitive tasks
    PRIORITY_CRITICAL = 5    // Highest priority, for critical kernel tasks
} priority_t;

// Basic Boolean type if not using <stdbool.h> directly in all files
// For kernel code, sometimes a custom one is preferred or stdbool is used.
// Let's assume stdbool.h will be included where needed, or define a KBOOL here.
// typedef uint8_t KBOOL;
// #define KTRUE  1
// #define KFALSE 0

#endif // KERNEL_TYPES_H
