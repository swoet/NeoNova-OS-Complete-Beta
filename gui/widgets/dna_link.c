#include "dna_link.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dna_link_t* dna_link_create(int source_id, int target_id, dna_link_type_t type, const char* label) {
    dna_link_t* link = (dna_link_t*)malloc(sizeof(dna_link_t));
    link->source_window_id = source_id;
    link->target_window_id = target_id;
    link->type = type;
    link->state = DNA_LINK_STATE_IDLE;
    link->label = label ? strdup(label) : NULL;
    link->on_action = NULL;
    link->user_data = NULL;
    return link;
}

void dna_link_render(const dna_link_t* link) {
    const char* type_str = "?";
    switch (link->type) {
        case DNA_LINK_TYPE_DATA: type_str = "data"; break;
        case DNA_LINK_TYPE_ACTION: type_str = "action"; break;
        case DNA_LINK_TYPE_HYBRID: type_str = "hybrid"; break;
    }
    const char* state_str = "?";
    switch (link->state) {
        case DNA_LINK_STATE_IDLE: state_str = "idle"; break;
        case DNA_LINK_STATE_ATTACHED: state_str = "attached"; break;
        case DNA_LINK_STATE_ACTIVE: state_str = "active"; break;
    }
    printf("[DNALink] %s: %d -> %d type=%s state=%s\n", link->label ? link->label : "(no label)", link->source_window_id, link->target_window_id, type_str, state_str);
}

void dna_link_attach(dna_link_t* link) {
    link->state = DNA_LINK_STATE_ATTACHED;
    printf("[DNALink] Attached link %s\n", link->label ? link->label : "");
}

void dna_link_detach(dna_link_t* link) {
    link->state = DNA_LINK_STATE_IDLE;
    printf("[DNALink] Detached link %s\n", link->label ? link->label : "");
}

void dna_link_perform_action(dna_link_t* link) {
    link->state = DNA_LINK_STATE_ACTIVE;
    printf("[DNALink] Performing action for link %s\n", link->label ? link->label : "");
    if (link->on_action) link->on_action(link, link->user_data);
} 