#ifndef JIT_BACKEND_H
#define JIT_BACKEND_H
#include <stdint.h>
#include <stddef.h>
#include "../bytecode_vm.h"

typedef enum {
    VM_ARCH_X86_64 = 0,
    VM_ARCH_ARM,
    VM_ARCH_RISCV,
    VM_ARCH_PHOTONIC,
    VM_ARCH_UNKNOWN
} vm_arch_t;

typedef struct jit_backend {
    const char* name;
    int (*init)(void);
    int (*compile)(vm_t* vm);
} jit_backend_t;

// Extern declarations for each backend
extern jit_backend_t jit_x86_64_backend;
extern jit_backend_t jit_arm_backend;
extern jit_backend_t jit_riscv_backend;
extern jit_backend_t jit_photonic_backend;

// Select backend by architecture
defined in jit_backend.c
jit_backend_t* select_jit_backend(vm_arch_t arch);

#endif // JIT_BACKEND_H 