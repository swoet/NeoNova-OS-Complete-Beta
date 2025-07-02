// window manager and compositor

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_WINDOWS 32
#define MAX_WORKSPACES 8

typedef struct {
    int id;
    char title[64];
    int x, y, w, h;
    bool visible;
} window_t;

typedef struct {
    window_t windows[MAX_WINDOWS];
    int window_count;
    int active_workspace;
} desktop_manager_t;

desktop_manager_t g_desktop_manager = {0};

void desktop_manager_init() {
    g_desktop_manager.window_count = 0;
    g_desktop_manager.active_workspace = 0;
    printf("[DesktopManager] Initialized.\n");
}

void desktop_manager_add_window(const char* title, int x, int y, int w, int h) {
    if (g_desktop_manager.window_count < MAX_WINDOWS) {
        window_t* win = &g_desktop_manager.windows[g_desktop_manager.window_count++];
        win->id = g_desktop_manager.window_count;
        strncpy(win->title, title, 63); win->title[63] = '\0';
        win->x = x; win->y = y; win->w = w; win->h = h; win->visible = true;
        printf("[DesktopManager] Window '%s' added at (%d,%d) size %dx%d\n", title, x, y, w, h);
    }
}

void desktop_manager_switch_workspace(int ws) {
    if (ws >= 0 && ws < MAX_WORKSPACES) {
        g_desktop_manager.active_workspace = ws;
        printf("[DesktopManager] Switched to workspace %d\n", ws);
    }
}

void desktop_manager_composite() {
    printf("[DesktopManager] Compositing %d windows on workspace %d\n", g_desktop_manager.window_count, g_desktop_manager.active_workspace);
    for (int i = 0; i < g_desktop_manager.window_count; ++i) {
        window_t* win = &g_desktop_manager.windows[i];
        if (win->visible) {
            printf("  Drawing window %d: '%s'\n", win->id, win->title);
        }
    }
}
