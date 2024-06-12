#include "block.h"
#include <stdio.h>


int main() {
    long before_rotation = 0b1000;
    long after_rotation = rotateBlockContents180(before_rotation, 4);
    printf("Before we rotated the value was %ld\n", before_rotation);
    printf("AFter we rotated the value was %ld\n", after_rotation);
    return 0;
}
