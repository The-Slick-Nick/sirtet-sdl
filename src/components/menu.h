#ifndef MENU_H
#define MENU_H

#include "state_runner.h"
#include "inputs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

/******************************************************************************
 * Type declarations
******************************************************************************/


// Takes a StateRunner, pointer to "global" state, and pointer to "local" state
typedef void (*menufunc_t)(StateRunner*, void*, void*);

typedef struct {
    int cur_option;
    int num_options;
    int max_options;

    menufunc_t *commands;  // 2D array of a menucode mapped to an option
    SDL_Texture **labels;
} Menu;


// A wrapper over Menu that automatically handles label generation & recreation,
// at the cost of restricting menu textures to text only
typedef struct {

    Menu *menu;

    TTF_Font *active_font;
    TTF_Font *inactive_font;

    SDL_Color active_col;
    SDL_Color inactive_col;

    size_t label_w;
    char *label_text;
} TextMenu;

/******************************************************************************
 * Initialization & deconstruction
******************************************************************************/


Menu* Menu_init(int max_options);
void Menu_deconstruct(Menu* self);
Menu* Menu_build(void *data, size_t data_size, int max_options);
size_t Menu_requiredBytes(int max_options);


TextMenu* TextMenu_init(
    size_t max_options, size_t max_lbl_size, TTF_Font *active_font, SDL_Color *active_col, TTF_Font *inactive_font, SDL_Color *inactive_col
);

void TextMenu_deconstruct(TextMenu* self);


/******************************************************************************
 * Menu content operations
******************************************************************************/

int Menu_nextOption(Menu *self);
int Menu_prevOption(Menu *self);


int Menu_addOption(Menu *self);
int Menu_clearLabel(Menu* self, int index);
SDL_Texture* Menu_getLabel(Menu *self, int index);
int Menu_setLabel(Menu *self, int index, SDL_Texture *label);


void Menu_setCommand(
    Menu *self, int index, Menucode menucode, menufunc_t command
);
// run command on the currenctly selected option
void Menu_runCommand(
    Menu *self, Menucode menucode, StateRunner *runner,
    void *app_data, void *state_data
);

/******************************************************************************
 * TextMenu content operations
******************************************************************************/


// NOTE: Do I wrap EVERY Menu method with a TextMenu equivalent,
// or only those where I need to do something special?

int TextMenu_nextOption(TextMenu *self);
int TextMenu_prevOption(TextMenu *self);
int TextMenu_addOption(TextMenu *self, char *txt);


void TextMenu_setCommand(
    TextMenu *self, int index, Menucode menucode, menufunc_t command
);
// run command on the currenctly selected option
void TextMenu_runCommand(
    TextMenu *self, Menucode menucode, StateRunner *runner,
    void *app_data, void *state_data
);

char* TextMenu_getLabelText(TextMenu *self, int optnum);
int TextMenu_updateText(TextMenu *self, int optnum, const char* text);

/******************************************************************************
 * Menu draw operations
******************************************************************************/

void Menu_draw(Menu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags);

/******************************************************************************
 * TextMenu draw operations
******************************************************************************/
void TextMenu_draw(TextMenu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags);

#endif
