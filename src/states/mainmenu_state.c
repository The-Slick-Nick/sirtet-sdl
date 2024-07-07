
#include "mainmenu_state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include "application_state.h"
#include "state_runner.h"
#include "inputs.h"
#include "game_state.h"

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

MainMenuState* MainMenuState_init(SDL_Renderer *rend, TTF_Font *menu_font) {

    // return value
    MainMenuState *menustate = (MainMenuState*)malloc(sizeof(MainMenuState));

    // Preprocessing
    SDL_Surface *surf = TTF_RenderText_Solid(
        menu_font, "Main Menu", (SDL_Color){255, 255, 255}
    );

    // Definition
    *menustate = (MainMenuState){

        .title_banner=SDL_CreateTextureFromSurface(rend, surf),
        .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS)
    };

    // Postprocessing
    MenucodeMap *mcodes = menustate->menucode_map;

    printf("Mapping menu keys...\n");
    Menucode_addMap(mcodes, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN2, 1, 1, 1);


    printf("Freeing surface...\n");
    SDL_FreeSurface(surf);

    return menustate;

}

int MainMenuState_deconstruct(void* self) {

    MainMenuState *menustate = (MainMenuState*)self;


    MenucodeMap_deconstruct(menustate->menucode_map);
    SDL_DestroyTexture(menustate->title_banner);

    free(self);
    return 0;
}


/******************************************************************************
 * State running
******************************************************************************/


StateFuncStatus MainMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
) {
    
    /***** Recasting & Extraction *****/

    // Recact
    ApplicationState *app_state = (ApplicationState*)application_data;
    MainMenuState *menu_state = (MainMenuState*)state_data;

    // Extract for less typing
    SDL_Renderer *rend = app_state->rend;
    bool *menu_codes = menu_state->menucode_states;
    int *hardware_codes = app_state->hardware_states;
    MenucodeMap *keymaps = menu_state->menucode_map;


    /***** Inputs *****/
    processMenucodes(menu_codes, hardware_codes, keymaps);

    /***** Process state *****/

    if (Menucode_pressed(menu_codes, MENUCODE_EXIT)) {
        return STATEFUNC_QUIT;
    }

    if (Menucode_pressed(menu_codes, MENUCODE_SELECT)) {
        // GameState *new_state = GameState_init(rend, app_state->menu_font);
        GameState *new_state = GameState_init(app_state);
        StateRunner_addState(state_runner, new_state, GameState_run, GameState_deconstruct);
    }

    /***** Draw *****/
    SDL_Rect loc = {.x=10, .y=10};
    SDL_QueryTexture(menu_state->title_banner, NULL, NULL, &loc.w, &loc.h);
    SDL_RenderCopy(rend, menu_state->title_banner, NULL, &loc);


    // return STATEFUNC_QUIT;
    return STATEFUNC_CONTINUE;
}
