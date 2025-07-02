#include "jit_backend.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "../arch/x86_64/jit_backend.c"

static int x86_64_jit_init(void) {
    printf("[JIT-x86_64] Initialized\n");
    return 0;
}

static int x86_64_jit_compile(vm_t* vm) {
    // Call the real backend implementation
    return jit_backend_x86_64(vm);
}

jit_backend_t jit_x86_64_backend = {
    .name = "x86_64",
    .init = x86_64_jit_init,
    .compile = x86_64_jit_compile
}; 