#include "cpu_manager.h"
#include <windows.h>
#include <stdio.h>

static ULONGLONG last_idle = 0, last_kernel = 0, last_user = 0;
static float last_cpu_usage = 0.0f;

float cpu_manager_get_usage(void) {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return last_cpu_usage; // fallback
    }
    ULONGLONG idle = ((ULONGLONG)idleTime.dwHighDateTime << 32) | idleTime.dwLowDateTime;
    ULONGLONG kernel = ((ULONGLONG)kernelTime.dwHighDateTime << 32) | kernelTime.dwLowDateTime;
    ULONGLONG user = ((ULONGLONG)userTime.dwHighDateTime << 32) | userTime.dwLowDateTime;
    ULONGLONG sys = (kernel - last_kernel) + (user - last_user);
    ULONGLONG idle_diff = idle - last_idle;
    float usage = 0.0f;
    if (sys > 0) usage = 1.0f - ((float)idle_diff / (float)sys);
    last_idle = idle; last_kernel = kernel; last_user = user;
    last_cpu_usage = usage;
    return usage;
}

void cpu_manager_update(void) { float u = cpu_manager_get_usage(); printf("[CPUManager] CPU usage: %.2f\n", u); }
void cpu_manager_scale(void) {
    // Use Windows Job Objects or SetPriorityClass to scale CPU usage
}
void cpu_manager_prioritize(void) {
    // Use SetThreadPriority or SetPriorityClass to prioritize critical CPU tasks
}
void cpu_manager_power_adjust(void) {
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && ps.ACLineStatus == 0) {
        // On battery: reduce CPU frequency (if supported)
    }
} 