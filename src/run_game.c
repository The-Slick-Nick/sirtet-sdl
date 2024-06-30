
#include "block.h"
#include "component_drawing.h"
#include "coordinates.h"
#include "grid.h"
#include "inputs.h"
#include "run_game.h"

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


int updateGame(GameState *game_state) {

    // new block time baby
    if (game_state->primary_block.id == INVALID_BLOCK_ID) {
        int new_id = BlockIds_provisionId(&game_state->block_ids, game_state->primary_block.size);
        assert(new_id != INVALID_BLOCK_ID);

        long new_contents = game_state->block_presets[(rand() + game_state->num_presets) % game_state->num_presets];
        game_state->primary_block = (Block){
            .id=new_id,
            .position=(Point){.x=5, .y=5},
            .contents=new_contents,
            .size=4
        };


        if (!GameGrid_canBlockExist(&game_state->game_grid, &game_state->primary_block)) {
            printf("Game over!\n");
            return -1;
        }

        printf("New block id is %d\n", game_state->primary_block.id);
        printf("New contents representation is %ld\n", new_contents);
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_ROTATE)) {
        printf("Rotation!\n");
        if (GameGrid_canBlockInfoExist(
                &game_state->game_grid, 4, rotateBlockContentsCw90(game_state->primary_block.contents, 4), game_state->primary_block.position
        )) {

            game_state->primary_block.contents = rotateBlockContentsCw90(game_state->primary_block.contents, 4);
        }
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_LEFT)) {
        if (
            GameGrid_canBlockInfoExist(
                &game_state->game_grid, 4, game_state->primary_block.contents,
                Point_translate(game_state->primary_block.position, (Point){.x=-1, .y=0})
            )
        ) {
            Block_translate(&game_state->primary_block, (Point){.x=-1, .y=0});
        }
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_RIGHT)) {
        if (
            GameGrid_canBlockInfoExist(
                &game_state->game_grid, 4, game_state->primary_block.contents,
                Point_translate(game_state->primary_block.position, (Point){.x=1, .y=0})
            )
        ) {
            Block_translate(&game_state->primary_block, (Point){.x=1, .y=0});
        }
    }

    if (game_state->god_mode) {
        game_state->move_counter++;
    }

    if ( Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_DOWN) || game_state->move_counter > 1000 ) {
        game_state->move_counter = 0;

        Point down_translation = (Point){.x=0, .y=1};
        Point up_translation = (Point){.x=0, .y=-1};
        Block projected_block;

        projected_block = (Block){
            .size=game_state->primary_block.size,
            .contents=game_state->primary_block.contents,
            .id=-1,
            .position=(Point){.x=game_state->primary_block.position.x, .y=game_state->primary_block.position.y + 1}
        };

        if (GameGrid_canBlockExist(&game_state->game_grid, &projected_block)) {
            Block_translate(&game_state->primary_block, down_translation);
        }
        else {
            GameGrid_commitBlock(&game_state->game_grid, &game_state->primary_block);
            game_state->primary_block.id = INVALID_BLOCK_ID;
        }
    }

    GameGrid_resolveRows(&game_state->game_grid, &game_state->block_ids);
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

    /*** Key Mapping & input code setups ***/

    int hardware_states[(int)SDL_NUM_SCANCODES] = {INT_MIN};
    bool gamecode_states[(int)NUM_GAMECODES] = {false};

    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }
    for (int i = 0; i < (int)NUM_GAMECODES; i++) { gamecode_states[i] = false; }


    /*** Game object initialization ***/

    // Initialize game config & state

    int all_ids[256] = {0};
    int grid_contents[GRID_WIDTH * GRID_HEIGHT] = {-1};
    int block_ids[256] = {0};

    long block_presets[7] = {
        0b0100010001000100,
        0b0000011001100000,
        0b0100010001100000,
        0b0010001001100000,
        0b0000010011100000,
        0b0011011000000000,
        0b1100011000000000
    };

    GameState game_state = {
        // Native type
        .move_counter = 0,
        .god_mode = false,
        .num_presets=7,
        .block_presets=block_presets,

        // Structs to be pointed to
        .game_grid = (GameGrid){.width=GRID_WIDTH, .height=GRID_HEIGHT, .contents=grid_contents},
        .primary_block = (Block){.id=INVALID_BLOCK_ID, .size=4},
        .block_ids = (BlockIds){.head=0, .max_ids=256, .id_array=all_ids},
        .keymaps=(GamecodeMap){.head=0},


        // Arrays
        .gamecode_states=gamecode_states,
    };

    Gamecode_addMap(&game_state.keymaps, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(&game_state.keymaps, GAMECODE_ROTATE, SDL_SCANCODE_UP, 1, 1, 1);
    Gamecode_addMap(&game_state.keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(&game_state.keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, 100);
    Gamecode_addMap(&game_state.keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, 100);
    Gamecode_addMap(&game_state.keymaps, GAMECODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, INT_MAX, 100);

    GameGrid_clear(&game_state.game_grid);

    const int grid_draw_height = (3 * WINDOW_HEIGHT) / 4;
    const int cell_size =  grid_draw_height / GRID_HEIGHT;

    const int grid_draw_width = GRID_WIDTH * cell_size;
    SDL_Rect draw_window = { .x=10, .y=10, .w=grid_draw_width, .h=grid_draw_height };



    /*** Main Loop ***/

    int frames_processed = 0;
    while (true) {

        clock_t frame_start = clock();

        /***** PROCESS INPUTS *****/
        processHardwareInputs(hardware_states);
        processGamecodes(gamecode_states, hardware_states, &game_state.keymaps);

        /***** UPDATE *****/
        if (Gamecode_pressed(gamecode_states, GAMECODE_QUIT)) {
            printf("Quitting...\n");
            break;
        }

        if (hardware_states[SDL_SCANCODE_G] == 1) {
            game_state.god_mode = (game_state.god_mode == false);
            printf("God mode toggled\n");
        }

        if ( updateGame(&game_state) != 0 ) {
            return 0;
        }

        /***** DRAW *****/

        SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
        SDL_RenderClear(rend);

        if (game_state.primary_block.id != INVALID_BLOCK_ID) {
            drawBlock(rend, draw_window, &game_state.primary_block, &game_state.game_grid);
        }

        drawGrid(rend, draw_window, &game_state.game_grid);

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
