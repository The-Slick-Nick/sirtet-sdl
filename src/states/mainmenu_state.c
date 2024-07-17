
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

void menufunc_incTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

void menufunc_decTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

void menufunc_incOption(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);


void menufunc_decOption(
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

    // Definition
    *menustate = (MainMenuState){

        // TODO: A constant/config detail for option count?
        .mainmenu=Menu_init(16),

        .init_level=MIN_LEVEL,
        .block_size=INIT_TILE_SIZE,

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

    Menu *mainmenu = menustate->mainmenu;
    int tilesize_idx = Menu_addOption(mainmenu);
    int start_idx = Menu_addOption(mainmenu);

    menustate->menuopt_tilesize = tilesize_idx;
    menustate->menuopt_start = start_idx;
    

    // TODO: Refactor "block_size" here to "tile size"

    /*** Assign Option Commands ***/
    Menu_setCommand(mainmenu, tilesize_idx, MENUCODE_INCREMENT_VALUE, menufunc_incTileSize);
    Menu_setCommand(mainmenu, tilesize_idx, MENUCODE_DECREMENT_VALUE, menufunc_decTileSize);
    Menu_setCommand(mainmenu, start_idx, MENUCODE_SELECT, menufunc_startGame);


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

    SDL_DestroyTexture(menustate->title_banner);
    MenucodeMap_deconstruct(menustate->menucode_map);
    free(menustate->menucode_states);
    Menu_deconstruct(menustate->mainmenu);
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

    MainMenuState *menu_state = (MainMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;


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
    Gamecode_addMap(keymaps, GAMECODE_ROTATE, SDL_SCANCODE_DOWN, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_QUIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_LEFT, SDL_SCANCODE_LEFT, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_RIGHT, SDL_SCANCODE_RIGHT, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_MOVE_UP, SDL_SCANCODE_UP, 1, INT_MAX, move_cd);
    Gamecode_addMap(keymaps, GAMECODE_PAUSE, SDL_SCANCODE_P, 1, 1, 1);

    GameState *new_state = GameState_init(
        app_state->rend, app_state->fonts.vt323_24, keymaps,
        menu_state->init_level,
        menu_state->block_size,
        num_presets, (long*)(block_presets + preset_offset),
        7, palette_prototypes
    );

    StateRunner_addState(
        state_runner, new_state, GameState_run, GameState_deconstruct
    );
}


void menufunc_incTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    MainMenuState *menu_state = (MainMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;

    if (menu_state->block_size >= MAX_TILE_SIZE) {
        return;
    }
    menu_state->block_size++;

    int opt = menu_state->menuopt_tilesize;
    Menu *menu = menu_state->mainmenu;

    Menu_clearLabel(menu, opt);

}

void menufunc_decTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    MainMenuState *menu_state = (MainMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;

    if (menu_state->block_size <= MIN_TILE_SIZE) {
        return;
    }
    menu_state->block_size--;

    int opt = menu_state->menuopt_tilesize;
    Menu *menu = menu_state->mainmenu;

    Menu_clearLabel(menu, opt);
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
    Menu *menu = menu_state->mainmenu;

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
            printf("Menucode %d pressed\n", mc);
            Menu_runCommand(
                menu, mc, state_runner, application_data, state_data
            );
        }
    }

    if (Menucode_pressed(menu_codes, MENUCODE_MOVE_DOWN)) {
        int cur = menu->cur_option;
        int next = Menu_nextOption(menu);

        if (cur != next) {
            Menu_clearLabel(menu, cur);
            Menu_clearLabel(menu, next);
        }
    }

    if (Menucode_pressed(menu_codes, MENUCODE_MOVE_UP)) {
        int cur = menu->cur_option;
        int next = Menu_prevOption(menu);

        if (cur != next) {
            Menu_clearLabel(menu, cur);
            Menu_clearLabel(menu, next);
        }
    }

    /***** Draw Prep *****/

    // manual for now, will generalize perhaps later
    
    int tileop = menu_state->menuopt_tilesize;
    int startop = menu_state->menuopt_start;

    if (Menu_getLabel(menu, tileop) == NULL) {
        printf("Rerendering tilesize label\n");

        SDL_Color lblcol = menu->cur_option == tileop ?
            MENUCOL_ACTIVE : MENUCOL_INACTIVE;

        char buffer[32];
        snprintf(buffer, 32, "Tile Size %d", menu_state->block_size);
        SDL_Surface *surf = TTF_RenderText_Solid(
            menu_state->label_font, buffer, lblcol
        );
        SDL_Texture *lbl = SDL_CreateTextureFromSurface(rend, surf);
        Menu_setLabel(menu, tileop, lbl);
        SDL_FreeSurface(surf);
    }

    if (Menu_getLabel(menu, startop) == NULL) {
        printf("Rerendering start label\n");
        SDL_Color lblcol = menu->cur_option == startop ?
            MENUCOL_ACTIVE : MENUCOL_INACTIVE;

        SDL_Surface *surf = TTF_RenderText_Solid(
            menu_state->label_font, "Start", lblcol
        );

        Menu_setLabel(
            menu, startop,
            SDL_CreateTextureFromSurface(rend, surf)
        );

        SDL_FreeSurface(surf);
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
    // manual for now, to be a Menu method later

    // tile size option
    int size_w, size_h;
    SDL_Texture *size_tex = Menu_getLabel(menu, tileop);
    SDL_QueryTexture(size_tex, NULL, NULL, &size_w, &size_h);
    SDL_Rect size_loc = {
        (wind_w / 2) - (size_w / 2),
        yoffset + size_h / 2,
        size_w, size_h
    };
    yoffset += size_loc.h;
    SDL_RenderCopy(rend, size_tex, NULL, &size_loc);

    // start option
    int start_w, start_h;
    SDL_Texture *start_tex = Menu_getLabel(menu, startop);
    SDL_QueryTexture(start_tex, NULL, NULL, &start_w, &start_h);
    SDL_Rect start_loc = {
        (wind_w / 2) - (start_w / 2),
        yoffset + start_h / 2,
        start_w, start_h
    };
    SDL_RenderCopy(rend, start_tex, NULL, &start_loc);

    return 0;
}


