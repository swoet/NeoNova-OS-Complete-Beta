#include "ai_orb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ai_orb_widget_t* ai_orb_create(int x, int y, int radius, unsigned int color, const char* label) {
    ai_orb_widget_t* orb = (ai_orb_widget_t*)malloc(sizeof(ai_orb_widget_t));
    orb->x = x; orb->y = y; orb->radius = radius;
    orb->color = color;
    orb->state = AI_ORB_STATE_IDLE;
    orb->label = label ? strdup(label) : NULL;
    orb->suggestion = NULL;
    orb->listening = false;
    orb->animating = false;
    orb->anim_phase = 0.0f;
    orb->on_suggest = NULL;
    orb->user_data = NULL;
    return orb;
}

void ai_orb_render(const ai_orb_widget_t* orb) {
    const char* state_str = "?";
    switch (orb->state) {
        case AI_ORB_STATE_IDLE: state_str = "idle"; break;
        case AI_ORB_STATE_LISTENING: state_str = "listening"; break;
        case AI_ORB_STATE_THINKING: state_str = "thinking"; break;
        case AI_ORB_STATE_SUGGESTING: state_str = "suggesting"; break;
        case AI_ORB_STATE_ACTING: state_str = "acting"; break;
    }
    printf("[AIOrb] %s at (%d,%d) r=%d color=#%06X state=%s%s phase=%.2f\n",
        orb->label ? orb->label : "(no label)", orb->x, orb->y, orb->radius, orb->color, state_str,
        orb->listening ? " [LISTENING]" : "", orb->anim_phase);
    if (orb->suggestion) printf("  Suggestion: %s\n", orb->suggestion);
}

void ai_orb_set_state(ai_orb_widget_t* orb, ai_orb_state_t state) {
    orb->state = state;
}

void ai_orb_listen(ai_orb_widget_t* orb, bool enable) {
    orb->listening = enable;
    orb->state = enable ? AI_ORB_STATE_LISTENING : AI_ORB_STATE_IDLE;
    printf("[AIOrb] Listening %s\n", enable ? "enabled" : "disabled");
}

void ai_orb_suggest(ai_orb_widget_t* orb, const char* suggestion) {
    if (orb->suggestion) free((void*)orb->suggestion);
    orb->suggestion = suggestion ? strdup(suggestion) : NULL;
    orb->state = AI_ORB_STATE_SUGGESTING;
    printf("[AIOrb] Suggestion: %s\n", suggestion);
    if (orb->on_suggest) orb->on_suggest(orb, suggestion, orb->user_data);
}

void ai_orb_animate(ai_orb_widget_t* orb, float phase) {
    orb->animating = true;
    orb->anim_phase = phase;
    printf("[AIOrb] Animating phase to %.2f\n", phase);
    orb->animating = false;
} 