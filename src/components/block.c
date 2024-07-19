#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "block.h"
#include "coordinates.h"



/* Block "contents" are represented as a bit mask,
 * with each bit number referring to a particular
 * block "cell" within an individual grid (based
 * on block size), where the bit being set indicates
 * a block cell present, and the bit not being set
 * indicates a block cell being abesnt.
 *
 * Consider the block's internal grid for a block size of 4
 *
 * [00] [01] [02] [03]
 * [04] [05] [06] [07]
 * [08] [09] [10] [11]
 * [12] [13] [14] [15]
 *
 * A square block would have the representation
 * 0b0000011001100000, with bits 5, 6, 9, and 10 being
 * set
*/


/* For rotations, block cells as coordinates in the complex plane
*
* (-i + 1) | ( i + 1)
* ---------------------
* (-i - 1) | ( i - 1)
*
* To rotate 90deg clockwise, multiply by i
* To rotate 90deg counterclockwise, multiply by -i
* To rotate 180deg, multiply by -1
*/


/******************************************************************************
 * Struct initialization, deconstruction
******************************************************************************/
BlockDb* BlockDb_init(int size) {

    BlockDb *retval = (BlockDb*)malloc(sizeof(BlockDb));

    *retval = (BlockDb){
        .max_ids=size,
        .head=0,
        .ids=(int*)calloc(size, sizeof(int)),
        .sizes=(int*)malloc(size * sizeof(int)),
        .contents=(long*)malloc(size * sizeof(long)),
        .positions=(Point*)malloc(size * sizeof(Point)),
        .colors=(SDL_Color*)malloc(size * sizeof(SDL_Color))
    };
    return retval;
}

int BlockDb_deconstruct(BlockDb *self) {

    free(self->ids);
    free(self->sizes);
    free(self->contents);
    free(self->positions);
    free(self->colors);
    free(self);
    return 0;
}



/******************************************************************************
******************************************************************************/

// Convert a bit number representing a position with a block's contents to its 
// relative coordinates to the center
Point contentBitToPoint(int bitNum, int blockSize) {

    int halfSize = blockSize / 2;
    int rowNum = bitNum / blockSize;
    int colNum = bitNum % blockSize;

    // For even block sizes, the center is on a corner, so we adjust here
    if ((blockSize & 1) == 0) {

        if (rowNum >= halfSize) {
            rowNum++;
        }

        if (colNum >= halfSize) {
            colNum++;
        }
    }

    return (Point) {
        .x = colNum - halfSize,
        .y = halfSize - rowNum
    };

}

// Convert a set of center-relative coordinates to its bit
// number representation 
int pointToContentBit(Point point, int blockSize) {
    
    int halfSize = blockSize / 2;

    int col = point.x + halfSize;
    int row = halfSize - point.y;

    // For even block sizes, the center is on a corner, so we adjust here
    if ((blockSize & 1) == 0) {
        if (point.x > 0) {
            col--;
        }

        if (point.y < 0) {
            row--;
        }
    }

    return blockSize * row + col;
}


long transformBlockContents(long contents, int blockSize, Point transform) {
    long newContents = 0;
    Point originalPoint;
    Point newPoint;
    int newBit;

    for (int bitNum = 0; bitNum < blockSize * blockSize; bitNum++) {

        if (((1L << bitNum) & contents) == 0) {
            continue;
        }

        originalPoint = contentBitToPoint(bitNum, blockSize);
        newPoint = Point_transform(originalPoint, transform);
        newBit = pointToContentBit(newPoint, blockSize);
        newContents |= (1L << newBit);
    }

    return newContents;
}



long rotateBlockContentsCw90(long contents, int blockSize) {
    return transformBlockContents(contents, blockSize, (Point){0, -1});
}

long rotateBlockContentsCcw90(long contents, int blockSize) {
    return transformBlockContents(contents, blockSize, (Point){0, 1});
}

long rotateBlockContents180(long contents, int blockSize) {
    return transformBlockContents(contents, blockSize, (Point){-1, 0});
}

// Count the number of active cells in a contents mask
int getCellCount(long contents, int block_size) {

    int count = 0;
    for (int bit_num = 0; bit_num < block_size * block_size; bit_num++) {
        if ((contents & 1L) == 1) {
            count++;
        }

        contents >>= 1;
    }
    return count;
}

/* ===========================================================================
 * BlockDb methods
 =========================================================================== */


// Provision and create a new block, returning its id
int BlockDb_createBlock(
    BlockDb *self, int size, long contents, Point position, SDL_Color color
) {

    int return_id = INVALID_BLOCK_ID;

    if (contents == 0L) {
        return return_id;
    }

    for (int attempt = 1; attempt <= self->max_ids; attempt++) {

        if (self->ids[self->head] == 0) {
            return_id = self->head;
            self->ids[return_id] = getCellCount(contents, size);
            self->sizes[return_id] = size;
            self->contents[return_id] = contents;
            self->positions[return_id] = position;
            self->colors[return_id] = color;
            return return_id;
        }

        self->head = (self->head + 1) % self->max_ids;
    }

    return return_id;
}


// Transform a block's contents in place
int BlockDb_transformBlock(BlockDb *self, int block_id, Point transform) {

    if (block_id <= INVALID_BLOCK_ID) {
        return -1;
    }

    if (self->ids[block_id] <= 0) {
        return -1;
    }

    self->contents[block_id] = transformBlockContents(self->contents[block_id], self->sizes[block_id], transform);
    return 0;
}

// Translate a block's position in place
int BlockDb_translateBlock(BlockDb *self, int block_id, Point translate) {

    if (!BlockDb_doesBlockExist(self, block_id)) {
        return -1;
    }

    Point new_pos = Point_translate(self->positions[block_id], translate);
    self->positions[block_id] = new_pos;
    return 0;
}

// Determine if a block's contents has a particular bit set
bool BlockDb_isContentBitSet(BlockDb *self, int block_id, int content_bit) {
    if (!BlockDb_doesBlockExist(self, block_id)) {
        return false;
    }
    return (self->contents[block_id] & (1L << content_bit)) != 0L;
}

// Identify if a block id has live cells
bool BlockDb_doesBlockExist(BlockDb *self, int block_id) {
    if (block_id <= INVALID_BLOCK_ID) {
        return false;
    }
    return self->ids[block_id] > 0;
}


/*** Property getters & setters ***/

int BlockDb_getBlockSize(BlockDb *self, int block_id) {

    return self->sizes[block_id];
}

// Set the size parameter for an existing block
// NOTE: Exercise caution using this, as it will trigger a reinterpretation
// of the `contents` mask, which could have strange side effects
int BlockDb_setBlockSize(BlockDb *self, int block_id, int size) {
    if (!BlockDb_doesBlockExist(self, block_id)) {
        return -1;
    }

    self->sizes[block_id] = size;
    return 0;
}
// NOTE: Should this be allowable? Setting a block's size post-creation seems like a bad ides


long BlockDb_getBlockContents(BlockDb *self, int block_id) {
    return self->contents[block_id];
}

int BlockDb_setBlockContents(BlockDb *self, int block_id, long contents) {

    if (!BlockDb_doesBlockExist(self, block_id)) {
        return -1;
    }

    self->contents[block_id] = contents;
    return 0;
}


Point BlockDb_getBlockPosition(BlockDb *self, int block_id) {
    return self->positions[block_id];
}


int BlockDb_setBlockPosition(BlockDb *self, int block_id, Point position) {

    if (!BlockDb_doesBlockExist(self, block_id)) {
        return -1;
    }
    self->positions[block_id] = position;
    return 0;
}

SDL_Color BlockDb_getBlockColor(BlockDb *self, int block_id) {
    return self->colors[block_id];
}

int BlockDb_setBlockColor(BlockDb *self, int block_id, SDL_Color color) {
    if (!BlockDb_doesBlockExist(self, block_id)) {
        return -1;
    }
    self->colors[block_id] = color;
    return 0;
}


/**
 * @brief Decrease the number of content cells represented by a block id
 * @param self - Pointer to BlockDb to decrement within
 * @param by - Integer quantity to decrement by. Cannot be greater than the
 *             current number of cells represented by this block.
 */
int BlockDb_decrementCellCount(BlockDb *self, int block_id, int by) {

    if (!BlockDb_doesBlockExist(self, block_id) || by > self->ids[block_id]) {
        return -1;
    }

    self->ids[block_id] -= by;
    return 0;
}


/**
 * @brief Increment the number of content cells represented by a block
 * @param self - Pointer to BlockDb struct
 * @param by - Integer quantity to increment by
 */
int BlockDb_incrementCellCount(BlockDb *self, int block_id, int by) {

    if (!BlockDb_doesBlockExist(self, block_id) || self->ids[block_id] == 0) {
        return -1;
    }

    self->ids[block_id] += by;
    return 0;
}

// Return the recorded cell count for a block.
// NOTE:
//      * Since this count can be maniuplated independently, it will not
//        necessarily match the number of bits set in `contents` maek
int BlockDb_getCellCount(BlockDb *self, int block_id) {

    if (block_id <= INVALID_BLOCK_ID) {
        return -1;
    }

    return self->ids[block_id];
    return 0;
}


// Remove the indicated block from existence
int BlockDb_removeBlock(BlockDb *self, int block_id) {

    if (!BlockDb_doesBlockExist(self, block_id)) {
        return -1;
    }

    self->ids[block_id] = 0;
    return 0;
}

