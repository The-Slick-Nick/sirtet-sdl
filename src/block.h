#ifndef BLOCK_H
#define BLOCK_H


#include "coordinates.h"



// maximum number of blocks that may exist
// TODO Perhaps this is something to handle in a global config instead?
#define MAX_BLOCK_COUNT 128 

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

// transformation on the integer representation of a block's contents
long transformBlockContents(long contents, int blockSize, Point transform);

// in-place transformation of a block
void Block_transform(Block* block, Point transform); 

// in-place translation of a block
void Block_translate(Block* self, Point translation);

long rotateBlockContentsCw90(long contents, int blockSize);
long rotateBlockContentsCcw90(long contents, int blockSize);
long rotateBlockContents180(long contents, int blockSize);


int BlockIds_provisionId(BlockIds *self, int block_size);
int BlockIds_removeId(BlockIds *ids, int to_remove);



// Convert a bit number representing a position with a block's contents to its 
// relative coordinates to the center
// i.e. for bitNum = 3 and blockSize = 4, the corrseponding coordinates would
// be (2, 2) (see diagram above)
Point contentBitToPoint(int bitNum, int blockSize);

int pointToContentBit(Point point, int blockSize); 

#endif

