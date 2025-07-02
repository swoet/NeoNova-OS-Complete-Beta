# Driver Manager Modules

This directory contains modules for advanced driver management in NeoNova OS:

- **ai_driver_generator.[c/h]**: Provides AI-assisted (rules-based for now) driver suggestion based on hardware fingerprints. Designed to be easily extended with real AI/ML logic in the future.
- **cloud_driver_fetcher.[c/h]**: Handles secure cloud driver fetching and offline caching. Currently a stub that simulates fetch and cache; ready for integration with real cloud APIs.

## Usage

These modules are called by the unified driver framework when a new hardware device is detected and no local driver is found. They log their actions for easy testing and debugging.

Extend the rules in `ai_driver_generator.c` and implement real cloud logic in `cloud_driver_fetcher.c` as needed. 