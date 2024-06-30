/* game_state.h
*
* Defines struct and methods for handling state of
* the game.
*
* State follows a (as of yet to be actually written) standard
* for a state-runner to run
*/

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <SDL2/SDL.h>

#include "block.h"
#include "grid.h"
#include "inputs.h"

#include "state_runner.h"
#include "application_state.h"


#define GRID_WIDTH 10
#define GRID_HEIGHT 24

// Structure representing current state of game.
//
// This struct wraps information that the game cares about
typedef struct {

    bool *gamecode_states;      // boolean flag array for gamecodes (indexed by Gamecode)

    bool god_mode;              // boolean flag for ultra-easy mode
    int move_counter;           // number of frames since last movement

    BlockIds block_ids;         // Block ID repository to handle creating unique ids for blocks
    GameGrid game_grid;         // Grid struct storing committed blocks
    Block primary_block;        // Main block being dropped
    SDL_Rect draw_window;       // Region/coordinates of screen to draw grid on

    long *block_presets;        // Array of block content masks to draw from
    int num_presets;            // Number of block content presets in *block_presets
    GamecodeMap keymaps;        // collection of hardware -> gamecode key mappings

} GameState;


// Initialize and return a pointer for GameState
void* GameState_init();

// Deconstruct a GameState by pointer reference
int GameState_deconstruct(void* self);

// Run the update portion of the main game loop
int updateGame(GameState *game_state);

// Run one frame of game
// int runGameFrame(void *global_state_data, void *state_data);
int runGameFrame(StateRunner *state_runner, ApplicationState *application_state, void *state_data);

#endif
