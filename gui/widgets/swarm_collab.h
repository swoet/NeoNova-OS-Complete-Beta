#pragma once
#include <stdbool.h>

#define MAX_SWARM_USERS 16
#define MAX_SWARM_DESKTOPS 8

typedef enum {
    SWARM_STATE_IDLE,
    SWARM_STATE_ACTIVE,
    SWARM_STATE_MERGED,
    SWARM_STATE_FEDERATED
} swarm_state_t;

typedef struct swarm_user {
    int user_id;
    char name[32];
    bool ai_enabled;
} swarm_user_t;

typedef struct swarm_session {
    int session_id;
    swarm_user_t users[MAX_SWARM_USERS];
    int user_count;
    int desktop_ids[MAX_SWARM_DESKTOPS];
    int desktop_count;
    bool ai_federated;
    swarm_state_t state;
} swarm_session_t;

void swarm_collab_init(swarm_session_t* session, int session_id);
void swarm_collab_add_user(swarm_session_t* session, int user_id, const char* name, bool ai_enabled);
void swarm_collab_merge_desktops(swarm_session_t* session, int desktop_id);
void swarm_collab_federate_ai(swarm_session_t* session);
void swarm_collab_render(const swarm_session_t* session); 