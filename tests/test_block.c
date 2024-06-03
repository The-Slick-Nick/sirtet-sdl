#include <stdio.h>
#include "../src/block.h"
#include "EWENIT.h"



void test_rotate_90cw() {
    /* test rotating a block 90 degrees clockwise */
    Block result;
    Block block1 = {
        .position = {.x = 0, .y = 0},
        .structure = 1
    };
    result = Block_rotateCw90(block1);
    ASSERT_EQUAL_INT(result.structure, 1);

    Block block2 = {
        .position = {.x = 0, .y = 0},
        .structure = 0
    };
    result = Block_rotateCw90(block2);
    ASSERT_EQUAL_INT(result.structure, 0);


    Block block3 = {
        .position = {.x = 0, .y = 0},
        .structure = 1 << 0
    };
    result = Block_rotateCw90(block3);
    ASSERT_EQUAL_INT(result.structure, 1 << 3);
}


int main() {
    EWENIT_START;
    ADD_CASE(test_rotate_90cw);
    EWENIT_END_VERBOSE;
    return 0;
}
