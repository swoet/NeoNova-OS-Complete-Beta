#include "window_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../core/driver_manager/device_manager.h"
#include "widgets/label.h"
#include "widgets/list.h"
#include "widgets/button.h"
#include <ctype.h>
#include "../network/net_stack.h"
#include "../security/mac.c"
#include "../security/sandbox.c"

#define MAX_NOTIFICATIONS 8
static char notifications[MAX_NOTIFICATIONS][128];
static int notification_count = 0;
void add_notification(const char* msg) {
    if (notification_count < MAX_NOTIFICATIONS) {
        strncpy(notifications[notification_count++], msg, 127);
        notifications[notification_count-1][127] = '\0';
    } else {
        for (int i = 1; i < MAX_NOTIFICATIONS; ++i)
            strncpy(notifications[i-1], notifications[i], 128);
        strncpy(notifications[MAX_NOTIFICATIONS-1], msg, 127);
        notifications[MAX_NOTIFICATIONS-1][127] = '\0';
    }
}
void render_notification_history(int x, int y) {
    printf("[Notifications] Recent events:\n");
    for (int i = 0; i < notification_count; ++i) {
        printf("  - %s\n", notifications[i]);
    }
}

// Helper: find window by ID
static window_t* find_window(window_manager_t* wm, int window_id) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    window_t* w = d->windows;
    while (w) {
        if (w->id == window_id) return w;
        w = w->next;
    }
    return NULL;
}

void wm_init(window_manager_t* wm) {
    memset(wm, 0, sizeof(*wm));
    for (int i = 0; i < MAX_DESKTOPS; ++i) {
        wm->desktops[i].id = i;
        wm->desktops[i].windows = NULL;
        wm->desktops[i].window_count = 0;
    }
    wm->current_desktop = 0;
    wm->next_window_id = 1;
    printf("[WindowManager] Initialized with %d desktops.\n", MAX_DESKTOPS);
}

window_t* wm_create_window(window_manager_t* wm, const char* title, int x, int y, int w, int h) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    if (d->window_count >= MAX_WINDOWS_PER_DESKTOP) return NULL;
    window_t* win = (window_t*)malloc(sizeof(window_t));
    win->id = wm->next_window_id++;
    win->x = x; win->y = y; win->width = w; win->height = h;
    win->z_order = d->window_count;
    win->z_depth = 1.0f + 0.1f * d->window_count;
    win->transparency = 0.92f;
    win->focused = false;
    win->visible = true;
    strncpy(win->title, title, sizeof(win->title)-1);
    win->title[sizeof(win->title)-1] = '\0';
    win->next = d->windows;
    d->windows = win;
    d->window_count++;
    printf("[WindowManager] Created window %d: '%s'\n", win->id, win->title);
    return win;
}

void wm_destroy_window(window_manager_t* wm, int window_id) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    window_t **prev = &d->windows, *cur = d->windows;
    while (cur) {
        if (cur->id == window_id) {
            *prev = cur->next;
            free(cur);
            d->window_count--;
            printf("[WindowManager] Destroyed window %d\n", window_id);
            return;
        }
        prev = &cur->next;
        cur = cur->next;
    }
}

void wm_move_window(window_manager_t* wm, int window_id, int new_x, int new_y) {
    window_t* win = find_window(wm, window_id);
    if (win) {
        win->x = new_x; win->y = new_y;
        printf("[WindowManager] Moved window %d to (%d,%d)\n", window_id, new_x, new_y);
    }
}

void wm_resize_window(window_manager_t* wm, int window_id, int new_w, int new_h) {
    window_t* win = find_window(wm, window_id);
    if (win) {
        win->width = new_w; win->height = new_h;
        printf("[WindowManager] Resized window %d to %dx%d\n", window_id, new_w, new_h);
    }
}

void wm_focus_window(window_manager_t* wm, int window_id) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    window_t* w = d->windows;
    while (w) { w->focused = false; w = w->next; }
    window_t* win = find_window(wm, window_id);
    if (win) {
        win->focused = true;
        printf("[WindowManager] Focused window %d\n", window_id);
    }
}

void wm_snap_window(window_manager_t* wm, int window_id, int snap_type) {
    window_t* win = find_window(wm, window_id);
    if (!win) return;
    // snap_type: 0=left, 1=right, 2=top, 3=bottom, 4=fullscreen
    switch (snap_type) {
        case 0: win->x = 0; win->width /= 2; break;
        case 1: win->x = 640; win->width /= 2; break; // Example: screen width=1280
        case 2: win->y = 0; win->height /= 2; break;
        case 3: win->y = 360; win->height /= 2; break; // Example: screen height=720
        case 4: win->x = 0; win->y = 0; win->width = 1280; win->height = 720; break;
    }
    printf("[WindowManager] Snapped window %d (type %d)\n", window_id, snap_type);
}

void wm_tile_windows(window_manager_t* wm) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    int n = d->window_count, i = 0;
    if (n == 0) return;
    int cols = 1, rows = n;
    while (cols * cols < n) ++cols;
    rows = (n + cols - 1) / cols;
    int w = 1280 / cols, h = 720 / rows;
    window_t* win = d->windows;
    for (int r = 0; r < rows && win; ++r) {
        for (int c = 0; c < cols && win; ++c, win = win->next) {
            win->x = c * w;
            win->y = r * h;
            win->width = w;
            win->height = h;
            printf("[WindowManager] Tiled window %d to (%d,%d,%d,%d)\n", win->id, win->x, win->y, win->width, win->height);
        }
    }
}

void wm_switch_desktop(window_manager_t* wm, int desktop_id) {
    if (desktop_id < 0 || desktop_id >= MAX_DESKTOPS) return;
    wm->current_desktop = desktop_id;
    printf("[WindowManager] Switched to desktop %d\n", desktop_id);
}

void wm_handle_gesture(window_manager_t* wm, int gesture_type, int arg0, int arg1) {
    // gesture_type: 0=swipe, 1=pinch, etc.
    printf("[WindowManager] Gesture %d (%d,%d)\n", gesture_type, arg0, arg1);
    // Example: swipe left/right to switch desktop
    if (gesture_type == 0 && arg0 == -1) wm_switch_desktop(wm, wm->current_desktop - 1);
    if (gesture_type == 0 && arg0 == 1) wm_switch_desktop(wm, wm->current_desktop + 1);
}

void wm_render(window_manager_t* wm) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    window_t* win = d->windows;
    printf("[WindowManager] Rendering desktop %d with %d windows:\n", d->id, d->window_count);
    while (win) {
        printf("  Window %d: '%s' (%d,%d,%d,%d) z=%.2f alpha=%.2f %s\n", win->id, win->title, win->x, win->y, win->width, win->height, win->z_depth, win->transparency, win->focused ? "[FOCUSED]" : "");
        win = win->next;
    }
}

void wm_device_manager_rescan_action(window_manager_t* wm) {
    printf("[DeviceManager UI] Rescan triggered by user.\n");
    device_manager_rescan();
    // In a real UI, update/redraw the window contents
    wm_create_device_manager_window(wm); // For now, just recreate/print
}

void wm_device_manager_details_action(window_manager_t* wm, list_widget_t* dev_list) {
    int idx = dev_list->selected_index;
    int dev_count = 0;
    const device_info_t* devs = device_manager_list(&dev_count);
    if (idx >= 0 && idx < dev_count) {
        const device_info_t* d = &devs[idx];
        printf("[DeviceManager UI] Details for device %d:\n", idx);
        printf("  Driver: %s\n", d->driver_name);
        printf("  Status: %d\n", d->driver_status);
        switch (d->fingerprint.bus_type) {
            case BUS_TYPE_PCI:
                printf("  PCI vendor=0x%04X device=0x%04X class=0x%02X sub=0x%02X\n",
                    d->fingerprint.vendor_id, d->fingerprint.device_id,
                    d->fingerprint.class_code, d->fingerprint.subclass_code);
                break;
            case BUS_TYPE_USB:
                printf("  USB vendor=0x%04X product=0x%04X class=0x%02X sub=0x%02X\n",
                    d->fingerprint.usb_vendor_id, d->fingerprint.usb_product_id,
                    d->fingerprint.usb_class, d->fingerprint.usb_subclass);
                break;
            case BUS_TYPE_LEGACY:
                printf("  LEGACY type=%d\n", d->fingerprint.legacy_type);
                break;
            default:
                printf("  UNKNOWN\n");
        }
    } else {
        printf("[DeviceManager UI] No device selected.\n");
    }
}

void wm_create_device_manager_window(window_manager_t* wm) {
    add_notification("Device Manager opened");
    int win_id = wm_create_window(wm, "Device Manager", 150, 150, 500, 400);
    // Widgets
    label_widget_t* title = label_create(160, 160, "Device Manager");
    list_widget_t* dev_list = list_create(160, 180, 480, 200);
    button_widget_t* rescan_btn = button_create(160, 390, 100, 30, "Rescan", (void (*)(void*))wm_device_manager_rescan_action, wm);
    button_widget_t* details_btn = button_create(270, 390, 100, 30, "Details", (void (*)(void*))wm_device_manager_details_action, dev_list);
    // Fill device list
    int dev_count = 0;
    const device_info_t* devs = device_manager_list(&dev_count);
    char buf[128];
    for (int i = 0; i < dev_count; ++i) {
        const device_info_t* d = &devs[i];
        const char* icon = "";
        const char* color = "";
        switch (d->driver_status) {
            case 0: icon = "\xE2\x9C\x94"; color = "\033[32m"; break; // OK: green check
            case 1: icon = "\xF0\x9F\x93\xA6"; color = "\033[36m"; break; // Cloud: cyan package
            case 2: icon = "\xF0\x9F\xA7\xA0"; color = "\033[35m"; break; // AI: purple robot
            case -1: icon = "\xE2\x9C\x98"; color = "\033[31m"; break; // Missing: red X
            default: icon = "?"; color = "\033[0m"; break;
        }
        switch (d->fingerprint.bus_type) {
            case BUS_TYPE_PCI:
                snprintf(buf, sizeof(buf), "PCI 0x%04X:0x%04X class=0x%02X sub=0x%02X [%s]",
                    d->fingerprint.vendor_id, d->fingerprint.device_id,
                    d->fingerprint.class_code, d->fingerprint.subclass_code, d->driver_name);
                break;
            case BUS_TYPE_USB:
                snprintf(buf, sizeof(buf), "USB 0x%04X:0x%04X class=0x%02X sub=0x%02X [%s]",
                    d->fingerprint.usb_vendor_id, d->fingerprint.usb_product_id,
                    d->fingerprint.usb_class, d->fingerprint.usb_subclass, d->driver_name);
                break;
            case BUS_TYPE_LEGACY:
                snprintf(buf, sizeof(buf), "LEGACY type=%d [%s]", d->fingerprint.legacy_type, d->driver_name);
                break;
            default:
                snprintf(buf, sizeof(buf), "UNKNOWN [%s]", d->driver_name);
        }
        list_add_item(dev_list, buf, icon, color);
    }
    // Simulate selection and details label
    list_select_at(dev_list, 0); // Select first device by default
    char details[256] = "";
    if (dev_list->selected_index >= 0 && dev_list->selected_index < dev_count) {
        const device_info_t* d = &devs[dev_list->selected_index];
        snprintf(details, sizeof(details), "Driver: %s | Status: %d", d->driver_name, d->driver_status);
    } else {
        snprintf(details, sizeof(details), "No device selected.");
    }
    label_widget_t* details_label = label_create(160, 390 - 40, details);
    // Render widgets
    label_render(title);
    list_render(dev_list);
    label_render(details_label);
    button_render(rescan_btn);
    button_render(details_btn);
    render_notification_history(160, 430);
    // In a real UI, handle up/down key events to call list_select_next/prev and update details_label
}

void wm_device_manager_event_loop(window_manager_t* wm) {
    // Setup widgets
    label_widget_t* title = label_create_a11y(160, 160, "Device Manager", "Device Manager Window");
    list_widget_t* dev_list = list_create_a11y(160, 180, 480, 200, "Device List");
    button_widget_t* rescan_btn = button_create_a11y(160, 390, 100, 30, "Rescan", (void (*)(void*))wm_device_manager_rescan_action, wm, "Rescan Devices (Alt+R)");
    button_widget_t* details_btn = button_create_a11y(270, 390, 100, 30, "Details", (void (*)(void*))wm_device_manager_details_action, dev_list, "Show Device Details (Alt+D)");
    void* widgets[3] = {dev_list, rescan_btn, details_btn};
    int widget_types[3] = {1, 2, 2}; // 1=list, 2=button
    int focus_idx = 0;
    dev_list->focused = 1; rescan_btn->focused = 0; details_btn->focused = 0;
    // Fill device list
    int dev_count = 0;
    const device_info_t* devs = device_manager_list(&dev_count);
    char buf[128];
    for (int i = 0; i < dev_count; ++i) {
        const device_info_t* d = &devs[i];
        const char* icon = "";
        const char* color = "";
        switch (d->driver_status) {
            case 0: icon = "\xE2\x9C\x94"; color = "\033[32m"; break;
            case 1: icon = "\xF0\x9F\x93\xA6"; color = "\033[36m"; break;
            case 2: icon = "\xF0\x9F\xA7\xA0"; color = "\033[35m"; break;
            case -1: icon = "\xE2\x9C\x98"; color = "\033[31m"; break;
            default: icon = "?"; color = "\033[0m"; break;
        }
        switch (d->fingerprint.bus_type) {
            case BUS_TYPE_PCI:
                snprintf(buf, sizeof(buf), "PCI 0x%04X:0x%04X class=0x%02X sub=0x%02X [%s]",
                    d->fingerprint.vendor_id, d->fingerprint.device_id,
                    d->fingerprint.class_code, d->fingerprint.subclass_code, d->driver_name);
                break;
            case BUS_TYPE_USB:
                snprintf(buf, sizeof(buf), "USB 0x%04X:0x%04X class=0x%02X sub=0x%02X [%s]",
                    d->fingerprint.usb_vendor_id, d->fingerprint.usb_product_id,
                    d->fingerprint.usb_class, d->fingerprint.usb_subclass, d->driver_name);
                break;
            case BUS_TYPE_LEGACY:
                snprintf(buf, sizeof(buf), "LEGACY type=%d [%s]", d->fingerprint.legacy_type, d->driver_name);
                break;
            default:
                snprintf(buf, sizeof(buf), "UNKNOWN [%s]", d->driver_name);
        }
        list_add_item(dev_list, buf, icon, color);
    }
    list_select_at(dev_list, 0);
    char details[256] = "";
    int running = 1;
    while (running) {
        // Render
        label_render(title);
        list_render(dev_list);
        if (dev_list->selected_index >= 0 && dev_list->selected_index < dev_count) {
            const device_info_t* d = &devs[dev_list->selected_index];
            snprintf(details, sizeof(details), "Driver: %s | Status: %d", d->driver_name, d->driver_status);
        } else {
            snprintf(details, sizeof(details), "No device selected.");
        }
        label_widget_t* details_label = label_create_a11y(160, 390 - 40, details, "Device Details");
        label_render(details_label);
        button_render(rescan_btn);
        button_render(details_btn);
        // Focus navigation: Tab/Shift+Tab, arrows, Enter/Space, shortcuts
        printf("[DeviceManager UI] Tab/Shift+Tab=focus, j/k=up/down, Enter=activate, R=Rescan, D=Details, q=quit: ");
        int ch = getchar();
        while (ch == '\n') ch = getchar();
        if (tolower(ch) == 'q') running = 0;
        else if (ch == 9) { // Tab
            ((int*[]){&dev_list->focused, &rescan_btn->focused, &details_btn->focused})[focus_idx][0] = 0;
            focus_idx = (focus_idx + 1) % 3;
            ((int*[]){&dev_list->focused, &rescan_btn->focused, &details_btn->focused})[focus_idx][0] = 1;
            if (((label_widget_t*)widgets[focus_idx])->accessibility_label)
                printf("[ScreenReader] Focused: %s\n", ((label_widget_t*)widgets[focus_idx])->accessibility_label);
        } else if (ch == 'Z') { // Shift+Tab stand-in
            ((int*[]){&dev_list->focused, &rescan_btn->focused, &details_btn->focused})[focus_idx][0] = 0;
            focus_idx = (focus_idx - 1 + 3) % 3;
            ((int*[]){&dev_list->focused, &rescan_btn->focused, &details_btn->focused})[focus_idx][0] = 1;
            if (((label_widget_t*)widgets[focus_idx])->accessibility_label)
                printf("[ScreenReader] Focused: %s\n", ((label_widget_t*)widgets[focus_idx])->accessibility_label);
        } else if (widget_types[focus_idx] == 1) { // List focused
            if (tolower(ch) == 'j') list_select_next(dev_list);
            else if (tolower(ch) == 'k') list_select_prev(dev_list);
            else if (ch == 10 || ch == 13 || ch == ' ') {
                // Enter/Space: show details
                wm_device_manager_details_action(wm, dev_list);
                if (dev_list->accessibility_label)
                    printf("[ScreenReader] Activated: %s\n", dev_list->accessibility_label);
            }
        } else if (widget_types[focus_idx] == 2) { // Button focused
            if (ch == 10 || ch == 13 || ch == ' ') {
                button_widget_t* btn = (button_widget_t*)widgets[focus_idx];
                btn->pressed = 1;
                button_render(btn);
                btn->on_click(btn->user_data);
                btn->pressed = 0;
                if (btn->accessibility_label)
                    printf("[ScreenReader] Activated: %s\n", btn->accessibility_label);
            }
        }
        // Keyboard shortcuts
        if (ch == 'R') {
            rescan_btn->pressed = 1; button_render(rescan_btn);
            rescan_btn->on_click(rescan_btn->user_data);
            rescan_btn->pressed = 0;
            if (rescan_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", rescan_btn->accessibility_label);
        } else if (ch == 'D') {
            details_btn->pressed = 1; button_render(details_btn);
            details_btn->on_click(details_btn->user_data);
            details_btn->pressed = 0;
            if (details_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", details_btn->accessibility_label);
        }
        free(details_label);
    }
    free(title); free(dev_list); free(rescan_btn); free(details_btn);
}

void wm_create_network_manager_window(window_manager_t* wm) {
    add_notification("Network Manager opened");
    int win_id = wm_create_window(wm, "Network Manager", 200, 200, 520, 420);
    label_widget_t* title = label_create(210, 210, "Network Manager");
    list_widget_t* if_list = list_create(210, 230, 480, 180);
    button_widget_t* up_btn = button_create(210, 420, 80, 30, "Up", NULL, NULL);
    button_widget_t* down_btn = button_create(300, 420, 80, 30, "Down", NULL, NULL);
    button_widget_t* config_btn = button_create(390, 420, 100, 30, "Configure", NULL, NULL);
    button_widget_t* ping_btn = button_create(210, 460, 80, 30, "Ping", NULL, NULL);
    button_widget_t* dns_btn = button_create(300, 460, 80, 30, "DNS", NULL, NULL);
    // Fill interface list
    net_if_t ifs[8];
    int n = net_if_list(ifs, 8);
    char buf[128];
    for (int i = 0; i < n; ++i) {
        const char* icon = ifs[i].up ? "\xE2\x9C\x94" : "\xE2\x9C\x98";
        const char* color = ifs[i].up ? "\033[32m" : "\033[31m";
        snprintf(buf, sizeof(buf), "%s  MAC=%02X:%02X:%02X:%02X:%02X:%02X  IP=%u.%u.%u.%u  %s",
            ifs[i].name,
            ifs[i].mac[0], ifs[i].mac[1], ifs[i].mac[2], ifs[i].mac[3], ifs[i].mac[4], ifs[i].mac[5],
            (ifs[i].ip_addr >> 24) & 0xFF, (ifs[i].ip_addr >> 16) & 0xFF, (ifs[i].ip_addr >> 8) & 0xFF, ifs[i].ip_addr & 0xFF,
            ifs[i].up ? "UP" : "DOWN");
        list_add_item(if_list, buf, icon, color);
    }
    // Render widgets
    label_render(title);
    list_render(if_list);
    button_render(up_btn);
    button_render(down_btn);
    button_render(config_btn);
    button_render(ping_btn);
    button_render(dns_btn);
    render_notification_history(210, 500);
    // In a real UI, wire up button actions and handle events
}

void wm_network_manager_event_loop(window_manager_t* wm) {
    label_widget_t* title = label_create_a11y(210, 210, "Network Manager", "Network Manager Window");
    list_widget_t* if_list = list_create_a11y(210, 230, 480, 180, "Network Interface List");
    button_widget_t* up_btn = button_create_a11y(210, 420, 80, 30, "Up", NULL, NULL, "Bring Interface Up (Alt+U)");
    button_widget_t* down_btn = button_create_a11y(300, 420, 80, 30, "Down", NULL, NULL, "Bring Interface Down (Alt+W)");
    button_widget_t* config_btn = button_create_a11y(390, 420, 100, 30, "Configure", NULL, NULL, "Configure Interface (Alt+C)");
    button_widget_t* ping_btn = button_create_a11y(210, 460, 80, 30, "Ping", NULL, NULL, "Ping Test (Alt+P)");
    button_widget_t* dns_btn = button_create_a11y(300, 460, 80, 30, "DNS", NULL, NULL, "DNS Test (Alt+N)");
    void* widgets[6] = {if_list, up_btn, down_btn, config_btn, ping_btn, dns_btn};
    int widget_types[6] = {1,2,2,2,2,2};
    int focus_idx = 0;
    if_list->focused = 1; up_btn->focused = 0; down_btn->focused = 0; config_btn->focused = 0; ping_btn->focused = 0; dns_btn->focused = 0;
    net_if_t ifs[8];
    int n = net_if_list(ifs, 8);
    char buf[128];
    for (int i = 0; i < n; ++i) {
        const char* icon = ifs[i].up ? "\xE2\x9C\x94" : "\xE2\x9C\x98";
        const char* color = ifs[i].up ? "\033[32m" : "\033[31m";
        snprintf(buf, sizeof(buf), "%s  MAC=%02X:%02X:%02X:%02X:%02X:%02X  IP=%u.%u.%u.%u  %s",
            ifs[i].name,
            ifs[i].mac[0], ifs[i].mac[1], ifs[i].mac[2], ifs[i].mac[3], ifs[i].mac[4], ifs[i].mac[5],
            (ifs[i].ip_addr >> 24) & 0xFF, (ifs[i].ip_addr >> 16) & 0xFF, (ifs[i].ip_addr >> 8) & 0xFF, ifs[i].ip_addr & 0xFF,
            ifs[i].up ? "UP" : "DOWN");
        list_add_item(if_list, buf, icon, color);
    }
    list_select_at(if_list, 0);
    static char notification[256] = "";
    int running = 1;
    while (running) {
        // Render
        label_render(title);
        list_render(if_list);
        button_render(up_btn);
        button_render(down_btn);
        button_render(config_btn);
        button_render(ping_btn);
        button_render(dns_btn);
        label_widget_t* notif_label = label_create_a11y(210, 500, notification, "Notification Banner");
        label_render(notif_label);
        free(notif_label);
        // Focus navigation: Tab/Shift+Tab, arrows, Enter/Space, shortcuts
        printf("[NetMgr UI] Tab/Shift+Tab=focus, j/k=up/down, Enter=activate, U=Up, W=Down, C=Config, P=Ping, N=DNS, q=quit: ");
        int ch = getchar();
        while (ch == '\n') ch = getchar();
        if (tolower(ch) == 'q') running = 0;
        else if (ch == 9) { // Tab
            ((int*[]){&if_list->focused, &up_btn->focused, &down_btn->focused, &config_btn->focused, &ping_btn->focused, &dns_btn->focused})[focus_idx][0] = 0;
            focus_idx = (focus_idx + 1) % 6;
            ((int*[]){&if_list->focused, &up_btn->focused, &down_btn->focused, &config_btn->focused, &ping_btn->focused, &dns_btn->focused})[focus_idx][0] = 1;
            if (((label_widget_t*)widgets[focus_idx])->accessibility_label)
                printf("[ScreenReader] Focused: %s\n", ((label_widget_t*)widgets[focus_idx])->accessibility_label);
        } else if (ch == 'Z') { // Shift+Tab stand-in
            ((int*[]){&if_list->focused, &up_btn->focused, &down_btn->focused, &config_btn->focused, &ping_btn->focused, &dns_btn->focused})[focus_idx][0] = 0;
            focus_idx = (focus_idx - 1 + 6) % 6;
            ((int*[]){&if_list->focused, &up_btn->focused, &down_btn->focused, &config_btn->focused, &ping_btn->focused, &dns_btn->focused})[focus_idx][0] = 1;
            if (((label_widget_t*)widgets[focus_idx])->accessibility_label)
                printf("[ScreenReader] Focused: %s\n", ((label_widget_t*)widgets[focus_idx])->accessibility_label);
        } else if (widget_types[focus_idx] == 1) { // List focused
            if (tolower(ch) == 'j') list_select_next(if_list);
            else if (tolower(ch) == 'k') list_select_prev(if_list);
        } else if (widget_types[focus_idx] == 2) { // Button focused
            if (ch == 10 || ch == 13 || ch == ' ') {
                button_widget_t* btn = (button_widget_t*)widgets[focus_idx];
                btn->pressed = 1;
                button_render(btn);
                // Button actions
                if (btn == up_btn && if_list->selected_index >= 0) {
                    net_if_set_up(ifs[if_list->selected_index].name, 1);
                    snprintf(notification, sizeof(notification), "%s is now UP", ifs[if_list->selected_index].name);
                } else if (btn == down_btn && if_list->selected_index >= 0) {
                    net_if_set_up(ifs[if_list->selected_index].name, 0);
                    snprintf(notification, sizeof(notification), "%s is now DOWN", ifs[if_list->selected_index].name);
                } else if (btn == config_btn && if_list->selected_index >= 0) {
                    char ip[32], mask[32], gw[32];
                    printf("[NetMgr UI] Enter new IP: ");
                    scanf("%31s", ip);
                    printf("[NetMgr UI] Enter new netmask: ");
                    scanf("%31s", mask);
                    printf("[NetMgr UI] Enter new gateway: ");
                    scanf("%31s", gw);
                    uint32_t ip_addr = inet_addr(ip);
                    uint32_t netmask = inet_addr(mask);
                    uint32_t gateway = inet_addr(gw);
                    net_if_configure(ifs[if_list->selected_index].name, ip_addr, netmask, gateway);
                    snprintf(notification, sizeof(notification), "%s configured to IP %s", ifs[if_list->selected_index].name, ip);
                } else if (btn == ping_btn) {
                    snprintf(notification, sizeof(notification), "Ping complete (stub)");
                } else if (btn == dns_btn) {
                    snprintf(notification, sizeof(notification), "DNS test complete (stub)");
                }
                btn->pressed = 0;
                if (btn->accessibility_label)
                    printf("[ScreenReader] Activated: %s\n", btn->accessibility_label);
            }
        }
        // Keyboard shortcuts
        if (ch == 'U' && if_list->selected_index >= 0) {
            up_btn->pressed = 1; button_render(up_btn);
            net_if_set_up(ifs[if_list->selected_index].name, 1);
            snprintf(notification, sizeof(notification), "%s is now UP", ifs[if_list->selected_index].name);
            up_btn->pressed = 0;
            if (up_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", up_btn->accessibility_label);
        } else if (ch == 'W' && if_list->selected_index >= 0) {
            down_btn->pressed = 1; button_render(down_btn);
            net_if_set_up(ifs[if_list->selected_index].name, 0);
            snprintf(notification, sizeof(notification), "%s is now DOWN", ifs[if_list->selected_index].name);
            down_btn->pressed = 0;
            if (down_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", down_btn->accessibility_label);
        } else if (ch == 'C' && if_list->selected_index >= 0) {
            config_btn->pressed = 1; button_render(config_btn);
            char ip[32], mask[32], gw[32];
            printf("[NetMgr UI] Enter new IP: ");
            scanf("%31s", ip);
            printf("[NetMgr UI] Enter new netmask: ");
            scanf("%31s", mask);
            printf("[NetMgr UI] Enter new gateway: ");
            scanf("%31s", gw);
            uint32_t ip_addr = inet_addr(ip);
            uint32_t netmask = inet_addr(mask);
            uint32_t gateway = inet_addr(gw);
            net_if_configure(ifs[if_list->selected_index].name, ip_addr, netmask, gateway);
            snprintf(notification, sizeof(notification), "%s configured to IP %s", ifs[if_list->selected_index].name, ip);
            config_btn->pressed = 0;
            if (config_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", config_btn->accessibility_label);
        } else if (ch == 'P') {
            ping_btn->pressed = 1; button_render(ping_btn);
            snprintf(notification, sizeof(notification), "Ping complete (stub)");
            ping_btn->pressed = 0;
            if (ping_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", ping_btn->accessibility_label);
        } else if (ch == 'N') {
            dns_btn->pressed = 1; button_render(dns_btn);
            snprintf(notification, sizeof(notification), "DNS test complete (stub)");
            dns_btn->pressed = 0;
            if (dns_btn->accessibility_label)
                printf("[ScreenReader] Activated: %s\n", dns_btn->accessibility_label);
        }
    }
    free(title); free(if_list); free(up_btn); free(down_btn); free(config_btn); free(ping_btn); free(dns_btn);
}

void wm_create_security_center_window(window_manager_t* wm) {
    int win_id = wm_create_window(wm, "Security Center", 250, 250, 520, 420);
    label_widget_t* title = label_create(260, 260, "Security Center");
    list_widget_t* policy_list = list_create(260, 280, 480, 80);
    list_widget_t* sandbox_list = list_create(260, 370, 480, 80);
    // List MAC policies
    extern mac_policy_t mac_policies[];
    extern size_t mac_policy_count;
    char buf[128];
    for (size_t i = 0; i < mac_policy_count; ++i) {
        snprintf(buf, sizeof(buf), "%s -> %s perms=0x%X", mac_policies[i].subject, mac_policies[i].object, mac_policies[i].permissions);
        list_add_item(policy_list, buf, "\xF0\x9F\x94\x92", "\033[36m");
    }
    // List sandboxes
    extern sandbox_t sandboxes[];
    extern int sandbox_count;
    for (int i = 0; i < sandbox_count; ++i) {
        snprintf(buf, sizeof(buf), "Sandbox %u", sandboxes[i].id);
        list_add_item(sandbox_list, buf, "\xF0\x9F\x94\x91", "\033[35m");
    }
    // Render widgets
    label_render(title);
    label_render(label_create(260, 275, "MAC Policies:"));
    list_render(policy_list);
    label_render(label_create(260, 365, "Active Sandboxes:"));
    list_render(sandbox_list);
    render_notification_history(260, 460);
    // In a real UI, add event log, buttons for policy load/audit, and manage sandboxes
}

void wm_security_center_event_loop(window_manager_t* wm) {
    label_widget_t* title = label_create_a11y(260, 260, "Security Center", "Security Center Window");
    list_widget_t* policy_list = list_create_a11y(260, 280, 480, 80, "MAC Policy List");
    list_widget_t* sandbox_list = list_create_a11y(260, 370, 480, 80, "Sandbox List");
    void* widgets[2] = {policy_list, sandbox_list};
    int widget_types[2] = {1,1};
    int focus_idx = 0;
    policy_list->focused = 1; sandbox_list->focused = 0;
    // List MAC policies
    extern mac_policy_t mac_policies[];
    extern size_t mac_policy_count;
    char buf[128];
    for (size_t i = 0; i < mac_policy_count; ++i) {
        snprintf(buf, sizeof(buf), "%s -> %s perms=0x%X", mac_policies[i].subject, mac_policies[i].object, mac_policies[i].permissions);
        list_add_item(policy_list, buf, "\xF0\x9F\x94\x92", "\033[36m");
    }
    // List sandboxes
    extern sandbox_t sandboxes[];
    extern int sandbox_count;
    for (int i = 0; i < sandbox_count; ++i) {
        snprintf(buf, sizeof(buf), "Sandbox %u", sandboxes[i].id);
        list_add_item(sandbox_list, buf, "\xF0\x9F\x94\x91", "\033[35m");
    }
    list_select_at(policy_list, 0);
    list_select_at(sandbox_list, 0);
    int running = 1;
    while (running) {
        label_render(title);
        label_render(label_create_a11y(260, 275, "MAC Policies:", "MAC Policies Label"));
        list_render(policy_list);
        label_render(label_create_a11y(260, 365, "Active Sandboxes:", "Active Sandboxes Label"));
        list_render(sandbox_list);
        render_notification_history(260, 460);
        printf("[SecurityCenter UI] Tab/Shift+Tab=focus, j/k=up/down, Enter=select, P=select policy, S=select sandbox, q=quit: ");
        int ch = getchar();
        while (ch == '\n') ch = getchar();
        if (tolower(ch) == 'q') running = 0;
        else if (ch == 9) { // Tab
            ((int*[]){&policy_list->focused, &sandbox_list->focused})[focus_idx][0] = 0;
            focus_idx = (focus_idx + 1) % 2;
            ((int*[]){&policy_list->focused, &sandbox_list->focused})[focus_idx][0] = 1;
            if (((label_widget_t*)widgets[focus_idx])->accessibility_label)
                printf("[ScreenReader] Focused: %s\n", ((label_widget_t*)widgets[focus_idx])->accessibility_label);
        } else if (ch == 'Z') { // Shift+Tab stand-in
            ((int*[]){&policy_list->focused, &sandbox_list->focused})[focus_idx][0] = 0;
            focus_idx = (focus_idx - 1 + 2) % 2;
            ((int*[]){&policy_list->focused, &sandbox_list->focused})[focus_idx][0] = 1;
            if (((label_widget_t*)widgets[focus_idx])->accessibility_label)
                printf("[ScreenReader] Focused: %s\n", ((label_widget_t*)widgets[focus_idx])->accessibility_label);
        } else if (widget_types[focus_idx] == 1) {
            if (tolower(ch) == 'j') {
                if (focus_idx == 0) list_select_next(policy_list);
                else list_select_next(sandbox_list);
            } else if (tolower(ch) == 'k') {
                if (focus_idx == 0) list_select_prev(policy_list);
                else list_select_prev(sandbox_list);
            } else if (ch == 10 || ch == 13 || ch == ' ') {
                // Enter/Space: stub for selection
                printf("[ScreenReader] Activated: %s\n", ((list_widget_t*)widgets[focus_idx])->accessibility_label);
            }
        }
        // Keyboard shortcuts
        if (ch == 'P') {
            printf("[ScreenReader] Activated: %s\n", policy_list->accessibility_label);
        } else if (ch == 'S') {
            printf("[ScreenReader] Activated: %s\n", sandbox_list->accessibility_label);
        }
    }
    free(title); free(policy_list); free(sandbox_list);
}

void wm_set_high_contrast_mode(int enabled) {
    label_set_high_contrast(enabled);
    list_set_high_contrast(enabled);
    button_set_high_contrast(enabled);
}

void wm_pull_window(window_manager_t* wm, int window_id) {
    window_t* win = find_window(wm, window_id);
    if (win) {
        win->z_depth -= 0.5f;
        if (win->z_depth < 0.1f) win->z_depth = 0.1f;
        printf("[HoloFlow] Pulled window %d closer (z=%.2f)\n", window_id, win->z_depth);
    }
}
void wm_push_window(window_manager_t* wm, int window_id) {
    window_t* win = find_window(wm, window_id);
    if (win) {
        win->z_depth += 0.5f;
        if (win->z_depth > 10.0f) win->z_depth = 10.0f;
        printf("[HoloFlow] Pushed window %d deeper (z=%.2f)\n", window_id, win->z_depth);
    }
}
void wm_fan_windows(window_manager_t* wm) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    window_t* win = d->windows;
    int i = 0;
    while (win) {
        win->z_depth = 1.0f + 0.2f * i;
        win->x = 100 + 40 * i;
        win->y = 100 + 20 * i;
        win->transparency = 0.7f + 0.03f * i;
        printf("[HoloFlow] Fanned window %d to (z=%.2f, alpha=%.2f)\n", win->id, win->z_depth, win->transparency);
        win = win->next;
        i++;
    }
}
void wm_set_window_transparency(window_manager_t* wm, int window_id, float alpha) {
    window_t* win = find_window(wm, window_id);
    if (win) {
        if (alpha < 0.1f) alpha = 0.1f;
        if (alpha > 1.0f) alpha = 1.0f;
        win->transparency = alpha;
        printf("[HoloFlow] Set window %d transparency to %.2f\n", window_id, alpha);
    }
} 