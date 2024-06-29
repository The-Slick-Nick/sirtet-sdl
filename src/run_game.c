
#include "block.h"
#include "component_drawing.h"
#include "coordinates.h"
#include "grid.h"
#include "inputs.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>
#include <limits.h>

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1080

#define GRID_WIDTH 100
#define GRID_HEIGHT 100

int run(){

    SDL_Init(SDL_INIT_VIDEO);

    /*** SDL & draw initialization ***/
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

    SDL_Rect draw_window = { .x=10, .y=10, .w=400, .h=400 };

    /*** Game object initialization ***/

    int grid_contents[GRID_WIDTH * GRID_HEIGHT] = {-1};
    GameGrid ref_grid = { .width=GRID_WIDTH, .height=GRID_HEIGHT, .contents=grid_contents };
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
        .contents=0b0100010001000100,
        .size=4
    };


    /*** Key Mapping & input code setups ***/

    int hardware_states[(int)SDL_NUM_SCANCODES] = {INT_MIN};
    bool gamecode_states[(int)NUM_GAMECODES] = {false};

    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }
    for (int i = 0; i < (int)NUM_GAMECODES; i++) { gamecode_states[i] = false; }

    GamecodeMap keymap = {.head=0};
    Gamecode_addMap(&keymap, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(&keymap, GAMECODE_ROTATE, SDL_SCANCODE_UP, 1, 1, 1);
    Gamecode_addMap(&keymap, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(&keymap, GAMECODE_SPEEDUP, SDL_SCANCODE_DOWN, 1, INT_MAX, 1);
    Gamecode_addMap(&keymap, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, 100);
    Gamecode_addMap(&keymap, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, 100);

    /*** Main Loop ***/

    Uint64 start_time = SDL_GetTicks64();
    const Uint64 ten_seconds_ms = 10 * 1000;
    int move_counter = 0;

    while (SDL_GetTicks64() - start_time < 60 * ten_seconds_ms) {


        /***** PROCESS INPUTS *****/
        processHardwareInputs(hardware_states);
        processGamecodes(gamecode_states, hardware_states, &keymap);

        /***** UPDATE *****/

        if (Gamecode_pressed(gamecode_states, GAMECODE_QUIT)) {
            printf("Quitting...\n");
            break;
        }

        if (Gamecode_pressed(gamecode_states, GAMECODE_ROTATE)) {
            printf("Rotation!\n");
            if (GameGrid_canBlockInfoExist(
                    &ref_grid, 4, rotateBlockContentsCw90(primary_block.contents, 4), primary_block.position
            )) {

                primary_block.contents = rotateBlockContentsCw90(primary_block.contents, 4);
            }
        }

        if (Gamecode_pressed(gamecode_states, GAMECODE_MOVE_LEFT)) {
            printf("Move left!\n");
            if (
                GameGrid_canBlockInfoExist(
                    &ref_grid, 4, primary_block.contents,
                    Point_translate(primary_block.position, (Point){.x=-1, .y=0})
                )
            ) {
                Block_translate(&primary_block, (Point){.x=-1, .y=0});
            }
        }

        if (Gamecode_pressed(gamecode_states, GAMECODE_MOVE_RIGHT)) {
            printf("Move right!\n");
            if (
                GameGrid_canBlockInfoExist(
                    &ref_grid, 4, primary_block.contents,
                    Point_translate(primary_block.position, (Point){.x=1, .y=0})
                )
            ) {
                Block_translate(&primary_block, (Point){.x=1, .y=0});
            }
        }


        if (Gamecode_pressed(gamecode_states, GAMECODE_SPEEDUP)) {
            move_counter += 10;
        } 
        else {
            move_counter++;
        }

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


        /***** DRAW *****/

        SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
        SDL_RenderClear(rend);
        drawBlock(rend, draw_window, &primary_block, &ref_grid);
        drawGrid(rend, draw_window, &ref_grid);

        SDL_RenderPresent(rend);
    }


    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}
