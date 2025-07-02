#include "process_manager.h"
#include <stdio.h>
#include <string.h>

void process_manager_init(process_table_t* pt) {
    memset(pt, 0, sizeof(*pt));
    pt->next_process_id = 1;
    printf("[ProcessManager] Initialized.\n");
}

bool mac_enforce_policy(const char* subject, const char* object, int action) {
    // Simple example: allow all for now, extend with real policy logic
    printf("[MAC] Enforcing policy: %s -> %s (action %d)\n", subject, object, action);
    return true;
}

void sandbox_process(process_t* proc) {
    proc->sandboxed = true;
    printf("[Sandbox] Process %d ('%s') sandboxed.\n", proc->id, proc->name);
}

int process_create(process_table_t* pt, const char* name, int app_id) {
    if (pt->process_count >= MAX_PROCESSES) return -1;
    if (!mac_enforce_policy(name, "system", 1)) return -1;
    process_t* proc = &pt->processes[pt->process_count++];
    proc->id = pt->next_process_id++;
    strncpy(proc->name, name, sizeof(proc->name)-1);
    proc->name[sizeof(proc->name)-1] = '\0';
    proc->state = PROC_RUNNING;
    proc->app_id = app_id;
    proc->window_id = -1;
    sandbox_process(proc);
    printf("[ProcessManager] Created process %d for app %d ('%s')\n", proc->id, app_id, proc->name);
    return proc->id;
}

int process_destroy(process_table_t* pt, int process_id) {
    for (int i = 0; i < pt->process_count; ++i) {
        if (pt->processes[i].id == process_id) {
            printf("[ProcessManager] Destroyed process %d ('%s')\n", process_id, pt->processes[i].name);
            for (int j = i; j < pt->process_count - 1; ++j) pt->processes[j] = pt->processes[j+1];
            pt->process_count--;
            return 0;
        }
    }
    return -1;
}

void process_schedule(process_table_t* pt) {
    // Simple round-robin scheduler
    for (int i = 0; i < pt->process_count; ++i) {
        if (pt->processes[i].state == PROC_RUNNING) {
            if (!mac_enforce_policy(pt->processes[i].name, "system", 2)) continue;
            printf("[ProcessManager] Scheduled process %d ('%s')\n", pt->processes[i].id, pt->processes[i].name);
            // Simulate running the process
        }
    }
}

void process_list(process_table_t* pt) {
    printf("[ProcessManager] Process list (%d total):\n", pt->process_count);
    for (int i = 0; i < pt->process_count; ++i) {
        printf("  Process %d: '%s' (app %d) state %d\n", pt->processes[i].id, pt->processes[i].name, pt->processes[i].app_id, pt->processes[i].state);
    }
} 