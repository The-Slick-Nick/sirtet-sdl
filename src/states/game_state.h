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


#define GRID_WIDTH 10
#define GRID_HEIGHT 24

// Structure representing current state of game.
//
// This struct wraps information that the game cares about
typedef struct {

    int state_num;
    bool *gamecode_states;      // boolean flag array for gamecodes (indexed by Gamecode)

    bool god_mode;              // boolean flag for ultra-easy mode
    int move_counter;           // number of frames since last movement

    BlockDb block_db;
    GameGrid game_grid;         // Grid struct storing committed blocks



    int primary_block;          // id of main block dropping from top to bottom
    SDL_Rect draw_window;       // Region/coordinates of screen to draw grid on

    long *block_presets;        // Array of block content masks to draw from
    int num_presets;            // Number of block content presets in *block_presets
    GamecodeMap keymaps;        // collection of hardware -> gamecode key mappings

} GameState;


// Initialize and return a pointer for GameState
GameState* GameState_init(int state_num);

// Deconstruct a GameState by pointer reference
int GameState_deconstruct(void* self);

void GameState_debugPrint(GameState *self);

// Run the update portion of the main game loop
int updateGame(GameState *game_state);

// Run one frame of game
int runGameFrame(StateRunner *state_runner, void *application_data, void *state_data);


int runGameFramePaused(StateRunner *state_runner, void *application_data, void *state_data);


#endif
