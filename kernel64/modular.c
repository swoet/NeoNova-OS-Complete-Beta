// module loader

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <assert.h>
#include "modular.h"
#include <openssl/sha.h>

// Module types
typedef enum {
    MODULE_TYPE_DRIVER,
    MODULE_TYPE_FILESYSTEM,
    MODULE_TYPE_SCHEDULER,
    MODULE_TYPE_OTHER
} module_type_t;

// Module interface
typedef struct kernel_module {
    const char* name;
    module_type_t type;
    int (*init)(void);
    int (*deinit)(void);
    void* private_data;
    struct kernel_module* next;
} kernel_module_t;

// Hybrid kernel: service registry
static kernel_service_t* service_list = NULL;

// Modular filesystem registry
static fs_module_t* fs_list = NULL;

// IPC: Multiple named channels and message queues
#define MAX_IPC_CHANNELS 16
#define MAX_IPC_QUEUE 64

typedef struct {
    char name[64];
    HANDLE hPipe;
    ipc_message_t queue[MAX_IPC_QUEUE];
    int queue_head, queue_tail;
    CRITICAL_SECTION lock;
} ipc_channel_t;

static ipc_channel_t ipc_channels[MAX_IPC_CHANNELS] = {0};
static int ipc_channel_count = 0;

static ipc_channel_t* find_or_create_channel(const char* name) {
    for (int i = 0; i < ipc_channel_count; ++i) {
        if (strcmp(ipc_channels[i].name, name) == 0) return &ipc_channels[i];
    }
    if (ipc_channel_count < MAX_IPC_CHANNELS) {
        strncpy(ipc_channels[ipc_channel_count].name, name, sizeof(ipc_channels[ipc_channel_count].name)-1);
        InitializeCriticalSection(&ipc_channels[ipc_channel_count].lock);
        ipc_channel_count++;
        return &ipc_channels[ipc_channel_count-1];
    }
    return NULL;
}

// Example: trusted module signature (in real use, this would be more secure)
static const char* trusted_signature = "trusted_module_signature";

static int verify_module_signature(kernel_module_t* mod) {
    if (!mod || !mod->private_data) return 0;
    // For demonstration, compare signature string
    return strcmp((const char*)mod->private_data, trusted_signature) == 0;
}

static int check_module_permissions(kernel_module_t* mod) {
    // Example: only allow drivers and filesystems for now
    return (mod->type == MODULE_TYPE_DRIVER || mod->type == MODULE_TYPE_FILESYSTEM);
}

// Register a module (load)
int register_module(kernel_module_t* mod) {
    if (!mod || !mod->init) return -1;
    if (!verify_module_signature(mod)) {
        printf("[Security] Module signature verification failed for %s\n", mod->name);
        return -3;
    }
    if (!check_module_permissions(mod)) {
        printf("[Security] Permission denied for module %s\n", mod->name);
        return -4;
    }
    if (mod->init() != 0) return -2;
    mod->next = module_list;
    module_list = mod;
    return 0;
}

// Unregister a module (unload)
int unregister_module(const char* name) {
    kernel_module_t **cur = &module_list, *prev = NULL;
    while (*cur) {
        if ((*cur)->name && name && strcmp((*cur)->name, name) == 0) {
            if ((*cur)->deinit) {
                int deinit_result = (*cur)->deinit();
                if (deinit_result != 0) {
                    // Rollback: try to re-init
                    printf("[Recovery] Deinit failed for %s, attempting rollback\n", (*cur)->name);
                    if ((*cur)->init && (*cur)->init() == 0) {
                        printf("[Recovery] Rollback succeeded for %s\n", (*cur)->name);
                        return -2;
                    } else {
                        printf("[Recovery] Rollback failed for %s, isolating module\n", (*cur)->name);
                        // Optionally quarantine or mark as failed
                        return -3;
                    }
                }
            }
            kernel_module_t* to_remove = *cur;
            *cur = (*cur)->next;
            return 0;
        }
        prev = *cur;
        cur = &((*cur)->next);
    }
    return -1; // Not found
}

// Find a module by name
typedef int (*module_callback_t)(kernel_module_t* mod, void* ctx);
kernel_module_t* find_module(const char* name) {
    kernel_module_t* cur = module_list;
    while (cur) {
        if (cur->name && name && strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

// Iterate modules (for management, recovery, etc.)
void foreach_module(module_callback_t cb, void* ctx) {
    kernel_module_t* cur = module_list;
    while (cur) {
        cb(cur, ctx);
        cur = cur->next;
    }
}

// Recovery: restart or isolate failed modules/services
void recover_from_module_failure(const char* name) {
    printf("[Recovery] Module failure: %s\n", name);
    kernel_module_t* mod = find_module(name);
    if (mod && mod->deinit) mod->deinit();
    if (mod && mod->init) {
        int r = mod->init();
        if (r == 0) {
            printf("[Recovery] Module %s restarted successfully.\n", name);
            return;
        }
    }
    printf("[Recovery] Could not restart module %s. Isolating.\n", name);
    // Optionally, mark as quarantined, notify admin, etc.
}

int register_service(kernel_service_t* svc) {
    if (!svc || !svc->start) return -1;
    svc->next = service_list;
    service_list = svc;
    return 0;
}

int unregister_service(const char* name) {
    kernel_service_t **cur = &service_list;
    while (*cur) {
        if ((*cur)->name && name && strcmp((*cur)->name, name) == 0) {
            *cur = (*cur)->next;
            return 0;
        }
        cur = &((*cur)->next);
    }
    return -1;
}

// IPC: Multiple named channels and message queues
int send_ipc_message(const ipc_message_t* msg) {
    if (!msg) return -1;
    ipc_channel_t* ch = find_or_create_channel("neonova_ipc");
    if (!ch) return -2;
    EnterCriticalSection(&ch->lock);
    int next = (ch->queue_tail + 1) % MAX_IPC_QUEUE;
    if (next == ch->queue_head) {
        LeaveCriticalSection(&ch->lock);
        printf("[IPC] Queue full\n");
        return -3;
    }
    ch->queue[ch->queue_tail] = *msg;
    ch->queue_tail = next;
    LeaveCriticalSection(&ch->lock);
    // Optionally, signal waiting receivers
    return 0;
}

int receive_ipc_message(ipc_message_t* msg) {
    if (!msg) return -1;
    ipc_channel_t* ch = find_or_create_channel("neonova_ipc");
    if (!ch) return -2;
    EnterCriticalSection(&ch->lock);
    if (ch->queue_head == ch->queue_tail) {
        LeaveCriticalSection(&ch->lock);
        return -3; // Empty
    }
    *msg = ch->queue[ch->queue_head];
    ch->queue_head = (ch->queue_head + 1) % MAX_IPC_QUEUE;
    LeaveCriticalSection(&ch->lock);
    return 0;
}

int register_fs_module(fs_module_t* fs) {
    if (!fs || !fs->ops) return -1;
    fs->next = fs_list;
    fs_list = fs;
    return 0;
}

int unregister_fs_module(const char* name) {
    fs_module_t **cur = &fs_list;
    while (*cur) {
        if ((*cur)->name && name && strcmp((*cur)->name, name) == 0) {
            *cur = (*cur)->next;
            return 0;
        }
        cur = &((*cur)->next);
    }
    return -1;
}

fs_module_t* find_fs_module(const char* name) {
    fs_module_t* cur = fs_list;
    while (cur) {
        if (cur->name && name && strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}
