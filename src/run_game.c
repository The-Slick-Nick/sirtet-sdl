#include "inputs.h"
#include "run_game.h"
#include "game_state.h"
#include "application_state.h"

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


#define TARGET_FPS 60



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


    /*** Main Loop ***/
    while (true) {

        /* Non-game related stuff */
        processHardwareInputs(global_state->hardware_states);

        /* Run game-state specific code */
        if (runGameFrame((void*)global_state, (void*)game_state)) {
            break;
        }
    }

    GameState_deconstruct((void*)game_state);
    ApplicationState_deconstruct(global_state);
    SDL_Quit();
    return 0;
}
