#pragma once
#include <stdbool.h>

#define MAX_AGENTS 32

typedef enum {
    AGENT_TYPE_OPTIMIZER,
    AGENT_TYPE_SUGGESTER,
    AGENT_TYPE_MONITOR,
    AGENT_TYPE_HEALTH,
    AGENT_TYPE_SOCIAL
} agent_type_t;

typedef enum {
    AGENT_STATE_IDLE,
    AGENT_STATE_ACTIVE,
    AGENT_STATE_INTERACTING
} agent_state_t;

typedef struct agent {
    int id;
    agent_type_t type;
    agent_state_t state;
    const char* goal;
    int x, y;
    unsigned int color;
    float anim_phase;
    void (*on_interact)(struct agent*, void* user_data);
    void* user_data;
} agent_t;

typedef struct agent_manager {
    agent_t agents[MAX_AGENTS];
    int agent_count;
} agent_manager_t;

void agent_manager_init(agent_manager_t* mgr);
agent_t* agent_create(agent_manager_t* mgr, agent_type_t type, const char* goal, int x, int y, unsigned int color);
void agent_manager_tick(agent_manager_t* mgr);
void agent_render(const agent_t* agent); 