/* grid.h
* 
* Defines the structure of the main GameGrid and logic surrounding
* interactions with blocks that exist within the grid.
*/

#ifndef GRID_H
#define GRID_H

#include "block.h"
#include <stdbool.h>


// 10 wide, 20 tall is standard tetris
// TODO perhaps this should be in a global config file?
#define GRID_HEIGHT 20
#define GRID_WIDTH 10


typedef struct {

    int width;
    int height;

    // stores identifying numbers (block ids) that have 
    int *contents;

} GameGrid;


// Convert a block's content bit to grid coordaintes
Point blockContentBitToGridCoords(int content_bit, int block_size, Point block_position);

// Identify if the given block is compatible with current grid
bool GameGrid_canBlockExist(GameGrid *self, Block *block);

// Identify if the provided block data is compatible with current grid
bool GameGrid_canBlockInfoExist(
    GameGrid *self, int block_size, long block_contents, Point block_position
);

// add a block's cells to the grid. Modifies provided grid and block in place
int GameGrid_commitBlock(GameGrid* self, Block* block);


// TODO For the below two in-place methods, change return type
// to an integer status code (0 = success, -1 = fail)

// Reset all of a grid's contents
void GameGrid_clear(GameGrid* grid);  

// Reset a grid's contents, clearing encountered blocks
void GameGrid_reset(GameGrid* grid, BlockIds* ids);  

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid* grid, BlockIds* ids);

#endif
