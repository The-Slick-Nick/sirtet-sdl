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
