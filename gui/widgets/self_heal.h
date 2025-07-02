#pragma once
#include "quantum_timeline.h"
#include <stdbool.h>

typedef struct self_heal_monitor {
    int window_id;
    int unstable_count;
    int last_state_id;
    quantum_timeline_t* timeline;
    void (*on_rewind)(struct self_heal_monitor*, int state_id);
} self_heal_monitor_t;

void self_heal_monitor_init(self_heal_monitor_t* mon, int window_id, quantum_timeline_t* timeline);
void self_heal_monitor_tick(self_heal_monitor_t* mon, bool is_unstable);
void self_heal_trigger_rewind(self_heal_monitor_t* mon, int state_id); 