#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_mixer.h>

#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sirtet.h"
#include "sirtet_audio.h"
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

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        char buff[ERRMSG_SZ];
        snprintf(buff, 64, "Error starting SDL in ApplicationState: %s\n", SDL_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }


    if (TTF_Init() != 0) {
        char buff[ERRMSG_SZ];
        snprintf(buff, ERRMSG_SZ, "Error starting TTF in ApplicationState: %s\n", TTF_GetError());
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    if ( SirtetAudio_start() != 0 ) {
        char buff[ERRMSG_SZ];
        // NOTE: Intent is to have audio api handle any error messaging
        return NULL;
    }


    /***** Window and Renderer *****/

    SDL_Window *wind = SDL_CreateWindow(
        "Sirtet", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

    char buffer[FILEPATH_SZ];

    strcpy(buffer, asset_folder);
    strcat(buffer, "/fonts/Lekton-Bold.ttf");
    retval->fonts.lekton_12 = TTF_OpenFont(buffer, 12);
    retval->fonts.lekton_24 = TTF_OpenFont(buffer, 24);

    if (
        retval->fonts.lekton_12 == NULL
        || retval->fonts.lekton_24 == NULL
    ) {
        char buff[ERRMSG_SZ];
        snprintf(
            buff, ERRMSG_SZ,
            "Error loading Lekton font in ApplicationState: %s\n",
            TTF_GetError()
        );
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    strcpy(buffer, asset_folder);
    strcat(buffer, "/fonts/VT323.ttf");
    retval->fonts.vt323_12 = TTF_OpenFont(buffer, 12);
    retval->fonts.vt323_24 = TTF_OpenFont(buffer, 24);

    if (
        retval->fonts.vt323_12 == NULL
        || retval->fonts.vt323_24 == NULL
    ) {
        char buff[ERRMSG_SZ];
        snprintf(
            buff, 64,
            "Error loading VT323 font in ApplicationState: %s\n",
            TTF_GetError()
        );
        Sirtet_setError(buff);
        free(retval);
        return NULL;
    }

    /***** Load images *****/

    // logo
    strcpy(buffer, asset_folder);
    strcat(buffer, "/img/Logo.bmp");
    SDL_Surface *logo_surf = SDL_LoadBMP(buffer);
    if (logo_surf == NULL) {
        char errbuff[STATIC_ARRMAX];
        snprintf(
            errbuff, STATIC_ARRMAX,
            "Error loading logo:\n    %s\n", 
            SDL_GetError()
        );
        Sirtet_setError(errbuff);
        return NULL;
    }

    // bg images
    strcpy(buffer, asset_folder);
    strcat(buffer, "/img/BG_TopRight.bmp");
    SDL_Surface *bg_tr_surf = SDL_LoadBMP(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/img/BG_TopLeft.bmp");
    SDL_Surface *bg_tl_surf = SDL_LoadBMP(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/img/BG_BottomLeft.bmp");
    SDL_Surface *bg_bl_surf = SDL_LoadBMP(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/img/BG_BottomRight.bmp");
    SDL_Surface *bg_br_surf = SDL_LoadBMP(buffer);


    if (bg_tr_surf == NULL ||
        bg_tl_surf == NULL ||
        bg_bl_surf == NULL ||
        bg_br_surf == NULL
    ) {
        char errbuff[ERRMSG_SZ];
        snprintf(
            errbuff, STATIC_ARRMAX,
            "Error loading menu background:\n    %s\n", 
            SDL_GetError()
        );
        Sirtet_setError(errbuff);
        return NULL;
    }


    retval->images.logo = SDL_CreateTextureFromSurface(rend, logo_surf);

    retval->images.bg_topright = SDL_CreateTextureFromSurface(rend, bg_tr_surf);
    retval->images.bg_topleft = SDL_CreateTextureFromSurface(rend, bg_tl_surf);
    retval->images.bg_bottomright = SDL_CreateTextureFromSurface(rend, bg_br_surf);
    retval->images.bg_bottomleft = SDL_CreateTextureFromSurface(rend, bg_bl_surf);
    if (
        retval->images.logo == NULL ||
        retval->images.bg_topright == NULL ||
        retval->images.bg_topleft == NULL ||
        retval->images.bg_bottomright == NULL ||
        retval->images.bg_bottomleft == NULL
    ) {
        char errbuff[ERRMSG_SZ];
        snprintf(
            errbuff, STATIC_ARRMAX,
            "Error loading image assets:\n    %s\n", 
            SDL_GetError()
        );
        Sirtet_setError(errbuff);
        return NULL;
    }

    SDL_FreeSurface(logo_surf);
    SDL_FreeSurface(bg_tr_surf);


    /***** Load sounds *****/

    strcpy(buffer, asset_folder);
    strcat(buffer, "/sounds/mech_kb_click4.wav");
    retval->sounds.short_click = SirtetAudio_loadSound(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/sounds/deep_bump.wav");
    retval->sounds.bump = SirtetAudio_loadSound(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/sounds/short_boop.wav");
    retval->sounds.boop = SirtetAudio_loadSound(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/sounds/beep_scale_success.wav");
    retval->sounds.boop_scale = SirtetAudio_loadSound(buffer);

    strcpy(buffer, asset_folder);
    strcat(buffer, "/sounds/beep_scale_reverse.wav");
    retval->sounds.boop_scale_reverse = SirtetAudio_loadSound(buffer);

    if (
        SirtetAudio_soundInvalid(retval->sounds.short_click) ||
        SirtetAudio_soundInvalid(retval->sounds.bump) ||
        SirtetAudio_soundInvalid(retval->sounds.boop) ||
        SirtetAudio_soundInvalid(retval->sounds.boop_scale) ||
        SirtetAudio_soundInvalid(retval->sounds.boop_scale_reverse)
    ) {
        return NULL;
    }


    /***** Load saved data *****/

    retval->hiscores = ScoreList_init(HISCORES_MAX_SIZE, HISCORES_NAME_LEN);


    char hs_path[FILEPATH_SZ];
    strcpy(hs_path, Sirtet_getAppdataPath());
    strcat(hs_path, "/hiscores.txt");
    FILE *hiscore_file = fopen(hs_path, "r");
    if (hiscore_file != NULL) {
        ScoreList_readFile(retval->hiscores, hiscore_file);
        fclose(hiscore_file);
    }


    return retval;
}


int ApplicationState_deconstruct(ApplicationState* self) {


    /*** Clean up & export any saved data ***/

    char hs_path[FILEPATH_SZ];
    strcpy(hs_path, Sirtet_getAppdataPath());
    strcat(hs_path, "/hiscores.txt");

    FILE* hiscore_file = fopen(hs_path, "w");
    if (hiscore_file == NULL) {
        Sirtet_setError("Error writing highscores to file\n");
        return -1;
    }

    ScoreList_sort(self->hiscores);
    ScoreList_toFile(self->hiscores, hiscore_file);
    fclose(hiscore_file);


    /*** Free memory ***/

    ScoreList_deconstruct(self->hiscores);
    SDL_DestroyWindow(self->wind);
    SDL_DestroyRenderer(self->rend);
    SDL_DestroyTexture(self->images.logo);

    TTF_CloseFont(self->fonts.lekton_24);
    TTF_CloseFont(self->fonts.lekton_12);
    TTF_CloseFont(self->fonts.vt323_24);
    TTF_CloseFont(self->fonts.vt323_12);

    SirtetAudio_unloadSound(self->sounds.short_click);
    SirtetAudio_unloadSound(self->sounds.bump);

    free(self->hardware_states);
    free(self);


    TTF_Quit();
    SDL_Quit();
    SirtetAudio_end();
    return 0;
}
