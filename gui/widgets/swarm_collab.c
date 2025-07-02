#include "swarm_collab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swarm_collab_init(swarm_session_t* session, int session_id) {
    session->session_id = session_id;
    session->user_count = 0;
    session->desktop_count = 0;
    session->ai_federated = false;
    session->state = SWARM_STATE_IDLE;
    memset(session->users, 0, sizeof(session->users));
    memset(session->desktop_ids, 0, sizeof(session->desktop_ids));
}

void swarm_collab_add_user(swarm_session_t* session, int user_id, const char* name, bool ai_enabled) {
    if (session->user_count < MAX_SWARM_USERS) {
        swarm_user_t* u = &session->users[session->user_count++];
        u->user_id = user_id;
        strncpy(u->name, name, sizeof(u->name)-1);
        u->name[sizeof(u->name)-1] = '\0';
        u->ai_enabled = ai_enabled;
        session->state = SWARM_STATE_ACTIVE;
        printf("[Swarm] Added user %s (AI=%d)\n", name, ai_enabled);
    }
}

void swarm_collab_merge_desktops(swarm_session_t* session, int desktop_id) {
    if (session->desktop_count < MAX_SWARM_DESKTOPS) {
        session->desktop_ids[session->desktop_count++] = desktop_id;
        session->state = SWARM_STATE_MERGED;
        printf("[Swarm] Merged desktop %d\n", desktop_id);
    }
}

void swarm_collab_federate_ai(swarm_session_t* session) {
    session->ai_federated = true;
    session->state = SWARM_STATE_FEDERATED;
    printf("[Swarm] AI federation enabled\n");
}

void swarm_collab_render(const swarm_session_t* session) {
    const char* state_str = "?";
    switch (session->state) {
        case SWARM_STATE_IDLE: state_str = "idle"; break;
        case SWARM_STATE_ACTIVE: state_str = "active"; break;
        case SWARM_STATE_MERGED: state_str = "merged"; break;
        case SWARM_STATE_FEDERATED: state_str = "federated"; break;
    }
    printf("[Swarm] Session %d | Users: %d | Desktops: %d | AI: %s | State: %s\n", session->session_id, session->user_count, session->desktop_count, session->ai_federated ? "federated" : "local", state_str);
    for (int i = 0; i < session->user_count; ++i) {
        printf("  User %d: %s (AI=%d)\n", session->users[i].user_id, session->users[i].name, session->users[i].ai_enabled);
    }
    for (int d = 0; d < session->desktop_count; ++d) {
        printf("  Desktop %d\n", session->desktop_ids[d]);
    }
} 