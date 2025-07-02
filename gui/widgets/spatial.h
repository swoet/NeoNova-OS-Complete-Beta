#pragma once
#include <stdbool.h>

#define MAX_SPATIAL_OBJECTS 32

typedef enum {
    SPATIAL_TYPE_WINDOW,
    SPATIAL_TYPE_ORB,
    SPATIAL_TYPE_AGENT,
    SPATIAL_TYPE_STREAM,
    SPATIAL_TYPE_PORTAL
} spatial_type_t;

typedef enum {
    SPATIAL_STATE_IDLE,
    SPATIAL_STATE_MOVING,
    SPATIAL_STATE_PINNED,
    SPATIAL_STATE_PORTAL_OPEN
} spatial_state_t;

typedef struct spatial_object {
    int id;
    spatial_type_t type;
    char label[64];
    float x, y, z;
    float orientation[4]; // Quaternion (w,x,y,z)
    spatial_state_t state;
    int portal_id;
} spatial_object_t;

typedef struct spatial_manager {
    spatial_object_t objects[MAX_SPATIAL_OBJECTS];
    int object_count;
} spatial_manager_t;

void spatial_manager_init(spatial_manager_t* mgr);
spatial_object_t* spatial_object_create(spatial_manager_t* mgr, spatial_type_t type, const char* label, float x, float y, float z);
void spatial_object_move(spatial_object_t* obj, float x, float y, float z);
void spatial_object_render(const spatial_object_t* obj);
void spatial_open_portal(spatial_manager_t* mgr, int from_id, int to_id); 