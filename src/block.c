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


long _transformBlockContents(long contents, int blockSize, Point transform) {
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
    return _transformBlockContents(contents, blockSize, (Point){0, -1});
}

long rotateBlockContentsCcw90(long contents, int blockSize) {
    return _transformBlockContents(contents, blockSize, (Point){0, 1});
}


long rotateBlockContents180(long contents, int blockSize) {
    return _transformBlockContents(contents, blockSize, (Point){-1, 0});
}
