#include "power_manager.h"
#include "governor.h"
#include "thermal.h"
#include "suspend.h"
#include "usage_learning.h"
#include <stdio.h>

void power_manager_init(void) {
    governor_init();
    thermal_init();
    usage_learning_init();
    suspend_init();
    printf("[PowerManager] Initialized.\n");
}

void power_manager_tick(void) {
    governor_adjust();
    thermal_check();
    usage_learning_learn();
    printf("[PowerManager] Tick.\n");
}

void power_manager_shutdown(void) {
    printf("[PowerManager] Shutdown.\n");
    suspend_enter();
} 