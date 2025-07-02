// real-time scheduler

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "real_time.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>

// Resource usage structure
typedef struct {
    uint32_t cpu_usage;
    uint32_t ram_usage;
    uint32_t gpu_usage;
    uint32_t io_usage;
    // Add more fields as needed
} resource_stats_t;

// Real process structure for scheduling
#define MAX_PROCESSES 128

typedef struct {
    int pid;
    int priority; // 0 (highest) to 31 (lowest)
    uint64_t deadline; // For real-time tasks (ms since boot)
    uint64_t exec_time; // ms used in current period
    int is_realtime;
    int is_foreground;
    int is_running;
    HANDLE hProcess;
} sched_process_t;

static sched_process_t proc_table[MAX_PROCESSES] = {0};
static int proc_count = 0;

// System resource stats (Windows API)
static resource_stats_t system_stats = {0};

// Update resource stats
void update_resource_stats(void) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    system_stats.ram_usage = (uint32_t)((memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1024 / 1024);
    // CPU usage: use GetSystemTimes
    static FILETIME prevIdle = {0}, prevKernel = {0}, prevUser = {0};
    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);
    ULONGLONG sys = (FileTimeToULL(kernel) + FileTimeToULL(user)) - (FileTimeToULL(prevKernel) + FileTimeToULL(prevUser));
    ULONGLONG idleDiff = FileTimeToULL(idle) - FileTimeToULL(prevIdle);
    system_stats.cpu_usage = (uint32_t)(100 - (100.0 * idleDiff / sys));
    prevIdle = idle; prevKernel = kernel; prevUser = user;
    // TODO: GPU/IO usage (platform-specific)
}

// Dynamic scaling logic
void scale_resources(void) {
    // Example: if CPU > 90% or RAM > 90%, deprioritize background
    if (system_stats.cpu_usage > 90 || system_stats.ram_usage > 90) {
        defer_background_loads();
    }
}

// Prioritize active/foreground processes
void prioritize_processes(void) {
    for (int i = 0; i < proc_count; ++i) {
        if (proc_table[i].is_foreground) {
            SetPriorityClass(proc_table[i].hProcess, HIGH_PRIORITY_CLASS);
        } else {
            SetPriorityClass(proc_table[i].hProcess, BELOW_NORMAL_PRIORITY_CLASS);
        }
    }
}

// Defer background loads
void defer_background_loads(void) {
    for (int i = 0; i < proc_count; ++i) {
        if (!proc_table[i].is_foreground) {
            SuspendThread(proc_table[i].hProcess);
        }
    }
}

// Adjust for power/hardware availability
void adjust_for_power(void) {
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps)) {
        if (ps.ACLineStatus == 0) { // On battery
            for (int i = 0; i < proc_count; ++i) {
                SetPriorityClass(proc_table[i].hProcess, IDLE_PRIORITY_CLASS);
            }
        }
    }
}

// Error handling and recovery
void recover_from_resource_failure(void) {
    // Try to free memory, restart lowest-priority process, or log and notify
    printf("[Scheduler] Resource failure detected. Attempting recovery...\n");
    for (int i = proc_count-1; i >= 0; --i) {
        if (!proc_table[i].is_foreground) {
            TerminateProcess(proc_table[i].hProcess, 1);
            printf("[Scheduler] Terminated background process %d to recover resources.\n", proc_table[i].pid);
            return;
        }
    }
    printf("[Scheduler] No background process to terminate. Notifying admin.\n");
}

// Helper for FILETIME to ULONGLONG
static ULONGLONG FileTimeToULL(FILETIME ft) {
    return (((ULONGLONG)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

// Add process to scheduler
int scheduler_add_process(int pid, HANDLE hProcess, int priority, int is_realtime, uint64_t deadline, int is_foreground) {
    if (proc_count >= MAX_PROCESSES) return -1;
    proc_table[proc_count] = (sched_process_t){
        .pid = pid,
        .priority = priority,
        .deadline = deadline,
        .exec_time = 0,
        .is_realtime = is_realtime,
        .is_foreground = is_foreground,
        .is_running = 1,
        .hProcess = hProcess
    };
    ++proc_count;
    return 0;
}

// Real-time scheduling tick
void scheduler_tick(void) {
    update_resource_stats();
    scale_resources();
    prioritize_processes();
    adjust_for_power();
    // Real-time: check deadlines
    uint64_t now = GetTickCount64();
    for (int i = 0; i < proc_count; ++i) {
        if (proc_table[i].is_realtime && proc_table[i].deadline < now) {
            printf("[Scheduler] Process %d missed deadline!\n", proc_table[i].pid);
            recover_from_resource_failure();
        }
    }
}
