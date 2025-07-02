// Built-in hypervisor/container support

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

// Minimal hypervisor/container manager
int launch_container(const char* cmd) {
#ifdef _WIN32
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    if (!CreateProcessA(NULL, (LPSTR)cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        printf("[Virtualization] Failed to launch container: %s\n", cmd);
        return -1;
    }
    printf("[Virtualization] Launched container (PID=%lu)\n", pi.dwProcessId);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
#else
    pid_t pid = fork();
    if (pid == 0) {
        // Child: new namespace (Linux only)
        #ifdef __linux__
        unshare(CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET);
        #endif
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(1);
    } else if (pid > 0) {
        printf("[Virtualization] Launched container (PID=%d)\n", pid);
        int status = 0;
        waitpid(pid, &status, 0);
        return status;
    } else {
        printf("[Virtualization] Failed to fork container.\n");
        return -1;
    }
#endif
}
