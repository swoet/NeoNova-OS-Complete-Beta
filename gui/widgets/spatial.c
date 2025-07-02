#include "spatial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void spatial_manager_init(spatial_manager_t* mgr) {
    mgr->object_count = 0;
    memset(mgr->objects, 0, sizeof(mgr->objects));
}

spatial_object_t* spatial_object_create(spatial_manager_t* mgr, spatial_type_t type, const char* label, float x, float y, float z) {
    if (mgr->object_count < MAX_SPATIAL_OBJECTS) {
        spatial_object_t* o = &mgr->objects[mgr->object_count];
        o->id = mgr->object_count;
        o->type = type;
        strncpy(o->label, label, sizeof(o->label)-1);
        o->label[sizeof(o->label)-1] = '\0';
        o->x = x; o->y = y; o->z = z;
        o->orientation[0] = 1.0f; o->orientation[1] = 0.0f; o->orientation[2] = 0.0f; o->orientation[3] = 0.0f;
        o->state = SPATIAL_STATE_IDLE;
        o->portal_id = -1;
        mgr->object_count++;
        return o;
    }
    return NULL;
}

void spatial_object_move(spatial_object_t* obj, float x, float y, float z) {
    obj->x = x; obj->y = y; obj->z = z;
    obj->state = SPATIAL_STATE_MOVING;
    printf("[Spatial] Moved %s to (%.1f,%.1f,%.1f)\n", obj->label, x, y, z);
}

void spatial_object_render(const spatial_object_t* obj) {
    const char* type_str = "?";
    switch (obj->type) {
        case SPATIAL_TYPE_WINDOW: type_str = "window"; break;
        case SPATIAL_TYPE_ORB: type_str = "orb"; break;
        case SPATIAL_TYPE_AGENT: type_str = "agent"; break;
        case SPATIAL_TYPE_STREAM: type_str = "stream"; break;
        case SPATIAL_TYPE_PORTAL: type_str = "portal"; break;
    }
    const char* state_str = "?";
    switch (obj->state) {
        case SPATIAL_STATE_IDLE: state_str = "idle"; break;
        case SPATIAL_STATE_MOVING: state_str = "moving"; break;
        case SPATIAL_STATE_PINNED: state_str = "pinned"; break;
        case SPATIAL_STATE_PORTAL_OPEN: state_str = "portal_open"; break;
    }
    printf("[Spatial] %s %d '%s' at (%.1f,%.1f,%.1f) state=%s portal=%d\n", type_str, obj->id, obj->label, obj->x, obj->y, obj->z, state_str, obj->portal_id);
}

void spatial_open_portal(spatial_manager_t* mgr, int from_id, int to_id) {
    if (from_id >= 0 && from_id < mgr->object_count && to_id >= 0 && to_id < mgr->object_count) {
        spatial_object_t* from = &mgr->objects[from_id];
        spatial_object_t* to = &mgr->objects[to_id];
        from->state = SPATIAL_STATE_PORTAL_OPEN;
        from->portal_id = to_id;
        printf("[Spatial] Opened HoloPortal from '%s' to '%s'\n", from->label, to->label);
    }
} 