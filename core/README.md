# Core System

This directory contains the core logic for the NeoNova OS, including architecture abstraction, execution layers, and system services.

## Architecture-Agnostic Execution Layer

NeoNova OS features a portable bytecode/JIT execution system that allows core logic and user applications to run on any supported CPU architecture (x86_64, ARM, RISC-V, photonic, etc.).

- **Portable Bytecode/JIT:** Core logic is compiled to a portable bytecode, which is then JIT-compiled or interpreted to native instructions at runtime.
- **Architecture Backends:** Each supported architecture provides a backend for translating bytecode to native instructions, with optimizations for performance.
- **Modularity:** The execution layer is modular, allowing new architectures to be added easily.
- **Security:** Bytecode execution is sandboxed, with strict validation and isolation.
- **Recovery:** The system can recover from execution faults by isolating or restarting affected modules.

See the `arch/` subdirectories for architecture-specific backend implementations. 