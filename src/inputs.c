#include "inputs.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>



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

// Populate gamecode flag array based on key mappings and customized state of hardware
int processGamecodes(bool *gamecode_states, int *hardware_states, GamecodeMap *all_mappings) {


    // preprocess
    for (int int_gamecode = 0; int_gamecode < (int)NUM_GAMECODES; int_gamecode++) {
        gamecode_states[int_gamecode] = false;
    }

    GamecodeMapItem mapping;
    for (int map_i = 0; map_i < all_mappings->head; map_i++) {

        mapping = all_mappings->mappings[map_i];

        int frame_count = hardware_states[(int)mapping.hardware_code];

        if (
            frame_count >= mapping.frame_start
            && frame_count <= mapping.frame_end
            && (
                mapping.frame_interval == 0
                || (frame_count - mapping.frame_start) % mapping.frame_interval == 0
            )
        ) {
            gamecode_states[(int)mapping.virtual_code] = true;
        }
    }

    return 0;

}


// Add a mapping of hardware code to virtual code
//
// Acts as a blueprint to flag the virtual_code as pressed if the number of
// frames that a hardware code has been pressed for is from frame_start
// to frame_end, both endpoints inclusive.
// int addKeymap(
//     GamecodeMap *mapping, Gamecode virtual_code, SDL_Scancode hardware_code,
//     int frame_start, int frame_end
// ) {
//
//     // TODO: Return -1 instead?
//     assert(mapping->head < MAX_GAMECODE_MAPS);
//
//     *(mapping->mappings + mapping->head) = (GamecodeMapItem){
//         .virtual_code=virtual_code,
//         .hardware_code=hardware_code,
//         .frame_start=frame_start,
//         .frame_end=frame_end
//     };
//
//     mapping->head += 1;
//
//     return 0;
// }


// Add a mapping of hardware code to virtual code
//
// Acts as a blueprint to flag the virtual_code as pressed if the number of
// frames that a hardware code has been pressed for is from frame_start
// to frame_end, both endpoints inclusive.
int Gamecode_addMap(
    GamecodeMap *mapping, Gamecode virtual_code, SDL_Scancode hardware_code,
    int frame_start, int frame_end, int frame_interval
) {

    // TODO: Return -1 instead?
    assert(mapping->head < MAX_GAMECODE_MAPS);

    *(mapping->mappings + mapping->head) = (GamecodeMapItem){
        .virtual_code=virtual_code,
        .hardware_code=hardware_code,
        .frame_start=frame_start,
        .frame_end=frame_end,
        .frame_interval=frame_interval

    };

    mapping->head += 1;

    return 0;
}


// Identify if a given gamecode is active by parsing an boolean array indexed by gamecodes
bool Gamecode_pressed(bool *gamecode_arr, Gamecode gamecode) {
    return gamecode_arr[(int)gamecode];
}
