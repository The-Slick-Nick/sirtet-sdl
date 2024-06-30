/* game_state.c
*
* Declares logic and whatnot for handling the primary game state
* The main game logic is found here
*
* State follows a (as of yet to be actually written) standard
* for a state-runner to run
*/

#include <limits.h>
#include <assert.h>

#include "game_state.h"
#include "application_state.h"
#include "component_drawing.h"

/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

// Create & initialize a GameState struct, returning
// it as a void pointer (for state-runner compatability).
//
// Assumes that GameState_deconstruct() is eventually called
// on the return value
void* GameState_init() {

    /*** Supplementary data ***/

    long preset_prototypes[7] = {
        0b0100010001000100,
        0b0000011001100000,
        0b0100010001100000,
        0b0010001001100000,
        0b0000010011100000,
        0b0011011000000000,
        0b1100011000000000
    };

    long* block_presets = (long*)malloc(7 * sizeof(long));
    memcpy(block_presets, preset_prototypes, 7 * sizeof(long));

    int* all_ids = (int*)calloc(256, sizeof(long));

    // TODO replace the 720 with whatever the configured window height is
    const int grid_draw_height = (3 * 720) / 4;
    const int cell_size =  grid_draw_height / GRID_HEIGHT;
    const int grid_draw_width = GRID_WIDTH * cell_size;

    bool *gamecode_states = (bool*)calloc((int)NUM_GAMECODES, sizeof(bool));
    int *grid_contents = (int*)malloc(GRID_WIDTH * GRID_HEIGHT * sizeof(int));

    memset(grid_contents, -1, sizeof(int));


    /*** Initialize struct ***/

    GameState *retval = (GameState*)malloc(sizeof(GameState));
    *(retval) = (GameState){
        .move_counter=0,
        .god_mode=false,
        .num_presets=7,
        .block_presets=block_presets,

        .game_grid = (GameGrid){.width=GRID_WIDTH, .height=GRID_HEIGHT, .contents=grid_contents},
        .primary_block = (Block){.id=INVALID_BLOCK_ID, .size=4},
        .block_ids = (BlockIds){.head=0, .max_ids=256, .id_array=all_ids},
        .keymaps=(GamecodeMap){.head=0},
        .draw_window=(SDL_Rect){ .x=10, .y=10, .w=grid_draw_width, .h=grid_draw_height },

        .gamecode_states=gamecode_states
    };


    /*** Post-creation processing ***/
    Gamecode_addMap(&retval->keymaps, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(&retval->keymaps, GAMECODE_ROTATE, SDL_SCANCODE_UP, 1, 1, 1);
    Gamecode_addMap(&retval->keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(&retval->keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, 100);
    Gamecode_addMap(&retval->keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, 100);
    Gamecode_addMap(&retval->keymaps, GAMECODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, INT_MAX, 100);

    GameGrid_clear(&retval->game_grid);


    return (void*)retval;
}

// Go through process of deconstructing a GameState struct,
// freeing any memory allocated in _init() call
//
// Takes a void* pointer that is recast to GameState* for compatability
// with state runner
int GameState_deconstruct(void* self) {
    GameState *game_state = (GameState*)self;

    free(game_state->block_presets);
    free(game_state->block_ids.id_array);
    free(game_state->gamecode_states);
    free(game_state->game_grid.contents);

    free(self);
    return 0;
}


/*=============================================================================
 State handling
=============================================================================*/

// Run a single frame of game
// "state-runner function" for GameState
int runGameFrame(void *global_state_data, void *state_data) {

    /* Recasting */
    ApplicationState *global_state = (ApplicationState*)global_state_data;
    GameState *game_state = (GameState*)state_data;

    /* Relevant variable extraction */
    SDL_Renderer *rend = global_state->rend;
    int *hardware_states = global_state->hardware_states;

    SDL_Rect draw_window = game_state->draw_window;


    /***** PROCESS INPUTS *****/
    processGamecodes(game_state->gamecode_states, hardware_states, &game_state->keymaps);

    /***** UPDATE *****/
    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_QUIT)) {
        printf("Quitting...\n");
        return -1;
    }

    if (hardware_states[SDL_SCANCODE_G] == 1) {
        game_state->god_mode = (game_state->god_mode == false);
        printf("God mode toggled\n");
    }

    if ( updateGame(game_state) != 0 ) {
        return -1;
    }

    /***** DRAW *****/

    SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
    SDL_RenderClear(rend);

    if (game_state->primary_block.id != INVALID_BLOCK_ID) {
        drawBlock(rend, draw_window, &game_state->primary_block, &game_state->game_grid);
    }

    drawGrid(rend, draw_window, &game_state->game_grid);

    SDL_RenderPresent(rend);
    return 0;
}

// Update portion of main game loop
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

    if (!game_state->god_mode) {
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


