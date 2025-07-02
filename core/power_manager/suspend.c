#include "suspend.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <powrprof.h>
#pragma comment(lib, "PowrProf.lib")
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

void suspend_init(void) { printf("[Suspend] Initialized.\n"); }

void suspend_enter(void) {
#ifdef _WIN32
    printf("[Suspend] Entering suspend (Windows SetSuspendState).\n");
    SetSuspendState(FALSE, TRUE, FALSE);
#else
    printf("[Suspend] Entering suspend (POSIX).\n");
    // Try to suspend using system call (Linux: systemctl suspend, macOS: pmset)
    #if defined(__linux__)
    int ret = system("systemctl suspend");
    if (ret != 0) printf("[Suspend] systemctl suspend failed.\n");
    #elif defined(__APPLE__)
    int ret = system("pmset sleepnow");
    if (ret != 0) printf("[Suspend] pmset sleepnow failed.\n");
    #else
    printf("[Suspend] Suspend not supported on this POSIX platform.\n");
    #endif
#endif
}

void suspend_resume(void) {
    printf("[Suspend] Resume (handled by OS on wake).\n");
} 