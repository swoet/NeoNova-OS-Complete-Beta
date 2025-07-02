#include "usage_learning.h"
#include <stdio.h>
#include <time.h>

static const char* USAGE_LOG_FILE = "usage_learning.log";

void usage_learning_init(void) {
    FILE* f = fopen(USAGE_LOG_FILE, "a");
    if (f) { fprintf(f, "[Init] Usage learning started at %ld\n", time(NULL)); fclose(f); }
    printf("[UsageLearning] Initialized.\n");
}

void usage_learning_learn(void) {
    FILE* f = fopen(USAGE_LOG_FILE, "r");
    if (!f) { printf("[UsageLearning] No log file found.\n"); return; }
    char line[256]; int count = 0;
    printf("[UsageLearning] Log entries:\n");
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line); count++;
    }
    printf("[UsageLearning] Total log entries: %d\n", count);
    fclose(f);
} 