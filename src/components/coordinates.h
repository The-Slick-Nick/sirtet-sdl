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


Point Point_transform(Point basis, Point transformation);
Point Point_translate(Point basis, Point translation);




#endif
