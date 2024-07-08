
#ifndef MAINMENU_STATE_H
#define MAINMENU_STATE_H

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "inputs.h"
#include "state_runner.h"
#include "application_state.h"

// State representing the main menu
// TODO: Once this is functioning correctly, extract
// and split parts out into common/separate menu and menustate
// structs or seomthing like that
typedef struct {

    bool *menucode_states;      // Bool array indicating if menu signals are active
    SDL_Texture *title_banner;  // Texture with menu title showing

    MenucodeMap *menucode_map;

    SDL_Texture *level_label;   // Labels currently selected starting level

} MainMenuState;

MainMenuState* MainMenuState_init(ApplicationState *app_state);

// Tear down a MainMenuState, following behaviour set by state_runner
int MainMenuState_deconstruct(void* self);



/******************************************************************************
 * State running
******************************************************************************/

StateFuncStatus MainMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
);


#endif
