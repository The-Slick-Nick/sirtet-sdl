/******************************************************************************
 * hiscores_state.h
 *
 * Defines a state acting as a menu reporting on the highscores (
 * as stored in global ApplicationState)
******************************************************************************/

#ifndef HISCORES_STATE_H
#define HISCORES_STATE_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "hiscores.h"
#include "state_runner.h"
#include "inputs.h"


// NOTE: ScoreList *hiscores is in ApplicationState, so is not stored here
typedef struct {

    size_t n_lbls;
    SDL_Texture **name_lbls; // array of name lbl pointers
    SDL_Texture **score_lbls; // array of score lbl pointers

    bool *menucode_states;
    MenucodeMap *mcodes;


} HiscoresState;


HiscoresState* HiscoresState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font, ScoreList *hiscores);

int HiscoresState_deconstruct(void *self);




/******************************************************************************
 *
******************************************************************************/




/******************************************************************************
 * State running
******************************************************************************/
int HiscoresState_run(StateRunner *runner, void *app_data, void *state_data);

// int HiscoresState_run(
//     StateRunner *state_runner, void *app_data, void *state_data
// );

// int MainMenuState_run(
//     StateRunner *state_runner, void *application_data, void *state_data
// ) {
//     
//     /***** Recasting & Extraction *****/
//
//     // Recast
//     ApplicationState *app_state = (ApplicationState*)application_data;
//     MainMenuState *menu_state = (MainMenuState*)state_data;
//
//     // Extract for less typing
//     SDL_Renderer *rend = app_state->rend;
//     bool *menu_codes = menu_state->menucode_states;
//     int *hardware_codes = app_state->hardware_states;
//     MenucodeMap *keymaps = menu_state->menucode_map;
//     TextMenu *menu = menu_state->mainmenu;
//
//     char strbuffer[32];  // A general-purpose string buffer
//     SDL_Color col_active = {255, 255, 255};
//     SDL_Color col_inactive = {155, 155, 155};
//
//
//     /***** Inputs *****/
//     processMenucodes(menu_codes, hardware_codes, keymaps);
//
//     /***** Process state *****/
//
//     if (Menucode_pressed(menu_codes, MENUCODE_EXIT)) {
//         StateRunner_setPopCount(state_runner, 1);
//         return 0;
//     }
//
//     for (int mc = 0; mc < NUM_MENUCODES; mc++) {
//
//         // use func map for given option index
//         if (Menucode_pressed(menu_codes, mc)) {
//             TextMenu_runCommand(
//                 menu, mc, state_runner, application_data, state_data
//             );
//         }
//     }
//
//     if (Menucode_pressed(menu_codes, MENUCODE_MOVE_DOWN)) {
//         TextMenu_nextOption(menu);
//     }
//
//     if (Menucode_pressed(menu_codes, MENUCODE_MOVE_UP)) {
//         TextMenu_prevOption(menu);
//     }
//
//     
//
//     /***** Draw *****/
//
//     SDL_Color bg = BACKGROUNDCOL;
//     SDL_SetRenderDrawColor(rend, bg.r, bg.g, bg.b, bg.a);
//     SDL_RenderClear(rend);
//
//
//     const int option_padding = 24;
//
//     SDL_Window *wind = app_state->wind;
//     int wind_w, wind_h;
//     SDL_GetWindowSize(wind, &wind_w, &wind_h);
//     int yoffset = 0;
//
//     // title
//     // aspect ratio of logo is 3:1
//     int title_w = (3 * wind_w) / 4;
//     int title_h = title_w / 3;
//     SDL_Rect title_loc = {
//         .x = (wind_w / 2) - (title_w / 2),
//         .y=yoffset + title_h / 2,
//         .w=title_w,
//         .h=title_h
//     };
//     SDL_RenderCopy(rend, menu_state->title_logo, NULL, &title_loc);
//     yoffset += title_loc.y + title_loc.h + option_padding;
//
//     // options
//     SDL_Rect draw_window = {0, yoffset, wind_w, wind_h - yoffset};
//
//     SDL_Color actcol = MENUCOL_ACTIVE;
//     SDL_Color inaccol = MENUCOL_INACTIVE;
//
//     TextMenu_draw(
//         menu, rend, &draw_window,
//         menu_state->label_font, &actcol,
//         menu_state->label_font, &inaccol,
//         0
//     );
//
//     return 0;
// }
//
//

#endif
