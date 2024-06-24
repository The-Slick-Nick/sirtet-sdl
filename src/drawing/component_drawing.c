
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
#include "../components/coordinates.h"
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

