#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#include "sirtet.h"
#include "sirtet_audio.h"
#include "backgrounds.h"
#include "colorpalette.h"
#include "hiscores_state.h"
#include "mainmenu_state.h"
#include "menu.h"
#include "state_runner.h"
#include "inputs.h"
#include "application_state.h"
#include "settingsmenu_state.h"
#include "game_state.h"


/******************************************************************************
 * menufunc predeclarations
 *
 * NOTE: These may otherwise go in a header file, but these need not be shared
 * with any other file, I'm choosing to define them here
 *
******************************************************************************/
void menufunc_startGame(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

void menufunc_exitGame(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

void menufunc_openSettings(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

void menufunc_openHiscores(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/


/**
 * @brief - Initialize state for the main menu
 * @param rend - SDL_Renderer pointer to render textures with
 * @param menu_font - TTF_Font pointer to render menu options with
 */
MainMenuState* MainMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, SDL_Texture *title_logo,
    SDL_Texture *bg_tl, SDL_Texture *bg_tr, SDL_Texture *bg_bl,
    SDL_Texture *bg_br,
    SirtetAudio_sound menusound_move
) {

    assert(INIT_TILE_SIZE >= MIN_TILE_SIZE);
    assert(INIT_TILE_SIZE <= MAX_TILE_SIZE);
    assert(MIN_LEVEL >= 0);
    assert(MIN_LEVEL <= MAX_LEVEL);
    
    // return value
    MainMenuState *menustate = (MainMenuState*)malloc(sizeof(MainMenuState));
    if (menustate == NULL) {
        Sirtet_setError("Error initializing MainMenuState memory\n");
        return NULL;
    }

    // Preprocessing
    SDL_Surface *title_surf = TTF_RenderText_Solid(
        menu_font, "Sirtet", MENUCOL_ACTIVE
    );

    if (title_surf == NULL) {
        char buff[64];
        // snprintf(char *restrict s, size_t maxlen, const char *restrict format, ...)
        snprintf(buff, 64, "Error rendering text: %s\n", SDL_GetError());
        Sirtet_setError(buff);
        free(menustate);
        return NULL;
    }


    /*** Initialization ***/

    *menustate = (MainMenuState){

        .settings = GameSettings_init(32, 32), 
        .mainmenu=TextMenu_init(16, 32, menusound_move),
        .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS),

        .label_font=menu_font,
        .title_logo=title_logo,
        .title_banner=SDL_CreateTextureFromSurface(rend, title_surf)
    };
    SDL_FreeSurface(title_surf);

    if (menustate->mainmenu == NULL) {
        Sirtet_setError("Error allocating memory for options\n");
        return NULL;
    }


    /*** Display setup (labels/texture/bgs) ***/

    menustate->background = (PanningBg) {
        .xpos = 0,
        .ypos = 0,
        .xvel = 0.866,  // cos(30deg)
        .yvel = 0.500,  // sin(30deg)

        .topleft = bg_tl,
        .topright = bg_tr,
        .bottomleft = bg_bl,
        .bottomright = bg_br
    };


    /*** Settings defaults ***/

    GameSettings *settings = menustate->settings;
    if (settings == NULL) {
        Sirtet_setError("Error allocating memory for GameSettings\n");
        return NULL;
    }

    // Constants
    menustate->settings->init_level = MIN_LEVEL;
    menustate->settings->block_size = INIT_TILE_SIZE;

    // Key mapping

    const int mv_delay_factor = 3;  // delay until key repeat

    int move_cd = TARGET_FPS / 15;
    GamecodeMap *keymaps = menustate->settings->keymaps;

    GamecodeMap_addMap(keymaps, GAMECODE_ROTATE, SDL_SCANCODE_DOWN, 1, 1, 1);
    GamecodeMap_addMap(keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);

    GamecodeMap_addMap(keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, 1, 1);
    GamecodeMap_addMap(keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1 + (mv_delay_factor * move_cd), INT_MAX, move_cd);

    GamecodeMap_addMap(keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, 1, 1);
    GamecodeMap_addMap(keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1 + (mv_delay_factor * move_cd), INT_MAX, move_cd);

    GamecodeMap_addMap(keymaps, GAMECODE_MOVE_UP, SDL_SCANCODE_UP, 1, 1, 1);
    GamecodeMap_addMap(keymaps, GAMECODE_MOVE_UP, SDL_SCANCODE_UP, 1 + (mv_delay_factor * move_cd), INT_MAX, move_cd);

    GamecodeMap_addMap(keymaps, GAMECODE_PAUSE, SDL_SCANCODE_P, 1, 1, 1);
    GamecodeMap_addMap(keymaps, GAMECODE_HARD_DROP, SDL_SCANCODE_SPACE, 1, 1, 1);


    // Block presets
    long init_presets[7] = {
        0b0100010001000100,
        0b0000011001100000,
        0b0100010001100000,
        0b0010001001100000,
        0b0000010011100000,
        0b0011011000000000,
        0b1100011000000000
    };
    GameSettings_setPresets(settings, INIT_TILE_SIZE, 7, init_presets);

    // Color palette

    settings->palette = ColorPalette_initVa(
        "Default", 7, 
        (SDL_Color){190,83,28, 255},
        (SDL_Color){218,170,0, 255},
        (SDL_Color){101,141,27, 255},
        (SDL_Color){0,95,134, 255},
        (SDL_Color){155,0,0, 255},
        (SDL_Color){0,155,0, 255},
        (SDL_Color){0,0,155, 255}
    );


    /*** Menu configuration ***/

    menustate->mainmenu = TextMenu_init(16, 32, menusound_move);
    menustate->menucode_states = (bool*)calloc((int)NUM_MENUCODES, sizeof(bool));
    menustate->menucode_map = MenucodeMap_init(MAX_MENUCODE_MAPS);

    TextMenu *mainmenu = menustate->mainmenu;

    int start_idx = TextMenu_addOption(mainmenu, "Start");
    int settings_idx = TextMenu_addOption(mainmenu, "Settings");
    int hiscores_idx = TextMenu_addOption(mainmenu, "High Scores");
    int exit_idx = TextMenu_addOption(mainmenu, "Exit");

    menustate->menuopt_start = start_idx;
    menustate->menuopt_exit = exit_idx;

    TextMenu_setCommand(mainmenu, start_idx, MENUCODE_SELECT, menufunc_startGame);
    TextMenu_setCommand(mainmenu, exit_idx, MENUCODE_SELECT, menufunc_exitGame);
    TextMenu_setCommand(mainmenu, settings_idx, MENUCODE_SELECT, menufunc_openSettings);
    TextMenu_setCommand(mainmenu, hiscores_idx, MENUCODE_SELECT, menufunc_openHiscores);


    MenucodeMap *mcodes = menustate->menucode_map;
    MenucodePreset_standard(mcodes, 1, 1, 1);

    /*** ***/

    return menustate;
}

int MainMenuState_deconstruct(void* self) {

    // the error is here
    MainMenuState *menustate = (MainMenuState*)self;

    SDL_DestroyTexture(menustate->title_banner);
    MenucodeMap_deconstruct(menustate->menucode_map);
    free(menustate->menucode_states);
    TextMenu_deconstruct(menustate->mainmenu);
    GameSettings_deconstruct(menustate->settings);

    free(self);
    return 0;
}


/******************************************************************************
 * Menu funcs
 *
 * typedef void (*menufunc_t)(StateRunner*, ApplicationState*, MainMenuState*)
******************************************************************************/

void menufunc_startGame(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    /*** Unwrapping ***/
    MainMenuState *menu_state = (MainMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;
    GameSettings *settings = menu_state->settings;

    GameState *new_state = GameState_init(
        app_state->rend, app_state->fonts.vt323_24,
        settings,
        app_state->sounds.boop,
        app_state->sounds.boop_scale,
        app_state->sounds.boop_scale_reverse
    );

    StateRunner_addState(
        state_runner, new_state, GameState_run, GameState_deconstruct
    );
}

void menufunc_exitGame(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {
    StateRunner_setPopCount(state_runner, 1);
}

void menufunc_openSettings(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    ApplicationState *app_state = (ApplicationState*)app_data;
    MainMenuState *menu_state = (MainMenuState*)menu_data;

    SDL_Renderer *rend = app_state->rend;

    SettingsMenuState *new_state = SettingsMenuState_init(
        rend, app_state->fonts.vt323_24, menu_state->settings,
        app_state->sounds.short_click
    );

    if (new_state == NULL) {
        printf("Error in menufunc_openSettings:\n    %s\n:", Sirtet_getError());
    }

    StateRunner_addState(
        state_runner, (void*)new_state, 
        SettingsMenuState_run,
        SettingsMenuState_deconstruct
    );

}

void menufunc_openHiscores(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    ApplicationState *app_state = (ApplicationState*)app_data;

    HiscoresState *new_state = HiscoresState_init(
        app_state->rend, app_state->fonts.vt323_24, app_state->hiscores);

    if (new_state == NULL) {
        printf(
            "Error in menufunc_openHiscores:\n    %s\n:", Sirtet_getError());
        exit(1);
    }

    StateRunner_addState(
        state_runner, new_state, HiscoresState_run, HiscoresState_deconstruct);
}


/******************************************************************************
 * State running
******************************************************************************/


int MainMenuState_run(
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
    TextMenu *menu = menu_state->mainmenu;

    /***** Inputs *****/

    processMenucodes(menu_codes, hardware_codes, keymaps);


    /***** Process state *****/

    SDL_Rect bg_dims = {.x=0, .y=0};
    SDL_GetWindowSize(app_state->wind, &bg_dims.w, &bg_dims.h);

    if (PanningBg_move(&menu_state->background, &bg_dims) != 0) {
        printf("%s", Sirtet_getError());
        return -1;
    }


    if (Menucode_pressed(menu_codes, MENUCODE_EXIT)) {
        StateRunner_setPopCount(state_runner, 1);
        return 0;
    }

    for (int mc = 0; mc < NUM_MENUCODES; mc++) {

        // use func map for given option index
        if (Menucode_pressed(menu_codes, mc)) {
            TextMenu_runCommand(
                menu, mc, state_runner, application_data, state_data
            );
        }
    }

    if (Menucode_pressed(menu_codes, MENUCODE_MOVE_DOWN)) {
        TextMenu_nextOption(menu);
    }

    if (Menucode_pressed(menu_codes, MENUCODE_MOVE_UP)) {
        TextMenu_prevOption(menu);
    }

    

    /***** Draw *****/

    SDL_Color bg = BACKGROUNDCOL;
    SDL_SetRenderDrawColor(rend, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(rend);

    if (PanningBg_draw(&menu_state->background, rend, &bg_dims)) {
        printf("%s", Sirtet_getError());
        return -1;
    }


    const int option_padding = 24;

    SDL_Window *wind = app_state->wind;
    int wind_w, wind_h;
    SDL_GetWindowSize(wind, &wind_w, &wind_h);
    int yoffset = 0;

    // title
    int title_w, title_h;
    SDL_QueryTexture(menu_state->title_logo, NULL, NULL, &title_w, &title_h);

    SDL_Rect title_loc = {
        .x = (wind_w / 2) - (title_w / 2),
        .y = yoffset + title_h / 2,
        .w = title_w,
        .h = title_h
    };

    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderFillRect(rend, &title_loc);

    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawRect(rend, &title_loc);


    SDL_RenderCopy(rend, menu_state->title_logo, NULL, &title_loc);
    yoffset += title_loc.y + title_loc.h + option_padding;

    // options
    SDL_Rect draw_window = {0, yoffset, wind_w, wind_h - yoffset};

    SDL_Color actcol = MENUCOL_ACTIVE;
    SDL_Color inaccol = MENUCOL_INACTIVE;

    TextMenu_draw(
        menu, rend, &draw_window,
        menu_state->label_font, &actcol,
        menu_state->label_font, &inaccol,
        0
    );

    return 0;
}


