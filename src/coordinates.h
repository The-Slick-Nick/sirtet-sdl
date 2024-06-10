/*
* coordinates.h
*
* Anything/everything I can think of that I may need relating
* to coordinates/positioning/rotation/etc.
*/
#ifndef COORDINATES_H
#define COORDINATES_H

typedef struct {
    int x;
    int y;
} Point;


// Rotate a Point about the origin by using a specified rotation point
Point Point_transform(Point basis, Point transformation);




#endif
