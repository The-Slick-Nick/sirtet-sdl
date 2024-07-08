/* grid.h
* 
* Defines the structure of the main GameGrid and logic surrounding
* interactions with blocks that exist within the grid.
*/

#ifndef GRID_H
#define GRID_H

#include "block.h"
#include <stdbool.h>



typedef struct {

    int width;
    int height;

    // stores identifying numbers (block ids) that have 
    // Negative numbers (usually -1) indicate an invalid/unprovisioned
    // block ID (and thus that "cell" is empty)
    int *contents;

} GameGrid;


// Convert a block's content bit to grid coordinates
Point blockContentBitToGridCoords(int content_bit, int block_size, Point block_position);

// Identify if the given block is compatible with current grid
bool GameGrid_canBlockExist(GameGrid *self, BlockDb *db, int block_id);

// Identify if the provided block data is compatible with current grid
bool GameGrid_canBlockInfoExist(
    GameGrid *self, int block_size, long block_contents, Point block_position
);

// add a block's cells to the grid. Modifies provided grid and block in place
int GameGrid_commitBlock(GameGrid *self, BlockDb *db, int block_id);

// TODO For the below two in-place methods, change return type
// to an integer status code (0 = success, -1 = fail)

// Reset all of a grid's contents
int GameGrid_clear(GameGrid* grid);  

// Reset a grid's contents, clearing encountered blocks
int GameGrid_reset(GameGrid* grid, BlockDb *db);  

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid* grid, BlockDb *db);

// Calculate how many points to award based on the current grid state
int GameGrid_assessScore(GameGrid *self, int level);

#endif
