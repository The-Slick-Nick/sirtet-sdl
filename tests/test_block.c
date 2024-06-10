#include "../src/block.h"
#include "EWENIT.h"


void testContentRotationProperties() {
    /* Test the properties of clockwise rotation across all block sizes */
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


void testContentBitToPoint() {
    Point expected;
    Point actual;

    // ----------------------------------------
    INFO("blockSize 2");

    expected = (Point){1, 1};
    actual = contentBitToPoint(1, 2);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){1, -1};
    actual = contentBitToPoint(3, 2);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, -1};
    actual = contentBitToPoint(2, 2);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, 1};
    actual = contentBitToPoint(0, 2);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    // ----------------------------------------
    INFO("blockSize 3");

    expected = (Point){1, 1};
    actual = contentBitToPoint(2, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){1, -1};
    actual = contentBitToPoint(8, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, -1};
    actual = contentBitToPoint(6, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, 1};
    actual = contentBitToPoint(0, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    // check 0 coords
    expected = (Point){0, 1};
    actual = contentBitToPoint(1, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){1, 0};
    actual = contentBitToPoint(5, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){0, -1};
    actual = contentBitToPoint(7, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, 0};
    actual = contentBitToPoint(3, 3);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    // ----------------------------------------
    INFO("blockSize 4");

    expected = (Point){1, 1};
    actual = contentBitToPoint(6, 4);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){1, -1};
    actual = contentBitToPoint(10, 4);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, -1};
    actual = contentBitToPoint(9, 4);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, 1};
    actual = contentBitToPoint(5, 4);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    // ----------------------------------------
    INFO("blockSize 5");

    expected = (Point){1, 1};
    actual = contentBitToPoint(8, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){1, -1};
    actual = contentBitToPoint(18, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, -1};
    actual = contentBitToPoint(16, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-1, 1};
    actual = contentBitToPoint(6, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    // check 0 coords
    expected = (Point){0, 2};
    actual = contentBitToPoint(2, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){2, 0};
    actual = contentBitToPoint(14, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){0, -2};
    actual = contentBitToPoint(22, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){-2, 0};
    actual = contentBitToPoint(10, 5);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);


    // NOTE: I've only tested up to size 5, as the general pattern
    // is pretty well established once these succeed

}


int main() {
    EWENIT_START;
    ADD_CASE(testContentRotationProperties);
    ADD_CASE(testContentBitToPoint);
    EWENIT_END;
    return 0;
}
