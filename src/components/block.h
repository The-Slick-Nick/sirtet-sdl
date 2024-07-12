#ifndef BLOCK_H
#define BLOCK_H

#include <SDL2/SDL.h>

#include <stdbool.h>
#include "coordinates.h"


#define INVALID_BLOCK_ID -1

/* For a standard size 4 block
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/


/******************************************************************************
 * Struct definition, initialization, deconstruction
******************************************************************************/

// Struct to handle/store block information
// Behaves as an ECS, where a caller/user
// is given an integer block id that can be
// used to query the BlockDb for information
// about the block tied to that ID
typedef struct {
    int max_ids;
    int head;

    int *ids;
    int *sizes;
    long *contents;
    Point *positions;
    SDL_Color *colors;
} BlockDb;

// Initialize a BlockDb struct with the given size, returning
// a pointer to it
BlockDb* BlockDb_init(int size);

// Deconstruct a BlockDb and free its allocated memory
int BlockDb_deconstruct(BlockDb *self);

/******************************************************************************
******************************************************************************/

// transformation on the integer representation of a block's contents
long transformBlockContents(long contents, int blockSize, Point transform);


// count the number of active cells in a contents mask
int getCellCount(long contents, int block_size);


int BlockDb_transformBlock(BlockDb *self, int block_id, Point transform);
int BlockDb_translateBlock(BlockDb *self, int block_id, Point translate);
bool BlockDb_isContentBitSet(BlockDb *self, int block_id, int content_bit);

// Provision and create a new block, returning its id
int BlockDb_createBlock(
    BlockDb *self, int size, long contents, Point position, SDL_Color color);
bool BlockDb_doesBlockExist(BlockDb *self, int block_id);

// Getters & setters
int BlockDb_getBlockSize(BlockDb *self, int block_id);
int BlockDb_setBlockSize(BlockDb *self, int block_id, int size);  
// NOTE: Should this be allowable? Setting a block's size post-creation seems like a bad ides

long BlockDb_getBlockContents(BlockDb *self, int block_id);
int BlockDb_setBlockContents(BlockDb *self, int block_id, long contents);

Point BlockDb_getBlockPosition(BlockDb *self, int block_id);
int BlockDb_setBlockPosition(BlockDb *self, int block_id, Point position);

SDL_Color BlockDb_getBlockColor(BlockDb *self, int block_id);
int BlockDb_setBlockColor(BlockDb *self, int block_id, SDL_Color color);


// Block cell count management

int BlockDb_getCellCount(BlockDb *self, int block_id);
int BlockDb_decrementCellCount(BlockDb *self, int block_id, int by);
int BlockDb_incrementCellCount(BlockDb *self, int block_id, int by);
int BlockDb_removeBlock(BlockDb *self, int block_id);



// Convert a bit number representing a position with a block's contents to its 
// relative coordinates to the center
// i.e. for bitNum = 3 and blockSize = 4, the corrseponding coordinates would
// be (2, 2) (see diagram above)
Point contentBitToPoint(int bitNum, int blockSize);

// Convert an x,y coordinate to bit num in a bit mask representing
// a block's contents based on a given block size
int pointToContentBit(Point point, int blockSize); 

long rotateBlockContentsCw90(long contents, int blockSize);
long rotateBlockContentsCcw90(long contents, int blockSize);
long rotateBlockContents180(long contents, int blockSize);


#endif

