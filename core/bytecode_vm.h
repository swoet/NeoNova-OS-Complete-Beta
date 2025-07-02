#ifndef BYTECODE_VM_H
#define BYTECODE_VM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VM_MAX_REGS 16
#define VM_MAX_STACK 256
#define VM_MAX_CODE 1024

typedef enum {
    VM_NOP = 0,
    VM_LOAD_IMM,
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_JMP,
    VM_JZ,
    VM_HALT,
    // ... extend as needed ...
} vm_opcode_t;

typedef enum {
    VM_ARCH_X86_64,
    VM_ARCH_ARM,
    VM_ARCH_RISCV,
    VM_ARCH_PHOTONIC
} vm_arch_t;

typedef struct {
    uint32_t regs[VM_MAX_REGS];
    uint32_t stack[VM_MAX_STACK];
    uint32_t pc;
    uint32_t sp;
    uint8_t* code;
    size_t code_size;
    bool halted;
} vm_t;

int vm_run(vm_t* vm);
int vm_jit_compile(vm_t* vm, vm_arch_t arch);
void vm_recover(vm_t* vm);

#endif // BYTECODE_VM_H 