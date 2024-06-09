#ifndef BLOCK_H
#define BLOCK_H


#include "coordinates.h"

/* For a standard 
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/

typedef struct Block {
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

long rotateBlockContentsCw90(long contents, int blockSize);
long rotateBlockContentsCcw90(long contents, int blockSize);
long rotateBlockContents180(long contents, int blockSize);

#endif
