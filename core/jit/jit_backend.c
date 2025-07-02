#include "jit_backend.h"
#include <stdio.h>

// Forward declarations for each backend (defined in their own files)
jit_backend_t jit_x86_64_backend;
jit_backend_t jit_arm_backend;
jit_backend_t jit_riscv_backend;
jit_backend_t jit_photonic_backend;

jit_backend_t* select_jit_backend(vm_arch_t arch) {
    switch (arch) {
        case VM_ARCH_X86_64:
            printf("[JIT] Selected x86_64 backend\n");
            return &jit_x86_64_backend;
        case VM_ARCH_ARM:
            printf("[JIT] Selected ARM backend\n");
            return &jit_arm_backend;
        case VM_ARCH_RISCV:
            printf("[JIT] Selected RISC-V backend\n");
            return &jit_riscv_backend;
        case VM_ARCH_PHOTONIC:
            printf("[JIT] Selected photonic backend\n");
            return &jit_photonic_backend;
        default:
            printf("[JIT] Unknown architecture, no backend selected\n");
            return NULL;
    }
} 