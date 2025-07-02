#ifndef GAMING_MODE_H
#define GAMING_MODE_H
#include <stdbool.h>
#define MAX_GAMES 16

typedef struct game_entry {
    int id;
    char name[64];
    bool running;
} game_entry_t;

typedef struct gaming_mode_manager {
    game_entry_t games[MAX_GAMES];
    int game_count;
    int next_game_id;
} gaming_mode_manager_t;

void gaming_mode_init(gaming_mode_manager_t* gm);
int gaming_mode_register_game(gaming_mode_manager_t* gm, const char* name);
int gaming_mode_launch_game(gaming_mode_manager_t* gm, int game_id);
void gaming_mode_tick(gaming_mode_manager_t* gm);
void gaming_mode_list(gaming_mode_manager_t* gm);

// GPU API stub (real structure)
typedef struct gpu_api {
    int (*draw_frame)(void* frame_data);
    int (*present)(void);
    // Simple framebuffer
    uint32_t framebuffer[800 * 600];
    int width;
    int height;
} gpu_api_t;

static int gpu_draw_frame_impl(void* frame_data) {
    gpu_api_t* gpu = (gpu_api_t*)frame_data;
    // Simulate drawing by filling framebuffer with a color pattern
    for (int y = 0; y < gpu->height; ++y) {
        for (int x = 0; x < gpu->width; ++x) {
            gpu->framebuffer[y * gpu->width + x] = (x ^ y) | 0xFF000000;
        }
    }
    return 0;
}

static int gpu_present_impl(void) {
    // Simulate presenting the framebuffer
    printf("[GPU] Presenting frame to display.\n");
    return 0;
}

static gpu_api_t global_gpu_api = {
    .draw_frame = gpu_draw_frame_impl,
    .present = gpu_present_impl,
    .width = 800,
    .height = 600
};

#endif // GAMING_MODE_H 