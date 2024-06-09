#include "../src/block.h"
#include "EWENIT.h"


void testContentRotationProperties() {
    /* Test the properties of clockwise rotation across all block sizes */
    long outputContents;
    long inputContents;

    for (int blockSize = 1; blockSize <= 8; blockSize++) {
        INFO_FMT("blockSize = %d", blockSize);

        ASSERT_EQUAL_LONG(1L, rotateBlockContentsCw90(1L, blockSize));
        ASSERT_EQUAL_LONG(1L, rotateBlockContentsCcw90(1L, blockSize));
        ASSERT_EQUAL_LONG(1L, rotateBlockContents180(1L, blockSize));

        ASSERT_EQUAL_LONG(0L, rotateBlockContentsCw90(0L, blockSize));
        ASSERT_EQUAL_LONG(0L, rotateBlockContentsCcw90(0L, blockSize));
        ASSERT_EQUAL_LONG(0L, rotateBlockContents180(0L, blockSize));

        for (int bitNum = 0; bitNum < blockSize * blockSize; bitNum++) {
            long inputCcw90 = 1 << bitNum;
            long inputCw90 = 1 << bitNum;
            long input180 = 1 << bitNum;

            long outputCcw90 = inputCcw90;
            long outputCw90 = inputCw90; 
            long output180 = input180;

            // four rotations of any given type should come back to origin
            for (int rotationsDone = 0; rotationsDone < 4; rotationsDone++) {
                if (rotationsDone > 0) {
                    ASSERT_NOT_EQUAL_LONG(inputCcw90, outputCcw90);
                    ASSERT_NOT_EQUAL_LONG(inputCw90, outputCw90);
                    if (rotationsDone == 2) {
                        ASSERT_EQUAL_LONG(input180, output180);
                    }
                    else {
                        ASSERT_NOT_EQUAL_LONG(input180, output180);
                    }
                }
                outputCcw90 = rotateBlockContentsCcw90(outputCcw90, blockSize);
                outputCw90 = rotateBlockContentsCw90(outputCw90, blockSize);
                output180 = rotateBlockContents180(output180, blockSize);
            }
            ASSERT_EQUAL_LONG(inputCcw90, outputCcw90);
            ASSERT_EQUAL_LONG(inputCw90, outputCw90);
            ASSERT_EQUAL_LONG(input180, output180);

            /* Certain rotations reverse one another */
            inputContents = 1 << bitNum;
            ASSERT_EQUAL_LONG(
                inputContents,
                rotateBlockContentsCcw90(rotateBlockContentsCw90(inputContents, blockSize), blockSize)
            );

            ASSERT_EQUAL_LONG(
                rotateBlockContents180(inputContents, blockSize),
                rotateBlockContentsCcw90(rotateBlockContentsCcw90(inputContents, blockSize), blockSize)
            );

            ASSERT_EQUAL_LONG(
                rotateBlockContents180(inputContents, blockSize),
                rotateBlockContentsCw90(rotateBlockContentsCw90(inputContents, blockSize), blockSize)
            );
        }
    }
}



int main() {
    EWENIT_START;
    ADD_CASE(testRotationProperties);
    EWENIT_END_COMPACT;
    return 0;
}
