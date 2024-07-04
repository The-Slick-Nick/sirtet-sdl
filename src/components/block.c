#include <assert.h>
#include "block.h"
#include "coordinates.h"


/* Bit representation per block (for a 4-sizer)
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/


/* For rotations, represent in quadrants  in the complex plane
*
* (-i + 1) | ( i + 1)
* ---------------------
* (-i - 1) | ( i - 1)
*
* To rotate 90deg clockwise, multiply by i
* To rotate 90deg counterclockwise, multiply by -i
* To rotate 180deg, multiply by -1
*
*  Perform two rotations, first rotating 2x2 blocks 
*  around the overall center, than performing that
*  same rotation within each 2x2.
*/


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


// Transform a block in place, by the given transformation vector (point)
void Block_transform(Block* self, Point transform) {
    self->contents = transformBlockContents(self->contents, self->size, transform);
}

// translate a block in place, by the given translation vector (point)
void Block_translate(Block* self, Point translation) {
    self->position = Point_translate(self->position, translation);
}

// identify if given content bit is set (shorthand for bit magic)
bool Block_isContentBitSet(Block* self, int content_bit) {


    return 0L != (self->contents & (1L << content_bit));
}



// Get and assign a block_id from the given BlockIds struct for a block
// of provided size, returning the new id. If no ids are available,
// returns -1
int BlockIds_provisionId(BlockIds *self, int block_size) {

    const int block_count = self->max_ids;

    for (int self_checked = 0; self_checked < block_count; self_checked++) {
 
        if (0 == *(self->id_array + self->head)) {
            *(self->id_array + self->head) = *(self->id_array + self->head) + block_size;
            return self->head;
        }

        self->head = (self->head + 1) % block_count;
    }
    return -1;
}


// remove the contents at a particular id
int BlockIds_removeId(BlockIds *self, int to_remove) {
    (self->id_array)[to_remove] = 0;
    return 0;
}


// Decrease the number of instances of an id recorded
int BlockIds_decrementId(BlockIds* ids, int to_decrement, int by) {

    if (to_decrement < 0 || to_decrement >= ids->max_ids) {
        return -1;
    }

    if (ids->id_array[to_decrement] < by) {
        return -1;
    }

    if (by < 0) {
        return -1;
    }

    ids->id_array[to_decrement] = ids->id_array[to_decrement] - by;
    return 0;
}

// Increase the number of instances of an id recorded
int BlockIds_incrementId(BlockIds* ids, int to_increment, int by) {


    if (to_increment < 0 || to_increment >= ids->max_ids) {
        return -1;
    }

    if (by < 0) {
        return -1;
    }

    ids->id_array[to_increment] = ids->id_array[to_increment] + by;
    return 0;
}


/* ============================================================================
 * BlockDb (block refactor) section
============================================================================ */

// Provision and create a new block, returning its id
int BlockDb_createBlock(
    BlockDb *self, int size, long contents, Point position
) {

    int return_id = INVALID_BLOCK_ID;

    if (contents == 0) {
        return return_id;
    }

    for (int attempt = 1; attempt <= self->max_ids; attempt++) {

        if (self->ids[self->head] == 0) {
            return_id = self->head;
            self->ids[return_id] = getCellCount(contents, size);
            self->sizes[return_id] = size;
            self->contents[return_id] = contents;
            self->positions[return_id] = position;
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

int BlockDb_decrementCellCount(BlockDb *self, int block_id, int by) {

    if (!BlockDb_doesBlockExist(self, block_id) || by > self->ids[block_id]) {
        return -1;
    }

    self->ids[block_id] -= by;
    return 0;
}


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

