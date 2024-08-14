
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

#include "colorpalette.h"
#include "sirtet.h"
#include "component_drawing.h"
#include "assert.h"
#include "game_state.h"
#include "inputs.h"
#include "menu.h"
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
    SDL_Renderer *rend, TTF_Font *menu_font, GameSettings *settings,
    SirtetAudio_sound menusound_move
) {

    assert(MIN_TILE_SIZE <= MAX_TILE_SIZE);
    assert(MIN_TILE_SIZE <= INIT_TILE_SIZE);
    assert(INIT_TILE_SIZE <= MAX_TILE_SIZE);

    size_t n = sizeof(SettingsMenuState);
    SettingsMenuState *retval = (SettingsMenuState*)calloc(1, n);

    // NOTE: This pointer is not created by the SettingsMenuState, thus it
    // will not be freed by it
    retval->settings = settings;
    retval->menu_font = menu_font;


    /*** Block presets ***/

    long** preset_arr = (long**)calloc(3, sizeof(long*));
    if (preset_arr == NULL) {
        Sirtet_setError("Error allocating preset array in SettingMenuState\n");
        return NULL;
    }

    retval->blocksize_sel = settings->block_size;
    retval->presets = preset_arr;
    if (
        (preset_arr[0] = (long*)calloc(2, sizeof(long))) == NULL ||
        (preset_arr[1] = (long*)calloc(7, sizeof(long))) == NULL ||
        (preset_arr[2] = (long*)calloc(18, sizeof(long))) == NULL
    )
    {
        Sirtet_setError("Error allocating preset array in SettingsMenuState\n");
        return NULL;
    }


    // size 3
    size_t idx = 0;
    preset_arr[0][idx++] = 0b010110000;
    preset_arr[0][idx++] = 0b010010010;

    // size 4
    idx = 0;
    preset_arr[1][idx++] = 0b0100010001000100;
    preset_arr[1][idx++] = 0b0000011001100000;
    preset_arr[1][idx++] = 0b0100010001100000;
    preset_arr[1][idx++] = 0b0010001001100000;
    preset_arr[1][idx++] = 0b0000010011100000;
    preset_arr[1][idx++] = 0b0011011000000000;
    preset_arr[1][idx++] = 0b1100011000000000;

    // size 5
    idx = 0;
    preset_arr[2][idx++] = 0b0000000110011000010000000;
    preset_arr[2][idx++] = 0b0000001100001100010000000;
    preset_arr[2][idx++] = 0b0010000100001000010000100;
    preset_arr[2][idx++] = 0b0010000100001000011000000;
    preset_arr[2][idx++] = 0b0010000100001000110000000;
    preset_arr[2][idx++] = 0b0010000100011000100000000;
    preset_arr[2][idx++] = 0b0010000100001100001000000;
    preset_arr[2][idx++] = 0b0000001100011000010000000;
    preset_arr[2][idx++] = 0b0000000110001100010000000;
    preset_arr[2][idx++] = 0b0000001110001000010000000;
    preset_arr[2][idx++] = 0b0000001010011100000000000;
    preset_arr[2][idx++] = 0b0010000100001110000000000;
    preset_arr[2][idx++] = 0b0000001000011000011000000;
    preset_arr[2][idx++] = 0b0000000100011100010000000;
    preset_arr[2][idx++] = 0b0010000100001100010000000;
    preset_arr[2][idx++] = 0b0010000100011000010000000;
    preset_arr[2][idx++] = 0b0000001100001000011000000;
    preset_arr[2][idx++] = 0b0000001100001000011000000;


    /*** Color Palette Presets ***/

    retval->palette_selection = 0;
    retval->num_palettes = 4;

    size_t sz = sizeof(ColorPalette*);
    retval->palettes = (ColorPalette**)calloc(retval->num_palettes, sz);

    // TODO: STRETCH: Figure out a way to make this a parsable config file or something
    retval->palettes[0] = ColorPalette_initVa(
        "Default", 7, 
        (SDL_Color){190,83,28, 255},
        (SDL_Color){218,170,0, 255},
        (SDL_Color){101,141,27, 255},
        (SDL_Color){0,95,134, 255},
        (SDL_Color){155,0,0, 255},
        (SDL_Color){0,155,0, 255},
        (SDL_Color){0,0,155, 255}
    );

    retval->palettes[1] = ColorPalette_initVa(
        "Pastels", 8, 
        (SDL_Color){255, 173, 173, 255},
        (SDL_Color){255, 214, 165, 255},
        (SDL_Color){253, 255, 182, 255},
        (SDL_Color){202, 255, 191, 255},
        (SDL_Color){155, 246, 255, 255},
        (SDL_Color){160, 196, 255, 255},
        (SDL_Color){189, 178, 255, 255},
        (SDL_Color){255, 198, 255, 255}
    );

    retval->palettes[2] = ColorPalette_initVa(
        "Beach", 4, 
        (SDL_Color){250, 255, 175, 255},
        (SDL_Color){150, 201, 244, 255},
        (SDL_Color){63, 162, 246, 255},
        (SDL_Color){15, 103, 177, 255},
        (SDL_Color){15, 103, 177, 255}
    );

    retval->palettes[3] = ColorPalette_initVa(
        "Coffee", 5, 
        (SDL_Color){55, 34, 20, 255},
        (SDL_Color){121, 66, 40, 255},
        (SDL_Color){184, 115, 58, 255},
        (SDL_Color){190, 194, 203, 255},
        (SDL_Color){52, 48, 44, 255}
    );


    for (int i = 0; i < retval->num_palettes; i++) {
        if (retval->palettes[i] == NULL) {
            char buff[128];
            snprintf(
                buff, 128,
                "Error in establishing color palettes:\n    %s\n",
                Sirtet_getError()
            );
            Sirtet_setError(buff);
            return NULL;
        }
    }


    /*** Menu Setup ***/

    retval->menu = TextMenu_init(4, 64, menusound_move);
    retval->menucode_states = (bool*)calloc(NUM_MENUCODES, sizeof(bool));
    retval->menucode_map = MenucodeMap_init(MAX_MENUCODE_MAPS);

    char buffer[64];

    snprintf(buffer, 64, "Tile Size %d", settings->block_size);
    int tilesize_optn =  TextMenu_addOption(retval->menu, buffer);

    snprintf(buffer, 64, "Palette: %s", retval->palettes[0]->name);
    int palette_optn = TextMenu_addOption(retval->menu, buffer);

    int back_optn = TextMenu_addOption(retval->menu, "Back");

    retval->menuopt_tilesize = tilesize_optn;
    retval->menuopt_palette = palette_optn;

    TextMenu_setCommand(retval->menu, back_optn, MENUCODE_SELECT, menufunc_exitSettings);
    TextMenu_setCommand(retval->menu, tilesize_optn, MENUCODE_INCREMENT_VALUE, menufunc_incTileSize);
    TextMenu_setCommand(retval->menu, tilesize_optn, MENUCODE_DECREMENT_VALUE, menufunc_decTileSize);
    TextMenu_setCommand(retval->menu, palette_optn, MENUCODE_DECREMENT_VALUE, menufunc_prevPalette);
    TextMenu_setCommand(retval->menu, palette_optn, MENUCODE_INCREMENT_VALUE, menufunc_nextPalette);

    MenucodePreset_standard(retval->menucode_map, 1, 1, 1);



    return retval;
}

int SettingsMenuState_deconstruct(void *self) {

    /* The deconstruction function for this menu state is twofold -
     * (1) Copy the settings picked by the user during this state into
     *     the linked settings struct
     * (2) Free all memory
     *
     * By doing it this way, we can avoid new memory allocation and freeing
     * during the lifetime of the state. (The alternative, and previously used,
     * method was to copy chosen values into settings each time a value was
     * modified, thus resulting in a lot of alloc's and free's)
     *
     */


    /*** Unpacking/recasting ***/

    SettingsMenuState *settingsmenu = (SettingsMenuState*)self;
    GameSettings *settings = settingsmenu->settings;


    /*** Copy Size Selection ***/

    size_t block_size = settingsmenu->blocksize_sel;
    size_t preset_sz = (
        block_size == 3 ? 2 :
        block_size == 4 ? 7 :
        block_size == 5 ? 18 :  
        -1
    );
    assert(preset_sz >= 0);

    int idx = block_size - MIN_TILE_SIZE;
    assert(idx >= 0 && idx <= MAX_TILE_SIZE - MIN_TILE_SIZE);
    GameSettings_setPresets(
        settings, block_size, preset_sz, settingsmenu->presets[idx]);


    /*** Copy Palette Selection ***/

    GameSettings_setPalette(
        settings,
        settingsmenu->palettes[settingsmenu->palette_selection]
    );


    /*** Free Memory ***/

    TextMenu_deconstruct(settingsmenu->menu);
    MenucodeMap_deconstruct(settingsmenu->menucode_map);

    for (size_t sizei = 0; sizei <= MAX_TILE_SIZE - MIN_TILE_SIZE; sizei++) {
        free(settingsmenu->presets[sizei]);
    }
    free(settingsmenu->presets);

    for (size_t palidx = 0; palidx < settingsmenu->num_palettes; palidx++) {
        ColorPalette_deconstruct(settingsmenu->palettes[palidx]);
    }
    free(settingsmenu->palettes);

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
    ColorPalette *palette = settings_state->palettes[settings_state->palette_selection];


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


    // Blocks

    int block_size = settings_state->blocksize_sel;
    long *presets = settings_state->presets[block_size - 3];
    int preset_sz = (
        block_size == 3 ? 2 :
        block_size == 4 ? 7 :
        block_size == 5 ? 18 :  
        -1
    );
    assert(preset_sz >= 0);

    const int rows = (preset_sz / 2) + ((preset_sz & 1) == 1);
    const int cellsize_w = (wind_w / 2) / (block_size * 2);
    const int cellsize_h = (wind_h) / (block_size * rows);

    const int cell_size = cellsize_h > cellsize_w ? cellsize_w : cellsize_h;

    Point drawpos = {.x = wind_w / 2, .y = 0};
    for (int block_num = 0; block_num < preset_sz; block_num++) {

        SDL_Color drawcol;
        ColorPalette_getColor(palette, block_num % palette->size, &drawcol);

        drawBlockContents(
            rend, block_size,
            presets[block_num],
            &drawcol, &drawpos, cell_size, cell_size
        );

        drawpos.x += wind_w / 4;
        if (drawpos.x >= wind_w) {
            drawpos.y += block_size * cell_size;
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


void menufunc_incTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {

    /*** Recasting ***/
    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;


    /*** Unpacking ***/
    TextMenu *menu = menu_state->menu;
    size_t *block_size = &menu_state->blocksize_sel;  // selected block size
    int *menuopt = &menu_state->menuopt_tilesize;     // textmenu option index

    if (*block_size >= MAX_TILE_SIZE) {
        return;
    }
    (*block_size)++;

    char buff[32];
    snprintf(buff, 32, "Tile Size %ld\n", *block_size);

    TextMenu_updateText(menu, *menuopt, buff);
}

void menufunc_decTileSize(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {
    /*** Recasting ***/
    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;
    ApplicationState *app_state = (ApplicationState*)app_data;


    /*** Unpacking ***/
    TextMenu *menu = menu_state->menu;
    size_t *block_size = &menu_state->blocksize_sel;  // selected block size
    int *menuopt = &menu_state->menuopt_tilesize;     // textmenu option index

    if (*block_size <= MIN_TILE_SIZE) {
        return;
    }
    (*block_size)--;

    char buff[32];
    snprintf(buff, 32, "Tile Size %ld\n", *block_size);

    TextMenu_updateText(menu, *menuopt, buff);
}


// Use the next palette
static void menufunc_nextPalette(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {


    /*** Recasting ***/

    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;


    /*** Unpacking ***/

    int *selection = &menu_state->palette_selection;
    ColorPalette **palettes = menu_state->palettes;
    TextMenu *menu = menu_state->menu;
    int palette_opt = menu_state->menuopt_palette;



    if (*selection + 1 >= menu_state->num_palettes) {
        return;
    }
    (*selection)++;
    ColorPalette *this_palette = palettes[*selection];

    char newlbl[128];
    snprintf(newlbl, 128, "Palette: %s", this_palette->name);
    TextMenu_updateText(menu, palette_opt, newlbl);
}

// Use the previous palette
static void menufunc_prevPalette(
    StateRunner *state_runner, void *app_data,
    void *menu_data
) {
    /*** Recasting ***/
    SettingsMenuState *menu_state = (SettingsMenuState*)menu_data;

    int *selection = &menu_state->palette_selection;

    if (*selection <= 0) {
        return;
    }
    (*selection)--;

    char newlbl[128];
    snprintf(newlbl, 128, "Palette: %s", menu_state->palettes[*selection]->name);
    TextMenu_updateText(menu_state->menu, menu_state->menuopt_palette, newlbl);
}


