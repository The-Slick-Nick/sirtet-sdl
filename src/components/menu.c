#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>

#include "inputs.h"
#include "sirtet.h"
#include "menu.h"



/******************************************************************************
 * Initialization & deconstruction
******************************************************************************/

static inline size_t commandArrSize(int max_options) {
    return sizeof(menufunc_t*) * max_options * (int)NUM_MENUCODES;
}

static inline size_t labelArrSize(int max_options) {
    return sizeof(SDL_Texture*) * max_options;
}


Menu* Menu_init(int max_options, Mix_Chunk *move_sound) {

    if (max_options < 0)
        { return NULL; }

    size_t size = Menu_requiredBytes(max_options);
    void* mem = malloc(size);
    memset(mem, 0, size);

    Menu *retval = Menu_build(mem, size, max_options, move_sound);
    return retval;
}


void Menu_deconstruct(Menu* self) {
    free(self);
}


Menu* Menu_build(
    void *data, size_t data_size, int max_options,
    Mix_Chunk *move_sound
) {

    if (data_size < Menu_requiredBytes(max_options)) {
        return NULL;
    }

    Menu *menu = (Menu*)data;

    *menu = (Menu) {
        .num_options = 0,
        .max_options = max_options,
        .cur_option = 0,
        .commands = (menufunc_t*)(data + sizeof(Menu)),
        .labels = (SDL_Texture**)(data + sizeof(Menu) + commandArrSize(max_options)),
        .move_sound = move_sound
    };
    return menu;
}

size_t Menu_requiredBytes(int max_options) {

    size_t size = sizeof(Menu);

    size += commandArrSize(max_options);
    size += labelArrSize(max_options);

    return size;
}


TextMenu* TextMenu_init(
    size_t max_options, size_t max_lbl_size,
    Mix_Chunk *move_sound
) {

    TextMenu *menu = (TextMenu*)malloc(sizeof(TextMenu));

    menu->menu = Menu_init(max_options, move_sound);
    menu->prev_inac_col = (SDL_Color){0, 0, 0, 0};
    menu->prev_active_col = (SDL_Color){0, 0, 0, 0};

    menu->prev_inac_font = NULL;
    menu->prev_active_font = NULL;

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

    // TODO: Do we need to pass a preferred channel?
    if (self->move_sound != NULL) {
        Mix_PlayChannel(-1, self->move_sound, 1);
    }
    return ++self->cur_option;
}

int Menu_prevOption(Menu *self) {
    if (self->cur_option > 0) {

        // TODO: Do we need to pass a preferred channel?
        if (self->move_sound != NULL) {
            Mix_PlayChannel(-1, self->move_sound, 1);
        }

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

int TextMenu_addOption(TextMenu *self, const char *txt) {
    int new_opt = Menu_addOption(self->menu);
    if (new_opt == -1) { 
        return -1;
    }

    if (strlen(txt) > self->label_w) {
        return -1;
    }

    if (TextMenu_updateText(self, new_opt, txt) == -1) {
        return -1;
    }

    return new_opt;
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

char* TextMenu_getLabelText(TextMenu *self, int optnum) {

    if (optnum >= self->menu->num_options || optnum < 0) {
        return NULL;
    }

    int offset = optnum * (self->label_w + 1);
    return (char*)(self->label_text + offset);

}

/******************************************************************************
 * Menu draw operations
******************************************************************************/

int Menu_draw(Menu *self, SDL_Renderer *rend, SDL_Rect *draw_window, int flags) {
    
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
    return 0;
}

/******************************************************************************
 * TextMenu draw operations
******************************************************************************/

int TextMenu_draw(
    TextMenu *self, SDL_Renderer *rend, SDL_Rect *draw_window,
    TTF_Font *active_font, SDL_Color *active_col,
    TTF_Font *inac_font, SDL_Color *inac_col,
    int flags
) {

    SDL_Color default_col = {0,0,0,0};
    if (active_col == NULL) {
         active_col = &default_col;
    }

    if (inac_col == NULL) {
         inac_col = &default_col;
    }

    bool rerender_active = (
           active_col->r != self->prev_active_col.r
        || active_col->g != self->prev_active_col.g
        || active_col->b != self->prev_active_col.b
        || active_col->a != self->prev_active_col.a
    );

    bool rerender_inac = (
           inac_col->r != self->prev_inac_col.r
        || inac_col->g != self->prev_inac_col.g
        || inac_col->b != self->prev_inac_col.b
        || inac_col->a != self->prev_inac_col.a
    );

    for (int optnum = 0; optnum < self->menu->num_options; optnum++) {

        bool isactive = (optnum == self->menu->cur_option);
        SDL_Color rend_col;
        TTF_Font *rend_font;

        if (isactive) {
            if (rerender_active) {
                Menu_clearLabel(self->menu, optnum);
            }
            rend_col = *active_col;
            rend_font = active_font;
        }
        else {
            if (rerender_inac) {
                Menu_clearLabel(self->menu, optnum);
            }
            rend_col = *inac_col;
            rend_font = inac_font;
        }

        if (Menu_getLabel(self->menu, optnum) != NULL) {
            continue;
        }

        if (rend_font == NULL) {
            Menu_clearLabel(self->menu, optnum);
            continue;
        }

        char *text = TextMenu_getLabelText(self, optnum);

        SDL_Surface *lbl_surf = TTF_RenderText_Solid(rend_font, text, rend_col);
        if (lbl_surf == NULL) {
            char buff[64];
            snprintf(buff, 64, "Error creaing label: %s\n", TTF_GetError());
            Sirtet_setError(buff);
            return -1;
        }

        SDL_Texture *lbl_texture = SDL_CreateTextureFromSurface(rend, lbl_surf);
        if (lbl_texture == NULL) {
            char buff[64];
            snprintf(buff, 64, "Error creating label: %s\n", SDL_GetError());
            Sirtet_setError(buff);
            return -1;
        }

        Menu_setLabel(self->menu, optnum, lbl_texture);
    }

    return Menu_draw(self->menu, rend, draw_window, flags);
}

