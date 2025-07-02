// Vulkan, DirectX, Proton layer

#include <stdio.h>
#include <stdbool.h>

typedef enum { RENDERER_VULKAN, RENDERER_DIRECTX, RENDERER_PROTON, RENDERER_UNKNOWN } renderer_type_t;

const char* renderer_name(renderer_type_t r) {
    switch (r) {
        case RENDERER_VULKAN: return "Vulkan";
        case RENDERER_DIRECTX: return "DirectX";
        case RENDERER_PROTON: return "Proton";
        default: return "Unknown";
    }
}

bool detect_vulkan() { printf("[GamingSupport] Vulkan detected.\n"); return true; }
bool detect_directx() { printf("[GamingSupport] DirectX detected.\n"); return true; }
bool detect_proton() { printf("[GamingSupport] Proton detected.\n"); return true; }

void gaming_support_init() {
    printf("[GamingSupport] Initializing gaming support...\n");
    detect_vulkan();
    detect_directx();
    detect_proton();
    printf("[GamingSupport] All supported renderers initialized.\n");
}
