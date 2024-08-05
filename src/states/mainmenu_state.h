
#ifndef MAINMENU_STATE_H
#define MAINMENU_STATE_H

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "inputs.h"
#include "state_runner.h"
#include "menu.h"
#include "game_state.h"
#include "backgrounds.h"


// State representing the main menu
typedef struct mainmenustate {

    MenucodeMap *menucode_map;  // Mapping collection of hardware codes to menu codes
    bool *menucode_states;      // Bool array indicating if menu signals are active

    TTF_Font *label_font;
    SDL_Texture *title_logo;

    /* Menu option meta info */
    TextMenu *mainmenu;
    int menuopt_tilesize;
    int menuopt_start;
    int menuopt_exit;

    /* Game Settings */
    GameSettings *settings;

    /* Labels & Display */
    SDL_Texture *title_banner;  // Texture with menu title showing
    PanningBg background;


} MainMenuState;

MainMenuState* MainMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, SDL_Texture *title_logo,
    SDL_Texture *bg_tl, SDL_Texture *bg_tr, SDL_Texture *bg_bl,
    SDL_Texture *bg_br
);

// Tear down a MainMenuState, following behaviour set by state_runner
int MainMenuState_deconstruct(void* self);



/******************************************************************************
 * State running
******************************************************************************/

int MainMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
);


#endif
