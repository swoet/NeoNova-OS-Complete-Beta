#include "gaming_mode.h"
#include <stdio.h>
#include <string.h>

static int gpu_draw_frame(void* frame_data) {
    printf("[GPU] Drawing frame...\n");
    return 0;
}
static int gpu_present(void) {
    printf("[GPU] Presenting frame...\n");
    return 0;
}
gpu_api_t global_gpu_api = {
    .draw_frame = gpu_draw_frame,
    .present = gpu_present
};

void gaming_mode_init(gaming_mode_manager_t* gm) {
    memset(gm, 0, sizeof(*gm));
    gm->next_game_id = 1;
    printf("[GamingMode] Initialized.\n");
}

int gaming_mode_register_game(gaming_mode_manager_t* gm, const char* name) {
    if (gm->game_count >= MAX_GAMES) return -1;
    game_entry_t* game = &gm->games[gm->game_count++];
    game->id = gm->next_game_id++;
    strncpy(game->name, name, sizeof(game->name)-1);
    game->name[sizeof(game->name)-1] = '\0';
    game->running = false;
    printf("[GamingMode] Registered game %d: '%s'\n", game->id, game->name);
    return game->id;
}

int gaming_mode_launch_game(gaming_mode_manager_t* gm, int game_id) {
    for (int i = 0; i < gm->game_count; ++i) {
        if (gm->games[i].id == game_id && !gm->games[i].running) {
            gm->games[i].running = true;
            printf("[GamingMode] Launched game %d: '%s'\n", game_id, gm->games[i].name);
            return 0;
        }
    }
    return -1;
}

void gaming_mode_tick(gaming_mode_manager_t* gm) {
    for (int i = 0; i < gm->game_count; ++i) {
        if (gm->games[i].running) {
            printf("[GamingMode] Game %d ('%s') running.\n", gm->games[i].id, gm->games[i].name);
            global_gpu_api.draw_frame(NULL);
            global_gpu_api.present();
        }
    }
}

void gaming_mode_list(gaming_mode_manager_t* gm) {
    printf("[GamingMode] Game list (%d total):\n", gm->game_count);
    for (int i = 0; i < gm->game_count; ++i) {
        printf("  Game %d: '%s' %s\n", gm->games[i].id, gm->games[i].name, gm->games[i].running ? "[RUNNING]" : "");
    }
} 