
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


typedef struct mainmenustate MainMenuState;

// A menufunc should know about everything its statefunc knows about
// (so we match the argument signature)
typedef void (*menufunc_t)(StateRunner*, ApplicationState*, MainMenuState*);


typedef struct {
    menufunc_t sel_func;
    char text[32];
    menufunc_t commands[NUM_MENUCODES];

} MenuOption;



// State representing the main menu
struct mainmenustate {

    MenucodeMap *menucode_map;  // Mapping collection of hardware codes to menu codes
    bool *menucode_states;      // Bool array indicating if menu signals are active

    /* Menu option meta info */
    int menu_selection;         // Menu item # selected
    int num_options;            // Number of menu options
    MenuOption *options;

    /* Game Settings */
    int init_level;
    int block_size;

    /* Labels */
    SDL_Texture *title_banner;  // Texture with menu title showing
    SDL_Texture *level_label;   // Labels currently selected starting level
    SDL_Texture *blocksize_label;
    SDL_Texture *start_label;

};

// MainMenuState* MainMenuState_init(ApplicationState *app_state);
MainMenuState* MainMenuState_init(SDL_Renderer *rend, TTF_Font *menu_font);

// Tear down a MainMenuState, following behaviour set by state_runner
int MainMenuState_deconstruct(void* self);



/******************************************************************************
 * State running
******************************************************************************/

StateFuncStatus MainMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
);


#endif
