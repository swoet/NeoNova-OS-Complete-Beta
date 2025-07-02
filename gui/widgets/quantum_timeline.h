#pragma once
#include <stdbool.h>

#define MAX_TIMELINE_STATES 16
#define MAX_TIMELINE_BRANCHES 4

typedef struct timeline_state {
    int state_id;
    char label[64];
    char snapshot[256];
} timeline_state_t;

typedef struct quantum_timeline {
    int window_id;
    int state_count;
    timeline_state_t states[MAX_TIMELINE_STATES];
    int current_state;
    int branch_count;
    struct quantum_timeline* branches[MAX_TIMELINE_BRANCHES];
} quantum_timeline_t;

quantum_timeline_t* quantum_timeline_create(int window_id);
void quantum_timeline_save(quantum_timeline_t* timeline, const char* label, const char* snapshot);
void quantum_timeline_rewind(quantum_timeline_t* timeline, int state_id);
quantum_timeline_t* quantum_timeline_branch(quantum_timeline_t* timeline, const char* label);
void quantum_timeline_render(const quantum_timeline_t* timeline); 