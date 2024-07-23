#include "inputs.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************
 * Hardware Inputs
******************************************************************************/

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


/******************************************************************************
 * Game Inputs
******************************************************************************/

GamecodeMap* GamecodeMap_init(int max_maps) {

    if (max_maps < 0) {
        return NULL;
    }

    GamecodeMap *retval = (GamecodeMap*)malloc(sizeof(GamecodeMap));

    retval->head = 0;
    retval->size = max_maps;
    retval->mappings = (GamecodeMapItem*)malloc(max_maps * sizeof(GamecodeMapItem));

    return retval;

}

// Initialize a GamecodeMap as a direct copy of an existing one
GamecodeMap* GamecodeMap_initCopy(GamecodeMap *blueprint) {

    GamecodeMap *retval = (GamecodeMap*)malloc(sizeof(GamecodeMap));

    size_t n = blueprint->size * sizeof(GamecodeMapItem);

    retval->head = blueprint->head;
    retval->size = blueprint->size;
    retval->mappings = (GamecodeMapItem*)malloc(n);
    memcpy(retval->mappings, blueprint->mappings, n);

    return retval;
}

int GamecodeMap_deconstruct(GamecodeMap *self) {

    free(self->mappings);
    free(self);
    return 0;
}

/**
 * @brief Populate gamecode flag array based on key mappings and customized state of hardware
 * @param gamecode_states   Pointer to flag array for gamecode states. Should be of size (int)NUM_GAMECODE_STATES
 * @param hardware_states   Pointer to integer state array for hardware states. Should be of size (int)SDL_NUM_SCANCODES
 * @param all_mappings      Pointer to struct of GamecodeMap type key mappings.
*/
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

int GamecodeMap_addMap(
    GamecodeMap *self, Gamecode virtual_code, SDL_Scancode hardware_code,
    int frame_start, int frame_end, int frame_interval
) {

    if (self->head >= self->size) {
        return -1;
    }

    *(self->mappings + self->head) = (GamecodeMapItem){
        .virtual_code=virtual_code,
        .hardware_code=hardware_code,
        .frame_start=frame_start,
        .frame_end=frame_end,
        .frame_interval=frame_interval
    };
    self->head += 1;
    return 0;
}

// Clear all mapped keys from a GamecodeMap
void GamecodeMap_reset(GamecodeMap *self) {
    self->head = 0;
}


// Identify if a given gamecode is active by parsing an boolean array indexed by gamecodes
bool Gamecode_pressed(bool *gamecode_arr, Gamecode gamecode) {
    return gamecode_arr[(int)gamecode];
}


/******************************************************************************
 * Menu inputs
******************************************************************************/

MenucodeMap* MenucodeMap_init(int max_maps) {
    MenucodeMap *retval = (MenucodeMap*)malloc(sizeof(MenucodeMap));

    retval->head = 0;
    retval->size=max_maps;
    retval->mappings = (MenucodeMapItem*)malloc(max_maps * sizeof(MenucodeMapItem));
    return retval;
}

int MenucodeMap_deconstruct(MenucodeMap *self) {
    free(self->mappings);
    free(self);
    return 0;
}


/**
 * @brief Populate menucode flag array based on key mappings and customized 
 *                          state of hardware
 * @param menucode_states   Pointer to flag array for gamecode states. Should
 *                          be of size (int)NUM_MENUCODE_STATES
 * @param hardware_states   Pointer to integer state array for hardware states.
 *                          Should be of size (int)SDL_NUM_SCANCODES
 * @param all_mappings      Pointer to struct of menucodeMap type key mappings.
*/
int processMenucodes(
    bool *menucode_states, int *hardware_states, MenucodeMap *all_mappings
) {

    for (int int_menu = 0; int_menu < (int)NUM_MENUCODES; int_menu++) {
        menucode_states[int_menu] = false;
    }

    MenucodeMapItem map_itm;
    for (int map_i = 0; map_i < all_mappings->head; map_i++) {

        map_itm = all_mappings->mappings[map_i];

        int frame_count = hardware_states[(int)map_itm.hardware_code];

        if (
            frame_count >= map_itm.frame_start
            && frame_count <= map_itm.frame_end
            && (
                map_itm.frame_interval == 0
                || (frame_count - map_itm.frame_start) % map_itm.frame_interval == 0
            )
        ) {
            menucode_states[(int)map_itm.virtual_code] = true;
        }
    }

    return 0;
}

// Add a mapping of hardware code to virtual code
int Menucode_addMap(
    MenucodeMap *map_itm, Menucode virtual_code, SDL_Scancode hardware_code,
    int frame_start, int frame_end, int frame_interval
) {

    if (map_itm->head >= map_itm->size) {
        return -1;
    }

    map_itm->mappings[map_itm->head] = (MenucodeMapItem){
        .virtual_code=virtual_code,
        .hardware_code=hardware_code,
        .frame_start=frame_start,
        .frame_end=frame_end,
        .frame_interval=frame_interval
    };

    map_itm->head += 1;

    return 0;
}

// Identify if a given Menucode is pressed or not
bool Menucode_pressed(bool *menucode_arr, Menucode menucode) {
    return menucode_arr[(int)menucode];
}


// Set up a MenucodeMap with common standard controls (arrow keys, enter, etc.)
void MenucodePreset_standard(MenucodeMap *map) {

    Menucode_addMap(map, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_SELECT, SDL_SCANCODE_RETURN, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_SELECT, SDL_SCANCODE_RETURN2, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_SELECT, SDL_SCANCODE_KP_ENTER, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_INCREMENT_VALUE, SDL_SCANCODE_RIGHT, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_DECREMENT_VALUE, SDL_SCANCODE_LEFT, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_MOVE_UP, SDL_SCANCODE_UP, 1, 1, 1);
    Menucode_addMap(map, MENUCODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, 1, 1);
    

}
