
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include "component_drawing.h"
#include "inputs.h"
#include "state_runner.h"
#include "settingsmenu_state.h"
#include "application_state.h"


#define MAX_TILE_SIZE 5
#define MIN_TILE_SIZE 3
#define INIT_TILE_SIZE 4

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

void menufunc_incTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

void menufunc_decTileSize(
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

    char buffer[32];
    snprintf(buffer, 32, "Tile Size %d", settings->block_size);

    int tilesize_optn =  TextMenu_addOption(retval->menu, buffer);
    int back_optn = TextMenu_addOption(retval->menu, "Back");

    retval->menuopt_tilesize = tilesize_optn;

    TextMenu_setCommand(retval->menu, back_optn, MENUCODE_SELECT, menufunc_exitSettings);
    TextMenu_setCommand(retval->menu, tilesize_optn, MENUCODE_INCREMENT_VALUE, menufunc_incTileSize);
    TextMenu_setCommand(retval->menu, tilesize_optn, MENUCODE_DECREMENT_VALUE, menufunc_decTileSize);

    // TODO: Add some kind of preset/factory thing for standard menu controls
    Menucode_addMap(retval->menucode_map, MENUCODE_EXIT, SDL_SCANCODE_ESCAPE, 1, 1, 1);
    Menucode_addMap(retval->menucode_map, MENUCODE_MOVE_UP, SDL_SCANCODE_UP, 1, 1000, 30);
    Menucode_addMap(retval->menucode_map, MENUCODE_MOVE_DOWN, SDL_SCANCODE_DOWN, 1, 1000, 30);
    Menucode_addMap(retval->menucode_map, MENUCODE_SELECT, SDL_SCANCODE_RETURN, 1, 1000, 30);
    Menucode_addMap(retval->menucode_map, MENUCODE_SELECT, SDL_SCANCODE_KP_ENTER, 1, 1000, 30);

    Menucode_addMap(retval->menucode_map, MENUCODE_INCREMENT_VALUE, SDL_SCANCODE_RIGHT, 1, 1000, 30);
    Menucode_addMap(retval->menucode_map, MENUCODE_DECREMENT_VALUE, SDL_SCANCODE_LEFT, 1, 1000, 30);


    /*** Block Display Setup ***/
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
    GameSettings *settings = settings_state->settings;

    SDL_Color *palette = settings->palette;


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
        StateRunner_setPopCount(state_runner, 1);
        return 0;
    }

    if (Menucode_pressed(settings_state->menucode_states, MENUCODE_MOVE_UP)) {
        TextMenu_prevOption(settings_state->menu);
    }

    if (Menucode_pressed(settings_state->menucode_states, MENUCODE_MOVE_DOWN)) {
        TextMenu_nextOption(settings_state->menu);
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

    // menu
    SDL_Rect draw_window = {0, 0, wind_w / 2, wind_h};
    SDL_Color i_col = {0, 0, 0};
    SDL_Color a_col = {255, 255, 255};
    TextMenu_draw(
        settings_state->menu, app_state->rend, &draw_window,
        settings_state->menu_font, &a_col,
        settings_state->menu_font, &i_col,
        0
    );


    const int rows = (settings->preset_size / 2) + ((settings->preset_size & 1) == 1);
    const int cellsize_w = (wind_w / 2) / (settings->block_size * 2);
    const int cellsize_h = (wind_h) / (settings->block_size * rows);

    const int cell_size = cellsize_h > cellsize_w ? cellsize_w : cellsize_h;

    Point drawpos = {.x = wind_w / 2, .y = 0};
    for (int block_num = 0; block_num < settings->preset_size; block_num++) {

        SDL_Color drawcol = palette[block_num % settings->palette_size];

        drawBlockContents(
            rend, settings->block_size, settings->block_presets[block_num],
            &drawcol, &drawpos, cell_size, cell_size
        );

        drawpos.x += wind_w / 4;
        if (drawpos.x >= wind_w) {
            drawpos.y += settings->block_size * cell_size;
            drawpos.x = wind_w / 2;
        }
    }

    return 0;
}


/******************************************************************************
 * Menu funcs
 *
 * typedef void (*menufunc_t)(StateRunner*, ApplicationState*, MainMenuState*)
******************************************************************************/

void menufunc_exitSettings(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    printf("Exit settings...\n");
    StateRunner_setPopCount(state_runner, 1);

}


void populatePresets(GameSettings *settings) {

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

    int offset;
    switch (settings->block_size) {
        case 3:
            settings->preset_size = 2;
            offset = 0;
            break;
        case 4:
            settings->preset_size = 7;
            offset = 2;
            break;
        case 5:
            settings->preset_size = 18;
            offset = 9;
            break;
        default:
            printf(
                "Cannot determine a preset for tilesize %d\n",
                settings->block_size
            );
            exit(EXIT_FAILURE);
            break;
    }
    memcpy(
        settings->block_presets,
        (block_presets + offset),
        settings->preset_size * sizeof(long)
    );

}


void menufunc_incTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;
    TextMenu *menu = menu_state->menu;

    if (menu_state->settings->block_size >= MAX_TILE_SIZE) {
        return;
    }
    menu_state->settings->block_size++;
    // TODO: Adjust settings block_presets and preset_size on increment

    char buff[32];
    snprintf(buff, 32, "Tile Size %d\n", menu_state->settings->block_size);

    int opt = menu_state->menuopt_tilesize;
    TextMenu_updateText(menu, opt, buff);

    populatePresets(menu_state->settings);
}

void menufunc_decTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {
    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;
    TextMenu *menu = menu_state->menu;

    if (menu_state->settings->block_size <= MIN_TILE_SIZE) {
        return;
    }
    menu_state->settings->block_size--;
    // TODO: Adjust settings block_presets and preset_size on increment

    char buff[32];
    snprintf(buff, 32, "Tile Size %d\n", menu_state->settings->block_size);

    int opt = menu_state->menuopt_tilesize;
    TextMenu_updateText(menu, opt, buff);
    populatePresets(menu_state->settings);
}




