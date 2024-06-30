#ifndef run_game_h
#define run_game_h

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

#include "block.h"
#include "grid.h"
#include "inputs.h"

int run();



// Higher-level status/state of hardware and such
// to pass to lower-level virtual states
typedef struct {
    int *hardware_states;  // Array, indexed by SDL_Scancode, indicating # of frames a hardware input has been pressed (or released)
    SDL_Renderer *rend;     // Pointer to renderer struct in use by the application
    SDL_Window *wind;
} ApplicationState;

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

int updateGame(GameState *game_state);

#endif
