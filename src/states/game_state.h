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


#define GRID_WIDTH 10
#define GRID_HEIGHT 24


// Structure representing current state of game.
//
// This struct wraps information that the game cares about
typedef struct {

    GamecodeMap *keymaps;       // collection of hardware -> gamecode key mappings
    bool *gamecode_states;      // boolean flag array for gamecodes (indexed by Gamecode)

    int move_counter;           // number of frames since last movement
    int score;                  // Number of points accumulated
    int level;                  // Current game level. Affects speed & score
    int block_size;             // Block sizing standard for this gamestate

    BlockDb *block_db;
    GameGrid *game_grid;        // Grid struct storing committed blocks


    int primary_block;          // id of main block dropping from top to bottom
    int queued_block;           // Next block queued up

    long *block_presets;        // Array of block content masks to draw from
    int num_presets;            // Number of block content presets in *block_presets
    
    SDL_Color *palette;         // Array of SDL colors to use
    int palette_size;           // Number of colors in palette


    /* State/structs for display */
    SDL_Texture *pause_texture; // Texture with "pause" text

    SDL_Texture *score_label;
    SDL_Texture *level_label;

} GameState;


// Initialize and return a pointer for GameState
// GameState* GameState_init(ApplicationState *app_state);

GameState* GameState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, GamecodeMap *keymaps,
    int init_level,
    int block_size,
    int preset_size, long *block_presets,
    int palette_size, SDL_Color *palette
);
// Deconstruct a GameState by pointer reference
int GameState_deconstruct(void* self);


// Run one frame of game
StateFuncStatus GameState_run(StateRunner *state_runner, void *application_data, void *state_data);
StateFuncStatus GameState_runPaused(StateRunner *state_runner, void *application_data, void *state_data);
StateFuncStatus GameState_runGridAnimation(StateRunner *state_runner, void *app_data, void *state_data);
StateFuncStatus GameState_runGameOver(StateRunner *state_runner, void *app_data, void *state_data);

#endif
