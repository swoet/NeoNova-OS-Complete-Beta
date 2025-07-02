#include "io_manager.h"
#include <windows.h>
#include <stdio.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")

static PDH_HQUERY hQuery = NULL;
static PDH_HCOUNTER hCounter = NULL;

float io_manager_get_usage(void) {
    if (!hQuery) {
        if (PdhOpenQuery(NULL, 0, &hQuery) != ERROR_SUCCESS) {
            printf("[IOManager] Failed to open PDH query\n");
            return 0.0f;
        }
        if (PdhAddCounter(hQuery, L"\\PhysicalDisk(_Total)\\Disk Bytes/sec", 0, &hCounter) != ERROR_SUCCESS) {
            printf("[IOManager] Failed to add PDH counter\n");
            return 0.0f;
        }
    }
    if (PdhCollectQueryData(hQuery) != ERROR_SUCCESS) {
        printf("[IOManager] Failed to collect PDH data\n");
        return 0.0f;
    }
    PDH_FMT_COUNTERVALUE value;
    if (PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, NULL, &value) != ERROR_SUCCESS) {
        printf("[IOManager] Failed to get PDH counter value\n");
        return 0.0f;
    }
    // Normalize to a 0.0-1.0 scale (arbitrary: 1GB/s = 1.0)
    double max_bytes_per_sec = 1e9;
    float usage = (float)(value.doubleValue / max_bytes_per_sec);
    if (usage > 1.0f) usage = 1.0f;
    return usage;
}

void io_manager_update(void) { float u = io_manager_get_usage(); printf("[IOManager] IO usage: %.2f\n", u); }
void io_manager_scale(void) {
    // Use Windows APIs to monitor and scale IO usage (e.g., SetProcessWorkingSetSize, IO throttling)
}
void io_manager_prioritize(void) {
    // Use SetPriorityClass or IO priority APIs to prioritize IO tasks
}
void io_manager_power_adjust(void) {
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && ps.ACLineStatus == 0) {
        // On battery: reduce IO activity
    }
} 