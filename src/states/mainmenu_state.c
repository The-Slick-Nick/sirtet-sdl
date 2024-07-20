
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

#include "mainmenu_state.h"
#include "menu.h"
#include "state_runner.h"
#include "inputs.h"
#include "application_state.h"
#include "settingsmenu_state.h"
#include "game_state.h"


#define INIT_TILE_SIZE 4
#define MAX_TILE_SIZE 5
#define MIN_TILE_SIZE 3

#define MIN_LEVEL 0
#define MAX_LEVEL 10

#define MENUCOL_ACTIVE ((SDL_Color){0, 0, 0})
#define MENUCOL_INACTIVE ((SDL_Color){100, 100, 100})
#define BACKGROUNDCOL ((SDL_Color){155, 155, 155})

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


/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/



/**
 * @brief - Initialize state for the main menu
 * @param rend - SDL_Renderer pointer to render textures with
 * @param menu_font - TTF_Font pointer to render menu options with
 */
MainMenuState* MainMenuState_init(
    SDL_Renderer *rend, TTF_Font *menu_font, SDL_Texture *title_logo) {

    assert(INIT_TILE_SIZE >= MIN_TILE_SIZE);
    assert(INIT_TILE_SIZE <= MAX_TILE_SIZE);
    assert(MIN_LEVEL >= 0);
    assert(MIN_LEVEL <= MAX_LEVEL);

    SDL_Color col_active = {255, 255, 255};
    SDL_Color col_inactive = {155, 155, 155};

    // General purpose buffer to use for strings
    char strbuffer[32];

    // return value
    MainMenuState *menustate = (MainMenuState*)malloc(sizeof(MainMenuState));
    if (menustate == NULL) {
        return NULL;
    }

    // Preprocessing
    SDL_Surface *title_surf = TTF_RenderText_Solid(
        menu_font, "Sirtet", MENUCOL_ACTIVE
    );

    if (title_surf == NULL) {
        free(menustate);
        return NULL;
    }

    SDL_Color activecol = MENUCOL_ACTIVE;
    SDL_Color inaccol = MENUCOL_INACTIVE;


    /*** Initialization ***/

    *menustate = (MainMenuState){

        // TODO: A constant/config detail for option count?

        .settings = GameSettings_init(16, 16),
        .mainmenu=TextMenu_init(16, 32),
        .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS),

        .label_font=menu_font,
        .title_logo=title_logo,
        .title_banner=SDL_CreateTextureFromSurface(rend, title_surf)
    };
    SDL_FreeSurface(title_surf);

    if (menustate->mainmenu == NULL) {
        printf("Error allocating options\n");
        exit(EXIT_FAILURE);
    }


    /*** Settings defaults ***/

    GameSettings *settings = menustate->settings;
    if (settings == NULL) {
        printf("Error allocating GameSettings\n");
        exit(EXIT_FAILURE);
    }

    // TODO: Add a Gamecode_clearMaps to clear all maps
    menustate->settings->init_level = MIN_LEVEL;
    menustate->settings->block_size = INIT_TILE_SIZE;

    int move_cd = TARGET_FPS / 15;
    GamecodeMap *keymaps = menustate->settings->keymaps;
    Gamecode_addMap(keymaps, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_ROTATE, SDL_SCANCODE_DOWN, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_UP, SDL_SCANCODE_UP, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_PAUSE, SDL_SCANCODE_P, 1, 1, 1);


    /*** Menu configuration ***/

    menustate->mainmenu = TextMenu_init(16, 32);
    menustate->menucode_states = (bool*)calloc((int)NUM_MENUCODES, sizeof(bool));
    menustate->menucode_map = MenucodeMap_init(MAX_MENUCODE_MAPS);

    TextMenu *mainmenu = menustate->mainmenu;

    int start_idx = TextMenu_addOption(mainmenu, "Start");
    int exit_idx = TextMenu_addOption(mainmenu, "Exit");
    int settings_idx = TextMenu_addOption(mainmenu, "Settings");

    menustate->menuopt_start = start_idx;
    menustate->menuopt_exit = exit_idx;

    // TODO: Refactor "block_size" here to "tile size"
    TextMenu_setCommand(mainmenu, start_idx, MENUCODE_SELECT, menufunc_startGame);
    TextMenu_setCommand(mainmenu, exit_idx, MENUCODE_SELECT, menufunc_exitGame);
    TextMenu_setCommand(mainmenu, settings_idx, MENUCODE_SELECT, menufunc_openSettings);

    MenucodeMap *mcodes = menustate->menucode_map;
    Menucode_addMap(mcodes, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN2, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_KP_ENTER, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_INCREMENT_VALUE, SDL_SCANCODE_RIGHT, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_DECREMENT_VALUE, SDL_SCANCODE_LEFT, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_MOVE_UP, SDL_SCANCODE_UP, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, 1, 1);

    /*** ***/

    return menustate;
}

int MainMenuState_deconstruct(void* self) {

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

    // TODO: Move settings defaults into GameState_init method once
    // I have a settings menu implemented (intent is to default on
    // initialization, with the option to change later)
    long block_presets[2 + 7 + 18] = {
        /* blocksize 3 */
        0b010110000,
        0b010010010,

        /* blocksize 4 */
        0b0100010001000100,
        0b0000011001100000,
        0b0100010001100000,
        0b0010001001100000,
        0b0000010011100000,
        0b0011011000000000,
        0b1100011000000000,

        /* blocksize 5 */
        0b0000000110011000010000000,
        0b0000001100001100010000000,
        0b0010000100001000010000100,
        0b0010000100001000011000000,
        0b0010000100001000110000000,
        0b0010000100011000100000000,
        0b0010000100001100001000000,
        0b0000001100011000010000000,
        0b0000000110001100010000000,
        0b0000001110001000010000000,
        0b0000001010011100000000000,
        0b0010000100001110000000000,
        0b0000001000011000011000000,
        0b0000000100011100010000000,
        0b0010000100001100010000000,
        0b0010000100011000010000000,
        0b0000001100001000011000000,
        0b0000001100001000011000000
    };
    int preset_offset = (
        menu_state->settings->block_size == 3 ? 0 :
        menu_state->settings->block_size == 4 ? 2 :
        menu_state->settings->block_size == 5 ? 9 :
        -1
    );

    int num_presets = (
        menu_state->settings->block_size == 3 ? 2 :
        menu_state->settings->block_size == 4 ? 7 :
        menu_state->settings->block_size == 5 ? 18 :
        -1 
    );

    assert(preset_offset != -1);
    assert(num_presets != -1);


    // TODO: Add configurations for the following
    SDL_Color palette_prototypes[7] = {
        (SDL_Color){190,83,28},
        (SDL_Color){218,170,0},
        (SDL_Color){101,141,27},
        (SDL_Color){0,95,134},
        (SDL_Color){155,0,0},
        (SDL_Color){0,155,0},
        (SDL_Color){0,0,155}
    };


    settings->preset_size = num_presets;
    memcpy(
        settings->block_presets, (long*)(block_presets + preset_offset),
        num_presets * sizeof(long)
    );

    settings->palette_size = 7;
    memcpy(settings->palette, palette_prototypes, 7 * sizeof(SDL_Color));

    GameState *new_state = GameState_init(
        app_state->rend, app_state->fonts.vt323_24,
        settings
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
        rend, app_state->fonts.vt323_24, menu_state->settings
    );

    StateRunner_addState(
        state_runner, (void*)new_state, 
        SettingsMenuState_run,
        SettingsMenuState_deconstruct
    );

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

    char strbuffer[32];  // A general-purpose string buffer
    SDL_Color col_active = {255, 255, 255};
    SDL_Color col_inactive = {155, 155, 155};


    /***** Inputs *****/
    processMenucodes(menu_codes, hardware_codes, keymaps);

    /***** Process state *****/

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


    const int option_padding = 24;

    SDL_Window *wind = app_state->wind;
    int wind_w, wind_h;
    SDL_GetWindowSize(wind, &wind_w, &wind_h);
    int yoffset = 0;

    // title
    // aspect ratio of logo is 3:1
    int title_w = (3 * wind_w) / 4;
    int title_h = title_w / 3;
    SDL_Rect title_loc = {
        .x = (wind_w / 2) - (title_w / 2),
        .y=yoffset + title_h / 2,
        .w=title_w,
        .h=title_h
    };
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


