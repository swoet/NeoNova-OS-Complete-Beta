// main entry with Multiboot2 header

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/modular.h"
#include "../drivers/unified_driver_framework/driver_framework.h"
#include "include/real_time.h"
#include "../core/bytecode_vm.h"
#include "../security/secure_boot.c"
#include "../security/tpm.c"
#include "../security/mac.c"
#include "../security/sandbox.c"
#include "../security/encryption.c"
#include "../core/resource_manager/resource_manager.h"
#include "../network/net_stack.h"
#include "../core/power_manager/power_manager.h"
#include "../gui/window_manager.h"
#include "../core/app_runtime/app_runtime.h"
#include "../core/process_manager.h"
#include "../gaming/gaming_mode.h"
#include "../core/dev_tools/dev_tools.h"
#include "../cloud/ai_assistant.h"
#include "../drivers/input/input_manager.h"
#include "../core/driver_manager/device_manager.h"

// Example real driver for demonstration
int example_driver_init(void) { printf("[ExampleDriver] Initialized.\n"); return 0; }
int example_driver_deinit(void) { printf("[ExampleDriver] Deinitialized.\n"); return 0; }
int example_probe(hw_fingerprint_t* fp) { printf("[ExampleDriver] Probing device.\n"); return 0; }
int example_ioctl(int cmd, void* arg) { printf("[ExampleDriver] IOCTL %d\n", cmd); return 0; }

driver_t example_driver = {
    .base = {
        .name = "example_driver",
        .type = MODULE_TYPE_DRIVER,
        .init = example_driver_init,
        .deinit = example_driver_deinit,
        .private_data = NULL,
        .next = NULL
    },
    .probe = example_probe,
    .ioctl = example_ioctl
};

#define MAX_HW_DEVICES 64

// Example bytecode program: load 42 into reg0, halt
uint8_t test_bytecode[] = {
    VM_LOAD_IMM, 0x00, 42, 0, 0, 0, // reg0 = 42
    VM_HALT
};

void launch_test_vm(void) {
    vm_t vm = {0};
    vm.code = test_bytecode;
    vm.code_size = sizeof(test_bytecode);
    vm.pc = 0;
    vm.sp = 0;
    // Run with interpreter
    int result = vm_run(&vm);
    // Optionally, try JIT (x86_64 as example)
    int jit_result = vm_jit_compile(&vm, VM_ARCH_X86_64);
    // For demonstration, result is ignored; in production, check and handle errors
}

// Dummy kernel image and hash for secure boot demo
extern uint8_t _kernel_image_start[];
extern size_t _kernel_image_size;
#define EXPECTED_KERNEL_HASH 0x12345678 // Replace with real hash in production

void security_init(void) {
    // Secure Boot: verify kernel image
    if (!secure_boot_verify(_kernel_image_start, _kernel_image_size, EXPECTED_KERNEL_HASH)) {
        // Handle integrity failure (halt or recovery)
        while (1) { __asm__("hlt"); }
    }
    // TPM: check key storage
    char key_id[] = "system_key";
    uint8_t key[32];
    size_t key_len = sizeof(key);
    if (!tpm_load_key(key_id, key, &key_len)) {
        // Handle missing key (generate, enroll, or halt)
    }
    // MAC: load default policy
    mac_policy_t default_policy = {"kernel", "all", 0xFFFFFFFF};
    mac_load_policy(&default_policy, 1);
    // Sandbox: create a sandbox for a test service
    sandbox_t sb = {0};
    sandbox_create(&sb);
}

static window_manager_t* g_wm = NULL;
static int drag_window_id = -1;
static int drag_start_x = 0, drag_start_y = 0;
static int drag_last_mouse_x = 0, drag_last_mouse_y = 0;

// Helper: get focused window
static window_t* get_focused_window(window_manager_t* wm) {
    desktop_t* d = &wm->desktops[wm->current_desktop];
    window_t* w = d->windows;
    while (w) { if (w->focused) return w; w = w->next; }
    return NULL;
}

// Keyboard event handler
void on_kbd_event(const kbd_event_t* ev) {
    if (!g_wm || ev->type != KBD_EVENT_PRESS) return;
    window_t* win = get_focused_window(g_wm);
    if (!win) return;
    switch (ev->keycode) {
        case 0x1B: // Esc
            close_window_and_cleanup(g_wm, &app_rt, &proc_table, win->id);
            break;
        case 0x09: // Tab
            // Cycle focus
            wm_focus_window(g_wm, win->next ? win->next->id : g_wm->desktops[g_wm->current_desktop].windows->id);
            break;
        case 0x48: // Up arrow
            wm_move_window(g_wm, win->id, win->x, win->y - 10);
            break;
        case 0x50: // Down arrow
            wm_move_window(g_wm, win->id, win->x, win->y + 10);
            break;
        case 0x4B: // Left arrow
            wm_move_window(g_wm, win->id, win->x - 10, win->y);
            break;
        case 0x4D: // Right arrow
            wm_move_window(g_wm, win->id, win->x + 10, win->y);
            break;
        default:
            break;
    }
}
// Mouse event handler
void on_mouse_event(const mouse_event_t* ev) {
    if (!g_wm) return;
    static int mouse_x = 0, mouse_y = 0;
    if (ev->type == MOUSE_EVENT_MOVE) {
        mouse_x += ev->dx; mouse_y += ev->dy;
        if (drag_window_id != -1) {
            wm_move_window(g_wm, drag_window_id, drag_start_x + (mouse_x - drag_last_mouse_x), drag_start_y + (mouse_y - drag_last_mouse_y));
        }
    } else if (ev->type == MOUSE_EVENT_BUTTON) {
        if (ev->buttons & 0x01) { // Left click
            // Focus window under mouse (simple: pick first for now)
            window_t* w = g_wm->desktops[g_wm->current_desktop].windows;
            if (w) {
                wm_focus_window(g_wm, w->id);
                drag_window_id = w->id;
                drag_start_x = w->x; drag_start_y = w->y;
                drag_last_mouse_x = mouse_x; drag_last_mouse_y = mouse_y;
            }
        } else {
            drag_window_id = -1;
        }
    } else if (ev->type == MOUSE_EVENT_WHEEL) {
        window_t* win = get_focused_window(g_wm);
        if (win) {
            int new_w = win->width + ev->dz * 10;
            int new_h = win->height + ev->dz * 10;
            wm_resize_window(g_wm, win->id, new_w > 50 ? new_w : 50, new_h > 50 ? new_h : 50);
        }
    }
}

void kernel_main(void) {
    // Initialize modular kernel loader (implicit via static init)
    // Register example driver
    if (register_driver(&example_driver) != 0) {
        // Handle registration failure
    }

    // Hardware fingerprinting: scan PCI devices
    hw_fingerprint_t devices[MAX_HW_DEVICES];
    int num_devices = fingerprint_hardware(devices, MAX_HW_DEVICES);
    for (int i = 0; i < num_devices; ++i) {
        // Try to match a driver
        if (match_driver(&devices[i]) != 0) {
            // No driver found: try to generate generic driver or fetch from cloud
            if (generate_generic_driver(&devices[i]) != 0) {
                fetch_driver_from_cloud(&devices[i]);
            }
            // If still no driver, log or handle as unknown device
        }
    }

    // Initialize resource manager
    resource_manager_init();
    resource_manager_update();
    resource_manager_scale();
    resource_manager_prioritize();
    resource_manager_power_adjust();

    // Security system initialization
    security_init();

    // Initialize networking stack
    net_stack_init();

    // Initialize power manager
    power_manager_init();

    // Initialize UI framework
    static window_manager_t wm;
    wm_init(&wm);
    ui_framework_init();
    ui_framework_set_window_manager(&wm);
    // Create sample windows
    wm_create_window(&wm, "Terminal", 100, 100, 400, 300);
    wm_create_window(&wm, "File Manager", 200, 150, 500, 400);
    wm_create_window(&wm, "Web Browser", 300, 200, 600, 500);
    wm_create_device_manager_window(&wm);

    // Initialize app runtime
    static app_runtime_t app_rt;
    app_runtime_init(&app_rt);
    int app1 = app_runtime_register(&app_rt, "Text Editor", APP_TYPE_NATIVE);
    int app2 = app_runtime_register(&app_rt, "Calculator", APP_TYPE_WASM);
    int app3 = app_runtime_register(&app_rt, "IDE", APP_TYPE_JVM);
    int app4 = app_runtime_register(&app_rt, "Chat", APP_TYPE_ELECTRON);
    app_runtime_start(&app_rt, app1);
    app_runtime_start(&app_rt, app2);
    app_runtime_start(&app_rt, app3);
    app_runtime_start(&app_rt, app4);
    app_runtime_list(&app_rt);

    // Initialize process manager
    static process_table_t proc_table;
    process_manager_init(&proc_table);
    // Create a process for each app and associate
    int proc1 = process_create(&proc_table, "Text Editor", app1);
    int proc2 = process_create(&proc_table, "Calculator", app2);
    int proc3 = process_create(&proc_table, "IDE", app3);
    int proc4 = process_create(&proc_table, "Chat", app4);
    app_runtime_set_process_id(&app_rt, app1, proc1);
    app_runtime_set_process_id(&app_rt, app2, proc2);
    app_runtime_set_process_id(&app_rt, app3, proc3);
    app_runtime_set_process_id(&app_rt, app4, proc4);
    process_list(&proc_table);

    // Initialize gaming mode
    static gaming_mode_manager_t gm;
    gaming_mode_init(&gm);
    int game1 = gaming_mode_register_game(&gm, "Space Invaders");
    int game2 = gaming_mode_register_game(&gm, "Chess");
    gaming_mode_launch_game(&gm, game1);
    gaming_mode_list(&gm);
    // Initialize developer tools
    static dev_tools_manager_t dt;
    dev_tools_init(&dt);
    dev_tools_list(&dt);
    // Initialize AI assistant, edge compute, predictive loader
    static ai_assistant_manager_t ai;
    ai_assistant_init(&ai);
    ai_assistant_ask(&ai, "What is the weather today?");
    static edge_compute_engine_t ec;
    edge_compute_init(&ec);
    edge_compute_submit_task(&ec, "Process sensor data");
    static predictive_loader_t pl;
    predictive_loader_init(&pl);
    predictive_loader_predict(&pl, "User opens browser");

    // Initialize input manager and register handlers
    input_manager_init();
    input_manager_register_kbd_handler(on_kbd_event);
    input_manager_register_mouse_handler(on_mouse_event);

    // Launch a test VM instance (portable bytecode)
    launch_test_vm();

    // Initialize device manager (hybrid device enumeration/driver)
    device_manager_init();
    int dev_count = 0;
    const device_info_t* devs = device_manager_list(&dev_count);
    printf("[DeviceManager] Detected %d devices:\n", dev_count);
    for (int i = 0; i < dev_count; ++i) {
        const device_info_t* d = &devs[i];
        printf("  Device %d: ", i);
        switch (d->fingerprint.bus_type) {
            case BUS_TYPE_PCI:
                printf("PCI vendor=0x%04X device=0x%04X class=0x%02X sub=0x%02X ",
                    d->fingerprint.vendor_id, d->fingerprint.device_id,
                    d->fingerprint.class_code, d->fingerprint.subclass_code);
                break;
            case BUS_TYPE_USB:
                printf("USB vendor=0x%04X product=0x%04X class=0x%02X sub=0x%02X ",
                    d->fingerprint.usb_vendor_id, d->fingerprint.usb_product_id,
                    d->fingerprint.usb_class, d->fingerprint.usb_subclass);
                break;
            case BUS_TYPE_LEGACY:
                printf("LEGACY type=%d ", d->fingerprint.legacy_type);
                break;
            default:
                printf("UNKNOWN ");
        }
        printf("driver=%s\n", d->driver_name);
    }

    g_wm = &wm;

    int last_dev_count = 0;
    int hotplug_tick = 0;
    // Main kernel loop (stub)
    while (1) {
        // Periodically update resource stats and scale
        scheduler_tick();
        // Networking stack tick
        net_stack_tick();
        // Power manager tick
        power_manager_tick();
        // UI framework tick (renders windows)
        ui_framework_tick();
        // App runtime tick
        app_runtime_tick(&app_rt);
        // Process scheduling
        process_schedule(&proc_table);
        // Gaming mode tick
        gaming_mode_tick(&gm);
        // Developer tools tick
        dev_tools_tick(&dt);
        // AI assistant tick
        ai_assistant_tick(&ai);
        // Poll input events
        input_manager_poll();
        // Simulate hotplug every 1000 ticks
        if (++hotplug_tick >= 1000) {
            hotplug_tick = 0;
            int dev_count = 0;
            device_manager_rescan();
            const device_info_t* devs = device_manager_list(&dev_count);
            if (dev_count != last_dev_count) {
                printf("[Hotplug] Device count changed: %d -> %d\n", last_dev_count, dev_count);
                wm_create_device_manager_window(g_wm); // Update window
                last_dev_count = dev_count;
            }
        }
        // IPC: handle incoming messages (example)
        ipc_message_t msg;
        while (receive_ipc_message(&msg) == 0) {
            // Dispatch or handle message
            // ...
        }
        // ... handle interrupts, scheduling, etc. ...
    }
}

// Entry point for bootloader
void _start(void) {
    kernel_main();
    while (1) { __asm__("hlt"); }
}
