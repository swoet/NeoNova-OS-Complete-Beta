#include "quantum_timeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

quantum_timeline_t* quantum_timeline_create(int window_id) {
    quantum_timeline_t* t = (quantum_timeline_t*)malloc(sizeof(quantum_timeline_t));
    t->window_id = window_id;
    t->state_count = 0;
    t->current_state = -1;
    t->branch_count = 0;
    memset(t->states, 0, sizeof(t->states));
    memset(t->branches, 0, sizeof(t->branches));
    return t;
}

void quantum_timeline_save(quantum_timeline_t* timeline, const char* label, const char* snapshot) {
    if (timeline->state_count < MAX_TIMELINE_STATES) {
        timeline_state_t* s = &timeline->states[timeline->state_count];
        s->state_id = timeline->state_count;
        strncpy(s->label, label, sizeof(s->label)-1);
        s->label[sizeof(s->label)-1] = '\0';
        strncpy(s->snapshot, snapshot, sizeof(s->snapshot)-1);
        s->snapshot[sizeof(s->snapshot)-1] = '\0';
        timeline->current_state = timeline->state_count;
        timeline->state_count++;
        printf("[Timeline] Saved state %d: %s\n", s->state_id, s->label);
    }
}

void quantum_timeline_rewind(quantum_timeline_t* timeline, int state_id) {
    if (state_id >= 0 && state_id < timeline->state_count) {
        timeline->current_state = state_id;
        printf("[Timeline] Rewound to state %d: %s\n", state_id, timeline->states[state_id].label);
    }
}

quantum_timeline_t* quantum_timeline_branch(quantum_timeline_t* timeline, const char* label) {
    if (timeline->branch_count < MAX_TIMELINE_BRANCHES) {
        quantum_timeline_t* branch = quantum_timeline_create(timeline->window_id);
        char branch_label[64];
        snprintf(branch_label, sizeof(branch_label), "Branch: %s", label);
        quantum_timeline_save(branch, branch_label, "Initial branch state");
        timeline->branches[timeline->branch_count++] = branch;
        printf("[Timeline] Created branch %d: %s\n", timeline->branch_count-1, label);
        return branch;
    }
    return NULL;
}

void quantum_timeline_render(const quantum_timeline_t* timeline) {
    printf("[Timeline] Window %d | States: %d | Current: %d\n", timeline->window_id, timeline->state_count, timeline->current_state);
    for (int i = 0; i < timeline->state_count; ++i) {
        printf("  State %d: %s %s\n", i, timeline->states[i].label, (i == timeline->current_state) ? "[CURRENT]" : "");
    }
    for (int b = 0; b < timeline->branch_count; ++b) {
        printf("  Branch %d:\n", b);
        quantum_timeline_render(timeline->branches[b]);
    }
} 