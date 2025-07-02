#pragma once
#include <stdbool.h>

typedef enum {
    ORB_STATE_IDLE,
    ORB_STATE_ACTIVE,
    ORB_STATE_MORPHING,
    ORB_STATE_NOTIFICATION,
    ORB_STATE_DRAGGING
} orb_state_t;

typedef struct orb_widget {
    int x, y;
    int radius;
    unsigned int color;
    orb_state_t state;
    float morph; // 0.0=sphere, 1.0=fully morphed
    const char* label;
    void (*on_action)(struct orb_widget*, void* user_data);
    void* user_data;
    bool focused;
    bool animating;
} orb_widget_t;

orb_widget_t* orb_create(int x, int y, int radius, unsigned int color, const char* label);
void orb_render(const orb_widget_t* orb);
void orb_set_state(orb_widget_t* orb, orb_state_t state);
void orb_set_action(orb_widget_t* orb, void (*on_action)(orb_widget_t*, void*), void* user_data);
void orb_animate(orb_widget_t* orb, float target_morph);
void orb_morph(orb_widget_t* orb, float morph);
void orb_set_label(orb_widget_t* orb, const char* label); 