#ifndef INPUTS_H
#define INPUTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <stdbool.h>


#define MAX_GAMECODE_MAPS 128  // maximum 

/* Note: use SDL_Scancode for a contiguous block of integers
*  representing key codes */

// Enum of codes representing game (and menu?) actions
typedef enum {
    GAMECODE_ROTATE = 0,
    NUM_GAMECODES
} Gamecode;

// Struct representing an individual mapping
// of hardware input to game signal
typedef struct {
    Gamecode virtual_code;
    SDL_Scancode hardware_code;
    int frame_start;
    int frame_end;

} GamecodeMapItem;


// Struct wrapping a list of gamecode mappings
typedef struct {
    int head;
    GamecodeMapItem mappings[MAX_GAMECODE_MAPS];
} GamecodeMap;


int addKeymap(GamecodeMap *mapping, Gamecode virtual_code, SDL_Scancode hardware_code, int frame_start, int frame_end);

int processHardwareInputs(int* hardware_states);
int processGamecodes(bool *gamecode_states, int *hardware_states, GamecodeMap *all_mappings);



#endif
