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
#include <SDL2/SDL_ttf.h>

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
    int score;                  // Number of points accumulated
    int level;                  // Current game level. Affects speed & score

    BlockDb *block_db;
    GameGrid *game_grid;        // Grid struct storing committed blocks


    int primary_block;          // id of main block dropping from top to bottom
    SDL_Rect draw_window;       // Region/coordinates of screen to draw grid on

    long *block_presets;        // Array of block content masks to draw from
    int num_presets;            // Number of block content presets in *block_presets
    GamecodeMap *keymaps;       // collection of hardware -> gamecode key mappings


    /* State/structs for alternate versions of state */
    SDL_Texture *pause_texture; // Texture with "pause" text

} GameState;


// Initialize and return a pointer for GameState
GameState* GameState_init(ApplicationState *app_state);

// Deconstruct a GameState by pointer reference
int GameState_deconstruct(void* self);

void GameState_debugPrint(GameState *self);

// Run the update portion of the main game loop
// int updateGame(GameState *game_state);
// I don't think this is necessary outside game_state.c, so I think I'll remove it

// Run one frame of game
StateFuncStatus GameState_run(StateRunner *state_runner, void *application_data, void *state_data);
StateFuncStatus GameState_runPaused(StateRunner *state_runner, void *application_data, void *state_data);
StateFuncStatus GameState_runGridAnimation(StateRunner *state_runner, void *app_data, void *state_data);

#endif
