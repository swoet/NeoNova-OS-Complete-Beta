#ifndef APP_RUNTIME_H
#define APP_RUNTIME_H
#include <stdbool.h>
#define MAX_APPS 32

typedef enum {
    APP_TYPE_NATIVE,
    APP_TYPE_WASM,
    APP_TYPE_JVM,
    APP_TYPE_ELECTRON
} app_type_t;

typedef struct app_container {
    int id;
    app_type_t type;
    char name[64];
    bool running;
    void* instance; // Pointer to VM/JVM/etc.
    int process_id; // Associated process
    int window_id; // Associated window
} app_container_t;

typedef struct app_runtime {
    app_container_t apps[MAX_APPS];
    int app_count;
    int next_app_id;
} app_runtime_t;

void app_runtime_init(app_runtime_t* rt);
int app_runtime_register(app_runtime_t* rt, const char* name, app_type_t type);
int app_runtime_start(app_runtime_t* rt, int app_id);
int app_runtime_stop(app_runtime_t* rt, int app_id);
int app_runtime_destroy(app_runtime_t* rt, int app_id);
void app_runtime_tick(app_runtime_t* rt);
void app_runtime_list(app_runtime_t* rt);
void app_runtime_set_process_id(app_runtime_t* rt, int app_id, int process_id);
void app_runtime_set_window_id(app_runtime_t* rt, int app_id, int window_id);

#endif // APP_RUNTIME_H 