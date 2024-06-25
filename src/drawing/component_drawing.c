
/* component_drawing.h
*
* Source file to handle the drawing of any components
* (blocks, grid, etc.)
*/

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>

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

    SDL_SetRenderDrawColor(rend, rim_color.r, rim_color.g, rim_color.g, rim_color.a);
    SDL_RenderFillRect(rend, &full_box);

    SDL_SetRenderDrawColor(rend, body_color.r, body_color.g, body_color.g, body_color.a);
    SDL_RenderFillRect(rend, &inner_box);

    return 0;
}

SDL_Color getCellColorById(int block_id) {
    return (SDL_Color){.r=155, .g=0, .b=0, .a=255 };
}

int drawGrid(
    SDL_Renderer *rend,
    SDL_Rect display_window,
    GameGrid *grid
) {

    int cell_width = display_window.w / grid->width;
    int cell_height = display_window.h / grid->height;

    for (int row = 0; row < grid->height; row++) {
        for (int col = 0; col < grid->width; col++) {

            int cell_idx = col + (row * grid->width);
            int cell_id = *(grid->contents + cell_idx);

            // Standard solution: invalid block IDs are negative
            if (cell_id < 0) {
                continue;
            }
            drawBlockCell(
                rend, 
                (Point){.x=display_window.x + (col * cell_width), .y=display_window.y + (row * cell_height) },
                cell_width,
                cell_height,
                getCellColorById(cell_id),
                getCellColorById(cell_id) // TODO Introduce some kind of scaling later
            );
         }
    }

    return 0;
}

