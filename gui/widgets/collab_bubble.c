#include "collab_bubble.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

collab_bubble_t* collab_bubble_create(int window_id) {
    collab_bubble_t* b = (collab_bubble_t*)malloc(sizeof(collab_bubble_t));
    b->window_id = window_id;
    b->state = COLLAB_BUBBLE_STATE_IDLE;
    b->user_count = 0;
    b->annotation[0] = '\0';
    memset(b->users, 0, sizeof(b->users));
    return b;
}

void collab_bubble_render(const collab_bubble_t* bubble) {
    const char* state_str = "?";
    switch (bubble->state) {
        case COLLAB_BUBBLE_STATE_IDLE: state_str = "idle"; break;
        case COLLAB_BUBBLE_STATE_ACTIVE: state_str = "active"; break;
        case COLLAB_BUBBLE_STATE_ANNOTATING: state_str = "annotating"; break;
    }
    printf("[CollabBubble] Window %d state=%s users=%d\n", bubble->window_id, state_str, bubble->user_count);
    for (int i = 0; i < bubble->user_count; ++i) {
        printf("  User %d: %s pointer=(%d,%d)%s\n", bubble->users[i].user_id, bubble->users[i].name, bubble->users[i].pointer_x, bubble->users[i].pointer_y, bubble->users[i].voice_active ? " [VOICE]" : "");
    }
    if (bubble->annotation[0]) printf("  Annotation: %s\n", bubble->annotation);
}

void collab_bubble_add_user(collab_bubble_t* bubble, int user_id, const char* name) {
    if (bubble->user_count < MAX_COLLAB_USERS) {
        collab_user_t* u = &bubble->users[bubble->user_count++];
        u->user_id = user_id;
        strncpy(u->name, name, sizeof(u->name)-1);
        u->name[sizeof(u->name)-1] = '\0';
        u->pointer_x = 0; u->pointer_y = 0; u->voice_active = false;
        bubble->state = COLLAB_BUBBLE_STATE_ACTIVE;
        printf("[CollabBubble] Added user %s\n", name);
    }
}

void collab_bubble_remove_user(collab_bubble_t* bubble, int user_id) {
    for (int i = 0; i < bubble->user_count; ++i) {
        if (bubble->users[i].user_id == user_id) {
            for (int j = i; j < bubble->user_count-1; ++j) bubble->users[j] = bubble->users[j+1];
            bubble->user_count--;
            printf("[CollabBubble] Removed user %d\n", user_id);
            break;
        }
    }
    if (bubble->user_count == 0) bubble->state = COLLAB_BUBBLE_STATE_IDLE;
}

void collab_bubble_annotate(collab_bubble_t* bubble, const char* annotation) {
    strncpy(bubble->annotation, annotation, sizeof(bubble->annotation)-1);
    bubble->annotation[sizeof(bubble->annotation)-1] = '\0';
    bubble->state = COLLAB_BUBBLE_STATE_ANNOTATING;
    printf("[CollabBubble] Annotation: %s\n", annotation);
}

void collab_bubble_set_pointer(collab_bubble_t* bubble, int user_id, int x, int y, bool voice) {
    for (int i = 0; i < bubble->user_count; ++i) {
        if (bubble->users[i].user_id == user_id) {
            bubble->users[i].pointer_x = x;
            bubble->users[i].pointer_y = y;
            bubble->users[i].voice_active = voice;
            printf("[CollabBubble] User %d pointer=(%d,%d) voice=%d\n", user_id, x, y, voice);
            break;
        }
    }
} 