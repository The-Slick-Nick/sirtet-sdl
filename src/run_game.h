#ifndef run_game_h
#define run_game_h

#include <stdbool.h>

#include "block.h"
#include "grid.h"
#include "inputs.h"

int run();



// Structure representing current state of game.
//
// This struct wraps information that the game cares about
typedef struct {

    bool *gamecode_states;

    bool god_mode;
    int move_counter;

    BlockIds block_ids;
    GameGrid game_grid;
    Block primary_block;

    long* block_presets;
    int num_presets;
    GamecodeMap keymaps;

} GameState;

int updateGame(GameState *game_state);

#endif
