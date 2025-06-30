#ifndef CPU_CONTEXT_X86_64_H
#define CPU_CONTEXT_X86_64_H

#include <stdint.h> // For sized integer types

// CPU registers structure for x86-64
// This structure holds the state of the CPU registers that need to be saved
// and restored during a context switch.
// The order might matter depending on assembly routines used for context switching.
typedef struct {
    // General Purpose Registers
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp; // Base Pointer
    uint64_t rdi; // Destination Index
    uint64_t rsi; // Source Index
    uint64_t rdx; // Data Register
    uint64_t rcx; // Counter Register
    uint64_t rbx; // Base Register
    uint64_t rax; // Accumulator Register

    // Interrupt/Stack frame related (pushed by ISR stub or context switch code)
    // These might include error code, interrupt number for interrupt contexts.
    // For a task switch, these might be just RIP, CS, RFLAGS, RSP, SS.
    // The provided structure had these separate, let's keep that for now.

    // Segment Registers (typically only SS, CS are actively managed in 64-bit long mode for tasks)
    // DS, ES, FS, GS might be set to specific values (e.g., 0 for DS, ES, SS in user mode,
    // or used for specific purposes like FS/GS for thread-local storage).
    // For a task context switch, only CS, SS, RSP, RIP, RFLAGS are strictly part of the stack frame.
    // The others are saved as part of the general context.
    // uint64_t ds; // Data Segment (often unused or set to kernel data segment)
    // uint64_t es; // Extra Segment (often unused)
    // uint64_t fs; // FS Segment (used for TLS or by kernel)
    // uint64_t gs; // GS Segment (used for TLS or by kernel)

    // Instruction Pointer, CPU Flags, Stack Pointer
    uint64_t rip;    // Instruction Pointer
    uint64_t cs;     // Code Segment
    uint64_t rflags; // CPU Flags
    uint64_t rsp;    // Stack Pointer
    uint64_t ss;     // Stack Segment

    // Page Table Register
    uint64_t cr3;    // Page Directory Base Register (PDBR)

    // Note: Floating Point Unit (FPU/SSE/AVX) state is not included here
    // but would be necessary for a complete context switch. This usually involves
    // FXSAVE/FXRSTOR instructions and a 512-byte aligned memory region.
    // unsigned char fpu_state[512] __attribute__((aligned(16)));

} cpu_state_t;
// Renaming to cpu_context_x86_64_t might be more specific if a generic cpu_state_t is ever needed.
// For now, keeping cpu_state_t as per the original code, but it's x86-64 specific.

#endif // CPU_CONTEXT_X86_64_H
