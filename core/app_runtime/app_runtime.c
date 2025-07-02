#include "app_runtime.h"
#include <stdio.h>
#include <string.h>

void app_runtime_init(app_runtime_t* rt) {
    memset(rt, 0, sizeof(*rt));
    rt->next_app_id = 1;
    printf("[AppRuntime] Initialized.\n");
}

int app_runtime_register(app_runtime_t* rt, const char* name, app_type_t type) {
    if (rt->app_count >= MAX_APPS) return -1;
    app_container_t* app = &rt->apps[rt->app_count++];
    app->id = rt->next_app_id++;
    app->type = type;
    strncpy(app->name, name, sizeof(app->name)-1);
    app->name[sizeof(app->name)-1] = '\0';
    app->running = false;
    app->instance = NULL;
    printf("[AppRuntime] Registered app %d: '%s' (type %d)\n", app->id, app->name, app->type);
    return app->id;
}

int app_runtime_start(app_runtime_t* rt, int app_id) {
    for (int i = 0; i < rt->app_count; ++i) {
        if (rt->apps[i].id == app_id && !rt->apps[i].running) {
            rt->apps[i].running = true;
            printf("[AppRuntime] Started app %d: '%s'\n", app_id, rt->apps[i].name);
            return 0;
        }
    }
    return -1;
}

int app_runtime_stop(app_runtime_t* rt, int app_id) {
    for (int i = 0; i < rt->app_count; ++i) {
        if (rt->apps[i].id == app_id && rt->apps[i].running) {
            rt->apps[i].running = false;
            printf("[AppRuntime] Stopped app %d: '%s'\n", app_id, rt->apps[i].name);
            return 0;
        }
    }
    return -1;
}

int app_runtime_destroy(app_runtime_t* rt, int app_id) {
    for (int i = 0; i < rt->app_count; ++i) {
        if (rt->apps[i].id == app_id) {
            printf("[AppRuntime] Destroyed app %d: '%s'\n", app_id, rt->apps[i].name);
            // Shift remaining apps down
            for (int j = i; j < rt->app_count - 1; ++j) rt->apps[j] = rt->apps[j+1];
            rt->app_count--;
            return 0;
        }
    }
    return -1;
}

void app_runtime_tick(app_runtime_t* rt) {
    for (int i = 0; i < rt->app_count; ++i) {
        if (rt->apps[i].running) {
            printf("[AppRuntime] App %d ('%s') running.\n", rt->apps[i].id, rt->apps[i].name);
            // TODO: Call into WASM/JVM/Electron/etc. instance
        }
    }
}

void app_runtime_list(app_runtime_t* rt) {
    printf("[AppRuntime] App list (%d total):\n", rt->app_count);
    for (int i = 0; i < rt->app_count; ++i) {
        printf("  App %d: '%s' (type %d) %s\n", rt->apps[i].id, rt->apps[i].name, rt->apps[i].type, rt->apps[i].running ? "[RUNNING]" : "");
    }
}

void app_runtime_set_process_id(app_runtime_t* rt, int app_id, int process_id) {
    for (int i = 0; i < rt->app_count; ++i) {
        if (rt->apps[i].id == app_id) {
            rt->apps[i].process_id = process_id;
            printf("[AppRuntime] Set process %d for app %d ('%s')\n", process_id, app_id, rt->apps[i].name);
            return;
        }
    }
}

void app_runtime_set_window_id(app_runtime_t* rt, int app_id, int window_id) {
    for (int i = 0; i < rt->app_count; ++i) {
        if (rt->apps[i].id == app_id) {
            rt->apps[i].window_id = window_id;
            printf("[AppRuntime] Set window %d for app %d ('%s')\n", window_id, app_id, rt->apps[i].name);
            return;
        }
    }
}
 