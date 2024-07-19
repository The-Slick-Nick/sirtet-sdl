
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include "inputs.h"
#include "state_runner.h"
#include "settingsmenu_state.h"
#include "application_state.h"



/******************************************************************************
 * menufunc predeclarations
 *
 * NOTE: These may otherwise go in a header file, but these need not be shared
 * with any other file, I'm choosing to define them here
 *
******************************************************************************/

void menufunc_exitSettings(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

SettingsMenuState* SettingsMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, GameSettings *settings
) {

    size_t n = sizeof(SettingsMenuState);
    SettingsMenuState *retval = (SettingsMenuState*)calloc(1, n);

    // NOTE: This pointer is not created by the SettingsMenuState, thus it
    // will not be freed by it
    retval->settings = settings;
    retval->menu_font = menu_font;

    /*** Menu Setup ***/

    retval->menu = TextMenu_init(4, 32);
    retval->menucode_states = (bool*)calloc(NUM_MENUCODES, sizeof(bool));
    retval->menucode_map = MenucodeMap_init(MAX_MENUCODE_MAPS);
    TextMenu_addOption(retval->menu, "Test 1");
    TextMenu_addOption(retval->menu, "Test 2");

    int back_optn = TextMenu_addOption(retval->menu, "Back");
    TextMenu_setCommand(
        retval->menu, back_optn, MENUCODE_SELECT, menufunc_exitSettings
    );

    Menucode_addMap(retval->menucode_map, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 100, 1);

    return retval;
}

int SettingsMenuState_deconstruct(void *self) {

    SettingsMenuState *settings = (SettingsMenuState*)self;
    TextMenu_deconstruct(settings->menu);
    MenucodeMap_deconstruct(settings->menucode_map);
    free(self);
    return 0;
}



/******************************************************************************
 * State running
******************************************************************************/


int SettingsMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
) {

    /*** Recasting ***/
    ApplicationState *app_state = (ApplicationState*)application_data;
    SettingsMenuState *settings_state = (SettingsMenuState*)state_data;

    /*** Unpacking ***/
    SDL_Renderer *rend = app_state->rend;
    SDL_Window *wind = app_state->wind;

    /*** Inputs ***/
    processMenucodes(
        settings_state->menucode_states,
        app_state->hardware_states,
        settings_state->menucode_map
    );

    if (app_state->hardware_states[(int)SDL_SCANCODE_Q] > 0) {
        printf("Hardware states for q is %d\n", app_state->hardware_states[(int)SDL_SCANCODE_Q]);
        StateRunner_setPopCount(state_runner, 1);
    }


    /*** Logic ***/
    if (Menucode_pressed(settings_state->menucode_states, MENUCODE_EXIT)) {
        printf("Exit pressed\n");
        StateRunner_setPopCount(state_runner, 1);
        return 0;
    }

    for (int mc = 0; mc < NUM_MENUCODES; mc++) {
        if (Menucode_pressed(settings_state->menucode_states, mc)) {

            TextMenu_runCommand(
                settings_state->menu, mc, state_runner,
                application_data, state_data
            );
        }
    }


    /*** Draw ***/

    int wind_w, wind_h;
    SDL_Color bgcol = {155, 155, 155};

    SDL_SetRenderDrawColor(rend, bgcol.r, bgcol.g, bgcol.b, bgcol.a);
    SDL_RenderClear(rend);

    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);

    SDL_Rect draw_window = {0, 0, wind_w, wind_h};
    SDL_Color a_col = {255, 255, 255};
    SDL_Color i_col = {155, 155, 155};
    TextMenu_draw(
        settings_state->menu, app_state->rend, &draw_window,
        settings_state->menu_font, &a_col,
        settings_state->menu_font, &i_col,
        0
    );

    return 0;
}


/******************************************************************************
 * menufunc definition
******************************************************************************/

void menufunc_exitSettings(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    printf("Exit settings...\n");
    StateRunner_setPopCount(state_runner, 1);

}
