#pragma once
#include <stdbool.h>

typedef enum {
    AI_ORB_STATE_IDLE,
    AI_ORB_STATE_LISTENING,
    AI_ORB_STATE_THINKING,
    AI_ORB_STATE_SUGGESTING,
    AI_ORB_STATE_ACTING
} ai_orb_state_t;

typedef struct ai_orb_widget {
    int x, y;
    int radius;
    unsigned int color;
    ai_orb_state_t state;
    const char* label;
    const char* suggestion;
    bool listening;
    bool animating;
    float anim_phase;
    void (*on_suggest)(struct ai_orb_widget*, const char* suggestion, void* user_data);
    void* user_data;
} ai_orb_widget_t;

ai_orb_widget_t* ai_orb_create(int x, int y, int radius, unsigned int color, const char* label);
void ai_orb_render(const ai_orb_widget_t* orb);
void ai_orb_set_state(ai_orb_widget_t* orb, ai_orb_state_t state);
void ai_orb_listen(ai_orb_widget_t* orb, bool enable);
void ai_orb_suggest(ai_orb_widget_t* orb, const char* suggestion);
void ai_orb_animate(ai_orb_widget_t* orb, float phase); 