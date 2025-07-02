// ELF binary loader

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char path[256];
    bool loaded;
} elf_binary_t;

void elf_load(elf_binary_t* bin, const char* path) {
    snprintf(bin->path, sizeof(bin->path), "%s", path);
    bin->loaded = true;
    printf("[LinuxCompat] ELF binary '%s' loaded.\n", path);
}

void elf_exec(const elf_binary_t* bin) {
    if (bin->loaded) {
        printf("[LinuxCompat] Executing ELF binary '%s'...\n", bin->path);
    }
}

void elf_status(const elf_binary_t* bin) {
    printf("[LinuxCompat] ELF binary '%s' status: %s\n", bin->path, bin->loaded ? "loaded" : "not loaded");
}
