#include <stdbool.h>
#include "grid.h"
#include "block.h"



// Identify if the given block
bool GameGrid_canBlockExist(GameGrid *self, Block *block) {
    return false;
}

// Identify if the provided block data is compatible with current grid
bool GameGrid_canBlockInfoExist(
    GameGrid *self, int block_size, long block_contents, Point block_position
) {
    return false;
}

// add a block's cells to the grid. Modifies provided grid and block in place
int GameGrid_commitBlock(GameGrid* self, Block* block) {
}

// Reset all of a grid's contents
void GameGrid_clear(GameGrid* grid) {
    for (int idx = 0; idx < grid->width * grid->height; idx++) {
         grid->contents[idx] = -1;
    };
}  

// Reset a grid's contents, clearing encountered blocks
void GameGrid_reset(GameGrid* grid, BlockIds* ids) {
}  

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid* grid, BlockIds* ids) {
    return 0;
}
