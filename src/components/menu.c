#include <string.h>
#include <SDL2/SDL.h>

#include "inputs.h"
#include "menu.h"



/******************************************************************************
 * Initialization & deconstruction
******************************************************************************/

static size_t commandArrSize(int max_options) {
    return sizeof(menufunc_t*) * max_options * (int)NUM_MENUCODES;
}

static size_t labelArrSize(int max_options) {
    return sizeof(SDL_Texture*) * max_options;
}


Menu* Menu_init(int max_options) {

    if (max_options < 0)
        { return NULL; }

    size_t size = Menu_requiredBytes(max_options);
    void* mem = malloc(size);
    memset(mem, 0, size);

    Menu *retval = Menu_build(mem, size, max_options);
    return retval;
}


void Menu_deconstruct(Menu* self) {
    free(self);
}


Menu* Menu_build(void *data, size_t data_size, int max_options) {

    if (data_size < Menu_requiredBytes(max_options)) {
        return NULL;
    }

    Menu *menu = (Menu*)data;

    *menu = (Menu) {
        .num_options = 0,
        .max_options = max_options,
        .cur_option = 0,
        .commands = (menufunc_t*)(data + sizeof(Menu)),
        .labels = (SDL_Texture**)(data + sizeof(Menu) + commandArrSize(max_options))

    };
    return menu;
}

size_t Menu_requiredBytes(int max_options) {

    size_t size = sizeof(Menu);

    size += commandArrSize(max_options);
    size += labelArrSize(max_options);

    return size;
}


// TextMenu* TextMenu_init(
//     size_t max_options, size_t max_lbl_size, TTF_Font *active_font, SDL_Color *active_col, TTF_Font *inactive_font, SDL_Color *inactive_col
// );
//
// void TextMenu_deconstruct(TextMenu* self);
TextMenu* TextMenu_init(
    size_t max_options, size_t max_lbl_size,
    TTF_Font *active_font, SDL_Color *active_col,
    TTF_Font *inactive_font, SDL_Color *inactive_col
) {

    TextMenu *menu = (TextMenu*)malloc(sizeof(TextMenu));

    menu->menu = Menu_init(max_options);
    menu->active_font = active_font;
    menu->active_col = *active_col;
    
    menu->inactive_font = inactive_font;
    menu->inactive_col = *inactive_col;

    menu->label_w = max_lbl_size;   
    menu->label_text = (char*)malloc(
        (1 + max_lbl_size)
        * max_options
        * sizeof(char)
    );
    return menu;
}

void TextMenu_deconstruct(TextMenu *self) {

    Menu_deconstruct(self->menu);
    free(self->label_text);
    free(self);
}


/******************************************************************************
 * Menu content operations
******************************************************************************/

int Menu_nextOption(Menu *self) {
    if (self->cur_option + 1 >= self->num_options) {
        return self->cur_option;
    }
    return ++self->cur_option;
}

int Menu_prevOption(Menu *self) {
    if (self->cur_option > 0) {
        return --self->cur_option;
    }
    return self->cur_option;
}

int Menu_addOption(Menu *self) {
    if (self->num_options == self->max_options) {
        return -1;
    }
    return self->num_options++;
}

void Menu_setCommand(
    Menu *self, int index, Menucode menucode, menufunc_t command
) {

    if (index >= self->num_options) {
        return;
    }

    size_t idx = (index * (int)NUM_MENUCODES + (int)menucode);
    self->commands[idx] = command;
}

// run command on the currenctly selected option
void Menu_runCommand(
    Menu *self, Menucode menucode, StateRunner *runner,
    void *app_data, void *state_data
) {

    if (self->num_options == 0) {
        return;
    }

    int idx = (int)NUM_MENUCODES * self->cur_option + (int)menucode;
    menufunc_t command = self->commands[idx];
    if (command != NULL) {
        command(runner, app_data, state_data);
    }

}

int Menu_clearLabel(Menu* self, int index) {
    if (index < 0 || index >= self->num_options) {
        return -1;
    }

    if (self->labels[index] == NULL) {
        return 0;
    }

    SDL_DestroyTexture(self->labels[index]);
    self->labels[index] = NULL;
    return 0;
}


int Menu_setLabel(Menu *self, int index, SDL_Texture *label) {
    if (index < 0 || index >= self->num_options) {
        return -1;
    }

    if (self->labels[index] != NULL) {
        Menu_clearLabel(self, index);
    }

    self->labels[index] = label;

    return 0;
}
    


SDL_Texture* Menu_getLabel(Menu *self, int index) {
    if (index < 0 || index >= self->num_options) {
        return NULL;
    }

    return self->labels[index];
}

/******************************************************************************
 * TextMenu content operations
******************************************************************************/

// NOTE: Do I wrap EVERY Menu method with a TextMenu equivalent,
// or only those where I need to do something special?

int TextMenu_nextOption(TextMenu *self) {
    int prev = self->menu->cur_option;
    int next = Menu_nextOption(self->menu);

    if (prev != next) {
        Menu_clearLabel(self->menu, prev);
        Menu_clearLabel(self->menu, next);
    }
    return next;
}

int TextMenu_prevOption(TextMenu *self) {
    int prev = self->menu->cur_option;
    int next = Menu_prevOption(self->menu);

    if (prev != next) {
        Menu_clearLabel(self->menu, prev);
        Menu_clearLabel(self->menu, next);
    }
    return next;
}

int TextMenu_addOption(TextMenu *self, char *txt) {
    int new_opt = Menu_addOption(self->menu);
    if (new_opt == -1) { 
        return -1;
    }

    if (strlen(txt) > self->label_w) {
        return -1;
    }

    return TextMenu_updateText(self, new_opt, txt);
}


void TextMenu_setCommand(
    TextMenu *self, int index, Menucode menucode, menufunc_t command
) {
    Menu_setCommand(self->menu, index, menucode, command);
}


// run command on the currenctly selected option
void TextMenu_runCommand(
    TextMenu *self, Menucode menucode, StateRunner *runner,
    void *app_data, void *state_data
) {
    Menu_runCommand(self->menu, menucode, runner, app_data, state_data);
}

int TextMenu_updateText(
    TextMenu *self, int optnum, const char* text
) {
    if (strlen(text) > self->label_w) {
        return -1;
    }

    char *idx = self->label_text + (optnum * (self->label_w + 1));
    strcpy(idx, text);

    Menu_clearLabel(self->menu, optnum);
    return 0;
}

/******************************************************************************
 * Menu draw operations
******************************************************************************/

void Menu_draw(Menu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags) {
    
    // NOTE: `flags` argument meant to be a bitmask of different drawing options.
    // (centering, alignment, etc etc)
    // I've not implemented those yet, but wanted to use this interface/argument
    // for future use

    int yoffset = 0;
    for (int opt_i = 0; opt_i < self->num_options; opt_i++) {

        SDL_Texture *label = self->labels[opt_i];

        if (label == NULL) {
            continue;
        }

        int txt_w, txt_h;
        SDL_QueryTexture(label, NULL, NULL, &txt_w, &txt_h);

        SDL_Rect dest = {
            (draw_window->x + (draw_window->w / 2)) - (txt_w / 2),
            (draw_window->y + yoffset) + (txt_h / 2),
            txt_w,
            txt_h
        };
        yoffset += txt_h;
        SDL_RenderCopy(rend, label, NULL, &dest);
    }
}

/******************************************************************************
 * TextMenu draw operations
******************************************************************************/
void TextMenu_draw(
    TextMenu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags
) {
    // TODO: Custom logic to turn NULLs into textures
    Menu_draw(self->menu, rend, draw_window, flags);
}
