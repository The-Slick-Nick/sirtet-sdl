#include "coordinates.h"


// Transform a point `basis` by a given transofmation by
// interpreting the points as complex numbers x + yi and
// multiplying
Point Point_transform(Point basis, Point transformation) {

    Point returnVal = {
        .x = (basis.x * transformation.x) - (basis.y * transformation.y),
        .y = (basis.x * transformation.y) + (basis.y * transformation.x)
    };

    return returnVal;
}

Point Point_translate(Point basis, Point translation) {
    Point returnVal = {
        .x = (basis.x + translation.x),
        .y = (basis.y + translation.y)
    };
    
    return returnVal;
}
