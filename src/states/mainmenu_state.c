
#include "mainmenu_state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "application_state.h"
#include "state_runner.h"
#include "inputs.h"
#include "game_state.h"

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

// MainMenuState* MainMenuState_init(SDL_Renderer *rend, TTF_Font *menu_font) {
MainMenuState* MainMenuState_init(ApplicationState *app_state) {

    /* Convenience extractions */
    SDL_Renderer *rend = app_state->rend;
    TTF_Font *menu_font = app_state->menu_font;

    // General purpose buffer to use for strings
    char strbuffer[32];

    // return value
    MainMenuState *menustate = (MainMenuState*)malloc(sizeof(MainMenuState));

    // Preprocessing
    SDL_Surface *title_surf = TTF_RenderText_Solid(
        menu_font, "Main Menu", (SDL_Color){255, 255, 255}
    );

    // snprintf(char *, unsigned long, const char *, ...)
    snprintf(strbuffer, 32, "Level: %d", app_state->init_level);
    SDL_Surface *level_surf = TTF_RenderText_Solid(menu_font, strbuffer, (SDL_Color){255, 255, 255});

    // Definition
    *menustate = (MainMenuState){

        .title_banner=SDL_CreateTextureFromSurface(rend, title_surf),
        .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS),
        .level_label=SDL_CreateTextureFromSurface(rend, level_surf)
    };

    // Postprocessing
    MenucodeMap *mcodes = menustate->menucode_map;

    printf("Mapping menu keys...\n");
    Menucode_addMap(mcodes, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN2, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_INCREMENT_VALUE, SDL_SCANCODE_RIGHT, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_DECREMENT_VALUE, SDL_SCANCODE_LEFT, 1, 1, 1);


    printf("Freeing surface...\n");
    SDL_FreeSurface(title_surf);
    SDL_FreeSurface(level_surf);

    return menustate;

}

int MainMenuState_deconstruct(void* self) {

    MainMenuState *menustate = (MainMenuState*)self;


    MenucodeMap_deconstruct(menustate->menucode_map);
    SDL_DestroyTexture(menustate->title_banner);
    SDL_DestroyTexture(menustate->level_label);

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

    // Recast
    ApplicationState *app_state = (ApplicationState*)application_data;
    MainMenuState *menu_state = (MainMenuState*)state_data;

    // Extract for less typing
    SDL_Renderer *rend = app_state->rend;
    bool *menu_codes = menu_state->menucode_states;
    int *hardware_codes = app_state->hardware_states;
    MenucodeMap *keymaps = menu_state->menucode_map;

    char strbuffer[32];  // A general-purpose string buffer


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

    if (Menucode_pressed(menu_codes, MENUCODE_INCREMENT_VALUE)) {
        app_state->init_level++;
        if (menu_state->level_label != NULL) {
            SDL_DestroyTexture(menu_state->level_label);
            menu_state->level_label = NULL;
        }
    }

    if (Menucode_pressed(menu_codes, MENUCODE_DECREMENT_VALUE) && app_state->init_level > 0) {
        app_state->init_level--;
        if (menu_state->level_label != NULL) {
            SDL_DestroyTexture(menu_state->level_label);
            menu_state->level_label = NULL;
        }
    }


    // Draw prep
    if (menu_state->level_label == NULL) {
        snprintf(strbuffer, 32, "Level: %d", app_state->init_level);
        SDL_Surface *level_surf = TTF_RenderText_Solid(
            app_state->menu_font, strbuffer, (SDL_Color){255, 255, 255}
        );
        menu_state->level_label = SDL_CreateTextureFromSurface(rend, level_surf);
        SDL_FreeSurface(level_surf);
    }

    /***** Draw *****/
    int yoffset = 0;
    SDL_Rect title_loc = {.x=10, .y=10};
    SDL_QueryTexture(menu_state->title_banner, NULL, NULL, &title_loc.w, &title_loc.h);
    SDL_RenderCopy(rend, menu_state->title_banner, NULL, &title_loc);
    yoffset += title_loc.h;

    SDL_Rect level_loc = {.x=10, .y=10 + yoffset};
    SDL_QueryTexture(menu_state->level_label, NULL, NULL, &level_loc.w, &level_loc.h);
    SDL_RenderCopy(rend, menu_state->level_label, NULL, &level_loc);
    yoffset += level_loc.h;



    // return STATEFUNC_QUIT;
    return STATEFUNC_CONTINUE;
}
