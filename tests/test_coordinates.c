#include "EWENIT.h"
#include "../src/block.h"
#include "../src/coordinates.h"


void testPointTransform() {
    
    // 1 + i
    Point testPoint = {.x=1, .y=1};
    Point result;

    result = Point_transform(testPoint, (Point){0, 0});
    ASSERT_EQUAL_INT(result.x, 0);
    ASSERT_EQUAL_INT(result.y, 0);


    result = Point_transform(testPoint, (Point){1, 0});
    ASSERT_EQUAL_INT(result.x, 1);
    ASSERT_EQUAL_INT(result.y, 1);

    result = Point_transform(testPoint, (Point){0, 1});
    ASSERT_EQUAL_INT(result.x, -1);
    ASSERT_EQUAL_INT(result.y, 1);

    result = Point_transform(testPoint, (Point){-1, 0});
    ASSERT_EQUAL_INT(result.x, -1);
    ASSERT_EQUAL_INT(result.y, -1);

    result = Point_transform(testPoint, (Point){0, -1});
    ASSERT_EQUAL_INT(result.x, 1);
    ASSERT_EQUAL_INT(result.y, -1);

    result = Point_transform(testPoint, (Point){1, 1});
    ASSERT_EQUAL_INT(result.x, 0);
    ASSERT_EQUAL_INT(result.y, 2);

    /* Below are extra examples I've discovered of things
     * needing testing from actual things I've found */
    result = Point_transform((Point){-4, 4}, (Point){0, 1});
    ASSERT_EQUAL_INT(result.x, -4);
    ASSERT_EQUAL_INT(result.y, -4);

    result = Point_transform((Point){-4, -4}, (Point){0, 1});
    ASSERT_EQUAL_INT(result.x, 4);
    ASSERT_EQUAL_INT(result.y, -4);

    result = Point_transform((Point){-4, 4}, (Point){-1, 0});
    ASSERT_EQUAL_INT(result.x, 4);
    ASSERT_EQUAL_INT(result.y, -4);

    result = Point_transform((Point){4, -4}, (Point){0, 1});
    ASSERT_EQUAL_INT(result.x, 4);
    ASSERT_EQUAL_INT(result.y, 4);

    result = Point_transform((Point){0, 1}, (Point){0, -1});
    ASSERT_EQUAL_INT(result.x, 1);
    ASSERT_EQUAL_INT(result.y, 0);

}


int main() {
    EWENIT_START;
    ADD_CASE(testPointTransform);

    EWENIT_END;
}

