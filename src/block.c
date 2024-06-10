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
long rotateBlockContentsCw90(long contents, int blockSize) { return 0; }
long rotateBlockContentsCcw90(long contents, int blockSize) { return 0; }
long rotateBlockContents180(long contents, int blockSize) { return 0; }


// Convert a bit number representing a position with a block's contents to its 
// relative coordinates to the center
Point contentBitToPoint(int bitNum, int blockSize) {

    int halfSize = blockSize / 2;
    int rowNum = bitNum / blockSize;
    int colNum = bitNum % blockSize;

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
