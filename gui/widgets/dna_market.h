#pragma once
#include "dna_link.h"
#include <stdbool.h>

#define MAX_DNA_MARKET_ENTRIES 32

typedef struct dna_market_entry {
    int id;
    char label[64];
    dna_link_type_t type;
    char data[128];
    int merged_from[2]; // IDs of merged entries, -1 if not merged
} dna_market_entry_t;

typedef struct dna_market {
    dna_market_entry_t entries[MAX_DNA_MARKET_ENTRIES];
    int count;
} dna_market_t;

void dna_market_init(dna_market_t* market);
int dna_market_add(dna_market_t* market, const char* label, dna_link_type_t type, const char* data);
void dna_market_list(const dna_market_t* market);
int dna_market_merge(dna_market_t* market, int id1, int id2, const char* new_label);
void dna_market_render(const dna_market_t* market); 