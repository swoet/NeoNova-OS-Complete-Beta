#include "jit_backend.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "../arch/riscv/jit_backend.c"

static int riscv_jit_init(void) {
    printf("[JIT-RISC-V] Initialized\n");
    return 0;
}

static int riscv_jit_compile(vm_t* vm) {
    return jit_backend_riscv(vm);
}

jit_backend_t jit_riscv_backend = {
    .name = "RISC-V",
    .init = riscv_jit_init,
    .compile = riscv_jit_compile
}; 