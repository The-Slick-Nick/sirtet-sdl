/* grid.h
* 
* Defines the structure of the main GameGrid and logic surrounding
* interactions with blocks that exist within the grid.
*/

#ifndef GRID_H
#define GRID_H

#include "block.h"


// 10 wide, 20 tall is standard tetris
// TODO perhaps this should be in a global config file?
#define GRID_HEIGHT 20
#define GRID_WIDTH 10

#ifndef ARR_2D
#define ARR_2D(row, col, width)  ((row) * (width)) + (col)
#endif


typedef struct {

    // stores identifying numbers (block ids) that have 
    // landed in the grid
    int grid_contents[GRID_HEIGHT * GRID_WIDTH];

} GameGrid;



bool GameGrid_canBlockMove(GameGrid* grid, 

// add a block's cells to the grid
void GameGrid_commitBlock(GameGrid* grid, Block* block);

void GameGrid_clear(GameGrid* grid);  // reset all a grid's contents
void GameGrid_reset(GameGrid* grid, BlockIds* ids);  // reset a grid's contents,
// accounting for block ids

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid* grid, BlockIds* ids);

#endif
