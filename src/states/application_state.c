#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#include "application_state.h"


/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

ApplicationState* ApplicationState_init(char *asset_folder) {

    /***** Initialization *****/
    ApplicationState *retval = (ApplicationState*)malloc(sizeof(ApplicationState));
    if (retval == NULL) {
        printf("Error allocating ApplicationState\n");
        return NULL;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        const char *errmsg = SDL_GetError();
        printf("Error starting SDL: %s\n", errmsg);
        free(retval);
        return NULL;
    }

    if (TTF_Init() != 0) {
        const char* errmsg = TTF_GetError();
        printf("Error starting SDL_ttf: %s\n", errmsg);
        SDL_Quit();
        free(retval);
    }


    /***** Window and Renderer *****/

    SDL_Window *wind = SDL_CreateWindow(
        "Test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0
    );

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);

    if (wind == NULL) {
        const char *errmsg = SDL_GetError();
        printf("Error creating window: %s\n", errmsg);
        TTF_Quit();
        SDL_Quit();
        free(retval);
        return NULL;
    }


    if (!wind || !rend) {
        const char *errmsg = SDL_GetError();
        printf("Error creatingrenderer: %s\n", errmsg);
        SDL_DestroyWindow(wind);
        SDL_Quit();
        TTF_Quit();
        free(retval);
        return NULL;
    }

    /***** Build*****/
    int *hardware_states = (int*)malloc((int)SDL_NUM_SCANCODES * sizeof(int));
    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }


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

    if (retval->menu_font == NULL) {
        const char* errmsg = TTF_GetError();
        printf("Error loading font: %s\n", errmsg);
        ApplicationState_deconstruct(retval);
        return NULL;
    }

    if (retval->hardware_states == NULL) {
        printf("Error initializing hardware states.\n");
        ApplicationState_deconstruct(retval);
        return NULL;
    }

    return retval;
}


int ApplicationState_deconstruct(ApplicationState* self) {

    SDL_DestroyWindow(self->wind);
    SDL_DestroyRenderer(self->rend);
    free(self->hardware_states);
    free(self);


    TTF_Quit();
    SDL_Quit();
    return 0;
}
