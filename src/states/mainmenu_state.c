
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
#include "application_state.h"
#include "state_runner.h"
#include "inputs.h"
#include "game_state.h"


#define INIT_BLOCK_SIZE 4
#define MAX_BLOCK_SIZE 5
#define MIN_BLOCK_SIZE 3

#define MIN_LEVEL 0
#define MAX_LEVEL 10

#define MENUCOL_ACTIVE ((SDL_Color){255, 255, 255})
#define MENUCOL_INACTIVE ((SDL_Color){155, 155, 155})

/******************************************************************************
 * menufunc predeclarations
 *
 * NOTE: These may otherwise go in a header file, but these need not be shared
 * with any other file, I'm choosing to define them here
 *
******************************************************************************/
void menufunc_startGame(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);

void menufunc_incLevel(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);

void menufunc_decLevel(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);

void menufunc_incBlockSize(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);

void menufunc_decBlockSize(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);

void menufunc_incOption(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);


void menufunc_decOption(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
);



/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/



/**
 * @brief - Initialize state for the main menu
 * @param rend - SDL_Renderer pointer to render textures with
 * @param menu_font - TTF_Font pointer to render menu options with
 */
MainMenuState* MainMenuState_init(SDL_Renderer *rend, TTF_Font *menu_font) {

    assert(INIT_BLOCK_SIZE >= MIN_BLOCK_SIZE);
    assert(INIT_BLOCK_SIZE <= MAX_BLOCK_SIZE);
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
        menu_font, "Tetris", MENUCOL_ACTIVE
    );

    if (title_surf == NULL) {
        free(menustate);
        return NULL;
    }

    snprintf(strbuffer, 32, "Level: %d", 0);
    SDL_Surface *level_surf = TTF_RenderText_Solid(menu_font, strbuffer, col_active);

    if (level_surf == NULL) {
        free(menustate);
        SDL_FreeSurface(title_surf);
        return NULL;
    }

    // Definition
    *menustate = (MainMenuState){

        .num_options=3,
        .menu_selection=0,
        .options=(MenuOption*)malloc(3 * sizeof(MenuOption)),

        .init_level=MIN_LEVEL,
        .block_size=INIT_BLOCK_SIZE,

        .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS),

        // TODO: Initialze all as NULL and take advantage of lazy rendering
        .title_banner=SDL_CreateTextureFromSurface(rend, title_surf)
    };

    if (menustate->options == NULL) {
        printf("Error allocating options\n");
        MainMenuState_deconstruct(menustate);
        return NULL;
    }

    for (int i = 0; i < menustate->num_options; i++) {
        for (int gc = 0; gc < NUM_MENUCODES; gc++) {
            menustate->options[i].commands[gc] = NULL;
        }
    }

    strcpy(menustate->options[0].text, "Level 0");
    strcpy(menustate->options[1].text, "Block Size 4");
    strcpy(menustate->options[2].text, "Start Game");

    /*** Assign Option Commands ***/

    menustate->options[0].commands[MENUCODE_INCREMENT_VALUE] = menufunc_incLevel;
    menustate->options[0].commands[MENUCODE_DECREMENT_VALUE] = menufunc_decLevel;

    menustate->options[1].commands[MENUCODE_INCREMENT_VALUE] = menufunc_incBlockSize;
    menustate->options[1].commands[MENUCODE_DECREMENT_VALUE] = menufunc_decBlockSize;

    menustate->options[2].commands[MENUCODE_SELECT] = menufunc_startGame;

    for (int opt_i = 0; opt_i < menustate->num_options; opt_i++) {
        menustate->options[opt_i].commands[MENUCODE_MOVE_UP] = menufunc_decOption;
        menustate->options[opt_i].commands[MENUCODE_MOVE_DOWN] = menufunc_incOption;
    }


    if (
        menustate->menucode_states == NULL
        || menustate->menucode_map == NULL
        || menustate->options == NULL
    ) {
        MainMenuState_deconstruct(menustate);
        return NULL;
    }

    // Postprocessing
    MenucodeMap *mcodes = menustate->menucode_map;

    printf("Mapping menu keys...\n");
    Menucode_addMap(mcodes, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_RETURN2, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_SELECT, SDL_SCANCODE_KP_ENTER, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_INCREMENT_VALUE, SDL_SCANCODE_RIGHT, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_DECREMENT_VALUE, SDL_SCANCODE_LEFT, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_MOVE_UP, SDL_SCANCODE_UP, 1, 1, 1);
    Menucode_addMap(mcodes, MENUCODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, 1, 1);

    return menustate;
}

int MainMenuState_deconstruct(void* self) {

    MainMenuState *menustate = (MainMenuState*)self;

    for (int opt_i = 0; opt_i < menustate->num_options; opt_i++) {
        SDL_DestroyTexture(menustate->options[opt_i].label);
    }
    free(menustate->options);

    MenucodeMap_deconstruct(menustate->menucode_map);

    free(self);
    return 0;
}


/******************************************************************************
 * Menu funcs
 *
 * typedef void (*menufunc_t)(StateRunner*, ApplicationState*, MainMenuState*)
******************************************************************************/

void menufunc_startGame(StateRunner *state_runner, ApplicationState *app_state, MainMenuState *menu_state) {

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
        menu_state->block_size == 3 ? 0 :
        menu_state->block_size == 4 ? 2 :
        menu_state->block_size == 5 ? 9 :
        -1
    );

    int num_presets = (
        menu_state->block_size == 3 ? 2 :
        menu_state->block_size == 4 ? 7 :
        menu_state->block_size == 5 ? 18 :
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

    GamecodeMap *keymaps = GamecodeMap_init(MAX_GAMECODE_MAPS);

    int move_cd = TARGET_FPS / 15;
    Gamecode_addMap(keymaps, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_ROTATE, SDL_SCANCODE_UP, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_PAUSE, SDL_SCANCODE_P, 1, 1, 1);

    GameState *new_state = GameState_init(
        app_state->rend, app_state->menu_font, keymaps,
        menu_state->init_level,
        menu_state->block_size,
        num_presets, (long*)(block_presets + preset_offset),
        7, palette_prototypes
    );

    StateRunner_addState(
        state_runner, new_state, GameState_run, GameState_deconstruct
    );
}


void menufunc_incLevel(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
) {


    MenuOption *opt = &menu_state->options[menu_state->menu_selection];

    if (menu_state->init_level >= MAX_LEVEL) {
        return;
    }
    menu_state->init_level++;

    if (opt->label != NULL) {
        SDL_DestroyTexture(opt->label);
        opt->label = NULL;
    }

    snprintf(opt->text, 32, "Level %d", menu_state->init_level);
}

void menufunc_decLevel(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
) {

    if (menu_state->init_level <= MIN_LEVEL) {
        return;
    }
    menu_state->init_level--;

    MenuOption *opt = &menu_state->options[menu_state->menu_selection];

    if (opt->label != NULL) {
        SDL_DestroyTexture(opt->label);
        opt->label = NULL;
    }

    snprintf(opt->text, 32, "Level %d", menu_state->init_level);
}


void menufunc_incBlockSize(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
) {

    if (menu_state->block_size >= MAX_BLOCK_SIZE) {
        return;
    }
    menu_state->block_size++;

    MenuOption *opt = &menu_state->options[menu_state->menu_selection];

    if (opt->label != NULL) {
        SDL_DestroyTexture(opt->label);
        opt->label = NULL;
    }

    snprintf(opt->text, 32, "Block Size %d", menu_state->block_size);
}

void menufunc_decBlockSize(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
) {

    if (menu_state->block_size <= MIN_BLOCK_SIZE) {
        return;
    }
    menu_state->block_size--;

    MenuOption *opt = &menu_state->options[menu_state->menu_selection];

    if (opt->label != NULL) {
        SDL_DestroyTexture(opt->label);
        opt->label = NULL;
    }

    snprintf(opt->text, 32, "Block Size %d", menu_state->block_size);
}

void menufunc_incOption(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
) {

    if (menu_state->menu_selection >= menu_state->num_options - 1) {
        return;
    }

    MenuOption *prev_opt = &menu_state->options[menu_state->menu_selection];
    MenuOption *new_opt = &menu_state->options[menu_state->menu_selection + 1];
    
    SDL_DestroyTexture(prev_opt->label);
    SDL_DestroyTexture(new_opt->label);

    prev_opt->label = NULL;
    new_opt->label = NULL;

    menu_state->menu_selection++;
}


void menufunc_decOption(
    StateRunner *state_runner, ApplicationState *app_state,
    MainMenuState *menu_state
) {

    if (menu_state->menu_selection <= 0) {
        return;
    }

    MenuOption *prev_opt = &menu_state->options[menu_state->menu_selection];
    MenuOption *new_opt = &menu_state->options[menu_state->menu_selection - 1];
    
    SDL_DestroyTexture(prev_opt->label);
    SDL_DestroyTexture(new_opt->label);

    prev_opt->label = NULL;
    new_opt->label = NULL;

    menu_state->menu_selection--;
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
    SDL_Color col_active = {255, 255, 255};
    SDL_Color col_inactive = {155, 155, 155};


    /***** Inputs *****/
    processMenucodes(menu_codes, hardware_codes, keymaps);

    /***** Process state *****/

    if (Menucode_pressed(menu_codes, MENUCODE_EXIT)) {
        return STATEFUNC_QUIT;
    }

    for (int mc = 0; mc < NUM_MENUCODES; mc++) {

        // use func map for given option index
        if (Menucode_pressed(menu_codes, mc)) {

            int selection = menu_state->menu_selection;
            menufunc_t mfunc = menu_state->options[selection].commands[mc];

            if (mfunc != NULL) {
                mfunc(state_runner, app_state, menu_state);
            }

        }
    }

    // Draw prep
    for (int opt_i = 0; opt_i < menu_state->num_options; opt_i++) {

        MenuOption *opt = &menu_state->options[opt_i];
        if (opt->label != NULL) {
            continue;
        }

        SDL_Color label_col = (
            menu_state->menu_selection == opt_i
            ? (SDL_Color){255, 255, 255}
            : (SDL_Color){155, 155, 155}
        );

        SDL_Surface *label_surf = TTF_RenderText_Solid(
            app_state->menu_font, opt->text, label_col
        );

        opt->label = SDL_CreateTextureFromSurface(app_state->rend, label_surf);
        SDL_FreeSurface(label_surf);

    }

    /***** Draw *****/

    // title
    SDL_Window *wind = app_state->wind;
    int wind_w, wind_h;
    SDL_GetWindowSize(wind, &wind_w, &wind_h);
    int yoffset = 0;


    int title_w, title_h;
    SDL_QueryTexture(menu_state->title_banner, NULL, NULL, &title_w, &title_h);
    SDL_Rect title_loc = {
        .x = (wind_w / 2) - (title_w / 2),
        .y=yoffset + title_h / 2,
        .w=title_w,
        .h=title_h
    };
    SDL_RenderCopy(rend, menu_state->title_banner, NULL, &title_loc);
    yoffset += title_loc.y + title_loc.h;

    // options
    for (int opt_i = 0; opt_i < menu_state->num_options; opt_i++) {
        MenuOption *opt = &menu_state->options[opt_i];
        int label_w, label_h;

        if (opt->label == NULL) {
            continue;
        }
        SDL_QueryTexture(opt->label, NULL, NULL, &label_w, &label_h);
        SDL_Rect label_loc = {
            .x = (wind_w / 2) - (label_w / 2),
            .y=yoffset,
            .w=label_w,
            .h=label_h
        };

        SDL_RenderCopy(app_state->rend, opt->label, NULL, &label_loc);
        yoffset += label_h;
    }

