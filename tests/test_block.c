#include "block.h"
#include "coordinates.h"
#include "EWENIT.h"


void testContentRotationProperties() {
    /* Test the properties of clockwise rotation across all block sizes */
    long input_contents;

    for (int block_size = 1; block_size <= 8; block_size++) {
        INFO("-------------------------------");
        INFO_FMT("block_size = %d", block_size);

        // Note that we don't care about bits of higher order than are needed 
        // for the block_size
        long allSet = 0L;
        for (int i = 0; i < block_size * block_size; i++) {
            allSet <<= 1;
            allSet |= 1;
        }

        INFO_FMT("allSet for block_size %d is %ld", block_size, allSet);
        INFO("Checking rotation of all set bits");
        ASSERT_EQUAL_LONG(allSet, rotateBlockContentsCw90(allSet, block_size));
        ASSERT_EQUAL_LONG(allSet, rotateBlockContentsCcw90(allSet, block_size));
        ASSERT_EQUAL_LONG(allSet, rotateBlockContents180(allSet, block_size));

        INFO("Checking rotation of no set bits");
        ASSERT_EQUAL_LONG(0L, rotateBlockContentsCw90(0L, block_size));
        ASSERT_EQUAL_LONG(0L, rotateBlockContentsCcw90(0L, block_size));
        ASSERT_EQUAL_LONG(0L, rotateBlockContents180(0L, block_size));

        for (int bitNum = 0; bitNum < block_size * block_size; bitNum++) {
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

                outputCcw90 = rotateBlockContentsCcw90(outputCcw90, block_size);
                outputCw90 = rotateBlockContentsCw90(outputCw90, block_size);
                output180 = rotateBlockContents180(output180, block_size);

                // absolute center on an odd-numbered block size
                if (((block_size & 1) == 1) && (bitNum == ((block_size * block_size) / 2))) {
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
            input_contents = 1L << bitNum;
            ASSERT_EQUAL_LONG(
                input_contents,
                rotateBlockContentsCcw90(rotateBlockContentsCw90(input_contents, block_size), block_size)
            );

            ASSERT_EQUAL_LONG(
                rotateBlockContents180(input_contents, block_size),
                rotateBlockContentsCcw90(rotateBlockContentsCcw90(input_contents, block_size), block_size)
            );

            ASSERT_EQUAL_LONG(
                rotateBlockContents180(input_contents, block_size),
                rotateBlockContentsCw90(rotateBlockContentsCw90(input_contents, block_size), block_size)
            );
        }
    }
}

void testContentRotationManual() {
    /* Test manually input examples */

    long input_contents;

    input_contents = 1L << 63;
    ASSERT_EQUAL_LONG(rotateBlockContentsCcw90(input_contents, 8), (1L << 7));
    ASSERT_EQUAL_LONG(
        rotateBlockContentsCcw90(rotateBlockContentsCcw90(input_contents, 8), 8),
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

    for (int block_size = 2; block_size <= 8; block_size++) {
        for (int bit_num = 0; bit_num < block_size * block_size; bit_num++) {
            output = pointToContentBit(
                contentBitToPoint(bit_num, block_size),
                block_size
            );

            ASSERT_EQUAL_INT(output, bit_num);
        }
    }

}

/*=================================================================
 * Refactor from here below
==================================================================*/

void testBlockCreation() {
    /* Tests on setting up new blocks */

    int id_arr[128];
    int size_arr[128];
    long contents_arr[128];
    Point position_arr[128];

    memset(id_arr, 0, 128 * sizeof(int));

    BlockDb db = {
        .max_ids=128,
        .head=0,

        .ids=id_arr,
        .sizes=size_arr,
        .contents=contents_arr,
        .positions=position_arr
    };

    int new_block = BlockDb_createBlock(&db, 4, 0b0000011001100000L, (Point){.x=0, .y=0});
    ASSERT_GREATER_THAN_INT(new_block, INVALID_BLOCK_ID);
    ASSERT_TRUE(BlockDb_doesBlockExist(&db, new_block));

    // ensure set up correctly while also testing getters
    ASSERT_EQUAL_INT(BlockDb_getCellCount(&db, new_block), 4);
    ASSERT_EQUAL_INT(BlockDb_getBlockSize(&db, new_block), 4);
    ASSERT_EQUAL_LONG(BlockDb_getBlockContents(&db, new_block), 0b0000011001100000L);

    // fill up our db
    for (int i = 0; i < 127; i++) {
        BlockDb_createBlock(&db, 4, 0L, (Point){.x=0, .y=0});
    }

    // create one too many
    int bad_block = BlockDb_createBlock(&db, 4, 0L, (Point){.x=0, .y=0});
    ASSERT_EQUAL_INT(bad_block, INVALID_BLOCK_ID);
    ASSERT_FALSE(BlockDb_doesBlockExist(&db, bad_block));

}

void testBlockCellManipulation() {
    // Add and remove cells from blocks

    int id_arr[128];
    int size_arr[128];
    long contents_arr[128];
    Point position_arr[128];

    memset(id_arr, 0, 128 * sizeof(int));

    BlockDb db = {
        .max_ids=128,
        .head=0,

        .ids=id_arr,
        .sizes=size_arr,
        .contents=contents_arr,
        .positions=position_arr
    };

    // make a block with 4 cells
    // BlockDb_createBlock(BlockDb *self, int size, long contents, Point position)
    int block_id = BlockDb_createBlock(&db, 4, 0b0000111111110000L, (Point){.x=0, .y=0});

    ASSERT_EQUAL_INT(BlockDb_getCellCount(&db, block_id), 8);

    int retval = BlockDb_incrementCellCount(&db, block_id, 1);
    ASSERT_EQUAL_INT(retval, 0);  // success
    ASSERT_EQUAL_INT(BlockDb_getCellCount(&db, block_id), 9);

    retval = BlockDb_decrementCellCount(&db, block_id, 2);
    ASSERT_EQUAL_INT(retval, 0);  // success
    ASSERT_EQUAL_INT(BlockDb_getCellCount(&db, block_id), 7);

    retval = BlockDb_removeBlock(&db, block_id);
    ASSERT_EQUAL_INT(retval, 0);  // success

    ASSERT_EQUAL_INT(BlockDb_getCellCount(&db, block_id), 0);
    ASSERT_FALSE(BlockDb_doesBlockExist(&db, block_id));

    // Cannot directly increment from 0 without calling for block creation
    retval = BlockDb_incrementCellCount(&db, block_id, 1);
    ASSERT_EQUAL_INT(retval, -1);  // fail

    retval = BlockDb_decrementCellCount(&db, block_id, 1);
    ASSERT_EQUAL_INT(retval, -1);  // fail

    int block_id_2 = BlockDb_createBlock(&db, 4, 0b0000111111110000L, (Point){.x=0, .y=0});

    // decrement to 0 counts as a "manual" removal
    retval = BlockDb_decrementCellCount(&db, block_id_2, 8);
    ASSERT_EQUAL_INT(retval, 0);  // success
    ASSERT_FALSE(BlockDb_doesBlockExist(&db, block_id_2));

    // cannot decrement past 0
    int block_id_3 = BlockDb_createBlock(&db, 4, 0b0000111111110000L, (Point){.x=0, .y=0});
    retval = BlockDb_decrementCellCount(&db, block_id_3, 9);
    ASSERT_EQUAL_INT(retval, -1);  // fail
    
    // operation fails entirely - nothing removed
    ASSERT_EQUAL_INT(BlockDb_getCellCount(&db, block_id_3), 8);
}



void testTransformBlock() {
    int id_arr[128];
    int size_arr[128];
    long contents_arr[128];
    Point position_arr[128];

    memset(id_arr, 0, 128 * sizeof(int));

    BlockDb db = {
        .max_ids=128,
        .head=0,

        .ids=id_arr,
        .sizes=size_arr,
        .contents=contents_arr,
        .positions=position_arr
    };

    // int block_id = BlockDb_createBlock(BlockDb *self, int size, long contents, Point position)
    int block_id = BlockDb_createBlock(&db, 2, 0b0011L, (Point){0, 0});

    int retval = BlockDb_transformBlock(&db, block_id, (Point){0, 1});
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(BlockDb_getBlockContents(&db, block_id), 0b0101L);

    retval = BlockDb_transformBlock(&db, block_id, (Point){-1, 0});
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(BlockDb_getBlockContents(&db, block_id), 0b1010L);

}

void testTranslateBlock() {
    int id_arr[128];
    int size_arr[128];
    long contents_arr[128];
    Point position_arr[128];

    memset(id_arr, 0, 128 * sizeof(int));

    BlockDb db = {
        .max_ids=128,
        .head=0,

        .ids=id_arr,
        .sizes=size_arr,
        .contents=contents_arr,
        .positions=position_arr
    };

    Point new_pos;
    Point init_pos = {.x=2, .y=2};
    int retval;

    int block_id = BlockDb_createBlock(&db, 2, 0b1111, init_pos);
    retval = BlockDb_translateBlock(&db, block_id, (Point){1, 0});
    ASSERT_EQUAL_INT(retval, 0);

    new_pos = BlockDb_getBlockPosition(&db, block_id);
    ASSERT_EQUAL_INT(new_pos.x , 3);
    ASSERT_EQUAL_INT(new_pos.y , 2);


    BlockDb_setBlockPosition(&db, block_id, init_pos);
    retval = BlockDb_translateBlock(&db, block_id, (Point){0, 1});
    ASSERT_EQUAL_INT(retval, 0);

    new_pos = BlockDb_getBlockPosition(&db, block_id);
    ASSERT_EQUAL_INT(new_pos.x , 2);
    ASSERT_EQUAL_INT(new_pos.y , 3);


    BlockDb_setBlockPosition(&db, block_id, init_pos);
    retval = BlockDb_translateBlock(&db, block_id, (Point){-1, 0});
    ASSERT_EQUAL_INT(retval, 0);

    new_pos = BlockDb_getBlockPosition(&db, block_id);
    ASSERT_EQUAL_INT(new_pos.x , 1);
    ASSERT_EQUAL_INT(new_pos.y , 2);

    BlockDb_setBlockPosition(&db, block_id, init_pos);
    retval = BlockDb_translateBlock(&db, block_id, (Point){0, -1});
    ASSERT_EQUAL_INT(retval, 0);

    new_pos = BlockDb_getBlockPosition(&db, block_id);
    ASSERT_EQUAL_INT(new_pos.x , 2);
    ASSERT_EQUAL_INT(new_pos.y , 1);
}



void testGetCellCount() {
    
    INFO("Size 2");
    ASSERT_EQUAL_INT(getCellCount(0b1111L, 2), 4);
    ASSERT_EQUAL_INT(getCellCount(0b0000L, 2), 0);
    ASSERT_EQUAL_INT(getCellCount(0b0110L, 2), 2);

    INFO("Size 4");
    ASSERT_EQUAL_INT(getCellCount(0b1111111111111111L, 4), 16);
    ASSERT_EQUAL_INT(getCellCount(0b0000000000000000L, 4), 0);
    ASSERT_EQUAL_INT(getCellCount(0b1111111100000000L, 4), 8);

    INFO("Size 5 (odd)");
    ASSERT_EQUAL_INT(getCellCount(0b1111111111111111111111111L, 5), 25);
    ASSERT_EQUAL_INT(getCellCount(0b0000000000000000000000000L, 5), 0);
    ASSERT_EQUAL_INT(getCellCount(0b1111111111000000000000000L, 5), 10);
}


/*=================================================================
 * End refactor zone
==================================================================*/


int main() {
    EWENIT_START;
    ADD_CASE(testContentRotationProperties);
    ADD_CASE(testContentRotationManual);
    ADD_CASE(testContentBitToPoint);
    ADD_CASE(testPointToContentBit);
    ADD_CASE(testContentBitConversionProperties);

    ADD_CASE(testBlockCreation);
    ADD_CASE(testBlockCellManipulation);
    ADD_CASE(testTransformBlock);
    ADD_CASE(testTranslateBlock);
    ADD_CASE(testGetCellCount);

    EWENIT_END;
    return 0;
}
