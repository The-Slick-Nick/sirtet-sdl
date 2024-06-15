/* grid.h
* 
* Defines the structure and some logic for handling a grid 
* in which blocks will exist, live, and die.
*/

#ifndef GRID_H
#define GRID_H

#include "block.h"


// 10 wide, 20 tall is standard tetris
#define GRID_HEIGHT 20
#define GRID_WIDTH 10


typedef struct {

    // stores identifying numbers (block ids) that have 
    // landed in the grid
    int grid_contents[GRID_HEIGHT * GRID_WIDTH];

} GameGrid;

void GameGrid_clear(GameGrid* grid);  // reset all a grid's contents
void GameGrid_reset(GameGrid* grid, BlockIds* ids);  // reset a grid's contents,
// accounting for block ids

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid* grid, BlockIds* ids);

#endif
