#ifndef run_game_h
#define run_game_h

#include <stdbool.h>

#include "block.h"
#include "grid.h"
#include "inputs.h"

int run();



/* int updateGame(
    bool *gamecode_states, BlockIds *block_repo, Block *primary_block, long* block_presets, int num_presets,
    GameGrid *game_grid, bool *god_mode, int *move_counter
) { */


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

} GameState;


// Configuration/setup data for game. Not to be confused with GameState,
// which represents current, mutable game state
typedef struct {

    long* block_presets;
    int num_presets;
    GamecodeMap keymaps;

} GameData;


#endif
