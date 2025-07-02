#pragma once
#include <stdbool.h>

typedef enum {
    DNA_LINK_TYPE_DATA,
    DNA_LINK_TYPE_ACTION,
    DNA_LINK_TYPE_HYBRID
} dna_link_type_t;

typedef enum {
    DNA_LINK_STATE_IDLE,
    DNA_LINK_STATE_ATTACHED,
    DNA_LINK_STATE_ACTIVE
} dna_link_state_t;

typedef struct dna_link {
    int source_window_id;
    int target_window_id;
    dna_link_type_t type;
    dna_link_state_t state;
    const char* label;
    void (*on_action)(struct dna_link*, void* user_data);
    void* user_data;
} dna_link_t;

dna_link_t* dna_link_create(int source_id, int target_id, dna_link_type_t type, const char* label);
void dna_link_render(const dna_link_t* link);
void dna_link_attach(dna_link_t* link);
void dna_link_detach(dna_link_t* link);
void dna_link_perform_action(dna_link_t* link); 