// Hypervisor API glue

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int vm_id;
    bool running;
} vm_instance_t;

void hypervisor_launch_vm(vm_instance_t* vm, int id) {
    vm->vm_id = id;
    vm->running = true;
    printf("[Hypervisor] VM %d launched.\n", id);
}

void hypervisor_stop_vm(vm_instance_t* vm) {
    if (vm->running) {
        printf("[Hypervisor] VM %d stopped.\n", vm->vm_id);
        vm->running = false;
    }
}

void hypervisor_status(const vm_instance_t* vm) {
    printf("[Hypervisor] VM %d status: %s\n", vm->vm_id, vm->running ? "running" : "stopped");
}
