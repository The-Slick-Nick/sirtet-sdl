
#ifndef SETTINGSMENU_STATE_H
#define SETTINGSMENU_STATE_H

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "sirtet_audio.h"
#include "game_state.h"
#include "state_runner.h"
#include "menu.h"


typedef struct {

    GameSettings *settings;

    // Menu 
    TextMenu *menu;
    TTF_Font *menu_font;
    bool *menucode_states;
    MenucodeMap *menucode_map;
    int menuopt_tilesize;
    int menuopt_palette;

    // Block Preset Settings
    size_t blocksize_sel;  // Selected block size
    long **presets;  // indexed by blocksize_sel

    // Color Settings
    int palette_selection;
    size_t num_palettes;
    ColorPalette **palettes;  // Array of palette pointers


} SettingsMenuState;

SettingsMenuState* SettingsMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, GameSettings *settings,
    SirtetAudio_sound menusound_move
);

// Tear down a MainMenuState, following behaviour set by state_runner
int SettingsMenuState_deconstruct(void* self);


/******************************************************************************
 * State running
******************************************************************************/

int SettingsMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
);


#endif
