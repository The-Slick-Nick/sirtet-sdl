
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
#include <assert.h>
#include <time.h>


#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1080

#define GRID_WIDTH 10
#define GRID_HEIGHT 24

#define TARGET_FPS 60


// Game updating portion of main loop
int updateGame(
    bool *gamecode_states, BlockIds *block_repo, Block *primary_block, long* block_presets, int num_presets,
    GameGrid *game_grid, bool *god_mode, int *move_counter
) {


    // new block time baby
    if (primary_block->id == INVALID_BLOCK_ID) {
        int new_id = BlockIds_provisionId(block_repo, 4);
        assert(new_id != INVALID_BLOCK_ID);

        long new_contents = block_presets[(rand() + num_presets) % num_presets];
        *primary_block = (Block){
            .id=new_id,
            .position=(Point){.x=5, .y=5},
            .contents=new_contents,
            .size=4
        };

        if (!GameGrid_canBlockExist(game_grid, primary_block)) {
            printf("Game over!\n");
            return -1;
        }

        printf("New block id is %d\n", primary_block->id);
        printf("New contents representation is %ld\n", new_contents);
    }

    if (Gamecode_pressed(gamecode_states, GAMECODE_ROTATE)) {
        printf("Rotation!\n");
        if (GameGrid_canBlockInfoExist(
                game_grid, 4, rotateBlockContentsCw90(primary_block->contents, 4), primary_block->position
        )) {

            primary_block->contents = rotateBlockContentsCw90(primary_block->contents, 4);
        }
    }

    if (Gamecode_pressed(gamecode_states, GAMECODE_MOVE_LEFT)) {
        if (
            GameGrid_canBlockInfoExist(
                game_grid, 4, primary_block->contents,
                Point_translate(primary_block->position, (Point){.x=-1, .y=0})
            )
        ) {
            Block_translate(primary_block, (Point){.x=-1, .y=0});
        }
    }

    if (Gamecode_pressed(gamecode_states, GAMECODE_MOVE_RIGHT)) {
        if (
            GameGrid_canBlockInfoExist(
                game_grid, 4, primary_block->contents,
                Point_translate(primary_block->position, (Point){.x=1, .y=0})
            )
        ) {
            Block_translate(primary_block, (Point){.x=1, .y=0});
        }
    }

    if (!(*god_mode)) {
        (*move_counter)++;
    }

    if ( Gamecode_pressed(gamecode_states, GAMECODE_MOVE_DOWN) || *move_counter > 1000 ) {
        *move_counter = 0;

        Point down_translation = (Point){.x=0, .y=1};
        Point up_translation = (Point){.x=0, .y=-1};
        Block projected_block;

        projected_block = (Block){
            .size=primary_block->size,
            .contents=primary_block->contents,
            .id=-1,
            .position=(Point){.x=primary_block->position.x, .y=primary_block->position.y + 1}
        };

        if (GameGrid_canBlockExist(game_grid, &projected_block)) {
            Block_translate(primary_block, down_translation);
        }
        else {
            GameGrid_commitBlock(game_grid, primary_block);
            primary_block->id = INVALID_BLOCK_ID;
        }
    }

    GameGrid_resolveRows(game_grid, block_repo);
    return 0;


}

int run() {


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


    /*** Game object initialization ***/
    bool god_mode = false;

    const int grid_draw_height = (3 * WINDOW_HEIGHT) / 4;
    const int cell_size =  grid_draw_height / GRID_HEIGHT;

    const int grid_draw_width = GRID_WIDTH * cell_size;
    SDL_Rect draw_window = { .x=10, .y=10, .w=grid_draw_width, .h=grid_draw_height };


    const int num_presets = 7;
    long block_presets[7] = {
        0b0100010001000100,
        0b0000011001100000,
        0b0100010001100000,
        0b0010001001100000,
        0b0000010011100000,
        0b0011011000000000,
        0b1100011000000000
    };

    int grid_contents[GRID_WIDTH * GRID_HEIGHT] = {-1};
    GameGrid ref_grid = { .width=GRID_WIDTH, .height=GRID_HEIGHT, .contents=grid_contents };
    GameGrid_clear(&ref_grid);

    int all_ids[256] = {0};
    BlockIds id_repo = {
        .head = 0,
        .max_ids = 256,
        .id_array = all_ids
    };

    Block primary_block = { .id=INVALID_BLOCK_ID, .size=4 };

    /*** Key Mapping & input code setups ***/

    int hardware_states[(int)SDL_NUM_SCANCODES] = {INT_MIN};
    bool gamecode_states[(int)NUM_GAMECODES] = {false};

    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }
    for (int i = 0; i < (int)NUM_GAMECODES; i++) { gamecode_states[i] = false; }

    GamecodeMap keymap = {.head=0};
    Gamecode_addMap(&keymap, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(&keymap, GAMECODE_ROTATE, SDL_SCANCODE_UP, 1, 1, 1);
    Gamecode_addMap(&keymap, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(&keymap, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, 100);
    Gamecode_addMap(&keymap, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, 100);
    Gamecode_addMap(&keymap, GAMECODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, INT_MAX, 100);

    /*** Main Loop ***/

    int move_counter = 0;
    int frames_processed = 0;
    while (true) {

        clock_t frame_start = clock();

        /***** PROCESS INPUTS *****/
        processHardwareInputs(hardware_states);
        processGamecodes(gamecode_states, hardware_states, &keymap);

        /***** UPDATE *****/
        if (Gamecode_pressed(gamecode_states, GAMECODE_QUIT)) {
            printf("Quitting...\n");
            break;
        }

        if (hardware_states[SDL_SCANCODE_G] == 1) {
            god_mode = (god_mode == false);
            printf("God mode toggled\n");
        }

        if (
            updateGame(gamecode_states, &id_repo, &primary_block, block_presets, num_presets, &ref_grid, &god_mode, &move_counter) != 0
        ) {
            return 0;
        }


        /***** DRAW *****/

        SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
        SDL_RenderClear(rend);

        if (primary_block.id != INVALID_BLOCK_ID) {
            drawBlock(rend, draw_window, &primary_block, &ref_grid);
        }

        drawGrid(rend, draw_window, &ref_grid);

        SDL_RenderPresent(rend);

        // Manual keymapping print-out here (provide as a gamecode or create separate
        // menu gamecode structs?)
        if (hardware_states[SDL_SCANCODE_D] > 0 && (hardware_states[SDL_SCANCODE_D] - 1) % 1000 == 0) {
            printf("Frame time: %f seconds\n", ((double)(clock() - frame_start) / CLOCKS_PER_SEC));
        }
    }


    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}
