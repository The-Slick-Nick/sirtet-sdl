/* game_state.c
*
* Declares logic and whatnot for handling the primary game state
* The main game logic is found here
*
* State follows a (as of yet to be actually written) standard
* for a state-runner to run
*/

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>

#include "grid.h"
#include "block.h"
#include "game_state.h"
#include "application_state.h"
#include "component_drawing.h"
#include "inputs.h"
#include "state_runner.h"


// For dimension calculations
#define GAMEAREA_WEIGHT_W 4
#define SIDEBAR_WEIGHT_W 1

#define TOTAL_WEIGHT_W (GAMEAREA_WEIGHT_W + SIDEBAR_WEIGHT_W)

/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

// Create & initialize a GameState struct, returning
// it as a void pointer (for state-runner compatability).
//
// Assumes that GameState_deconstruct() is eventually called
// on the return value
GameState* GameState_init(ApplicationState *app_state) {


    // NOTE: I'm on the fence about passing the whole ApplicationState
    // into GameState constructor... it's more convenient, but does it too
    // tightly couple things?


    SDL_Renderer *rend = app_state->rend;
    TTF_Font *menu_font = app_state->menu_font;

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

    int window_h;
    int window_w;
    SDL_GetWindowSize(app_state->wind, &window_w, &window_h);

    const int grid_draw_height = (3 * window_h) / 4;
    const int cell_size =  grid_draw_height / GRID_HEIGHT;
    const int grid_draw_width = GRID_WIDTH * cell_size;

    SDL_Surface *surf = TTF_RenderText_Solid(menu_font, "Paused", (SDL_Color){255, 255, 255});
    SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);  // no longer needed


    // Maybe this moves into the actual draw portion?
    SDL_Rect dstrect = {.x=10, .y=10};
    SDL_QueryTexture(texture, NULL, NULL, &dstrect.w, &dstrect.h);


    /*** Initialize struct ***/
    GameState *retval = (GameState*)malloc(sizeof(GameState));
    *(retval) = (GameState){
        // single values 
        .move_counter=0,
        .score=0,
        .level=app_state->init_level,
        .num_presets=7,

        .primary_block = INVALID_BLOCK_ID,
        .queued_block = INVALID_BLOCK_ID,

        // structs and arrays
        .block_presets=(long*)malloc(7 * sizeof(long)),

        .block_db = BlockDb_init(256),

        .game_grid = GameGrid_init(GRID_WIDTH, GRID_HEIGHT),

        .keymaps = GamecodeMap_init(MAX_GAMECODE_MAPS),

        .gamecode_states=(bool*)calloc((int)NUM_GAMECODES, sizeof(bool)),

        .pause_texture=texture,
        .score_label=NULL,
        .level_label=NULL

    };

    /*** Post-creation processing ***/

    // Initialize block presets
    memcpy(retval->block_presets, preset_prototypes, 7 * sizeof(long));

    // Initialize grid cells
    GameGrid_clear(retval->game_grid);

    // Add some key mappings
    int move_cd = TARGET_FPS / 15;
    Gamecode_addMap(retval->keymaps, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(retval->keymaps, GAMECODE_ROTATE, SDL_SCANCODE_UP, 1, 1, 1);
    Gamecode_addMap(retval->keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(retval->keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, move_cd);
    Gamecode_addMap(retval->keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, move_cd);
    Gamecode_addMap(retval->keymaps, GAMECODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, INT_MAX, move_cd);
    Gamecode_addMap(retval->keymaps, GAMECODE_PAUSE, SDL_SCANCODE_P, 1, 1, 1);


    printf("Returning game state...\n");
    return retval;
}

// Go through process of deconstructing a GameState struct,
// freeing any memory allocated in _init() call
//
// Takes a void* pointer that is recast to GameState* for compatability
// with state runner
int GameState_deconstruct(void* self) {
    GameState *game_state = (GameState*)self;


    BlockDb_deconstruct(game_state->block_db);
    GameGrid_deconstruct(game_state->game_grid);

    free(game_state->block_presets);
    free(game_state->gamecode_states);

    GamecodeMap_deconstruct(game_state->keymaps);

    SDL_DestroyTexture(game_state->pause_texture);

    free(self);

    return 0;
}


/*=============================================================================
 * Logical components 
=============================================================================*/

// Update portion of main game loop
StateFuncStatus updateGame(StateRunner *state_runner, GameState *game_state) {

    // relevant variable extraction - for shorthand (
    // and to save my fingers from typing a lot)
    BlockDb *db = game_state->block_db;
    GameGrid *grid = game_state->game_grid;
    int *primary_block = &game_state->primary_block;
    int *queued_block = &game_state->queued_block;
    long *block_presets = game_state->block_presets;


    // TODO: Incorporate this into game_state later
    SDL_Color pallette[7] = {
        (SDL_Color){155, 0, 0},
        (SDL_Color){155, 155, 0},
        (SDL_Color){155, 0, 155},
        (SDL_Color){155, 155, 155},
        (SDL_Color){0, 155, 0},
        (SDL_Color){0, 155, 155},
        (SDL_Color){0, 0, 155}
    };

    // Must clear first due to animation timing
    GameGrid_resolveRows(grid, db);

    int rand_idx;
    long new_contents;

    if (*queued_block == INVALID_BLOCK_ID) {

        rand_idx = ((rand() + game_state->num_presets) % game_state->num_presets);
        long new_contents = block_presets[rand_idx];

        *queued_block = BlockDb_createBlock(
            db, 4, new_contents, (Point){0, 0}, pallette[rand_idx]
        );
    }

    if (*primary_block == INVALID_BLOCK_ID) {

        *primary_block = *queued_block;

        rand_idx = ((rand() + game_state->num_presets) % game_state->num_presets);
        long new_contents = block_presets[rand_idx];

        *queued_block = BlockDb_createBlock(
            db, 4, new_contents, (Point){0, 0}, pallette[rand_idx]
        );

        if (*primary_block == INVALID_BLOCK_ID || *queued_block == INVALID_BLOCK_ID) {
            return STATEFUNC_ERROR;
        }

        BlockDb_setBlockPosition(db, *primary_block, (Point){5, 5});

        if (!GameGrid_canBlockExist(grid, db, *primary_block)) {
            printf("Game over!\n");
            return STATEFUNC_QUIT;
        }

        printf("New block id is %d\n", game_state->primary_block);
        printf("New contents representation is %ld\n", new_contents);
    }


    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_ROTATE)) {

        long rotated_contents = rotateBlockContentsCw90(
            BlockDb_getBlockContents(db, *primary_block),
            BlockDb_getBlockSize(db, *primary_block)
        );

        if (GameGrid_canBlockInfoExist(
                grid,
                BlockDb_getBlockSize(db, *primary_block),
                rotated_contents,
                BlockDb_getBlockPosition(db, *primary_block)
            )
        ) {
            BlockDb_setBlockContents(db, *primary_block, rotated_contents);
        }
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_LEFT)) {

        Point new_pos = Point_translate(BlockDb_getBlockPosition(db, *primary_block), (Point){-1, 0});

        if (
            GameGrid_canBlockInfoExist(
                grid,
                BlockDb_getBlockSize(db, *primary_block),
                BlockDb_getBlockContents(db, *primary_block),
                new_pos
            )
        ) {
            BlockDb_setBlockPosition(db, *primary_block, new_pos);
        }
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_RIGHT)) {
        Point new_pos = Point_translate(BlockDb_getBlockPosition(db, *primary_block), (Point){1, 0});
        if (
            GameGrid_canBlockInfoExist(
                grid,
                BlockDb_getBlockSize(db, *primary_block),
                BlockDb_getBlockContents(db, *primary_block),
                new_pos
            )
        ) {
            BlockDb_setBlockPosition(db, *primary_block, new_pos);
        }
    }

    game_state->move_counter++;
    if (
        Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_DOWN)
        || game_state->move_counter > (TARGET_FPS / (1 + game_state->level))
    ) {
        game_state->move_counter = 0;

        Point new_pos = Point_translate(BlockDb_getBlockPosition(db, *primary_block), (Point){0, 1});

        if (GameGrid_canBlockInfoExist(grid, BlockDb_getBlockSize(db, *primary_block), BlockDb_getBlockContents(db, *primary_block), new_pos)) {

            BlockDb_setBlockPosition(db, *primary_block, new_pos);
        }
        else {
            GameGrid_commitBlock(grid, db, *primary_block);
            *primary_block = INVALID_BLOCK_ID;
        }
    }

    int to_inc = GameGrid_assessScore(grid, game_state->level);
    if (to_inc > 0) {
        SDL_DestroyTexture(game_state->score_label);
        game_state->score_label = NULL;
    }
    game_state->score += to_inc;

    GameGrid_prepareAnimation(grid, 3);
    if (grid->is_animating) {
        StateRunner_addState(
            state_runner, game_state, GameState_runGridAnimation, NULL
        );
    }
    return STATEFUNC_CONTINUE;
}


/*=============================================================================
 * Draw Components
=============================================================================*/

// Draw supplmental game info (Score, on deck, flair, etc.)
void drawInterface(ApplicationState *app_state, GameState *game_state) {

    // Convenience unpacking
    SDL_Renderer *rend = app_state->rend;
    TTF_Font *menu_font = app_state->menu_font;
    int score = game_state->score;
    int level = game_state->level;
    BlockDb *block_db = game_state->block_db;

    // helper vars
    char score_buffer[32];  // 32 is overkill but just in case...
    char level_buffer[16];  

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);

    const int sidebar_w = (SIDEBAR_WEIGHT_W * wind_w) / TOTAL_WEIGHT_W;
    const int sidebar_h = wind_h;
    const Point sidebar_origin = {
        .x=((GAMEAREA_WEIGHT_W * wind_w) / TOTAL_WEIGHT_W),
        .y=0
    };

    int yoffset = 0;

    // NOTE: Relies on updateGame(...) invalidating score texture on score change
    if (game_state->score_label == NULL) {
        snprintf(score_buffer, 32, "Score: %d", score);
        SDL_Surface *surf = TTF_RenderText_Solid(
            menu_font, score_buffer, (SDL_Color){255, 255, 255}
        );
        game_state->score_label = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
    }

    SDL_Rect dstrect = {.x=sidebar_origin.x, .y=sidebar_origin.y};
    SDL_QueryTexture(game_state->score_label, NULL, NULL, &dstrect.w, &dstrect.h);
    SDL_RenderCopy(rend, game_state->score_label, NULL, &dstrect);
    yoffset += dstrect.h;

    // NOTE: Relies on updateGame(...) invalidating level texture on level change
    if (game_state->level_label == NULL) {
        snprintf(level_buffer, 16, "Level: %d", level);
        SDL_Surface *lvl_surf = TTF_RenderText_Solid(
            menu_font, level_buffer, (SDL_Color){255, 255, 255}
        );
        game_state->level_label = SDL_CreateTextureFromSurface(rend, lvl_surf);
        SDL_FreeSurface(lvl_surf);
    }

    dstrect = (SDL_Rect){.x=sidebar_origin.x, .y=sidebar_origin.y + yoffset};
    SDL_QueryTexture(game_state->level_label, NULL, NULL, &dstrect.w, &dstrect.h);
    SDL_RenderCopy(rend, game_state->level_label, NULL, &dstrect);
    yoffset += dstrect.h;

    const int block_size = BlockDb_getBlockSize(game_state->block_db, game_state->queued_block);
    const int cell_size = sidebar_w / block_size;


    Point topleft = {.x=sidebar_origin.x, .y=sidebar_origin.y + yoffset};
    BlockDb_drawBlock(block_db, game_state->queued_block, rend, topleft, cell_size, cell_size);

}

// TODO: Return an integer status code
// Draw game area, including primary block and grid
void drawGameArea(ApplicationState *app_state, GameState *game_state, SDL_Rect draw_window) {

    SDL_Renderer *rend = app_state->rend;
    int primary_block = game_state->primary_block;
    BlockDb *db = game_state->block_db;
    GameGrid *grid = game_state->game_grid;

    int cellsize_w = draw_window.w / grid->width;
    int cellsize_h = draw_window.h / grid->height;

    // TODO: Reconfigure "draw window" - can just adjust origin here
    Point origin = {.x=draw_window.x, .y=draw_window.y};


    if (primary_block != INVALID_BLOCK_ID) {

        BlockDb_drawBlockOnGrid(db, primary_block, rend, origin, cellsize_w, cellsize_h);
    }

    GameGrid_drawGrid(grid, rend, db, origin, cellsize_w, cellsize_h);
}

// Base draw method for GameState - draws game area and sidebar information
void drawGame(ApplicationState *app_state, GameState *game_state) {

    SDL_Renderer *rend = app_state->rend;


    SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
    SDL_RenderClear(rend);

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);

    SDL_Rect game_area = {
        .x=0, .y=0,
        .w=(GAMEAREA_WEIGHT_W * wind_w) / TOTAL_WEIGHT_W,
        .h=wind_h
    };


    drawGameArea(app_state, game_state, game_area);
    drawInterface(app_state, game_state);

}



/*=============================================================================
 State runners
=============================================================================*/

// Run a single frame of game
// Primary "state-runner function" for GameState
StateFuncStatus GameState_run(
    StateRunner *state_runner, void *application_data, void *state_data
) {

    /* Recasting */
    GameState *game_state = (GameState*)state_data;
    ApplicationState *application_state = (ApplicationState*)application_data;

    /* Relevant variable extraction */
    SDL_Renderer *rend = application_state->rend;
    int *hardware_states = application_state->hardware_states;
    GamecodeMap *keymaps = game_state->keymaps;
    BlockDb *db = game_state->block_db;


    /***** PROCESS INPUTS *****/
    processGamecodes(game_state->gamecode_states, hardware_states, keymaps);

    /***** UPDATE *****/
    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_QUIT)) {
        printf("Quitting...\n");
        return STATEFUNC_QUIT;
    }

    // TODO: Add a StateRunner_setPopCount(...) method or something to replace status codes
    StateFuncStatus update_status = updateGame(state_runner, game_state);


    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_PAUSE)) {
        StateRunner_addState(state_runner, game_state, GameState_runPaused, NULL);
    }

    /***** DRAW *****/
    drawGame(application_state, game_state);
    return update_status;
}

/**
 * @brief Runs a paused version of the game, not updating anything but drawing
 */
StateFuncStatus GameState_runPaused(StateRunner *state_runner, void *application_data, void *state_data) {

    /* Recasting */
    GameState *game_state = (GameState*)state_data;
    ApplicationState *application_state = (ApplicationState*)application_data;

    /* Relevant variable extraction */
    SDL_Renderer *rend = application_state->rend;
    int *hardware_states = application_state->hardware_states;
    GamecodeMap *keymaps = game_state->keymaps;
    BlockDb *db = game_state->block_db;



    /***** PROCESS INPUTS *****/
    processGamecodes(game_state->gamecode_states, hardware_states, keymaps);

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_PAUSE)) {
        return STATEFUNC_QUIT;
    }

    /***** DRAWING *****/
    drawGame(application_state, game_state);

    // Pause overlay
    SDL_Rect dstrect = {.x=10, .y=10};
    SDL_QueryTexture(game_state->pause_texture, NULL, NULL, &dstrect.w, &dstrect.h);
    SDL_RenderCopy(rend, game_state->pause_texture, NULL, &dstrect);


    return STATEFUNC_CONTINUE;
}


// Run the grid animation
StateFuncStatus GameState_runGridAnimation(
    StateRunner *state_runner, void *app_data, void *state_data
) {

    // recasting
    GameState *game_state = (GameState*)state_data;
    ApplicationState *app_state = (ApplicationState*)app_data;

    // extraction
    SDL_Renderer *rend = app_state->rend;
    GameGrid *grid = game_state->game_grid;
    int *hardware_states = app_state->hardware_states;
    GamecodeMap *keymaps = game_state->keymaps;


    /***** PROCESS INPUTS *****/
    processGamecodes(game_state->gamecode_states, hardware_states, keymaps);

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_PAUSE)) {
        StateRunner_addState(state_runner, game_state, GameState_runPaused, NULL);
    }

    /***** UPDATE *****/

    GameGrid_runAnimationFrame(grid);
    if (!grid->is_animating) {
        return STATEFUNC_QUIT;
    }

    /***** DRAW *****/
    drawGame(app_state, game_state);
    return STATEFUNC_CONTINUE;
}




