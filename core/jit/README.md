# Modular JIT Backend System

This directory contains the modular JIT backend system for NeoNova OS's portable VM:

- **jit_backend.h/c**: Common interface and backend selection logic.
- **jit_x86_64.[c/h]**: x86_64 JIT backend (production-ready, real codegen and execution).
- **jit_arm.[c/h]**: ARM JIT backend (extension point for real codegen; see code for status).
- **jit_riscv.[c/h]**: RISC-V JIT backend (extension point for real codegen; see code for status).
- **jit_photonic.[c/h]**: Photonic JIT backend (extension point for real codegen; see code for status).

## Usage

The VM selects the appropriate JIT backend at runtime based on the target architecture. Each backend implements the same interface, making it easy to add new architectures.

To add a new backend, implement a new `jit_backend_t` object and add it to the selection logic in `jit_backend.c`.

All core, advanced, and critical JIT features for x86_64 are now real and production-ready. ARM, RISC-V, and Photonic are ready for production extension. 