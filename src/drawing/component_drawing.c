
/* component_drawing.h
*
* Source file to handle the drawing of any components
* (blocks, grid, etc.)
*/

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>
#include <assert.h>

#include "coordinates.h"
#include "block.h"
#include "grid.h"

#ifndef INT_MIN
#define INT_MIN(a, b) ((a) > (b) ? (b) : (a))
#endif


int divround(int num, int denom) {

    return num / denom;

    int retval = num / denom;

    if (num % denom > num / 2) {
        return retval + 1;
    }
    return retval;
}

// TODO: ensure 'location' is top left

// Lowest-level unit of "draw game component"
int drawBlockCell(
    SDL_Renderer *rend,
    Point location, int width, int height,
    SDL_Color base_color
) {

    /* Draw the full box as the background color, then overwrite the middle
    * with a smaller rectange of the body color */

    int offset_vertical = height / 10;
    int offset_horizontal = width / 10;

    SDL_Rect nw_box = {
        .x=location.x,
        .y=location.y,
        .w=width - offset_horizontal,
        .h=height - offset_vertical
    };

    SDL_Rect se_box = {
        .x=location.x + offset_horizontal,
        .y=location.y + offset_vertical,
        .w=width - offset_horizontal,
        .h=height - offset_vertical
    };

    SDL_Rect inner_box = {
        .x=location.x + offset_horizontal,
        .y=location.y + offset_vertical,
        .w=width - (2 * offset_horizontal),
        .h=height - (2 * offset_vertical)
    };

    SDL_Color nw_color = (SDL_Color){
        .r=INT_MIN(255, 12*base_color.r / 10),
        .g=INT_MIN(255, 12*base_color.g / 10),
        .b=INT_MIN(255, 12* base_color.b / 10),
        .a=255
    };

    SDL_Color se_color = (SDL_Color){
        .r=8*base_color.r / 10,
        .g=8*base_color.g / 10,
        .b =8* base_color.b / 10,
        .a=255
    };

    SDL_SetRenderDrawColor(rend, nw_color.r, nw_color.g, nw_color.b, nw_color.a);
    SDL_RenderFillRect(rend, &nw_box);

    SDL_SetRenderDrawColor(rend, se_color.r, se_color.g, se_color.b, se_color.a);
    SDL_RenderFillRect(rend, &se_box);

    SDL_SetRenderDrawColor(rend, base_color.r, base_color.g, base_color.b, base_color.a);
    SDL_RenderFillRect(rend, &inner_box);

    return 0;
}

SDL_Color getCellColorById(int block_id) {

    switch (block_id % 3) {
        case 0:
            return (SDL_Color){.r=155, .g=0, .b=0, .a=255 };
        case 1:
            return (SDL_Color){.r=0, .g=155, .b=0, .a=255} ;
        case 2:
            return (SDL_Color){.r=0, .g=0, .b=155, .a=255 };
        default:
            assert(false);
    }
}



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
) {

    for (int row = 0; row < self->height; row++) {
        for (int col = 0; col < (self->width - self->removed[row]); col++) {
            int cell_idx = col + (row * self->width);
            int cell_id = self->contents[cell_idx];

            if (cell_id == INVALID_BLOCK_ID) {
                continue;
            }


            SDL_Color body_color = BlockDb_getBlockColor(block_db, cell_id);
            Point cell_topleft = {
                .x=topleft.x + col * cell_width,
                .y=topleft.y + row * cell_height
            };

            // drawBlockCell(SDL_Renderer *rend, Point location, int width, int height, SDL_Color base_color)
            drawBlockCell(
                rend, cell_topleft, cell_width, cell_height, body_color
            );
        }
    }
    return 0;

}

// TODO: Migrate this back into block.c?

/**
 * @brief Draw a block directly at the desired location
 *        Draws from the origin of the blocks "invisible grid" of cells
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
) {

    SDL_Color base_color = BlockDb_getBlockColor(self, block_id);
    int block_size = BlockDb_getBlockSize(self, block_id);

    for (int row = 0; row < block_size; row++) {
        for (int col = 0; col < block_size; col++) {
            int bit_num = col + row * block_size;

            if (!BlockDb_isContentBitSet(self, block_id, bit_num)) {
                continue;
            }

            Point cell_loc = {
                .x=topleft.x + col * cell_width,
                .y=topleft.y + row * cell_height
            };

            drawBlockCell(rend, cell_loc, cell_width, cell_height, base_color);
        }
    }
    return 0;
}

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
) {

    Point block_pos = BlockDb_getBlockPosition(self, block_id);
    int block_size = BlockDb_getBlockSize(self, block_id);

    Point topleft = {
        .x=grid_topleft.x + cell_width * (block_pos.x - block_size / 2),
        .y=grid_topleft.y + cell_height * (block_pos.y - block_size / 2)
    };

    return BlockDb_drawBlock(self, block_id, rend, topleft, cell_width, cell_height);
}

