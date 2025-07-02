#include "agent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void agent_manager_init(agent_manager_t* mgr) {
    mgr->agent_count = 0;
    memset(mgr->agents, 0, sizeof(mgr->agents));
}

agent_t* agent_create(agent_manager_t* mgr, agent_type_t type, const char* goal, int x, int y, unsigned int color) {
    if (mgr->agent_count < MAX_AGENTS) {
        agent_t* a = &mgr->agents[mgr->agent_count];
        a->id = mgr->agent_count;
        a->type = type;
        a->state = AGENT_STATE_IDLE;
        a->goal = goal ? strdup(goal) : NULL;
        a->x = x; a->y = y;
        a->color = color;
        a->anim_phase = 0.0f;
        a->on_interact = NULL;
        a->user_data = NULL;
        mgr->agent_count++;
        return a;
    }
    return NULL;
}

void agent_manager_tick(agent_manager_t* mgr) {
    for (int i = 0; i < mgr->agent_count; ++i) {
        agent_t* a = &mgr->agents[i];
        a->anim_phase += 0.1f;
        if (a->anim_phase > 1.0f) a->anim_phase = 0.0f;
        if (a->state == AGENT_STATE_IDLE && (rand() % 10) == 0) {
            a->state = AGENT_STATE_ACTIVE;
            printf("[Agent] %d (%s) activated\n", a->id, a->goal);
        } else if (a->state == AGENT_STATE_ACTIVE && (rand() % 10) == 0) {
            a->state = AGENT_STATE_IDLE;
            printf("[Agent] %d (%s) idling\n", a->id, a->goal);
        }
    }
}

void agent_render(const agent_t* agent) {
    const char* type_str = "?";
    switch (agent->type) {
        case AGENT_TYPE_OPTIMIZER: type_str = "optimizer"; break;
        case AGENT_TYPE_SUGGESTER: type_str = "suggester"; break;
        case AGENT_TYPE_MONITOR: type_str = "monitor"; break;
        case AGENT_TYPE_HEALTH: type_str = "health"; break;
        case AGENT_TYPE_SOCIAL: type_str = "social"; break;
    }
    const char* state_str = "?";
    switch (agent->state) {
        case AGENT_STATE_IDLE: state_str = "idle"; break;
        case AGENT_STATE_ACTIVE: state_str = "active"; break;
        case AGENT_STATE_INTERACTING: state_str = "interacting"; break;
    }
    printf("[Agent] %d %s (%s) at (%d,%d) color=#%06X phase=%.2f\n", agent->id, type_str, state_str, agent->x, agent->y, agent->color, agent->anim_phase);
    printf("  Goal: %s\n", agent->goal ? agent->goal : "(none)");
} 