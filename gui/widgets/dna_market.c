#include "dna_market.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dna_market_init(dna_market_t* market) {
    market->count = 0;
    memset(market->entries, 0, sizeof(market->entries));
}

int dna_market_add(dna_market_t* market, const char* label, dna_link_type_t type, const char* data) {
    if (market->count < MAX_DNA_MARKET_ENTRIES) {
        dna_market_entry_t* e = &market->entries[market->count];
        e->id = market->count;
        strncpy(e->label, label, sizeof(e->label)-1);
        e->label[sizeof(e->label)-1] = '\0';
        e->type = type;
        strncpy(e->data, data, sizeof(e->data)-1);
        e->data[sizeof(e->data)-1] = '\0';
        e->merged_from[0] = -1; e->merged_from[1] = -1;
        printf("[DNAMarket] Added entry %d: %s\n", e->id, e->label);
        return market->count++;
    }
    return -1;
}

void dna_market_list(const dna_market_t* market) {
    printf("[DNAMarket] Listing %d entries:\n", market->count);
    for (int i = 0; i < market->count; ++i) {
        printf("  %d: %s (%d) data=%s\n", market->entries[i].id, market->entries[i].label, market->entries[i].type, market->entries[i].data);
    }
}

int dna_market_merge(dna_market_t* market, int id1, int id2, const char* new_label) {
    if (market->count < MAX_DNA_MARKET_ENTRIES && id1 >= 0 && id2 >= 0 && id1 < market->count && id2 < market->count) {
        dna_market_entry_t* e1 = &market->entries[id1];
        dna_market_entry_t* e2 = &market->entries[id2];
        dna_market_entry_t* e = &market->entries[market->count];
        e->id = market->count;
        snprintf(e->label, sizeof(e->label), "%s+%s:%s", e1->label, e2->label, new_label);
        e->type = DNA_LINK_TYPE_HYBRID;
        snprintf(e->data, sizeof(e->data), "%s|%s", e1->data, e2->data);
        e->merged_from[0] = id1; e->merged_from[1] = id2;
        printf("[DNAMarket] Merged %d and %d into %d: %s\n", id1, id2, e->id, e->label);
        return market->count++;
    }
    return -1;
}

void dna_market_render(const dna_market_t* market) {
    printf("[DNAMarket] Marketplace State:\n");
    for (int i = 0; i < market->count; ++i) {
        printf("  Entry %d: %s type=%d data=%s", market->entries[i].id, market->entries[i].label, market->entries[i].type, market->entries[i].data);
        if (market->entries[i].merged_from[0] != -1)
            printf(" (merged from %d,%d)", market->entries[i].merged_from[0], market->entries[i].merged_from[1]);
        printf("\n");
    }
} 