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
#define GAMEAREA_WEIGHT_W 3
#define SIDEBAR_WEIGHT_W 1

#define TOTAL_WEIGHT_W (GAMEAREA_WEIGHT_W + SIDEBAR_WEIGHT_W)


/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

/**
 * @brief Initialize the GameState, returning a pointer to it
 * @param rend - SDL_Renderer pointer used for label creation
 * @param menu_font - TTF_Font pointer to use for creating labels
 * @param keymaps - GamecodeMap pointer of pre-configured key mappings.
 *                  Note that while GameState_init does not allocate this
 *                  memory, GameState_deconstruct will free it
 * @param init_level - Level to begin game at
 * @param block_size - Block size to use for all blocks
 * @param preset_size - Number of block presets defined
 * @param block_presets - Array of bitmasks representing blocks to spawn
 * @param palette_size - Number of colors to pull from for blocks
 * @param palette - Array of SDL_Color to use for blocks
 */
GameState* GameState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, GamecodeMap *keymaps,
    int init_level,
    int block_size,
    int preset_size, long *block_presets,
    int palette_size, SDL_Color *palette
) {

    SDL_Surface *surf = TTF_RenderText_Solid(
        menu_font, "Paused", (SDL_Color){255, 255, 255}
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);  // no longer needed


    /*** Initialize struct ***/
    GameState *retval = (GameState*)malloc(sizeof(GameState));
    *(retval) = (GameState){
        // single values 
        .move_counter=0,
        .score=0,
        .level=init_level,
        .block_size=block_size,

        .primary_block = INVALID_BLOCK_ID,
        .queued_block = INVALID_BLOCK_ID,

        // structs and arrays
        .num_presets=preset_size,
        .block_presets=(long*)malloc(preset_size * sizeof(long)),

        .block_db = BlockDb_init(256),
        // .game_grid = GameGrid_init(GRID_WIDTH, GRID_HEIGHT),
        // size 4 => 10wide, 24tall
        .game_grid = GameGrid_init(
            2 * block_size + block_size / 2,
            6 * block_size
        ),


        .keymaps=keymaps,
        .gamecode_states=(bool*)calloc((int)NUM_GAMECODES, sizeof(bool)),

        .palette_size=palette_size,
        .palette=(SDL_Color*)malloc(sizeof(SDL_Color) * palette_size),

        .pause_texture=texture,
        .score_label=NULL,
        .level_label=NULL

    };

    /*** Post-creation processing ***/

    // Initialize block presets
    memcpy(retval->block_presets, block_presets, preset_size * sizeof(long));
    memcpy(retval->palette, palette, palette_size * sizeof(SDL_Color));

    // Initialize grid cells
    GameGrid_clear(retval->game_grid);

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
    GamecodeMap_deconstruct(game_state->keymaps);

    free(game_state->block_presets);
    free(game_state->palette);

    // NOTE: Memory not allocated by GameState, but still freed here.
    // This may be bad or dangerous (?)
    free(game_state->gamecode_states);


    SDL_DestroyTexture(game_state->pause_texture);
    SDL_DestroyTexture(game_state->score_label);
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


    // Must clear first due to animation timing
    GameGrid_resolveRows(grid, db);

    int rand_idx;
    long new_contents;

    if (*queued_block == INVALID_BLOCK_ID) {

        SDL_Color *palette = game_state->palette;
        int palette_size = game_state->palette_size;

        rand_idx = ((rand() + game_state->num_presets + palette_size));
        long new_contents = block_presets[rand_idx % game_state->num_presets];
        // TODO: Replace stored color with stored color_idx
        SDL_Color new_color = palette[rand_idx % palette_size];

        *queued_block = BlockDb_createBlock(
            db, game_state->block_size, new_contents, (Point){0, 0}, new_color
        );
    }

    if (*primary_block == INVALID_BLOCK_ID) {
        *primary_block = *queued_block;

        SDL_Color *palette = game_state->palette;
        int palette_size = game_state->palette_size;

        rand_idx = ((rand() + game_state->num_presets + palette_size));
        long new_contents = block_presets[rand_idx % game_state->num_presets];
        // TODO: Replace stored color with stored color_idx
        SDL_Color new_color = palette[rand_idx % palette_size];


        *queued_block = BlockDb_createBlock(
            db, game_state->block_size, new_contents, (Point){0, 0}, new_color
        );

        if (*primary_block == INVALID_BLOCK_ID || *queued_block == INVALID_BLOCK_ID) {
            return STATEFUNC_ERROR;
        }

        BlockDb_setBlockPosition(db, *primary_block, (Point){5, 5});

        if (!GameGrid_canBlockExist(grid, db, *primary_block)) {


            // TODO: Make a GameGrid_xxx method to achieve this
            // TODO: Hacky workaround with a SPECIAL animation
            // run call so as to not free game_state while I still need it
            for (int i = 0; i < grid->height; i++) {
                grid->to_remove[i] = grid->width;
                grid->is_animating = true;
            }

            grid->framerate = 5;
            grid->cooldown = 5;

            *primary_block = INVALID_BLOCK_ID;  // to avoid drawing
            StateRunner_addState(
                state_runner, (void*)game_state,
                GameState_runGameOver, GameState_deconstruct
            );

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
int drawInterface(ApplicationState *app_state, GameState *game_state) {

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
    int retval = BlockDb_drawBlock(block_db, game_state->queued_block, rend, topleft, cell_size, cell_size);
    if (retval < 0) { return retval; }
    return 0;
}

// Draw game area, including primary block and grid
int drawGameArea(ApplicationState *app_state, GameState *game_state, SDL_Rect draw_window) {

    SDL_Renderer *rend = app_state->rend;
    int primary_block = game_state->primary_block;
    BlockDb *db = game_state->block_db;
    GameGrid *grid = game_state->game_grid;

    int cellsize_w = draw_window.w / grid->width;
    int cellsize_h = draw_window.h / grid->height;

    Point origin = {.x=draw_window.x, .y=draw_window.y};

    int retval;
    if (primary_block != INVALID_BLOCK_ID) {

        retval = BlockDb_drawBlockOnGrid(db, primary_block, rend, origin, cellsize_w, cellsize_h);
        if (retval < 0) {
            return retval;
        }
    }

    retval = GameGrid_drawGrid(grid, rend, db, origin, cellsize_w, cellsize_h);
    if (retval < 0) {
        return retval;
    }
    return 0;
}

// Base draw method for GameState - draws game area and sidebar information
int drawGame(ApplicationState *app_state, GameState *game_state) {

    SDL_Renderer *rend = app_state->rend;


    SDL_SetRenderDrawColor(rend, 10, 20, 30, 255);
    SDL_RenderClear(rend);

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);


    int retval = 0;

    SDL_Rect game_area = {
        .x=0, .y=0,
        .w=(GAMEAREA_WEIGHT_W * wind_w) / TOTAL_WEIGHT_W,
        .h=wind_h
    };


    retval = drawGameArea(app_state, game_state, game_area);
    if (retval < 0) { return retval; }

    retval = drawInterface(app_state, game_state);
    if (retval < 0) { return retval; }

    return 0;
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
    // if (update_status == STATEFUNC_QUIT) {
    //     return STATEFUNC_QUIT;
    // }


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

// Final state to run - plays a game over animation
StateFuncStatus GameState_runGameOver(
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

    if (grid->is_animating) {
        GameGrid_runAnimationFrame(grid);
    }


    if (!grid->is_animating) {
        return STATEFUNC_QUIT;
    }

    /***** DRAW *****/
    drawGame(app_state, game_state);
    return STATEFUNC_CONTINUE;
}




