#pragma once
#include <stdbool.h>

#define MAX_COLLAB_USERS 8

typedef enum {
    COLLAB_BUBBLE_STATE_IDLE,
    COLLAB_BUBBLE_STATE_ACTIVE,
    COLLAB_BUBBLE_STATE_ANNOTATING
} collab_bubble_state_t;

typedef struct collab_user {
    int user_id;
    char name[32];
    int pointer_x, pointer_y;
    bool voice_active;
} collab_user_t;

typedef struct collab_bubble {
    int window_id;
    collab_bubble_state_t state;
    collab_user_t users[MAX_COLLAB_USERS];
    int user_count;
    char annotation[256];
} collab_bubble_t;

collab_bubble_t* collab_bubble_create(int window_id);
void collab_bubble_render(const collab_bubble_t* bubble);
void collab_bubble_add_user(collab_bubble_t* bubble, int user_id, const char* name);
void collab_bubble_remove_user(collab_bubble_t* bubble, int user_id);
void collab_bubble_annotate(collab_bubble_t* bubble, const char* annotation);
void collab_bubble_set_pointer(collab_bubble_t* bubble, int user_id, int x, int y, bool voice); 