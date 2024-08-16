/* game_state.c
*
* Declares logic and whatnot for handling the primary game state
* The main game logic is found here
*
* State follows a (as of yet to be actually written) standard
* for a state-runner to run
*/

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "sirtet.h"
#include "sirtet_audio.h"
#include "colorpalette.h"
#include "grid.h"
#include "block.h"
#include "game_state.h"
#include "application_state.h"
#include "gameover_state.h"
#include "component_drawing.h"
#include "inputs.h"
#include "state_runner.h"


// For dimension calculations
#define GAMEAREA_WEIGHT_W 3
#define SIDEBAR_WEIGHT_W 1

#define TOTAL_WEIGHT_W (GAMEAREA_WEIGHT_W + SIDEBAR_WEIGHT_W)


#define OUTSET_COL (SDL_Color){0, 0, 0, 255}
#define INSET_COL (SDL_Color){50, 50, 50, 255}

#define BORDER_SIZE 24


/******************************************************************************
 * GameSettings
******************************************************************************/

GameSettings* GameSettings_init(size_t max_preset_sz, size_t max_palette_sz) {

    GameSettings *retval = (GameSettings*)malloc(sizeof(GameSettings));

    retval->max_preset_size = max_preset_sz;
    retval->max_palette_size = max_palette_sz;

    retval->keymaps = GamecodeMap_init(MAX_GAMECODE_MAPS);

    retval->preset_size=0;
    retval->block_presets = (long*)calloc(max_preset_sz, sizeof(long));

    retval->palette = ColorPalette_initVa(
        "Default", 7, 
        (SDL_Color){190,83,28, 255},
        (SDL_Color){218,170,0, 255},
        (SDL_Color){101,141,27, 255},
        (SDL_Color){0,95,134, 255},
        (SDL_Color){155,0,0, 255},
        (SDL_Color){0,155,0, 255},
        (SDL_Color){0,0,155, 255}
    );


    return retval;
}

// Declare the settings config for blocks to spawn
int GameSettings_setPresets(GameSettings *self, int block_size, size_t src_len, long *src) {
    if (src_len > self->max_preset_size) {
        char buff[128];
        snprintf(
            buff, 128,
            "Provided preset length %llu exceeds max length of %llu\n",
            src_len, self->max_preset_size
        );
        Sirtet_setError(buff);
        return -1;
    }

    self->block_size = block_size;
    self->preset_size = src_len;
    memcpy(self->block_presets, src, src_len * sizeof(long));
    return 0;
}

// Assign a color palette to settings
int GameSettings_setPalette(GameSettings *self, ColorPalette *palette) {

    if (self == NULL) {
        Sirtet_setError(
            "GameSettings_setPalette passed a NULL pointer for self\n");
        return -1;
    }

    if (palette == NULL) {
        Sirtet_setError(
            "GameSettings_setPalette passed a NULL pointer for palette\n");
        return -1;
    }

    ColorPalette_deconstruct(self->palette);
    self->palette = ColorPalette_initCopy(palette);

    return 0;
}

void GameSettings_deconstruct(GameSettings *self) {

    GamecodeMap_deconstruct(self->keymaps);
    free(self->block_presets);
    free(self->palette);
    
    free(self);
}


/******************************************************************************
 * GameState
******************************************************************************/

/**
 * @brief Initialize the GameState, returning a pointer to it
 * @param rend - SDL_Renderer pointer used for label creation
 * @param menu_font - TTF_Font pointer to use for creating labels
 * @param settings - Pointer to a GameSettings struct describing various
 *                   details on how game should function
 */
GameState* GameState_init(
    SDL_Renderer *rend, TTF_Font *menu_font,
    GameSettings *settings,
    SirtetAudio_sound place_sound,
    SirtetAudio_sound success_sound,
    SirtetAudio_sound gameover_sound
) {

    // settings extraction
    int init_level = settings->init_level;
    int block_size = settings->block_size;

    GamecodeMap *keymaps = settings->keymaps;


    /*** Initialize struct ***/

    GameState *retval = (GameState*)malloc(sizeof(GameState));


    /*** Tracking ***/

    retval->move_counter = 0;
    retval->score = 0;
    retval->level = init_level;
    retval->lines_this_level = 0;


    /*** Block config ***/ 

    retval->block_db = BlockDb_init(256);
    retval->game_grid = GameGrid_init(
        2 * block_size + block_size / 2,
        6 * block_size
    );

    retval->block_size = block_size;
    retval->primary_block = INVALID_BLOCK_ID;
    retval->queued_block = INVALID_BLOCK_ID;

    // Palette
    retval->palette = ColorPalette_initCopy(settings->palette);

    // Presets
    size_t preset_n = sizeof(long) * settings->preset_size;
    retval->num_presets = settings->preset_size;
    retval->block_presets = (long*)malloc(preset_n);
    memcpy(retval->block_presets, settings->block_presets, preset_n);

    // Initialize grid cells
    GameGrid_clear(retval->game_grid);


    /*** Controls ***/

    retval->keymaps = GamecodeMap_initCopy(keymaps);
    retval->gamecode_states = (bool*)calloc((int)NUM_GAMECODES, sizeof(bool));


    /*** Sounds ***/

    retval->success_sound = success_sound;
    retval->place_sound = place_sound;
    retval->gameover_sound = gameover_sound;


    /*** Labels and such ***/

    retval->menu_font = menu_font;

    SDL_Surface *p_surf = TTF_RenderText_Solid(
        menu_font, "Paused", (SDL_Color){255, 255, 255}
    );
    SDL_Surface *n_surf = TTF_RenderText_Solid(
        menu_font, "Next", (SDL_Color){255, 255, 255}
    );
    retval->pause_texture = SDL_CreateTextureFromSurface(rend, p_surf);
    retval->next_label = SDL_CreateTextureFromSurface(rend, n_surf);
    SDL_FreeSurface(p_surf);
    SDL_FreeSurface(n_surf);

    // Re-renderable lables, that render at display time
    retval->score_label = NULL;
    retval->level_label = NULL;

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

    // NOTE: Freed here, as they are alloc'd and copied from GameSettings
    // If this ever changes to refer to a GameSettings pointer, this will
    // need undone
    free(game_state->block_presets);
    free(game_state->palette);
    GamecodeMap_deconstruct(game_state->keymaps);


    free(game_state->gamecode_states);

    SDL_DestroyTexture(game_state->pause_texture);
    SDL_DestroyTexture(game_state->score_label);
    SDL_DestroyTexture(game_state->pause_texture);
    SDL_DestroyTexture(game_state->next_label);

    free(self);

    return 0;
}


/*=============================================================================
 * Logical components 
=============================================================================*/

// Update portion of main game loop
int updateGame(
    StateRunner *state_runner,
    ApplicationState *app_state,
    GameState *game_state,
    SirtetAudio_sound *out_sound
) {

    // relevant variable extraction - for shorthand (
    // and to save my fingers from typing a lot)
    BlockDb *db = game_state->block_db;
    GameGrid *grid = game_state->game_grid;
    int *primary_block = &game_state->primary_block;
    int *queued_block = &game_state->queued_block;
    long *block_presets = game_state->block_presets;

    int score_to_inc = 0;


    // Must clear first due to animation timing
    game_state->lines_this_level += GameGrid_resolveRowsUp(grid, db);
    int level_ups = game_state->lines_this_level / LINES_PER_LEVEL;
    if (level_ups > 0) {
        game_state->level += level_ups;
        SDL_DestroyTexture(game_state->level_label);
        game_state->level_label = NULL;
        game_state->lines_this_level -= (level_ups * LINES_PER_LEVEL);
    }




    if (*queued_block == INVALID_BLOCK_ID) {

        int preset_idx = rand() % game_state->num_presets;
        int palette_idx = preset_idx % game_state->palette->size;

        long new_contents = block_presets[preset_idx];
        SDL_Color new_color;
        ColorPalette_getColor(game_state->palette, palette_idx, &new_color);

        *queued_block = BlockDb_createBlock(
            db, game_state->block_size, new_contents, (Point){0, 0}, new_color
        );
    }

    if (*primary_block == INVALID_BLOCK_ID) {
        *primary_block = *queued_block;

        int preset_idx = rand() % game_state->num_presets;
        int palette_idx = preset_idx % game_state->palette->size;

        long new_contents = block_presets[preset_idx];
        SDL_Color new_color;
        ColorPalette_getColor(game_state->palette, palette_idx, &new_color);

        *queued_block = BlockDb_createBlock(
            db, game_state->block_size, new_contents, (Point){0, 0}, new_color
        );

        // creation failed
        if (*primary_block == INVALID_BLOCK_ID
            || *queued_block == INVALID_BLOCK_ID) {
            return -1;
        }

        int block_size = BlockDb_getBlockSize(db, *primary_block);
        Point init_coord = {
            .x=grid->width / 2,
            .y=grid->height - ((block_size / 2) + ((block_size & 1) == 1)) 
        };

        BlockDb_setBlockPosition(db, *primary_block, init_coord);

        // New block can't exist
        if (!GameGrid_canBlockExist(grid, db, *primary_block)) {

            *out_sound = game_state->gameover_sound;

            SirtetAudio_playSound(app_state->sounds.boop_scale_reverse);


            SDL_Color act_col = {200, 50, 50, 255};
            SDL_Color dyn_col = {50, 50, 200, 255};
            GameoverState *go_state = GameoverState_init(
                app_state->rend, app_state->fonts.vt323_24,
                game_state->score, app_state->hiscores,
                &dyn_col, &act_col
            );

            if (go_state == NULL) {
                printf("Error: %s", Sirtet_getError());
                exit(1);
            }

            StateRunner_addState(
                state_runner, go_state,
                GameoverState_run, GameoverState_deconstruct
            );

            // On top of stack - show animation
            GameGrid_prepareAnimationAllRows(grid, 5);

            *primary_block = INVALID_BLOCK_ID;  // to avoid drawing
            StateRunner_addState(
                state_runner, (void*)game_state,
                GameState_runGridAnimation, GameState_deconstruct
            );


            StateRunner_setPopCount(state_runner, 1);
            return 0;
        }

        printf("New block id is %d\n", game_state->primary_block);
        printf("New contents representation is %ld\n", new_contents);
    }


    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_ROTATE)) {

        int block_size = BlockDb_getBlockSize(db, *primary_block);
        long block_contents = BlockDb_getBlockContents(db, *primary_block);
        Point block_position = BlockDb_getBlockPosition(db, *primary_block);

        long rotated_contents = rotateBlockContentsCw90(
            block_contents, block_size
        );

        // smart rotation 
        for (
            int x_delta = 0;
            x_delta <= block_size / 2;
            x_delta = (x_delta * -1) + (x_delta <= 0)
        ) {

            Point proj_pos = {block_position.x + x_delta, block_position.y};
            bool can_exist = GameGrid_canBlockInfoExist(
                grid, block_size, rotated_contents, proj_pos
            );
            if (can_exist)  {
                BlockDb_setBlockContents(db, *primary_block, rotated_contents);
                BlockDb_setBlockPosition(db, *primary_block, proj_pos);
                break;
            }
        }
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_LEFT)) {

        Point new_pos = Point_translate(
            BlockDb_getBlockPosition(db, *primary_block), (Point){-1, 0}
        );

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
        Point new_pos = Point_translate(
            BlockDb_getBlockPosition(db, *primary_block), (Point){1, 0}
        );
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
        Gamecode_pressed(game_state->gamecode_states, GAMECODE_MOVE_UP)
        ||
        // TODO: Include some kind of scaling function for difficulty
        game_state->move_counter > (TARGET_FPS / (1 + game_state->level))
    ) {
        game_state->move_counter = 0;

        Point new_pos = Point_translate(
            BlockDb_getBlockPosition(db, *primary_block),
            (Point){0, -1}
        );

        if (GameGrid_canBlockInfoExist(
            grid, BlockDb_getBlockSize(db, *primary_block),
            BlockDb_getBlockContents(db, *primary_block), new_pos
        )) {
            BlockDb_setBlockPosition(db, *primary_block, new_pos);
        }
        else {
            // block placed
            *out_sound = game_state->place_sound;
            GameGrid_commitBlock(grid, db, *primary_block);
            *primary_block = INVALID_BLOCK_ID;
        }
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_HARD_DROP)) {
        game_state->move_counter = 0;

        int block_size = BlockDb_getBlockSize(db, *primary_block);
        long block_contents = BlockDb_getBlockContents(db, *primary_block);
        Point block_pos = BlockDb_getBlockPosition(db, *primary_block);

        int dist = GameGrid_getDropDistance(
            grid, block_size, block_contents, block_pos
        );

        if (dist >= 0) {

            BlockDb_setBlockPosition(
                db, *primary_block,
                Point_translate(block_pos, (Point){0, -1 * dist})
            );
            GameGrid_commitBlock(grid, db, *primary_block);
            *primary_block = INVALID_BLOCK_ID;
            game_state->move_counter = 0;

            // extra score for hard dropping
            score_to_inc += 2 * dist;
        }

        // block placement is forced
        *out_sound = game_state->place_sound;
    }

    score_to_inc += GameGrid_assessScore(grid, game_state->level);

    if (score_to_inc > 0) {
        SDL_DestroyTexture(game_state->score_label);
        game_state->score_label = NULL;
    }
    game_state->score += score_to_inc;

    GameGrid_prepareAnimation(grid, 3);
    if (grid->is_animating) {

        // NOTE: Overrides place sound, so no double playing
        *out_sound = game_state->success_sound;

        SirtetAudio_playSound(app_state->sounds.boop_scale);
        StateRunner_addState(
            state_runner, game_state, GameState_runGridAnimation, NULL
        );
    }

    return 0;
}


/*=============================================================================
 * Draw Components
=============================================================================*/


// Draw the interface for queued block, returning the total height taken up
int drawNextBlock(
    ApplicationState *app_state, GameState *game_state,
    SDL_Rect *dest
) {

    const int block_size = BlockDb_getBlockSize(
        game_state->block_db, game_state->queued_block
    );
    const int cell_size = dest->w / block_size;

    SDL_Rect bgrect = {dest->x, dest->y, dest->w, dest->w};

    SDL_Color bgcol = INSET_COL;
    SDL_SetRenderDrawColor(app_state->rend, bgcol.r, bgcol.g, bgcol.b, bgcol.a);
    SDL_RenderFillRect(app_state->rend, &bgrect);
    
    BlockDb_drawBlock(
        game_state->block_db, game_state->queued_block,
        app_state->rend, (Point){dest->x, dest->y},
        cell_size, cell_size
    );

    return block_size * cell_size;
}

// Draw the interface area displaying level & score, returning height taken
int drawScoreArea(
    ApplicationState *app_state, GameState *game_state,
    SDL_Rect *dest
) {

    int total_h = 0;
    int max_w = 0;

    int score_h, score_w;
    SDL_QueryTexture(game_state->score_label, NULL, NULL, &score_w, &score_h);
    max_w = score_w > max_w ? score_w : max_w;
    total_h += score_h;

    int lvl_h, lvl_w;
    SDL_QueryTexture(game_state->level_label, NULL, NULL, &lvl_w, &lvl_h);
    max_w = lvl_w > max_w ? lvl_w : max_w;
    total_h += lvl_h;

    SDL_Rect bgrect = {dest->x, dest->y, dest->w, total_h};

    SDL_Color bgcol = INSET_COL;
    SDL_SetRenderDrawColor(app_state->rend, bgcol.r, bgcol.g, bgcol.b, bgcol.a);
    SDL_RenderFillRect(app_state->rend, &bgrect);

    // now actually draw the labels


    SDL_Rect dstrect = {dest->x, dest->y};

    dstrect.w = max_w;
    dstrect.h = score_h;
    SDL_RenderCopy(app_state->rend, game_state->score_label, NULL, &dstrect);
    dstrect.y += dstrect.h;

    // dstrect.w = lvl_w;
    dstrect.w = max_w;
    dstrect.h = lvl_h;
    SDL_RenderCopy(app_state->rend, game_state->level_label, NULL, &dstrect);

    return total_h;
}

// Draw supplmental game info (Score, on deck, flair, etc.)
int drawInterface(
    ApplicationState *app_state, GameState *game_state,
    SDL_Rect *draw_window, SDL_Rect *actual_draw
) {

    // Convenience unpacking
    SDL_Renderer *rend = app_state->rend;
    TTF_Font *menu_font = game_state->menu_font;

    int score = game_state->score;
    int level = game_state->level;

    // helper vars
    char score_buffer[32];  // 32 is overkill but just in case...
    char level_buffer[16];  

    SDL_Color bgcol = INSET_COL;
    SDL_SetRenderDrawColor(rend, bgcol.r, bgcol.g, bgcol.b, bgcol.a);

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);


    /***** Score + Level *****/

    // NOTE: Relies on updateGame(...) invalidating score texture on score change
    if (game_state->score_label == NULL) {
        snprintf(score_buffer, 32, "Score: %d", score);
        SDL_Surface *surf = TTF_RenderText_Solid(
            menu_font, score_buffer, (SDL_Color){255, 255, 255}
        );
        game_state->score_label = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);

    }

    // NOTE: Relies on updateGame(...) invalidating level texture on level change
    if (game_state->level_label == NULL) {
        snprintf(level_buffer, 16, "Level: %d", level);

        SDL_Surface *lvl_surf = TTF_RenderText_Solid(
            menu_font, level_buffer, (SDL_Color){255, 255, 255}
        );
        if (lvl_surf == NULL) {
            char buff[128];
            snprintf(
                buff, 128,
                "Error rendering text for level label: %s\n",
                TTF_GetError()
            );
            Sirtet_setError(buff);
            return -1;
        }

        game_state->level_label = SDL_CreateTextureFromSurface(rend, lvl_surf);
        if (game_state->level_label == NULL) {
            char buff[128];
            snprintf(
                buff, 128,
                "Error rendering text for level label: %s\n",
                SDL_GetError()
            );
            Sirtet_setError(buff);
            return -1;
        }
        SDL_FreeSurface(lvl_surf);
    }

    int yoffset = draw_window->y;
    SDL_Rect dstrect = {
        .x=draw_window->x,
        .y=yoffset,
        .w=draw_window->w,
        .h=draw_window->h
    };

    yoffset += drawScoreArea(app_state, game_state, &dstrect) + 24;

    /***** Next Up *****/

    dstrect = (SDL_Rect){.x=draw_window->x, .y=yoffset};
    SDL_QueryTexture(
        game_state->next_label, NULL, NULL, &dstrect.w, &dstrect.h
    );
    SDL_RenderCopy(rend, game_state->next_label, NULL, &dstrect);

    yoffset += dstrect.h;


    dstrect = (SDL_Rect){
        .x=draw_window->x, .y=yoffset,
        .w=draw_window->w,
        .h=draw_window->h
    };
    yoffset += drawNextBlock(app_state, game_state, &dstrect);


    if (actual_draw != NULL) {
        *actual_draw = (SDL_Rect){
            .x=draw_window->x,
            .y=draw_window->y,
            .w=draw_window->w,
            .h=yoffset
        };
    };
    return 0;
}

// Draw game area, including primary block and grid

/**
 * @brief - Draw the game area, including the grid, primary block, and any
 *          supplementary visual elements.
 * @param app_state - Pointer to struct representing application-wide data
 * @param game_state - Pointer to struct representing game-related state data
 * @param draw_window - SDL_Rect describing the area in which to draw the grid
 *                      Note that this method will attempt to fill the entire
 *                      rectangle if possible, but will maintain the grid's
 *                      aspect ratio and keep to square cells, possibly
 *                      drawing into a smaller space than described.
 * @param actual_draw - Pointer to SDL_Rect to write out final write dimensions
 *                      Ignored if NULL
 */
int drawGameArea(
    ApplicationState *app_state, GameState *game_state, SDL_Rect *draw_window,
    SDL_Rect *actual_draw
) {

    /* Unpacking */
    SDL_Renderer *rend = app_state->rend;
    int primary_block = game_state->primary_block;
    BlockDb *db = game_state->block_db;
    GameGrid *grid = game_state->game_grid;

    int cellsize_w = draw_window->w / grid->width;
    int cellsize_h = draw_window->h / grid->height;
    int cellsize = cellsize_w > cellsize_h ? cellsize_h : cellsize_w;

    SDL_Rect final_dims;
    final_dims.w = cellsize * grid->width;
    final_dims.h = cellsize * grid->height;

    int w_pad = draw_window->w - final_dims.w;
    int h_pad = draw_window->h - final_dims.h;

    final_dims.x = draw_window->x + (w_pad / 2);
    final_dims.y = draw_window->y + (h_pad / 2);

    if (actual_draw != NULL) {
        *actual_draw = final_dims;
    }

    Point origin = {.x=final_dims.x, .y=final_dims.y};

    SDL_Color col = INSET_COL;
    SDL_SetRenderDrawColor(rend, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(rend, &final_dims);

    int retval;
    retval = GameGrid_drawGrid(grid, rend, db, origin, cellsize, cellsize);
    if (retval < 0) {
        return retval;
    }

    /*** Block drawing ***/

    if (primary_block != INVALID_BLOCK_ID) {


        int block_size = BlockDb_getBlockSize(db, primary_block);
        long block_contents = BlockDb_getBlockContents(db, primary_block);
        Point block_pos = BlockDb_getBlockPosition(db, primary_block);
        SDL_Color block_col = BlockDb_getBlockColor(db, primary_block);

        Point topleft = {
            .x=origin.x + cellsize * (block_pos.x - block_size / 2),
            .y=origin.y + cellsize * (block_pos.y - block_size / 2)
        };

        // projected block
        int dist = GameGrid_getDropDistance(grid, block_size, block_contents, block_pos);
        SDL_Color drawcol = {block_col.r, block_col.g, block_col.b, 64};
        Point drawpos = {topleft.x, topleft.y - (dist * cellsize)};

        drawBlockContents(
            rend, block_size, block_contents,
            &drawcol, &drawpos,
            cellsize, cellsize
        );

        // current block
        drawBlockContents(
            rend, block_size, block_contents,
            &block_col, &topleft,
            cellsize, cellsize
        );
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

    int area_w = (GAMEAREA_WEIGHT_W * wind_w) / TOTAL_WEIGHT_W;
    int area_h = wind_h - (2 * BORDER_SIZE);

    /***** Game Area *****/
    SDL_Rect game_area_request = {
        .x=(wind_w - area_w) / 2,
        .y=(wind_h - area_h) / 2,
        .w=area_w,
        .h=area_h
    };


    SDL_Rect game_area;
    retval = drawGameArea(app_state, game_state, &game_area_request, &game_area);
    if (retval < 0) { return retval; }


    /***** Sidebar/Interface *****/
    SDL_Rect sidebar_area_request = {
        .x=(GAMEAREA_WEIGHT_W * wind_w) / TOTAL_WEIGHT_W,
        .y=game_area.y,
        .w=(SIDEBAR_WEIGHT_W * (wind_w - 2 * BORDER_SIZE)) / TOTAL_WEIGHT_W,
        .h=game_area.h
    };
    SDL_Rect sidebar_area;

    retval = drawInterface(app_state, game_state, &sidebar_area_request, &sidebar_area);
    if (retval < 0) { return retval; }

    return 0;
}



/*=============================================================================
 State runners
=============================================================================*/

// Run a single frame of game
// Primary "state-runner function" for GameState
int GameState_run(
    StateRunner *state_runner, void *application_data, void *state_data
) {

    /* Recasting */
    GameState *game_state = (GameState*)state_data;
    ApplicationState *application_state = (ApplicationState*)application_data;

    /* Relevant variable extraction */

    int *hardware_states = application_state->hardware_states;
    GamecodeMap *keymaps = game_state->keymaps;



    /*** PROCESS INPUTS ***/

    processGamecodes(game_state->gamecode_states, hardware_states, keymaps);

    /*** UPDATE ***/

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_QUIT)) {
        printf("Quitting...\n");
        StateRunner_setPopCount(state_runner, 1);
        return 0;
    }

    SirtetAudio_sound toplay = NULLSOUND;
    int update_status = updateGame(
        state_runner, application_state, game_state,
        &toplay
    );

    if (update_status == -1) {
        return -1;
    }

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_PAUSE)) {
        toplay = application_state->sounds.bump;
        StateRunner_addState(state_runner, game_state, GameState_runPaused, NULL);
    }


    /*** PLAY AUDIO ***/

    if (!SirtetAudio_soundInvalid(toplay)) {
        SirtetAudio_playSound(toplay);
    }


    /*** DRAW ***/

    drawGame(application_state, game_state);
    return 0;
}

/**
 * @brief Runs a paused version of the game, not updating anything but drawing
 */
int GameState_runPaused(StateRunner *state_runner, void *application_data, void *state_data) {

    /* Recasting */
    GameState *game_state = (GameState*)state_data;
    ApplicationState *application_state = (ApplicationState*)application_data;

    /* Relevant variable extraction */
    SDL_Renderer *rend = application_state->rend;
    int *hardware_states = application_state->hardware_states;
    GamecodeMap *keymaps = game_state->keymaps;




    /***** PROCESS INPUTS *****/
    processGamecodes(game_state->gamecode_states, hardware_states, keymaps);

    if (Gamecode_pressed(game_state->gamecode_states, GAMECODE_PAUSE)) {
        StateRunner_setPopCount(state_runner, 1);
        return 0;
    }

    /***** DRAWING *****/
    drawGame(application_state, game_state);

    // Pause overlay
    SDL_Rect dstrect = {.x=10, .y=10};
    SDL_QueryTexture(game_state->pause_texture, NULL, NULL, &dstrect.w, &dstrect.h);
    SDL_RenderCopy(rend, game_state->pause_texture, NULL, &dstrect);


    return 0;
}


// Run the grid animation
int GameState_runGridAnimation(
    StateRunner *state_runner, void *app_data, void *state_data
) {

    // recasting
    GameState *game_state = (GameState*)state_data;
    ApplicationState *app_state = (ApplicationState*)app_data;

    // extraction
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
        StateRunner_setPopCount(state_runner, 1);
        return 0;
    }

    /***** DRAW *****/
    drawGame(app_state, game_state);
    return 0;
}

