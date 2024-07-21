#ifndef INPUTS_H
#define INPUTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

/******************************************************************************
 * Hardware Inputs
******************************************************************************/

/* Note: use SDL_Scancode for a contiguous block of integers
*  representing key codes */

/**
 * @brief - Reset and flag array representing current hardware states
 * @param hardware_states - Integer array whose indices represent SDL_Scancode
 *                          values and whose values represent the number of 
 *                          consective frames the key has been pressed or
 *                          released, based on its sign
 */
int processHardwareInputs(int* hardware_states);

/******************************************************************************
 * Game Inputs
******************************************************************************/
#define MAX_GAMECODE_MAPS 128  // maximum 


// Enum of codes representing game (and menu?) actions
typedef enum {
    GAMECODE_ROTATE = 0,
    GAMECODE_SPEEDUP,
    GAMECODE_MOVE_LEFT,
    GAMECODE_MOVE_RIGHT,
    GAMECODE_MOVE_DOWN,
    GAMECODE_MOVE_UP,
    GAMECODE_QUIT,
    GAMECODE_PAUSE,
    NUM_GAMECODES
} Gamecode;

// Struct representing an individual mapping
// of hardware input to game signal
typedef struct {
    Gamecode virtual_code;
    SDL_Scancode hardware_code;
    int frame_start;
    int frame_end;
    int frame_interval;
} GamecodeMapItem;

// Struct wrapping a list of gamecode mappings
typedef struct {
    int head;
    int size;
    GamecodeMapItem *mappings;
} GamecodeMap;


GamecodeMap* GamecodeMap_init(int max_maps);

// Initialize a GamecodeMap as a direct copy of an existing one
GamecodeMap* GamecodeMap_initCopy(GamecodeMap *blueprint);

void GamecodeMap_reset(GamecodeMap *self);

int GamecodeMap_addMap(
    GamecodeMap *self, Gamecode virtual_code, SDL_Scancode hardware_code,
    int frame_start, int frame_end, int frame_interval
);

int GamecodeMap_deconstruct(GamecodeMap *self);


// TODO: Shouldn't this be prefixed with GamecodeMap?
// int Gamecode_addMap(
//     GamecodeMap *mapping, Gamecode virtual_code, SDL_Scancode hardware_code,
//     int frame_start, int frame_end, int frame_interval
// );


// Identify if a given gamecode is active by parsing an boolean array indexed by gamecodes
bool Gamecode_pressed(bool *gamecode_arr, Gamecode gamecode);

int processGamecodes(bool *gamecode_states, int *hardware_states, GamecodeMap *all_mappings);

/******************************************************************************
 * Menu Inputs
******************************************************************************/
// As another type of "virtual code"

#define MAX_MENUCODE_MAPS 128 // Max number of mappings we can maintain


// Enum of codes representing menu operations
typedef enum {
    MENUCODE_SELECT = 0,
    MENUCODE_MOVE_UP,
    MENUCODE_MOVE_DOWN,
    MENUCODE_MOVE_LEFT,
    MENUCODE_MOVE_RIGHT,
    MENUCODE_INCREMENT_VALUE,
    MENUCODE_DECREMENT_VALUE,
    MENUCODE_EXIT,
    NUM_MENUCODES
} Menucode;

// Struct representing an individual mapping
// of hardware input to game signal
typedef struct {
    Menucode virtual_code;
    SDL_Scancode hardware_code;
    int frame_start;
    int frame_end;
    int frame_interval;
} MenucodeMapItem;


// Struct wrapping a list of menucode mappings
typedef struct {
    int head;
    int size;
    MenucodeMapItem *mappings;
} MenucodeMap;

MenucodeMap* MenucodeMap_init(int max_maps);
int MenucodeMap_deconstruct(MenucodeMap *self);

// Add a mapping for a hardware input to menu input
int Menucode_addMap(
    MenucodeMap *mapping, Menucode virtual_code, SDL_Scancode hardware_code,
    int frame_start, int frame_end, int frame_interval
);

// Identify if a given menucode is active by parsing an boolean array indexed by menucodes
bool Menucode_pressed(bool *menucode_arr, Menucode menucode);

int processMenucodes(bool *menucode_states, int *hardware_states, MenucodeMap *all_mappings);















#endif
