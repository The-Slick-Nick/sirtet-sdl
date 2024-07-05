#include <limits.h>

#include "application_state.h"


/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

ApplicationState* ApplicationState_init() {

    SDL_Init(SDL_INIT_VIDEO);
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

    *(retval) = (ApplicationState){
        .rend=rend,
        .wind=wind,
        .hardware_states=hardware_states
    };
    return retval;
}

int ApplicationState_deconstruct(ApplicationState* self) {

    SDL_DestroyWindow(self->wind);
    free(self->hardware_states);
    free(self);
    return 0;
}
