#include "block.h"
#include "component_drawing.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1080


int main(int argc, char* argv[]) {


    // standard stuff
    SDL_Window *wind = SDL_CreateWindow(
        "Test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0
    );

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);

    if (!wind || !rend) {
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return -1;
    }

    Block primary_block = {
        .id=1,
        .position=(Point){.x=2, .y=2},
        .contents=0b0000011001100000,
        .size=4
    };

    SDL_Rect draw_window = {
        .x=10,
        .y=10,
        .w=400,
        .h=400
    };


    int grid_contents[36] = {-1};
    GameGrid ref_grid = {
        .width=6,
        .height=6,
        .contents=grid_contents
    };


    Uint64 start_time = SDL_GetTicks64();
    const Uint64 ten_seconds_ms = 10 * 1000;
    while (SDL_GetTicks64() - start_time < ten_seconds_ms) {


        SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
        SDL_RenderClear(rend);


        // drawBlock(SDL_Renderer *rend, SDL_Rect display_window, Block *block, GameGrid *ref_grid)
        drawBlock(rend, draw_window, &primary_block, &ref_grid);

        // drawBlockCell(
        //     rend,
        //     (Point){.x=10, .y=10},
        //     100, 100,
        //     (SDL_Color){.r=100, .g=0, .b=0, .a=0},
        //     (SDL_Color){.r=0, .g=0, .b=100, .a=0}
        // );

        SDL_RenderPresent(rend);
    }

    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}
