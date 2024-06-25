#include "block.h"
#include "component_drawing.h"
#include "coordinates.h"
#include "grid.h"
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


    SDL_Rect draw_window = {
        .x=10,
        .y=10,
        .w=400,
        .h=400
    };


    int grid_contents[100] = {-1};
    GameGrid ref_grid = {
        .width=10,
        .height=10,
        .contents=grid_contents
    };
    GameGrid_clear(&ref_grid);

    int all_ids[256] = {0};
    BlockIds id_repo = {
        .head = 0,
        .max_ids = 256,
        .id_array = all_ids
    };

    Block primary_block = {
        .id=BlockIds_provisionId(&id_repo, 4),
        .position=(Point){.x=5, .y=5},
        .contents=0b0000011001100000,
        .size=4
    };


    Uint64 start_time = SDL_GetTicks64();
    const Uint64 ten_seconds_ms = 10 * 1000;

    int move_counter = 0;

    while (SDL_GetTicks64() - start_time < ten_seconds_ms) {


        SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
        SDL_RenderClear(rend);


        if (move_counter > 1000) {
            move_counter = 0;


            Point down_translation = (Point){.x=0, .y=1};
            Point up_translation = (Point){.x=0, .y=-1};
            Block projected_block;

            projected_block = (Block){
                .size=primary_block.size,
                .contents=primary_block.contents,
                .id=-1,
                .position=(Point){.x=primary_block.position.x, .y=primary_block.position.y + 1}
            };

            if (GameGrid_canBlockExist(&ref_grid, &projected_block)) {
                Block_translate(&primary_block, down_translation);
            }
            else {
                GameGrid_commitBlock(&ref_grid, &primary_block);
                primary_block = (Block){
                    .id=BlockIds_provisionId(&id_repo, 4),
                    .position=(Point){.x=5, .y=5},
                    .contents=0b0000011001100000,
                    .size=4
                };
                printf("New block id is %d\n", primary_block.id);
            }
        }


        // drawBlock(SDL_Renderer *rend, SDL_Rect display_window, Block *block, GameGrid *ref_grid)
        drawBlock(rend, draw_window, &primary_block, &ref_grid);
        drawGrid(rend, draw_window, &ref_grid);


        SDL_RenderPresent(rend);
        move_counter++;
    }


    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}
