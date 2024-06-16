#include "block.h"
#include "coordinates.h"
#include "EWENIT.h"


void testContentRotationProperties() {
    /* Test the properties of clockwise rotation across all block sizes */
    long inputContents;

    for (int blockSize = 1; blockSize <= 8; blockSize++) {
        INFO("-------------------------------");
        INFO_FMT("blockSize = %d", blockSize);

        // Note that we don't care about bits of higher order than are needed 
        // for the blockSize
        long allSet = 0L;
        for (int i = 0; i < blockSize * blockSize; i++) {
            allSet <<= 1;
            allSet |= 1;
        }

        INFO_FMT("allSet for blockSize %d is %ld", blockSize, allSet);
        INFO("Checking rotation of all set bits");
        ASSERT_EQUAL_LONG(allSet, rotateBlockContentsCw90(allSet, blockSize));
        ASSERT_EQUAL_LONG(allSet, rotateBlockContentsCcw90(allSet, blockSize));
        ASSERT_EQUAL_LONG(allSet, rotateBlockContents180(allSet, blockSize));

        INFO("Checking rotation of no set bits");
        ASSERT_EQUAL_LONG(0L, rotateBlockContentsCw90(0L, blockSize));
        ASSERT_EQUAL_LONG(0L, rotateBlockContentsCcw90(0L, blockSize));
        ASSERT_EQUAL_LONG(0L, rotateBlockContents180(0L, blockSize));

        for (int bitNum = 0; bitNum < blockSize * blockSize; bitNum++) {
            INFO_FMT("bit number %d", bitNum);

            long inputCcw90 = 1L << bitNum;
            long inputCw90 = 1L << bitNum;
            long input180 = 1L << bitNum;

            long outputCcw90 = inputCcw90;
            long outputCw90 = inputCw90; 
            long output180 = input180;

            INFO("Checking rotational properties");
            // four rotations of any given type should come back to origin
            for (int rotationNum = 1; rotationNum <= 4; rotationNum++) {
                INFO_FMT("Rotation %d", rotationNum);

                outputCcw90 = rotateBlockContentsCcw90(outputCcw90, blockSize);
                outputCw90 = rotateBlockContentsCw90(outputCw90, blockSize);
                output180 = rotateBlockContents180(output180, blockSize);

                // absolute center on an odd-numbered block size
                if (((blockSize & 1) == 1) && (bitNum == ((blockSize * blockSize) / 2))) {
                    continue;
                }

                if (rotationNum == 4) {
                    ASSERT_EQUAL_LONG(inputCcw90, outputCcw90);
                    ASSERT_EQUAL_LONG(inputCw90, outputCw90);
                    ASSERT_EQUAL_LONG(input180, output180);
                }
                else {
                    ASSERT_NOT_EQUAL_LONG(inputCcw90, outputCcw90);
                    ASSERT_NOT_EQUAL_LONG(inputCw90, outputCw90);

                    if (rotationNum == 2) {
                        ASSERT_EQUAL_LONG(input180, output180);
                    }
                    else {
                        ASSERT_NOT_EQUAL_LONG(input180, output180);
                    }
                }


            }

            /* Certain rotations reverse one another */
            inputContents = 1L << bitNum;
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

void testContentRotationManual() {
    /* Test manually input examples */

    long inputContents;

    inputContents = 1L << 63;
    ASSERT_EQUAL_LONG(rotateBlockContentsCcw90(inputContents, 8), (1L << 7));
    ASSERT_EQUAL_LONG(
        rotateBlockContentsCcw90(rotateBlockContentsCcw90(inputContents, 8), 8),
        (1L << 0)
    );

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



    INFO("Block size 7");

    expected = (Point){0, 0};
    actual = contentBitToPoint(24, 7);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){0, 1};
    actual = contentBitToPoint(17, 7);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);


    expected = (Point){1, 0};
    actual = contentBitToPoint(25, 7);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);




    INFO("Block size 8"); 

    expected = (Point){-4, 4};
    actual = contentBitToPoint(0, 8);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

    expected = (Point){4, -4};
    actual = contentBitToPoint(63, 8);
    ASSERT_EQUAL_INT(expected.x, actual.x);
    ASSERT_EQUAL_INT(expected.y, actual.y);

}


void testPointToContentBit() {
    
    int expected;
    int actual;

    INFO("blockSize 2");
    expected = 1;
    actual = pointToContentBit((Point){1, 1}, 2);
    ASSERT_EQUAL_INT(expected, actual);
    
    expected = 3;
    actual = pointToContentBit((Point){1, -1}, 2);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 2;
    actual = pointToContentBit((Point){-1, -1}, 2);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 0;
    actual = pointToContentBit((Point){-1, 1}, 2);
    ASSERT_EQUAL_INT(expected, actual);


    INFO("blockSize 3");

    expected = 2;
    actual = pointToContentBit((Point){1, 1}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 8;
    actual = pointToContentBit((Point){1, -1}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 6;
    actual = pointToContentBit((Point){-1, -1}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 0;
    actual = pointToContentBit((Point){-1, 1}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    // check 0 coords
    expected = 1;
    actual = pointToContentBit((Point){0, 1}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 5;
    actual = pointToContentBit((Point){1, 0}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 7;
    actual = pointToContentBit((Point){0, -1}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 3;
    actual = pointToContentBit((Point){-1, 0}, 3);
    ASSERT_EQUAL_INT(expected, actual);

    /* Below are more examples I'm testing for test cases that have
     * been known to fail */


    expected = 24;
    actual = pointToContentBit((Point){0, 0}, 7);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 17;
    actual = pointToContentBit((Point){0, 1}, 7);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 25;
    actual = pointToContentBit((Point){1, 0}, 7);
    ASSERT_EQUAL_INT(expected, actual);



    expected = 0;
    actual = pointToContentBit((Point){-4, 4}, 8);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 56;
    actual = pointToContentBit((Point){-4, -4}, 8);
    ASSERT_EQUAL_INT(expected, actual);

    expected = 7;
    actual = pointToContentBit((Point){4, 4}, 8);
    ASSERT_EQUAL_INT(expected, actual);

}


void testContentBitConversionProperties() {
    /* Tests using
     * contentBitToPoint and pointToContentBit in tandem
     */
    int output;

    for (int blockSize = 2; blockSize <= 8; blockSize++) {
        for (int bitNum = 0; bitNum < blockSize * blockSize; bitNum++) {
            output = pointToContentBit(
                contentBitToPoint(bitNum, blockSize),
                blockSize
            );

            ASSERT_EQUAL_INT(output, bitNum);
        }
    }

}

void testSetupNewBlock() {
    /* Tests on setting up new blocks */

    const int max_blocks = 128;
    int id_arr[128] = {0};

    BlockIds my_ids = {.head = 0, .id_array = id_arr, .max_ids = 128};

    int new_id;
    for (int i = 0; i < max_blocks; i++) {
        new_id = provisionBlockId(&my_ids, 4);

        // PROPERTY: valid ids are positive
        ASSERT_GREATER_THAN_INT(new_id, -1);

        // PROPERTY: values stored are number of "cells" per block
        ASSERT_EQUAL_INT(my_ids.id_array[new_id], 4);
    }
   
    // PROPERTY: -1 should be an invalid id, returned when we couldn't provision an id
    new_id = provisionBlockId(&my_ids, 4);
    ASSERT_EQUAL_INT(new_id, -1);

    // PROPERTY: 0 should be a valid id
    removeBlockId(&my_ids, 0);
    new_id = provisionBlockId(&my_ids, 4);
    ASSERT_GREATER_THAN_INT(new_id, -1);
}


void testTransformBlock() {
    Block my_block = {.block_size = 2, .contents = 0b0011};

    transformBlock(&my_block, (Point){0, 1});
    ASSERT_EQUAL_INT(my_block.contents, 0b0101);

    transformBlock(&my_block, (Point){-1, 0});
    ASSERT_EQUAL_INT(my_block.contents, 0b1010);
}

void testTranslateBlock() {
    Block my_block;
    
    my_block = (Block){.position = (Point){.x = 2, .y = 2}};
    translateBlock(&my_block, (Point){1, 0});
    ASSERT_EQUAL_INT(my_block.position.x , 3);
    ASSERT_EQUAL_INT(my_block.position.y , 2);

    my_block = (Block){.position = (Point){.x = 2, .y = 2}};
    translateBlock(&my_block, (Point){0, 1});
    ASSERT_EQUAL_INT(my_block.position.x , 2);
    ASSERT_EQUAL_INT(my_block.position.y , 3);

    my_block = (Block){.position = (Point){.x = 2, .y = 2}};
    translateBlock(&my_block, (Point){-1, 0});
    ASSERT_EQUAL_INT(my_block.position.x , 1);
    ASSERT_EQUAL_INT(my_block.position.y , 2);

    my_block = (Block){.position = (Point){.x = 2, .y = 2}};
    translateBlock(&my_block, (Point){0, -1});
    ASSERT_EQUAL_INT(my_block.position.x , 2);
    ASSERT_EQUAL_INT(my_block.position.y , 1);
}


int main() {
    EWENIT_START;
    ADD_CASE(testContentRotationProperties);
    ADD_CASE(testContentRotationManual);
    ADD_CASE(testContentBitToPoint);
    ADD_CASE(testPointToContentBit);
    printf("hello");
    ADD_CASE(testContentBitConversionProperties);
    ADD_CASE(testSetupNewBlock);
    ADD_CASE(testTransformBlock);
    ADD_CASE(testTranslateBlock);
    EWENIT_END;
    // EWENIT_END_COMPACT;
    return 0;
}
