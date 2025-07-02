#include "governor.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// GUIDs for common power schemes
static const char* GUID_HIGH_PERF = "8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c";
static const char* GUID_BALANCED = "381b4222-f694-41f0-9685-ff5bb260df2e";
static const char* GUID_POWER_SAVER = "a1841308-3541-4fab-bc81-f71556f20b4a";

void governor_init(void) {
    printf("[Governor] Initialized.\n");
    system("powercfg /getactivescheme");
}

void governor_adjust(void) {
    printf("[Governor] Adjusting power plan.\n");
    // Example: switch to balanced
    int ret = system("powercfg /setactive " GUID_BALANCED);
    if (ret == 0) printf("[Governor] Switched to Balanced power plan.\n");
    else printf("[Governor] Failed to switch power plan.\n");
} 