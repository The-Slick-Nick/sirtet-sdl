#ifndef MENU_H
#define MENU_H

#include "state_runner.h"
#include "inputs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

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

    Mix_Chunk *move_sound;  // Sound effect upon moving options


} Menu;


// A wrapper over Menu that automatically handles label generation & recreation,
// at the cost of restricting menu textures to text only
typedef struct {

    Menu *menu;

    TTF_Font *prev_active_font;
    TTF_Font *prev_inac_font;

    SDL_Color prev_active_col;
    SDL_Color prev_inac_col;

    size_t label_w;
    char *label_text;
} TextMenu;

/******************************************************************************
 * Initialization & deconstruction
******************************************************************************/

// Initialize a Menu struct, returning a pointer to it
Menu* Menu_init(int max_options, Mix_Chunk *move_sound);

// Frees all memory associated with a Menu
void Menu_deconstruct(Menu* self);

// Set up a Menu struct with a given block of memory
Menu* Menu_build(
    void *data, size_t data_size, int max_options,
    Mix_Chunk *move_sound
);

// Identify the number of bytes required for a Menu struct
size_t Menu_requiredBytes(int max_options);

// Initialize a TextMenu struct, returning a pointer to it
TextMenu* TextMenu_init(
    size_t max_options, size_t max_lbl_size,
    Mix_Chunk *move_sound
);

// Free all memory associated with a TextMenu
void TextMenu_deconstruct(TextMenu* self);


/******************************************************************************
 * Menu content operations
******************************************************************************/
// Move a Menu's option to the next one, returning the new option index
int Menu_nextOption(Menu *self);
// Move a Menu's option to the previous one, returning the new option index
int Menu_prevOption(Menu *self);

// Add a new option to the menu, not exceeding max_options provided at _init.
// Return the option index created
int Menu_addOption(Menu *self);

// Destroy the Label at the given index, setting its pointer to NULL
int Menu_clearLabel(Menu* self, int index);

// Retrieve a pointer to the label texture for a given option
SDL_Texture* Menu_getLabel(Menu *self, int index);

// Set the label texture for a given option to one that is provided.
int Menu_setLabel(Menu *self, int index, SDL_Texture *label);

// Assign a function to be run on a given option in response to a menucode
void Menu_setCommand(
    Menu *self, int index, Menucode menucode, menufunc_t command
);

// Run the Menu's current option's assigned command
void Menu_runCommand(
    Menu *self, Menucode menucode, StateRunner *runner,
    void *app_data, void *state_data
);

/******************************************************************************
 * TextMenu content operations
******************************************************************************/

// Move a TextMenu's option to the next one, returning the new option index.
int TextMenu_nextOption(TextMenu *self);

// Move a TextMenu's option to the previous one, returning the new option index
int TextMenu_prevOption(TextMenu *self);

// Add a new option to the TextMenu, not exceeding max_options provided at _init.
// Return the option index created
int TextMenu_addOption(TextMenu *self, const char *txt);

// Assign a function to be run on a given option in response to a menucode
void TextMenu_setCommand(
    TextMenu *self, int index, Menucode menucode, menufunc_t command
);

// Run the TextMenu's current option's assigned command
void TextMenu_runCommand(
    TextMenu *self, Menucode menucode, StateRunner *runner,
    void *app_data, void *state_data
);

// Get a pointer to the text associated with a given option
char* TextMenu_getLabelText(TextMenu *self, int optnum);

// Update the text for a given option
int TextMenu_updateText(TextMenu *self, int optnum, const char* text);

/******************************************************************************
 * Menu draw operations
******************************************************************************/

// Draw the elements of a Menu within the provided region
int Menu_draw(Menu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags);

/******************************************************************************
 * TextMenu draw operations
******************************************************************************/

// Draw the elements of a TextMenu within the provided region
int TextMenu_draw(
    TextMenu *self, SDL_Renderer *rend, SDL_Rect *draw_window,
    TTF_Font *active_font, SDL_Color *active_col,
    TTF_Font *inac_font, SDL_Color *inac_col,
    int options
);


#endif
