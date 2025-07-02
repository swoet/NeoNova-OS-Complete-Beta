#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include <stdbool.h>
#define MAX_PROCESSES 64

typedef enum {
    PROC_RUNNING,
    PROC_WAITING,
    PROC_STOPPED
} process_state_t;

typedef struct process {
    int id;
    char name[64];
    process_state_t state;
    int app_id; // Associated app
    int window_id; // Associated window
    bool sandboxed;
    // Add more fields as needed (registers, stack, etc.)
} process_t;

typedef struct process_table {
    process_t processes[MAX_PROCESSES];
    int process_count;
    int next_process_id;
} process_table_t;

void process_manager_init(process_table_t* pt);
int process_create(process_table_t* pt, const char* name, int app_id);
int process_destroy(process_table_t* pt, int process_id);
void process_schedule(process_table_t* pt);
void process_list(process_table_t* pt);
bool mac_enforce_policy(const char* subject, const char* object, int action);
void sandbox_process(process_t* proc);

#endif // PROCESS_MANAGER_H 