#include "inputs.h"
#include "run_game.h"
#include "game_state.h"
#include "application_state.h"
#include "state_runner.h"
#include "states/application_state.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_shape.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



/* Primary program runner */
int run() {

    // Note: Also does all necessary SDL stuff here
    
    printf("Initializing application state...\n");
    ApplicationState *global_state = ApplicationState_init("assets");
    if (global_state == NULL) {
        return -1;
    }

    printf("Initializing game state...\n");
    GameState *game_state = GameState_init(global_state->rend, global_state->menu_font, 1);
    if (game_state == NULL) {
        return -1;
    }

    // State runner uses stack memory, but others use heap
    printf("Initializing state runner...\n");

    StateRunner *state_runner = StateRunner_init(32, 16);

    // Begin with game state
    printf("Pushing game state...\n");
    StateRunner_addState(state_runner, (void*)game_state, runGameFrame, GameState_deconstruct);
    assert(StateRunner_commitBuffer(state_runner) == 0);

    clock_t frame_start;
    double elapsed;
    double raw_fps;
    double actual_fps;

    int frame_counter = 0;

    char buffer[128];  // a general purpose string buffer



    /*** Main Loop ***/
    printf("Starting main loop...\n");
    while (state_runner->head >= 0) {

        frame_counter++;
        frame_start = clock();

        /* Non-game related stuff */
        processHardwareInputs(global_state->hardware_states);

        SDL_SetRenderDrawColor(global_state->rend, 10, 20, 30, 255);
        SDL_RenderClear(global_state->rend);

        /* Run game-state specific code */
        assert(StateRunner_runState(state_runner, (void*)global_state) == 0);
        assert(StateRunner_commitBuffer(state_runner) == 0);


        /* Draw FPS overlay */

        snprintf(buffer, 128, "%.2f FPS", actual_fps);
        SDL_Surface *fps_surf = TTF_RenderText_Solid(global_state->menu_font, buffer, (SDL_Color){.r=255});
        SDL_Texture *fps_texture = SDL_CreateTextureFromSurface(global_state->rend, fps_surf);

        // NOTE: We want to optimize this later to not free every frame
        SDL_FreeSurface(fps_surf);

        int txt_h, txt_w;
        SDL_QueryTexture(fps_texture, NULL, NULL, &txt_w, &txt_h);
        SDL_Rect fps_dest = (SDL_Rect){
                .x=WINDOW_WIDTH - txt_w,
                .y=WINDOW_HEIGHT - txt_h,
                .w=txt_w,
                .h=txt_h
        };
        SDL_RenderCopy(global_state->rend, fps_texture, NULL, &fps_dest);

        SDL_RenderPresent(global_state->rend);

        SDL_DestroyTexture(fps_texture);

        /*********************************************************************
         * Maintenance calculations
         ********************************************************************/

        elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        raw_fps = (1.0 / elapsed);

        while (elapsed < TARGET_SPF) {
            elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        }

        // calculate actual fps
        elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        actual_fps = (1.0 / elapsed);

        if (frame_counter >= TARGET_FPS) {
            frame_counter = 0;
        }

    }

    // NOTE: This also handles SDL_Quit and TTF_Quit and
    // all other such related things. I don't know if this
    // is the best way to do it, but it's how I'm doing it
    // for now.
    ApplicationState_deconstruct(global_state);
    return 0;
}
