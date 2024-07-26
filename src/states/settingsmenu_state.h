
#ifndef SETTINGSMENU_STATE_H
#define SETTINGSMENU_STATE_H

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

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

    // For block display
    int palette_selection;
    size_t n_palette_presets;
    size_t *palette_sizes;
    SDL_Color *palette_presets;


} SettingsMenuState;

SettingsMenuState* SettingsMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, GameSettings *settings
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
