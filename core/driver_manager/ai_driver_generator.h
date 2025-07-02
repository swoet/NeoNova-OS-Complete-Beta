#ifndef AI_DRIVER_GENERATOR_H
#define AI_DRIVER_GENERATOR_H
#include <stdint.h>
#include <stddef.h>
#include "../../drivers/unified_driver_framework/driver_framework.h"

// Suggest a generic driver for the given hardware fingerprint (rules-based, AI-ready)
int ai_suggest_driver(hw_fingerprint_t* fp);

#endif // AI_DRIVER_GENERATOR_H 