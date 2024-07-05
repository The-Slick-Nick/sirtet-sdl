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
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>



/* Primary program runner */
int run() {

    // Note: Also does all necessary SDL stuff here
    
    printf("Initializing application state...\n");
    ApplicationState *global_state = ApplicationState_init();
    if (global_state == NULL) {
        return -1;
    }

    printf("Initializing game state...\n");
    GameState *game_state = GameState_init(1);
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


    /*** Main Loop ***/
    printf("Starting main loop...\n");
    while (state_runner->head >= 0) {

        frame_counter++;
        frame_start = clock();

        /* Non-game related stuff */
        processHardwareInputs(global_state->hardware_states);

        /* Run game-state specific code */
        assert(StateRunner_runState(state_runner, (void*)global_state) == 0);
        assert(StateRunner_commitBuffer(state_runner) == 0);

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
            printf("    actual fps %f\n", actual_fps);
        }

    }

    ApplicationState_deconstruct(global_state);
    SDL_Quit();
    return 0;
}
