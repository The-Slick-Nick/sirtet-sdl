
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

#include "../components/block.h"
#include "coordinates.h"
#include "grid.h"
#include "draw_config.h"



// Lowest-level unit of "draw game component"
int drawBlockCell(
    SDL_Renderer *rend,
    Point location, int width, int height, SDL_Color body_color, SDL_Color rim_color
) {


    /* Draw the full box as the background color, then overwrite the middle
    * with a smaller rectange of the body color */
    SDL_Rect full_box = {.x=location.x, .y=location.y, .w=width, .h=height};
    SDL_Rect inner_box = {
        .x=location.x + (width / 10),
        .y=location.y + (height / 10),
        .w=(8*width) / 10,
        .h=(8*height) / 10
    };

    SDL_SetRenderDrawColor(rend, rim_color.r, rim_color.g, rim_color.b, rim_color.a);

    SDL_RenderFillRect(rend, &full_box);

    SDL_SetRenderDrawColor(rend, body_color.r, body_color.g, body_color.b, body_color.a);
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
    SDL_Color rim_color;


    for (int row = 0; row < grid->height; row++) {
        for (int col = 0; col < grid->width; col++) {

            int cell_idx = col + (row * grid->width);
            int cell_id = *(grid->contents + cell_idx);

            // Standard solution: invalid block IDs are negative
            if (cell_id < 0) {
                continue;
            }

            body_color = getCellColorById(cell_id);
            rim_color = (SDL_Color) {
                .r=(8 * body_color.r) / 10,
                .g=(8 * body_color.g) / 10,
                .b=(8 * body_color.b) / 10,
                .a=255
            };

            drawBlockCell(
                rend, 
                (Point){.x=display_window.x + (col * cell_width), .y=display_window.y + (row * cell_height) },
                cell_width,
                cell_height,
                body_color,
                rim_color
            );
         }
    }

    return 0;
}


// Draw a given block to the appropriate renderer.
int drawBlock(
    SDL_Renderer *rend,
    SDL_Rect display_window,
    Block *block,
    GameGrid *ref_grid
) {

    int cell_width = display_window.w / ref_grid->width;
    int cell_height = display_window.h / ref_grid->height;

    SDL_Color body_color = getCellColorById(block->id);
    SDL_Color rim_color = (SDL_Color) {
        .r=(8 * body_color.r) / 10,
        .g=(8 * body_color.g) / 10,
        .b=(8 * body_color.b) / 10,
        .a=255
    };

    for (int bit_num = 0; bit_num < (block->size * block->size); bit_num++) {
        if (Block_isContentBitSet(block, bit_num)) {

            Point block_coords = blockContentBitToGridCoords(bit_num, block->size, block->position);

            drawBlockCell(
                rend,
                (Point){
                     .x=display_window.x + (cell_width * block_coords.x),
                     .y=display_window.y + (cell_height * block_coords.y)
                },
                cell_width,
                cell_height,
                body_color,
                rim_color 
            );

        }
    }

    return 0;
}

