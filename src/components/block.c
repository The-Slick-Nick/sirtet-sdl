#include <assert.h>
#include "block.h"
#include "coordinates.h"


/* Bit representation per block (for a 4-sizer)
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/


   /* represent in quadrants  in the complex plane
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
    return 0 != (self->contents & (1L << content_bit));
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


