#ifndef MODULAR_H
#define MODULAR_H

#include <stddef.h>
#include <stdint.h>
#include <windows.h>

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

// API
int register_module(kernel_module_t* mod);
int unregister_module(const char* name);
kernel_module_t* find_module(const char* name);
typedef int (*module_callback_t)(kernel_module_t* mod, void* ctx);
void foreach_module(module_callback_t cb, void* ctx);
void recover_from_module_failure(const char* name);

// Hybrid kernel: IPC and service separation stubs

typedef enum {
    SERVICE_TYPE_DRIVER,
    SERVICE_TYPE_IPC,
    SERVICE_TYPE_MEMORY,
    SERVICE_TYPE_SCHEDULER,
    SERVICE_TYPE_OTHER
} service_type_t;

typedef struct kernel_service {
    const char* name;
    service_type_t type;
    int (*start)(void);
    int (*stop)(void);
    void* private_data;
    struct kernel_service* next;
} kernel_service_t;

// Register/unregister a kernel service
int register_service(kernel_service_t* svc);
int unregister_service(const char* name);

// IPC/message passing API (stub)
#define IPC_QUEUE_SIZE 128

typedef struct ipc_message {
    uint32_t src;
    uint32_t dest;
    uint32_t type;
    void* payload;
    size_t payload_size;
} ipc_message_t;

typedef struct ipc_queue {
    ipc_message_t queue[IPC_QUEUE_SIZE];
    int head, tail;
    CRITICAL_SECTION lock;
} ipc_queue_t;

static ipc_queue_t global_ipc_queue = {0};

static void ipc_queue_init(ipc_queue_t* q) {
    q->head = q->tail = 0;
    InitializeCriticalSection(&q->lock);
}

static int send_ipc_message(const ipc_message_t* msg) {
    ipc_queue_t* q = &global_ipc_queue;
    EnterCriticalSection(&q->lock);
    int next = (q->tail + 1) % IPC_QUEUE_SIZE;
    if (next == q->head) {
        LeaveCriticalSection(&q->lock);
        return -1; // Full
    }
    q->queue[q->tail] = *msg;
    q->tail = next;
    LeaveCriticalSection(&q->lock);
    return 0;
}

static int receive_ipc_message(ipc_message_t* msg) {
    ipc_queue_t* q = &global_ipc_queue;
    EnterCriticalSection(&q->lock);
    if (q->head == q->tail) {
        LeaveCriticalSection(&q->lock);
        return -1; // Empty
    }
    *msg = q->queue[q->head];
    q->head = (q->head + 1) % IPC_QUEUE_SIZE;
    LeaveCriticalSection(&q->lock);
    return 0;
}

// Modular Filesystem Interface

typedef struct fs_snapshot_info {
    uint64_t id;
    const char* name;
    uint64_t timestamp;
} fs_snapshot_info_t;

typedef struct fs_ops {
    int (*mount)(const char* device, const char* mountpoint);
    int (*unmount)(const char* mountpoint);
    int (*read)(const char* path, void* buf, size_t len, uint64_t offset);
    int (*write)(const char* path, const void* buf, size_t len, uint64_t offset);
    int (*snapshot)(const char* path, fs_snapshot_info_t* out_info);
    int (*restore_snapshot)(const fs_snapshot_info_t* info);
    int (*deduplicate)(const char* path);
    int (*encrypt)(const char* path, const void* key, size_t key_len);
    int (*decrypt)(const char* path, const void* key, size_t key_len);
    int (*backup)(const char* path, const char* dest);
    int (*restore_backup)(const char* backup_path, const char* dest);
    // Add more as needed
} fs_ops_t;

typedef struct fs_module {
    const char* name;
    fs_ops_t* ops;
    struct fs_module* next;
} fs_module_t;

int register_fs_module(fs_module_t* fs);
int unregister_fs_module(const char* name);
fs_module_t* find_fs_module(const char* name);

#endif // MODULAR_H 