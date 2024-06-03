#ifndef BLOCK_H
#define BLOCK_H


#include "coordinates.h"

/*
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/

typedef struct Block {

    int structure; // Mask representing positions within a 4x4 grid using 16 bits
} Block;


Block Block_rotateCw90(Block block);
Block Block_rotateCcw90(Block block);
Block Block_rotate180(Block block);

#endif
