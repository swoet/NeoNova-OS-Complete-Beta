// hybrid kernel core

// Main kernel logic is now initialized in main.c (see kernel_main)
// This file can be used for additional kernel services and logic.

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define MAX_PROCESSES 64
#define PAGE_SIZE 4096
#define MAX_PAGES 1024
#define IPC_QUEUE_SIZE 128

// Simple page allocator
static uint8_t memory_pool[PAGE_SIZE * MAX_PAGES];
static bool page_used[MAX_PAGES] = {0};

void* alloc_page() {
    for (int i = 0; i < MAX_PAGES; ++i) {
        if (!page_used[i]) {
            page_used[i] = true;
            return &memory_pool[i * PAGE_SIZE];
        }
    }
    return NULL;
}

void free_page(void* ptr) {
    size_t offset = (uint8_t*)ptr - memory_pool;
    if (offset % PAGE_SIZE == 0 && offset / PAGE_SIZE < MAX_PAGES) {
        page_used[offset / PAGE_SIZE] = false;
    }
}

// IPC: simple message queue
typedef struct ipc_message {
    uint32_t src;
    uint32_t dest;
    uint32_t type;
    void* payload;
    size_t payload_size;
} ipc_message_t;

static ipc_message_t ipc_queue[IPC_QUEUE_SIZE];
static int ipc_head = 0, ipc_tail = 0;

int send_ipc_message(const ipc_message_t* msg) {
    int next = (ipc_tail + 1) % IPC_QUEUE_SIZE;
    if (next == ipc_head) return -1; // Full
    ipc_queue[ipc_tail] = *msg;
    ipc_tail = next;
    return 0;
}

int receive_ipc_message(ipc_message_t* msg) {
    if (ipc_head == ipc_tail) return -1; // Empty
    *msg = ipc_queue[ipc_head];
    ipc_head = (ipc_head + 1) % IPC_QUEUE_SIZE;
    return 0;
}

// Simple round-robin scheduler
static int current_process = 0;
static int process_count = 0;
static int process_table[MAX_PROCESSES];

void scheduler_tick() {
    if (process_count == 0) return;
    current_process = (current_process + 1) % process_count;
    printf("[Scheduler] Switched to process %d\n", process_table[current_process]);
}
