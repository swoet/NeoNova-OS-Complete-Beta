// Portable Bytecode Virtual Machine (VM) Core
// Provides architecture-agnostic execution for NeoNova OS
// Modular, secure, and recoverable

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "jit/jit_backend.h"
#include <time.h>

#define VM_MAX_REGS 16
#define VM_MAX_STACK 256
#define VM_MAX_CODE 1024

// VM instruction set (expanded)
typedef enum {
    VM_NOP = 0,
    VM_LOAD_IMM,
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_JMP,
    VM_JZ,
    VM_LOAD,
    VM_STORE,
    VM_SYSCALL,
    VM_HALT,
    // ... extend as needed ...
} vm_opcode_t;

typedef enum {
    VM_ARCH_X86_64,
    VM_ARCH_ARM,
    VM_ARCH_RISCV,
    VM_ARCH_PHOTONIC
} vm_arch_t;

// VM state
typedef struct {
    uint32_t regs[VM_MAX_REGS];
    uint32_t stack[VM_MAX_STACK];
    uint32_t pc;
    uint32_t sp;
    uint8_t* code;
    size_t code_size;
    bool halted;
} vm_t;

// VM snapshot structure
typedef struct {
    uint32_t regs[VM_MAX_REGS];
    uint32_t stack[VM_MAX_STACK];
    uint32_t pc;
    uint32_t sp;
    size_t code_size;
    bool halted;
} vm_snapshot_t;

// Forward declarations for JIT backends
int jit_backend_x86_64(vm_t* vm);
int jit_backend_arm(vm_t* vm);
int jit_backend_riscv(vm_t* vm);
int jit_backend_photonic(vm_t* vm);

// Take a snapshot of the VM state
static void vm_snapshot(const vm_t* vm, vm_snapshot_t* snap) {
    memcpy(snap->regs, vm->regs, sizeof(vm->regs));
    memcpy(snap->stack, vm->stack, sizeof(vm->stack));
    snap->pc = vm->pc;
    snap->sp = vm->sp;
    snap->code_size = vm->code_size;
    snap->halted = vm->halted;
}

// Restore a snapshot to the VM
static void vm_restore(vm_t* vm, const vm_snapshot_t* snap) {
    memcpy(vm->regs, snap->regs, sizeof(vm->regs));
    memcpy(vm->stack, snap->stack, sizeof(vm->stack));
    vm->pc = snap->pc;
    vm->sp = snap->sp;
    vm->code_size = snap->code_size;
    vm->halted = snap->halted;
}

// Recovery logic: snapshot, rollback, restart, and log
void vm_recover(vm_t* vm) {
    static vm_snapshot_t last_snap;
    static int recovery_count = 0;
    if (!vm) return;
    printf("[VM] Fault detected. Attempting recovery...\n");
    // Try to rollback to last snapshot
    if (recovery_count < 3) {
        printf("[VM] Rolling back to last snapshot.\n");
        vm_restore(vm, &last_snap);
        recovery_count++;
        vm->halted = false;
        vm_run(vm);
    } else {
        printf("[VM] Recovery failed after 3 attempts. Isolating VM.\n");
        // Optionally, mark as quarantined, notify admin, etc.
        vm->halted = true;
    }
}

// VM interpreter loop
int vm_run(vm_t* vm) {
    if (!vm || !vm->code) return -1;
    static vm_snapshot_t last_snap;
    vm_snapshot(vm, &last_snap);
    vm->halted = false;
    while (!vm->halted && vm->pc < vm->code_size) {
        uint8_t op = vm->code[vm->pc++];
        switch (op) {
            case VM_NOP:
                break;
            case VM_LOAD_IMM: {
                uint8_t reg = vm->code[vm->pc++];
                uint32_t imm = *(uint32_t*)&vm->code[vm->pc];
                vm->pc += 4;
                if (reg < VM_MAX_REGS) vm->regs[reg] = imm;
                break;
            }
            case VM_ADD: {
                uint8_t r0 = vm->code[vm->pc++];
                uint8_t r1 = vm->code[vm->pc++];
                if (r0 < VM_MAX_REGS && r1 < VM_MAX_REGS)
                    vm->regs[r0] += vm->regs[r1];
                break;
            }
            case VM_SUB: {
                uint8_t r0 = vm->code[vm->pc++];
                uint8_t r1 = vm->code[vm->pc++];
                if (r0 < VM_MAX_REGS && r1 < VM_MAX_REGS)
                    vm->regs[r0] -= vm->regs[r1];
                break;
            }
            case VM_MUL: {
                uint8_t r0 = vm->code[vm->pc++];
                uint8_t r1 = vm->code[vm->pc++];
                if (r0 < VM_MAX_REGS && r1 < VM_MAX_REGS)
                    vm->regs[r0] *= vm->regs[r1];
                break;
            }
            case VM_DIV: {
                uint8_t r0 = vm->code[vm->pc++];
                uint8_t r1 = vm->code[vm->pc++];
                if (r0 < VM_MAX_REGS && r1 < VM_MAX_REGS && vm->regs[r1] != 0)
                    vm->regs[r0] /= vm->regs[r1];
                break;
            }
            case VM_JMP: {
                uint32_t addr = *(uint32_t*)&vm->code[vm->pc];
                vm->pc = addr;
                break;
            }
            case VM_JZ: {
                uint8_t reg = vm->code[vm->pc++];
                uint32_t addr = *(uint32_t*)&vm->code[vm->pc];
                vm->pc += 4;
                if (reg < VM_MAX_REGS && vm->regs[reg] == 0)
                    vm->pc = addr;
                break;
            }
            case VM_LOAD: {
                // Memory load: reg, addr
                uint8_t reg = vm->code[vm->pc++];
                uint32_t addr = *(uint32_t*)&vm->code[vm->pc];
                vm->pc += 4;
                // For now, just use stack as memory
                if (reg < VM_MAX_REGS && addr < VM_MAX_STACK)
                    vm->regs[reg] = vm->stack[addr];
                break;
            }
            case VM_STORE: {
                // Memory store: reg, addr
                uint8_t reg = vm->code[vm->pc++];
                uint32_t addr = *(uint32_t*)&vm->code[vm->pc];
                vm->pc += 4;
                if (reg < VM_MAX_REGS && addr < VM_MAX_STACK)
                    vm->stack[addr] = vm->regs[reg];
                break;
            }
            case VM_SYSCALL: {
                uint8_t syscall_id = vm->code[vm->pc++];
                uint32_t arg0 = *(uint32_t*)&vm->code[vm->pc];
                vm->pc += 4;
                uint32_t arg1 = *(uint32_t*)&vm->code[vm->pc];
                vm->pc += 4;
                switch (syscall_id) {
                    case 0: // print
                        printf("[VM_SYSCALL] Print: %u\n", arg0);
                        break;
                    case 1: // exit
                        printf("[VM_SYSCALL] Exit called with code %u\n", arg0);
                        vm->halted = true;
                        break;
                    case 2: { // get time
                        uint32_t t = (uint32_t)time(NULL);
                        if (arg0 < VM_MAX_REGS) vm->regs[arg0] = t;
                        break;
                    }
                    default:
                        printf("[VM_SYSCALL] Unknown syscall %u\n", syscall_id);
                        break;
                }
                break;
            }
            case VM_HALT:
                vm->halted = true;
                break;
            // ... implement more instructions ...
            default:
                // Security: Invalid opcode, halt and recover
                vm->halted = true;
                printf("[VM] Invalid opcode %d at pc=%u.\n", op, vm->pc-1);
                vm_recover(vm);
                break;
        }
    }
    return 0;
}

// VM JIT compile dispatcher
int vm_jit_compile(vm_t* vm, vm_arch_t arch) {
    jit_backend_t* backend = select_jit_backend(arch);
    if (!backend) return -1;
    if (backend->init) backend->init();
    return backend->compile(vm);
} 