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



/**
 * @brief Draw a block directly at the desired location
 *        Draws from the origin (top left coordinate) of the
 *        block's "invisible grid" of cells
 * @param self - BlockDb pointer containing block to be drawn
 * @param block_id - Integer block id of block to draw
 * @param rend  - SDL_Renderer pointer to renderer object
 * @param topleft - Top left location on window of invisble
 *                  "block grid" -- not necessarily where the 
 *                  block itself begins to be drawn.
 * @param cell_width - Width, in pixels, that block cells should be
 * @param cell_height - Height, in pixels, that block cells should be
 */
int BlockDb_drawBlock(
    BlockDb *self, int block_id,
    SDL_Renderer *rend, Point topleft, int cell_width, int cell_height
);

/**
 * @brief Draw a block positioned based on a grid's origin (top left) point.
 *        The Block's position is considered as its position within a
 *        grid at the specified origin, and the draw location is adjusted
 *        accordingly by this method.
 * @param self - BlockDb pointer containing block to be drawn
 * @param block_id - Integer block id of block to draw
 * @param rend  - SDL_Renderer pointer to renderer object
 * @param topleft - Top left location of the block's containing
 *                  grid origin point.
 *                  "block grid" -- not necessarily where the 
 *                  block itself begins to be drawn.
 * @param cell_width - Width, in pixels, to draw block cells
 * @param cell_height - Height, in pixels, to draw block cells
 */
int BlockDb_drawBlockOnGrid(
    BlockDb *self, int block_id,
    SDL_Renderer *rend, Point grid_topleft, int cell_width, int cell_height
);


/**
 * @brief - Draw a grid from the given top left coordinate.
 * @param self - GameGrid pointer of grid to draw
 * @param rend - SDL_Renderer pointer used to draw
 * @param block_db - Pointer to BlockDb to reference for block cell draw info
 * @param topleft - Top left Point of grid to draw
 * @param cell_width - Width, in pixels, to draw block cells
 * @param cell_height - Height, in pixels, to draw block cells
 */
int GameGrid_drawGrid(
    GameGrid *self, SDL_Renderer *rend, BlockDb *block_db,
    Point topleft, int cell_width, int cell_height
);

#endif


