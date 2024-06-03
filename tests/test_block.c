#include <stdio.h>
#include "../src/block.h"
#include "EWENIT.h"



void test_rotate_90cw() {
    /* test rotating a block 90 degrees clockwise */
    Block block1 = {
        .position = {.x = 0, .y = 0},
        .structure = 1
    };

    Block result = Block_rotateCw90(block1);
    ASSERT_EQUAL_INT(result.structure, 1);
}


int main() {
    EWENIT_START;
    ADD_CASE(test_rotate_90cw);
    EWENIT_END_VERBOSE;
    return 0;
}
