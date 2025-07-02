// Android container runtime

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int app_id;
    bool running;
} android_app_t;

void android_launch_app(android_app_t* app, int id) {
    app->app_id = id;
    app->running = true;
    printf("[AndroidCompat] App %d launched in container.\n", id);
}

void android_stop_app(android_app_t* app) {
    if (app->running) {
        printf("[AndroidCompat] App %d stopped.\n", app->app_id);
        app->running = false;
    }
}

void android_app_status(const android_app_t* app) {
    printf("[AndroidCompat] App %d status: %s\n", app->app_id, app->running ? "running" : "stopped");
}
