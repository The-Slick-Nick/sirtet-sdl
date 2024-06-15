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
    int block_id;   // Uniquely identifying number of this block

    Point position; // Coordinates representing the center point.
                    // If blockSize is odd, this is the center of a square.
                    // If blockSize is even, this is at a corner of four
                    // squares at the center

    long contents;  // Mask representing positions of individual block "cells"
                    // within a grid using bits - supports up to 8-tiled blocks.

    int blockSize;  // Dimension of block-placement grid.
                    // Does not necessarily indicate the number of squares
                    // used for this block, but the maximum supported
} Block;

// Struct to manage & track how block ids are assigned
typedef struct {
    int id_array[MAX_BLOCK_COUNT];
    int head;

} BlockIds;

long rotateBlockContentsCw90(long contents, int blockSize);
long rotateBlockContentsCcw90(long contents, int blockSize);
long rotateBlockContents180(long contents, int blockSize);


int provisionBlockId(BlockIds *ids, int blockSize);
int removeBlockId(BlockIds *ids, int toRemove);  // remove the contents at a particular id

// Convert a bit number representing a position with a block's contents to its 
// relative coordinates to the center
// i.e. for bitNum = 3 and blockSize = 4, the corrseponding coordinates would
// be (2, 2) (see diagram above)
Point contentBitToPoint(int bitNum, int blockSize);

int pointToContentBit(Point point, int blockSize); 

#endif

