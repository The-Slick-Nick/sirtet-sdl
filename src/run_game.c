#include "inputs.h"
#include "run_game.h"
#include "game_state.h"
#include "application_state.h"
#include "state_runner.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_shape.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>



/* Primary program runner */
int run() {

    // Note: Also does all necessary SDL stuff here
    ApplicationState *global_state = ApplicationState_init();
    if (global_state == NULL) {
        return -1;
    }

    GameState *game_state = GameState_init();
    if (game_state == NULL) {
        return -1;
    }

    // State runner uses stack memory, but others use heap
    void *states[32];
    void *states_buffer[16];

    state_func_t runners[32];
    state_func_t runners_buffer[16];

    deconstruct_func_t deconstructors[32];
    deconstruct_func_t deconstructors_buffer[16];

    StateRunner state_runner = {
        .head = -1,
        .size = 32,

        .buffer_head = 0,
        .buffer_tail = 0,
        .buffer_size = 16,

        .states = states,
        .deconstructors = deconstructors,
        .runners = runners,

        .states_buffer = states_buffer,
        .deconstructors_buffer = deconstructors_buffer,
        .runners_buffer = runners_buffer
    };

    // Begin with game state
    StateRunner_addState(&state_runner, (void*)game_state, runGameFrame, GameState_deconstruct);
    StateRunner_commitBuffer(&state_runner);


    clock_t frame_start;
    double elapsed;
    double raw_fps;
    double actual_fps;


    /*** Main Loop ***/
    while (state_runner.head >= 0) {

        frame_start = clock();

        /* Non-game related stuff */
        processHardwareInputs(global_state->hardware_states);

        /* Run game-state specific code */
        StateRunner_runState(&state_runner, global_state);
        StateRunner_commitBuffer(&state_runner);


        elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        raw_fps = (1.0 / elapsed);

        while (elapsed < TARGET_SPF) {
            elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        }

        // calculate actual fps
        elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        actual_fps = (1.0 / elapsed);

        // printf("    raw_fps %f\n    actual_fps %f\n", raw_fps, actual_fps);
    }

    ApplicationState_deconstruct(global_state);
    SDL_Quit();
    return 0;
}
