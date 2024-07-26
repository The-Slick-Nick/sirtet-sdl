
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <string.h>

#include "sirtet.h"
#include "component_drawing.h"
#include "assert.h"
#include "game_state.h"
#include "inputs.h"
#include "menu.h"
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

static void menufunc_nextPalette(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);

static void menufunc_prevPalette(
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
    int palette_optn = TextMenu_addOption(retval->menu, "Color Palette");
    int back_optn = TextMenu_addOption(retval->menu, "Back");

    retval->menuopt_tilesize = tilesize_optn;

    TextMenu_setCommand(retval->menu, back_optn, MENUCODE_SELECT, menufunc_exitSettings);
    TextMenu_setCommand(retval->menu, tilesize_optn, MENUCODE_INCREMENT_VALUE, menufunc_incTileSize);
    TextMenu_setCommand(retval->menu, tilesize_optn, MENUCODE_DECREMENT_VALUE, menufunc_decTileSize);
    TextMenu_setCommand(retval->menu, palette_optn, MENUCODE_DECREMENT_VALUE, menufunc_prevPalette);
    TextMenu_setCommand(retval->menu, palette_optn, MENUCODE_INCREMENT_VALUE, menufunc_nextPalette);

    MenucodePreset_standard(retval->menucode_map);


    /*** Block Display Setup ***/

    retval->palette_selection = 0;
    retval->n_palette_presets = 2;

    retval->palette_sizes = (size_t*)calloc(
        retval->n_palette_presets,
        sizeof(size_t)
    );
    if (retval->palette_sizes == NULL) {
        // TODO: Set error and exit
        return NULL;
    }

    // TODO: Create a struct/API for ColorPalette, because this is awful:
    retval->palette_sizes[0] = 7;
    retval->palette_sizes[1] = 8;

    size_t to_alloc = 0;
    for (int i = 0; i < retval->n_palette_presets; i++) {
        to_alloc += retval->palette_sizes[i];
    }

    retval->palette_presets = (SDL_Color*)calloc(to_alloc, sizeof(SDL_Color));
    if (retval->palette_presets == NULL) {
        // TODO: Set error and exit
        return NULL;
    }

    // TODO: Figure out how to set this within some config file or something
    SDL_Color preset_1[7] = {
        (SDL_Color){190,83,28, 255},
        (SDL_Color){218,170,0, 255},
        (SDL_Color){101,141,27, 255},
        (SDL_Color){0,95,134, 255},
        (SDL_Color){155,0,0, 255},
        (SDL_Color){0,155,0, 255},
        (SDL_Color){0,0,155, 255}
    };

    SDL_Color preset_2[8] = {
        (SDL_Color){255, 173, 173, 255},
        (SDL_Color){255, 214, 165, 255},
        (SDL_Color){253, 255, 182, 255},
        (SDL_Color){202, 255, 191, 255},
        (SDL_Color){155, 246, 255, 255},
        (SDL_Color){160, 196, 255, 255},
        (SDL_Color){189, 178, 255, 255},
        (SDL_Color){255, 198, 255, 255},
    };

    memcpy(retval->palette_presets + 0, preset_1, 7 * sizeof(SDL_Color));
    memcpy(retval->palette_presets + 7, preset_2, 8 * sizeof(SDL_Color));

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
            rend, settings->block_size,
            settings->block_presets[block_num], // <-- CULPRIT
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


static int populatePresets(GameSettings *settings) {

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
    char buff[64];
    size_t sz;
    long *preset_loc;
    switch (settings->block_size) {
        case 3:
            sz = 2;
            preset_loc = block_presets;
            break;
        case 4:
            sz = 7;
            preset_loc = (long*)(block_presets + 2);
            break;
        case 5:
            sz = 18;
            preset_loc = (long*)(block_presets + 9);
            break;
        default:
            snprintf(
                buff, 64, "Cannot determine a preset for tilesize %d\n",
                settings->block_size
            );
            Sirtet_setError(buff);
            // TODO: Identify a proper error return
            return -1;
    }

    GameSettings_setPresets(settings, sz, preset_loc);
    return 0;
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

    char buff[32];
    snprintf(buff, 32, "Tile Size %d\n", menu_state->settings->block_size);

    int opt = menu_state->menuopt_tilesize;
    TextMenu_updateText(menu, opt, buff);
    populatePresets(menu_state->settings);
}


// Use the next palette
static void menufunc_nextPalette(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {


    /*** Recasting ***/
    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;


    /*** Unpacking ***/
    GameSettings *settings = menu_state->settings;
    int *selection = &menu_state->palette_selection;
    SDL_Color *presets = menu_state->palette_presets;


    if (*selection + 1 >= menu_state->n_palette_presets) {
        return;
    }
    (*selection)++;
    
    // TODO: Extract below logic to inline func to use in both next & prev
    size_t offset = 0;
    for (int i = 0; i < *selection; i++) {
        offset += menu_state->palette_sizes[i];
    }
 
    // set settings palette
    size_t size = menu_state->palette_sizes[*selection];
    settings->palette_size = menu_state->palette_sizes[*selection];
    memcpy(settings->palette, (SDL_Color*)(presets + offset), size * sizeof(SDL_Color));
}

// Use the previous palette
static void menufunc_prevPalette(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {
    /*** Recasting ***/
    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;


    /*** Unpacking ***/
    GameSettings *settings = menu_state->settings;
    int *selection = &menu_state->palette_selection;
    SDL_Color *presets = menu_state->palette_presets;

    if (*selection <= 0) {
        return;
    }
    (*selection)--;

    size_t offset = 0;
    for (int i = 0; i < *selection; i++) {
        offset += menu_state->palette_sizes[i];
    }
 
    // set settings palette
    size_t size = menu_state->palette_sizes[*selection];
    settings->palette_size = menu_state->palette_sizes[*selection];
    memcpy(settings->palette, (SDL_Color*)(presets + offset), size * sizeof(SDL_Color));

}


