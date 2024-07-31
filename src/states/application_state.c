#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>


#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sirtet.h"
#include "application_state.h"


/*=============================================================================
 State Struct creation & destruction
=============================================================================*/

ApplicationState* ApplicationState_init(char *asset_folder) {

    /***** Initialization *****/
    ApplicationState *retval = (ApplicationState*)malloc(sizeof(ApplicationState));
    if (retval == NULL) {
        Sirtet_setError("Error allocating memory for ApplicationState\n");
        return NULL;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        char buff[64];
        snprintf(buff, 64, "Error starting SDL in ApplicationState: %s\n", SDL_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    if (TTF_Init() != 0) {
        char buff[64];
        snprintf(buff, 64, "Error starting TTF in ApplicationState: %s\n", TTF_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    // TODO: Final: clean this up if we never end up using pngs
    // int img_flags = (IMG_INIT_PNG);
    // if (!(IMG_Init(img_flags) & img_flags)) {
        // char buff[64];
        // snprintf(buff, 64, "Error starting SDL_image in ApplicationState: %s\n", IMG_GetError());
        // Sirtet_setError(buff);
        // free(retval);
        // return NULL;
    // }


    /***** Window and Renderer *****/

    SDL_Window *wind = SDL_CreateWindow(
        "Test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0
    );

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);
    // for translucent drawing
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

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
        printf("Error creating renderer: %s\n", errmsg);
        SDL_DestroyWindow(wind);
        SDL_Quit();
        TTF_Quit();
        free(retval);
        return NULL;
    }


    /***** Build*****/

    int *hardware_states = (int*)malloc((int)SDL_NUM_SCANCODES * sizeof(int));
    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }


    *(retval) = (ApplicationState){
        .rend=rend,
        .wind=wind,
        .hardware_states=hardware_states
    };

    if (retval->hardware_states == NULL) {
        printf("Error initializing hardware states.\n");
        ApplicationState_deconstruct(retval);
        return NULL;
    }


    /***** Load fonts *****/

    char buffer[1000];

    strcpy(buffer, asset_folder);
    strcat(buffer, "/Lekton-Bold.ttf");
    retval->fonts.lekton_12 = TTF_OpenFont(buffer, 12);
    retval->fonts.lekton_24 = TTF_OpenFont(buffer, 24);

    if (
        retval->fonts.lekton_12 == NULL
        || retval->fonts.lekton_24 == NULL
    ) {
        char buff[64];
        snprintf(buff, 64, "Error loading fonts in ApplicationState: %s\n", TTF_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    strcpy(buffer, asset_folder);
    strcat(buffer, "/VT323.ttf");
    retval->fonts.vt323_12 = TTF_OpenFont(buffer, 12);
    retval->fonts.vt323_24 = TTF_OpenFont(buffer, 24);

    if (
        retval->fonts.vt323_12 == NULL
        || retval->fonts.vt323_24 == NULL
    ) {
        char buff[64];
        snprintf(buff, 64, "Error loading fonts in ApplicationState: %s\n", TTF_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    /***** Load images *****/

    strcpy(buffer, asset_folder);
    strcat(buffer, "/img/Logo.bmp");
    SDL_Surface *logo_surf = SDL_LoadBMP(buffer);
    retval->images.logo = SDL_CreateTextureFromSurface(rend, logo_surf);
    SDL_FreeSurface(logo_surf);

    if (retval->images.logo == NULL) {
        char buff[64];
        snprintf(buff, 64, "Error loading images in ApplicationState: %s\n", TTF_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }


    /***** Load saved data *****/

    // TODO: When global cfg file is used, define an app-level name length
    // & highscores length to maintain
    retval->hiscores = ScoreList_init(100, 3);

    // TODO: Take a path to some appdata style folder and append filename to
    // path 

    char *path = "hiscores.txt";
    FILE *hiscore_file = fopen(path, "r");
    // TODO: Make this a hard error, once I figure out how to "create file if
    // not exists"
    if (hiscore_file != NULL) {
        ScoreList_readFile(retval->hiscores, hiscore_file);
        fclose(hiscore_file);
    }


    return retval;
}


int ApplicationState_deconstruct(ApplicationState* self) {

    SDL_DestroyWindow(self->wind);
    SDL_DestroyRenderer(self->rend);
    SDL_DestroyTexture(self->images.logo);

    TTF_CloseFont(self->fonts.lekton_24);
    TTF_CloseFont(self->fonts.lekton_12);
    TTF_CloseFont(self->fonts.vt323_24);
    TTF_CloseFont(self->fonts.vt323_12);

    char *path = "hiscores.txt";
    FILE* hiscore_file = fopen(path, "w");
    if (hiscore_file == NULL) {
        Sirtet_setError("Error writing highscores to file\n");
        return -1;
    }

    ScoreList_sort(self->hiscores);
    ScoreList_toFile(self->hiscores, hiscore_file);

    fclose(hiscore_file);
    ScoreList_deconstruct(self->hiscores);

    free(self->hardware_states);
    free(self);


    TTF_Quit();
    SDL_Quit();
    return 0;
}
