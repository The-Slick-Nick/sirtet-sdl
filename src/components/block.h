#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include <stdlib.h>
#include "coordinates.h"


// maximum number of blocks that may exist
// TODO Perhaps this is something to handle in a global config instead?
#define MAX_BLOCK_COUNT 128 
#define INVALID_BLOCK_ID -1

/* For a standard size 4 block
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/
typedef struct Block {
    int id; // Uniquely identifying number of this block

    Point position; // Coordinates representing the center point.
                    // If blockSize is odd, this is the center of a square.
                    // If blockSize is even, this is at a corner of four
                    // squares at the center

    long contents;  // Mask representing positions of individual block "cells"
                    // within a grid using bits - supports up to 8-tiled blocks.

    int size;       // Dimension of block-placement grid.
                    // Does not necessarily indicate the number of squares
                    // used for this block, but the maximum supported
} Block;

// Struct to manage & track how block ids are assigned
// TODO Don't hard-code a macroed size, value, have the struct contain the array & a size
typedef struct {
    int max_ids;   // Maximum number of ids that can be provisioned
    int *id_array; // Array counting number of cells per id (by index)
    int head;      // Pointer to current highest id provisioned so far
} BlockIds;


// Struct to handle/store block information
typedef struct {
    int max_ids;
    int head;

    int *ids;
    int *sizes;
    long *contents;
    Point *positions;
} BlockDb;

// transformation on the integer representation of a block's contents
long transformBlockContents(long contents, int blockSize, Point transform);


// count the number of active cells in a contents mask
int getCellCount(long contents, int block_size);


int BlockDb_transformBlock(BlockDb *self, int block_id, Point transform);
int BlockDb_translateBlock(BlockDb *self, int block_id, Point translate);
bool BlockDb_isContentBitSet(BlockDb *self, int block_id, int content_bit);

// Provision and create a new block, returning its id
int BlockDb_createBlock(BlockDb *self, int size, long contents, Point position);
bool BlockDb_doesBlockExist(BlockDb *self, int block_id);

// Getters & setters
int BlockDb_getBlockSize(BlockDb *self, int block_id);
int BlockDb_setBlockSize(BlockDb *self, int block_id, int size);  
// NOTE: Should this be allowable? Setting a block's size post-creation seems like a bad ides

long BlockDb_getBlockContents(BlockDb *self, int block_id);
int BlockDb_setBlockContents(BlockDb *self, int block_id, long contents);

Point BlockDb_getBlockPosition(BlockDb *self, int block_id);
int BlockDb_setBlockPosition(BlockDb *self, int block_id, Point position);


// Block cell count management

int BlockDb_getCellCount(BlockDb *self, int block_id);
int BlockDb_decrementCellCount(BlockDb *self, int block_id, int by);
int BlockDb_incrementCellCount(BlockDb *self, int block_id, int by);
int BlockDb_removeBlock(BlockDb *self, int block_id);


// in-place transformation of a block
void Block_transform(Block* block, Point transform); 

// in-place translation of a block
void Block_translate(Block* self, Point translation);

// identify if given content bit is set (shorthand for bit magic)
bool Block_isContentBitSet(Block* self, int content_bit);

long rotateBlockContentsCw90(long contents, int blockSize);
long rotateBlockContentsCcw90(long contents, int blockSize);
long rotateBlockContents180(long contents, int blockSize);


int BlockIds_provisionId(BlockIds *self, int block_size);
int BlockIds_removeId(BlockIds *ids, int to_remove);
int BlockIds_decrementId(BlockIds* ids, int to_decrement, int by);
int BlockIds_incrementId(BlockIds* ids, int to_decrement, int by);



// Convert a bit number representing a position with a block's contents to its 
// relative coordinates to the center
// i.e. for bitNum = 3 and blockSize = 4, the corrseponding coordinates would
// be (2, 2) (see diagram above)
Point contentBitToPoint(int bitNum, int blockSize);

int pointToContentBit(Point point, int blockSize); 

#endif

