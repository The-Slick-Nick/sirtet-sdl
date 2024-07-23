/* grid.h
* 
* Defines the structure of the main GameGrid and logic surrounding
* interactions with blocks that exist within the grid.
*/

#ifndef GRID_H
#define GRID_H

#include "block.h"
#include <stdbool.h>

#define DEFAULT_GRID_FRAMERATE 15  // One removal every X frames


/******************************************************************************
 * Strict declaration, initiation, deconstruction
******************************************************************************/

typedef struct {

    int width;
    int height;

    // stores identifying numbers (block ids) that have 
    // Negative numbers (usually -1) indicate an invalid
    // block ID (and thus that "cell" is empty)
    int *contents;

    /* Visual elements/representations */
    int cooldown;       // Number of frames until the next removal
    int framerate;      // Number of frames between removals
    bool is_animating;  // Flag to indicate if animation is in progress
    int *to_remove;     // Array (corresponding to row indices) of total number
                        // of cells needing removed
    int *removed;       // Array (corresponding to row indices) of number of cells removed

} GameGrid;


GameGrid *GameGrid_init(int width, int height);
int GameGrid_deconstruct(GameGrid *self);

/******************************************************************************
 * Content management
******************************************************************************/


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

// Reset all of a grid's contents
int GameGrid_clear(GameGrid* grid);  

// Reset a grid's contents, clearing encountered blocks
int GameGrid_reset(GameGrid* grid, BlockDb *db);  

// clears full rows of committed blocks, pushing remaining blocks downward
// Return number of lines cleared
int GameGrid_resolveRowsDown(GameGrid* grid, BlockDb *db);

// clears full rows of committed blocks, pushing remaining blocks upward.
// Return number of lines cleared
int GameGrid_resolveRowsUp(GameGrid* grid, BlockDb *db);

// Calculate how many points to award based on the current grid state
int GameGrid_assessScore(GameGrid *self, int level);

// Return the distance a block can drop from its current position, or -1 if 
// current position is invalid.
int GameGrid_getDropDistance(
    GameGrid *self, int block_size, long contents, Point position
);

/******************************************************************************
 * Display/animation management
******************************************************************************/

// Scan the grid and prepare contents for removal animation
int GameGrid_prepareAnimation(GameGrid *self, int framerate);

// Set up each grid row for animation
int GameGrid_prepareAnimationAllRows(GameGrid *self, int framerate);

// Run one frame of the GameGrid animation
int GameGrid_runAnimationFrame(GameGrid *self);

#endif
