#include "jit_backend.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "../arch/photonic/jit_backend.c"

static int photonic_jit_init(void) {
    printf("[JIT-Photonic] Initialized\n");
    return 0;
}

static int photonic_jit_compile(vm_t* vm) {
    // Call the real backend implementation (currently minimal)
    return jit_backend_photonic(vm);
}

jit_backend_t jit_photonic_backend = {
    .name = "Photonic",
    .init = photonic_jit_init,
    .compile = photonic_jit_compile
}; 