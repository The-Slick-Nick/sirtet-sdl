
#ifndef SETTINGSMENU_STATE_H
#define SETTINGSMENU_STATE_H

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "mainmenu_state.h"  // for GameSettings struct ref
#include "inputs.h"
#include "state_runner.h"
#include "menu.h"


typedef struct {

    GameSettings *settings;
    TextMenu *menu;

} SettingsMenuState;

SettingsMenuState* SettingsMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, SDL_Texture *title_logo
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
