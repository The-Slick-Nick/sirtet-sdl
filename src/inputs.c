#include "inputs.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <limits.h>



// hardware_states should be an array of size
// (int)SDL_Scancode.
//
// (int)SDL_Scancode is represented by the index
// in the array
//
// Values of the array are as follows:
// * Negative values represent the number of
//   frames a key has been not pressed
// * Positive values represent the number of
//   frames a key has been pressed for
// * 0 is used to represent an indeterminate state
//
// Assumes SDL has already undergone whatever
// initialization is required for registering
// inputs (SDL_Init, window created, etc.)
int processHardwareInputs(int* hardware_states) {


    // preprocess
    for (int int_scancode = 0; int_scancode < (int)SDL_NUM_SCANCODES; int_scancode++) {

        if ( hardware_states[int_scancode] < 0 && hardware_states[int_scancode] > INT_MIN ) {
            hardware_states[int_scancode]--;
        }
    }


    // process
    SDL_Event event;
    int scancode;
    while ( SDL_PollEvent(&event) > 0) {

        scancode = (int)event.key.keysym.scancode;

        if (event.type == SDL_KEYUP) {
            hardware_states[scancode] = -1;
        }

        if (event.type == SDL_KEYDOWN && hardware_states[scancode] < 0) {
            hardware_states[scancode] = 0;
        }
    }

    // postprocess
    for (int int_scancode = 0; int_scancode < (int)SDL_NUM_SCANCODES; int_scancode++) {

        // Only register keydowns that weren't already pressed to ignore auto-repeat
        if ( hardware_states[int_scancode] >= 0 && hardware_states[int_scancode] < INT_MAX ) {
            hardware_states[int_scancode]++;
        }
    }

    return 0;
}
