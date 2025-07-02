#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include <stdbool.h>
#define MAX_WINDOWS_PER_DESKTOP 32
#define MAX_DESKTOPS 8

typedef struct window {
    int id;
    int x, y, width, height;
    int z_order;
    float z_depth;
    float transparency;
    bool focused;
    bool visible;
    char title[64];
    struct window* next;
} window_t;

typedef struct desktop {
    int id;
    window_t* windows;
    int window_count;
} desktop_t;

typedef struct window_manager {
    desktop_t desktops[MAX_DESKTOPS];
    int current_desktop;
    int next_window_id;
} window_manager_t;

void wm_init(window_manager_t* wm);
window_t* wm_create_window(window_manager_t* wm, const char* title, int x, int y, int w, int h);
void wm_destroy_window(window_manager_t* wm, int window_id);
void wm_move_window(window_manager_t* wm, int window_id, int new_x, int new_y);
void wm_resize_window(window_manager_t* wm, int window_id, int new_w, int new_h);
void wm_focus_window(window_manager_t* wm, int window_id);
void wm_snap_window(window_manager_t* wm, int window_id, int snap_type);
void wm_tile_windows(window_manager_t* wm);
void wm_switch_desktop(window_manager_t* wm, int desktop_id);
void wm_handle_gesture(window_manager_t* wm, int gesture_type, int arg0, int arg1);
void wm_render(window_manager_t* wm);
void wm_create_device_manager_window(window_manager_t* wm);
void wm_device_manager_rescan_action(window_manager_t* wm);
void wm_device_manager_details_action(window_manager_t* wm, struct list_widget* dev_list);
void wm_create_network_manager_window(window_manager_t* wm);
void wm_create_security_center_window(window_manager_t* wm);
void add_notification(const char* msg);
void render_notification_history(int x, int y);
void wm_set_high_contrast_mode(int enabled);
void wm_security_center_event_loop(window_manager_t* wm);
void wm_pull_window(window_manager_t* wm, int window_id);
void wm_push_window(window_manager_t* wm, int window_id);
void wm_fan_windows(window_manager_t* wm);
void wm_set_window_transparency(window_manager_t* wm, int window_id, float alpha);

#endif // WINDOW_MANAGER_H 