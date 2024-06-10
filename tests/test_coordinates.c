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


}


int main() {
    EWENIT_START;
    ADD_CASE(testPointTransform);

    EWENIT_END;
}

