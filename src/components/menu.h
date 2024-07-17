#ifndef MENU_H
#define MENU_H

#include "state_runner.h"
#include "inputs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

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

/******************************************************************************
 * Initialization & deconstruction
******************************************************************************/


Menu* Menu_init(int max_options);
void Menu_deconstruct(Menu* self);
Menu* Menu_build(void *data, size_t data_size, int max_options);
size_t Menu_requiredBytes(int max_options);


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
 * Menu draw operations
******************************************************************************/

void Menu_draw(Menu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags);

#endif
