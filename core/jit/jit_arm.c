#include "jit_backend.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "../arch/arm/jit_backend.c"

static int arm_jit_init(void) {
    printf("[JIT-ARM] Initialized\n");
    return 0;
}

static int arm_jit_compile(vm_t* vm) {
    // Call the real backend implementation (currently minimal)
    return jit_backend_arm(vm);
}

jit_backend_t jit_arm_backend = {
    .name = "ARM",
    .init = arm_jit_init,
    .compile = arm_jit_compile
}; 