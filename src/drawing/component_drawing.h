/* component_drawing.h
*
* Header file to handle the drawing of any components
* (blocks, grid, etc.)
*/


#ifndef COMPONENT_DRAWING_H
#define COMPONENT_DRAWING_H

#include <SDL2/SDL.h>
#include "grid.h"
#include "block.h"
#include "coordinates.h"


int drawBlockCell(
    SDL_Renderer *rend,
    Point location, int width, int height,
    SDL_Color base_color
);


int drawGrid(
    SDL_Renderer *rend,
    SDL_Rect display_window,
    BlockDb *block_db,
    GameGrid *grid
);


int drawBlock(
    SDL_Renderer *rend,
    SDL_Rect display_window,
    BlockDb *block_db,
    int block_id,
    GameGrid *ref_grid
);


#endif


