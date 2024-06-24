/* component_drawing.h
*
* Header file to handle the drawing of any components
* (blocks, grid, etc.)
*/


#ifndef COMPONENT_DRAWING_H
#define COMPONENT_DRAWING_H

#include <SDL2/SDL.h>
#include "../components/coordinates.h"


int drawBlockCell(
    SDL_Renderer *rend,
    Point location, int width, int height, SDL_Color body_color, SDL_Color rim_color
);


#endif


