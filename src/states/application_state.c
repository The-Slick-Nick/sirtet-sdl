#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#include "application_state.h"


/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

ApplicationState* ApplicationState_init(char *asset_folder) {

    SDL_Init(SDL_INIT_VIDEO);
    assert(TTF_Init() == 0);


    // TODO: Null allocation checking
    ApplicationState *retval = (ApplicationState*)malloc(sizeof(ApplicationState));

    SDL_Window *wind = SDL_CreateWindow(
        "Test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0
    );

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);

    int *hardware_states = (int*)malloc((int)SDL_NUM_SCANCODES * sizeof(int));
    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }

    if (!wind || !rend) {
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return NULL;
    }

    // Will need a slightly different solution later when more fonts are needed
    char buffer[1000];
    strcpy(buffer, asset_folder);
    strcat(buffer, "/Lekton-Bold.ttf");

    *(retval) = (ApplicationState){
        .rend=rend,
        .wind=wind,
        .hardware_states=hardware_states,
        .menu_font=TTF_OpenFont(buffer, 24),
    };

    return retval;
}


int ApplicationState_deconstruct(ApplicationState* self) {

    SDL_DestroyWindow(self->wind);
    free(self->hardware_states);
    free(self);


    TTF_Quit();
    SDL_Quit();
    return 0;
}
