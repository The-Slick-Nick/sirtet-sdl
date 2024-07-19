
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

        // .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        // .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS),

typedef struct {

    GameSettings *settings;

    // Menu 
    TTF_Font *menu_font;
    bool *menucode_states;
    MenucodeMap *menucode_map;


    TextMenu *menu;

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
