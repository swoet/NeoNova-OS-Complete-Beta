# Adaptive Resource Manager

This directory contains the adaptive resource manager modules for NeoNova OS:

- **resource_manager.[c/h]**: Main interface for tracking and managing system resources (CPU, RAM, GPU, I/O).
- **cpu_manager.[c/h]**: Handles CPU-specific resource management (scaling, prioritization, power adjustment).
- **ram_manager.[c/h]**: Handles RAM-specific resource management.
- **gpu_manager.[c/h]**: Handles GPU-specific resource management.
- **io_manager.[c/h]**: Handles I/O-specific resource management.

## Usage

The resource manager is initialized at boot and periodically updated. It provides APIs for scaling, prioritization, and power-aware adjustments. All actions are logged for testing and debugging.

Extend each submodule to implement real resource management logic as needed. 