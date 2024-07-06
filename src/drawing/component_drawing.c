
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
#include "draw_config.h"


int divround(int num, int denom) {

    return num / denom;

    int retval = num / denom;

    if (num % denom > num / 2) {
        return retval + 1;
    }
    return retval;
}

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
        .r=12*base_color.r / 10,
        .g=12*base_color.g / 10,
        .b =12* base_color.b / 10,
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

int drawGrid(
    SDL_Renderer *rend,
    SDL_Rect display_window,
    GameGrid *grid
) {

    int cell_width = display_window.w / grid->width;
    int cell_height = display_window.h / grid->height;

    SDL_Color body_color;

    for (int row = 0; row < grid->height; row++) {
        for (int col = 0; col < grid->width; col++) {

            int cell_idx = col + (row * grid->width);
            int cell_id = *(grid->contents + cell_idx);

            // Standard solution: invalid block IDs are negative
            if (cell_id < 0) {
                continue;
            }

            body_color = getCellColorById(cell_id);

            drawBlockCell(
                rend, 
                (Point){.x=display_window.x + (col * cell_width), .y=display_window.y + (row * cell_height) },
                cell_width,
                cell_height,
                body_color
            );
         }
    }
    return 0;
}


int drawBlock(
    SDL_Renderer *rend,
    SDL_Rect display_window,
    BlockDb *block_db,
    int block_id,
    GameGrid *ref_grid
) {

    int cell_width = display_window.w / ref_grid->width;
    int cell_height = display_window.h / ref_grid->height;


    // TODO: Change this (once block refactor complete)
    SDL_Color base_color = getCellColorById(block_id);

    int block_size = BlockDb_getBlockSize(block_db, block_id);
    for (int bit_num = 0; bit_num < block_size * block_size; bit_num++) {

        if (BlockDb_isContentBitSet(block_db, block_id, bit_num)) {

            Point block_coords = blockContentBitToGridCoords(bit_num, block_size, BlockDb_getBlockPosition(block_db, block_id));
            drawBlockCell(
                rend, 
                (Point){
                     .x=display_window.x + (cell_width * block_coords.x),
                     .y=display_window.y + (cell_height * block_coords.y)
                },
                cell_width,
                cell_height,
                base_color
            );
        }
    }
    return 0;
}

