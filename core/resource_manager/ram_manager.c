#include "ram_manager.h"
#include <windows.h>
#include <stdio.h>
#include <psapi.h>

float ram_manager_get_usage(void) {
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        return (float)(mem.ullTotalPhys - mem.ullAvailPhys) / (float)mem.ullTotalPhys;
    }
    return 0.0f;
}

void ram_manager_update(void) { float u = ram_manager_get_usage(); printf("[RAMManager] RAM usage: %.2f\n", u); }
void ram_manager_scale(void) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalPhys = memInfo.ullTotalPhys;
    DWORDLONG availPhys = memInfo.ullAvailPhys;
    double usage = 1.0 - ((double)availPhys / (double)totalPhys);
    if (usage > 0.9) {
        // Throttle RAM-intensive tasks (integrate with process manager)
    }
}
void ram_manager_prioritize(void) {
    // Prioritize critical RAM tasks (integrate with scheduler)
}
void ram_manager_power_adjust(void) {
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && ps.ACLineStatus == 0) {
        // On battery: reduce RAM usage (e.g., trim working sets)
    }
} 