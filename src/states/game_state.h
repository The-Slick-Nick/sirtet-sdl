/* game_state.h
*
* Defines struct and methods for handling state of
* the game.
*
* State follows a (as of yet to be actually written) standard
* for a state-runner to run
*/

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "block.h"
#include "grid.h"
#include "inputs.h"
#include "colorpalette.h"

#include "sirtet_audio.h"
#include "state_runner.h"

#define LINES_PER_LEVEL 10


// A struct meant to segregate game initialization settings
typedef struct {

    size_t max_preset_size;     // Max number of block presets that can be used
    size_t max_palette_size;    // Max number of colors in palette

    int init_level;
    int block_size;

    GamecodeMap *keymaps;
    size_t preset_size;         // Number of block presets in use
    long *block_presets;        // Array of block presets

    ColorPalette *palette;

} GameSettings;


// Structure representing current state of game.
typedef struct {

    GamecodeMap *keymaps;       // collection of hardware -> gamecode key mappings
    bool *gamecode_states;      // boolean flag array for gamecodes (indexed by Gamecode)

    int move_counter;           // number of frames since last movement
    int score;                  // Number of points accumulated
    int level;                  // Current game level. Affects speed & score
    int lines_this_level;      // # of lines cleared this level
    int block_size;             // Block sizing standard for this gamestate

    BlockDb *block_db;
    GameGrid *game_grid;        // Grid struct storing committed blocks


    int primary_block;          // id of main block dropping from top to bottom
    int queued_block;           // Next block queued up

    long *block_presets;        // Array of block content masks to draw from
    int num_presets;            // Number of block content presets in *block_presets
    
    ColorPalette *palette;

    /* Sounds */
    SirtetAudio_sound place_sound;
    SirtetAudio_sound success_sound;
    SirtetAudio_sound gameover_sound;



    /* State/structs for display */
    TTF_Font *menu_font;
    SDL_Texture *pause_texture; // Texture with "pause" text

    SDL_Texture *score_label;
    SDL_Texture *level_label;
    SDL_Texture *next_label;

} GameState;

/******************************************************************************
 * GameSettings
******************************************************************************/


GameSettings* GameSettings_init(size_t max_preset_sz, size_t max_palette_sz);


void GameSettings_deconstruct(GameSettings *self);


int GameSettings_setPresets(GameSettings *self, int block_size, size_t src_len, long *src);
int GameSettings_setPalette(GameSettings *self, ColorPalette *palette);

/******************************************************************************
 * GameState
******************************************************************************/

// Initialize and return a pointer for GameState
GameState* GameState_init(
    SDL_Renderer *rend, TTF_Font *menu_font,
    GameSettings *settings,
    SirtetAudio_sound place_sound,    // block placed on grid
    SirtetAudio_sound success_sound,  // line complete
    SirtetAudio_sound gameover_sound  // game over
);
// Deconstruct a GameState by pointer reference
int GameState_deconstruct(void* self);


// Run one frame of game
int GameState_run(StateRunner *state_runner, void *application_data, void *state_data);
int GameState_runPaused(StateRunner *state_runner, void *application_data, void *state_data);
int GameState_runGridAnimation(StateRunner *state_runner, void *app_data, void *state_data);
int GameState_runGameOver(StateRunner *state_runner, void *app_data, void *state_data);


#endif
