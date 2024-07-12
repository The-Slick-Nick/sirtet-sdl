
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#include "mainmenu_state.h"
#include "application_state.h"
#include "state_runner.h"
#include "inputs.h"
#include "game_state.h"


/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/


#define INIT_BLOCK_SIZE 4
#define MAX_BLOCK_SIZE 5
#define MIN_BLOCK_SIZE 3

/**
 * @brief - Initialize state for the main menu
 * @param rend - SDL_Renderer pointer to render textures with
 * @param menu_font - TTF_Font pointer to render menu options with
 */
MainMenuState* MainMenuState_init(SDL_Renderer *rend, TTF_Font *menu_font) {

    assert(INIT_BLOCK_SIZE >= MIN_BLOCK_SIZE);
    assert(INIT_BLOCK_SIZE <= MAX_BLOCK_SIZE);

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
        menu_font, "Main Menu", (SDL_Color){255, 255, 255}
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

        .init_level=0,
        .block_size=INIT_BLOCK_SIZE,

        .menucode_states=(bool*)calloc((int)NUM_MENUCODES, sizeof(bool)),
        .menucode_map=MenucodeMap_init(MAX_MENUCODE_MAPS),

        .title_banner=SDL_CreateTextureFromSurface(rend, title_surf),
        .level_label=SDL_CreateTextureFromSurface(rend, level_surf),
        .start_label=NULL,
        .blocksize_label=NULL
    };

    if (
        menustate->menucode_states == NULL
        || menustate->menucode_map==NULL
        || menustate->title_banner==NULL
        || menustate->level_label==NULL
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
    SDL_Color col_active = {255, 255, 255};
    SDL_Color col_inactive = {155, 155, 155};


    /***** Inputs *****/
    processMenucodes(menu_codes, hardware_codes, keymaps);

    /***** Process state *****/

    if (Menucode_pressed(menu_codes, MENUCODE_EXIT)) {
        return STATEFUNC_QUIT;
    }

    if (Menucode_pressed(menu_codes, MENUCODE_SELECT)) {

        if (menu_state->menu_selection == 1) {

            long block_presets[3 + 7 + 12] = {
                /* blocksize 3 */
                0b010110000,
                0b010011000,
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
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100,
                0b0010000100001000010000100
            };
            int preset_offset = (
                menu_state->block_size == 3 ? 0 :
                menu_state->block_size == 4 ? 3 :
                menu_state->block_size == 5 ? 10 :
                -1
            );

            int num_presets = (
                menu_state->block_size == 3 ? 3 :
                menu_state->block_size == 4 ? 7 :
                menu_state->block_size == 5 ? 12 :
                8
            );


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
                rend, app_state->menu_font, keymaps,
                menu_state->init_level,
                menu_state->block_size,
                num_presets, (long*)(block_presets + preset_offset),
                7, palette_prototypes
            );

            StateRunner_addState(state_runner, new_state, GameState_run, GameState_deconstruct);
        }
    }

    if (Menucode_pressed(menu_codes, MENUCODE_MOVE_DOWN)) {

        int prev_menu = menu_state->menu_selection;
        int new_menu = prev_menu == 0 ? menu_state->num_options - 1 : prev_menu - 1;

        // TODO: More generic (list of options?) when I have more options
        if ((prev_menu == 0 || new_menu == 0) && menu_state->level_label != NULL) {
            SDL_DestroyTexture(menu_state->level_label);
            menu_state->level_label = NULL;
        }

        if ((prev_menu == 1 || new_menu == 1) && menu_state->start_label != NULL) {
            SDL_DestroyTexture(menu_state->start_label);
            menu_state->start_label = NULL;
        }

        if ((prev_menu == 2 || new_menu == 2) && menu_state->blocksize_label != NULL) {
            SDL_DestroyTexture(menu_state->blocksize_label);
            menu_state->blocksize_label = NULL;
        }

        menu_state->menu_selection = new_menu;

    }

    if (Menucode_pressed(menu_codes, MENUCODE_MOVE_UP)) {

        int prev_menu = menu_state->menu_selection;
        int new_menu = (prev_menu + 1) % menu_state->num_options;

        // TODO: More generic (list of options?) when I have more options
        if ((prev_menu == 0 || new_menu == 0) && menu_state->level_label != NULL) {
            SDL_DestroyTexture(menu_state->level_label);
            menu_state->level_label = NULL;
        }

        if ((prev_menu == 1 || new_menu == 1) && menu_state->start_label != NULL) {
            SDL_DestroyTexture(menu_state->start_label);
            menu_state->start_label = NULL;
        }

        if ((prev_menu == 2 || new_menu == 2) && menu_state->blocksize_label != NULL) {
            SDL_DestroyTexture(menu_state->blocksize_label);
            menu_state->blocksize_label = NULL;
        }


        menu_state->menu_selection = new_menu;
    }


    if (Menucode_pressed(menu_codes, MENUCODE_INCREMENT_VALUE)) {
        if (menu_state->menu_selection == 0) {
            menu_state->init_level++;
            if (menu_state->level_label != NULL) {
                SDL_DestroyTexture(menu_state->level_label);
                menu_state->level_label = NULL;
            }
        }
        else if (menu_state->menu_selection == 2) {
            int adj_size = menu_state->block_size - MIN_BLOCK_SIZE;

            int new_size = ((adj_size + 1) % (1 + MAX_BLOCK_SIZE - MIN_BLOCK_SIZE)) + MIN_BLOCK_SIZE;
            menu_state->block_size = new_size;

            if (menu_state->blocksize_label != NULL) {
                SDL_DestroyTexture(menu_state->blocksize_label);
                menu_state->blocksize_label = NULL;
            }
        }
    }

    if (Menucode_pressed(menu_codes, MENUCODE_DECREMENT_VALUE) && menu_state->init_level > 0) {
        if (menu_state->menu_selection == 0) {
            menu_state->init_level--;
            if (menu_state->level_label != NULL) {
                SDL_DestroyTexture(menu_state->level_label);
                menu_state->level_label = NULL;
            }
        }
    }


    // Draw prep
    if (menu_state->level_label == NULL) {

        SDL_Color level_col = menu_state->menu_selection == 0 ? col_active : col_inactive;

        snprintf(strbuffer, 32, "Level: %d", menu_state->init_level);
        SDL_Surface *level_surf = TTF_RenderText_Solid(app_state->menu_font, strbuffer, level_col);
        menu_state->level_label = SDL_CreateTextureFromSurface(rend, level_surf);
        SDL_FreeSurface(level_surf);
    }

    if (menu_state->start_label == NULL) {
        SDL_Color start_col = menu_state->menu_selection == 1 ? col_active : col_inactive;
        SDL_Surface *start_surf = TTF_RenderText_Solid(app_state->menu_font, "Start", start_col);
        menu_state->start_label = SDL_CreateTextureFromSurface(rend, start_surf);
        SDL_FreeSurface(start_surf);
    }

    if (menu_state->blocksize_label == NULL) {
        SDL_Color blocksize_col = menu_state->menu_selection == 2 ? col_active : col_inactive;
        snprintf(strbuffer, 32, "Blocksize: %d", menu_state->block_size);
        SDL_Surface *blocksize_surf= TTF_RenderText_Solid(app_state->menu_font, strbuffer, blocksize_col);
        menu_state->blocksize_label = SDL_CreateTextureFromSurface(rend, blocksize_surf);
        SDL_FreeSurface(blocksize_surf);
    }

    /***** Draw *****/

    SDL_Window *wind = app_state->wind;
    int wind_w, wind_h;
    SDL_GetWindowSize(wind, &wind_w, &wind_h);
    int yoffset = 0;

    // title
    int title_w, title_h;
    SDL_QueryTexture(menu_state->title_banner, NULL, NULL, &title_w, &title_h);
    SDL_Rect title_loc = {
        .x = (wind_w / 2) - (title_w / 2),
        .y=yoffset + title_h / 2,
        .w=title_w,
        .h=title_h
    };
    SDL_RenderCopy(rend, menu_state->title_banner, NULL, &title_loc);
    yoffset = title_loc.y + title_loc.h;

    // level
    int level_w, level_h;
    SDL_QueryTexture(menu_state->level_label, NULL, NULL, &level_w, &level_h);
    SDL_Rect level_loc = {
        .x = (wind_w / 2) - (level_w / 2),
        .y=yoffset,
        .w=level_w,
        .h=level_h
    };
    SDL_RenderCopy(rend, menu_state->level_label, NULL, &level_loc);
    yoffset += title_loc.h;


    // blocksize
    int size_w, size_h;
    SDL_QueryTexture(menu_state->blocksize_label, NULL, NULL, &size_w, &size_h);
    SDL_Rect size_loc = {
        .x = (wind_w / 2) - (size_w / 2),
        .y=yoffset,
        .w=size_w,
        .h=size_h
    };
    SDL_RenderCopy(rend, menu_state->blocksize_label, NULL, &size_loc);
    yoffset += size_loc.h;

    // start
    int start_w, start_h;
    SDL_QueryTexture(menu_state->start_label, NULL, NULL, &start_w, &start_h);
    SDL_Rect start_loc = {
        .x = (wind_w / 2) - (start_w / 2),
        .y=yoffset,
        .w=start_w,
        .h=start_h
    };
    SDL_RenderCopy(rend, menu_state->start_label, NULL, &start_loc);
    yoffset += title_loc.h;


    return STATEFUNC_CONTINUE;
}
