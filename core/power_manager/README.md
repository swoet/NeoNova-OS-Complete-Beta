# Power Management Modules

This directory contains the power management modules for NeoNova OS:

- **power_manager.[c/h]**: Main interface for power management (init, tick, shutdown).
- **governor.[c/h]**: Adaptive governor logic for CPU/GPU scaling.
- **thermal.[c/h]**: Thermal throttling and temperature checks.
- **suspend.[c/h]**: Suspend/resume state management.
- **usage_learning.[c/h]**: Usage pattern learning for power optimization.

## Usage

The power manager is initialized at boot and periodically ticked. Each submodule logs its actions for testing and debugging.

Extend each submodule to implement real power management logic as needed. 