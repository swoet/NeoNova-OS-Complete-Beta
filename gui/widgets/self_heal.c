#include "self_heal.h"
#include <stdio.h>

void self_heal_monitor_init(self_heal_monitor_t* mon, int window_id, quantum_timeline_t* timeline) {
    mon->window_id = window_id;
    mon->unstable_count = 0;
    mon->last_state_id = -1;
    mon->timeline = timeline;
    mon->on_rewind = NULL;
}

void self_heal_monitor_tick(self_heal_monitor_t* mon, bool is_unstable) {
    if (is_unstable) {
        mon->unstable_count++;
        printf("[SelfHeal] Window %d instability detected (%d)\n", mon->window_id, mon->unstable_count);
        if (mon->unstable_count >= 3 && mon->timeline && mon->timeline->state_count > 0) {
            int rewind_id = mon->timeline->state_count - 2;
            if (rewind_id >= 0) {
                printf("[SelfHeal] Auto-rewinding window %d to state %d\n", mon->window_id, rewind_id);
                quantum_timeline_rewind(mon->timeline, rewind_id);
                if (mon->on_rewind) mon->on_rewind(mon, rewind_id);
                mon->unstable_count = 0;
                mon->last_state_id = rewind_id;
            }
        }
    } else {
        mon->unstable_count = 0;
    }
}

void self_heal_trigger_rewind(self_heal_monitor_t* mon, int state_id) {
    if (mon->timeline && state_id >= 0 && state_id < mon->timeline->state_count) {
        printf("[SelfHeal] Manual rewind window %d to state %d\n", mon->window_id, state_id);
        quantum_timeline_rewind(mon->timeline, state_id);
        if (mon->on_rewind) mon->on_rewind(mon, state_id);
        mon->last_state_id = state_id;
    }
} 