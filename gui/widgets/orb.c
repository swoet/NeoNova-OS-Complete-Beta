#include "orb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

orb_widget_t* orb_create(int x, int y, int radius, unsigned int color, const char* label) {
    orb_widget_t* orb = (orb_widget_t*)malloc(sizeof(orb_widget_t));
    orb->x = x; orb->y = y; orb->radius = radius;
    orb->color = color;
    orb->state = ORB_STATE_IDLE;
    orb->morph = 0.0f;
    orb->label = label ? strdup(label) : NULL;
    orb->on_action = NULL;
    orb->user_data = NULL;
    orb->focused = false;
    orb->animating = false;
    return orb;
}

void orb_render(const orb_widget_t* orb) {
    const char* state_str = "?";
    switch (orb->state) {
        case ORB_STATE_IDLE: state_str = "idle"; break;
        case ORB_STATE_ACTIVE: state_str = "active"; break;
        case ORB_STATE_MORPHING: state_str = "morphing"; break;
        case ORB_STATE_NOTIFICATION: state_str = "notify"; break;
        case ORB_STATE_DRAGGING: state_str = "dragging"; break;
    }
    printf("[Orb] %s at (%d,%d) r=%d color=#%06X morph=%.2f state=%s %s\n",
        orb->label ? orb->label : "(no label)", orb->x, orb->y, orb->radius, orb->color, orb->morph, state_str, orb->focused ? "[FOCUSED]" : "");
}

void orb_set_state(orb_widget_t* orb, orb_state_t state) {
    orb->state = state;
}

void orb_set_action(orb_widget_t* orb, void (*on_action)(orb_widget_t*, void*), void* user_data) {
    orb->on_action = on_action;
    orb->user_data = user_data;
}

void orb_animate(orb_widget_t* orb, float target_morph) {
    orb->animating = true;
    // For demo, just set morph instantly
    orb->morph = target_morph;
    orb->state = ORB_STATE_MORPHING;
    printf("[Orb] Animating morph to %.2f\n", target_morph);
    orb->animating = false;
}

void orb_morph(orb_widget_t* orb, float morph) {
    orb->morph = morph;
    orb->state = ORB_STATE_MORPHING;
    printf("[Orb] Morph set to %.2f\n", morph);
}

void orb_set_label(orb_widget_t* orb, const char* label) {
    if (orb->label) free((void*)orb->label);
    orb->label = label ? strdup(label) : NULL;
} 