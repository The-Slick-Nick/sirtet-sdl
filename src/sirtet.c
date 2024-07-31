/******************************************************************************
 * sirtet.c
 *
 *
 * This file defines the main application runner in function run()
******************************************************************************/
#include "inputs.h"
#include "sirtet.h"
#include "mainmenu_state.h"
#include "application_state.h"
#include "state_runner.h"
#include "states/application_state.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_shape.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define ERRMSG_SZ 128
static char glob_errmsg[ERRMSG_SZ] = {0};

void Sirtet_setError(const char *errmsg) {


    size_t errlen = strlen(errmsg);
    size_t copylen = errlen > ERRMSG_SZ - 1 ? ERRMSG_SZ - 1 : errlen;

    // memcpy instead of strcpy for custom truncation logic
    memcpy(glob_errmsg, errmsg, copylen);
    glob_errmsg[copylen + 1] = '\0';
}

char* Sirtet_getError() {
    return glob_errmsg;

}


/* Primary program runner */
int run() {

    // (these todos are in a general location because I didn't notice
    // specific lines of things lol)
    // TODO: Implement SRS (or similar) system - with dynamic "block size"
    //       based on content needs (some "blocksize 4" blocks can be represented
    //       in a 3x3 grid)
    // TODO: Block prediction overlay
    // TODO: Sound effects and (maybe) music(?)


    srand((int)time(NULL));

    printf("Initializing application state...\n");
    ApplicationState *global_state = ApplicationState_init("assets");
    if (global_state == NULL) {
        printf("%s\n", Sirtet_getError());
        return -1;
    }

    printf("Initializing main menu...\n");
    MainMenuState *mainmenu_state = MainMenuState_init(
        global_state->rend, global_state->fonts.vt323_24, global_state->images.logo);
    if (mainmenu_state == NULL) {
        printf("%s\n", Sirtet_getError());
        return -1;
    }

    printf("Initializing state runner...\n");
    StateRunner *state_runner = StateRunner_init(32, 16);
    if (state_runner == NULL) {
        printf("%s\n", Sirtet_getError());
        return -1;
    }

    printf("Pushing main menu state...\n");
    StateRunner_addState(
        state_runner, (void*)mainmenu_state, MainMenuState_run,
        MainMenuState_deconstruct
    );

    if (StateRunner_commitBuffer(state_runner) != 0) {
        printf("%s\n", Sirtet_getError());
        return -1;
    }

    clock_t frame_start;
    double elapsed;
    double raw_fps = 0.0;
    double actual_fps = 0.0;

    int frame_counter = 0;

    char buffer[128];  // a general purpose string buffer

    /*** Main Loop ***/
    printf("Starting main loop...\n");
    while (state_runner->head >= 0) {

        frame_counter++;
        frame_start = clock();

        /* Non-game related stuff */
        processHardwareInputs(global_state->hardware_states);

        SDL_SetRenderDrawColor(global_state->rend, 10, 20, 30, 255);
        SDL_RenderClear(global_state->rend);

        /* Run game-state specific code */
        StateRunner_runState(state_runner, (void*)global_state);

        if (StateRunner_commitBuffer(state_runner) != 0) {
            printf("%s\n", Sirtet_getError());
            return -1;
        }


        /* Draw FPS overlay */

        TTF_Font *fps_font = global_state->fonts.vt323_12;

        snprintf(buffer, 128, "%.2f ACTUAL FPS", actual_fps);
        SDL_Surface *fps_surf = TTF_RenderText_Solid(fps_font, buffer, (SDL_Color){.r=255});
        if (fps_surf == NULL) {
            printf("%s\n", TTF_GetError());
            return -1;
        }

        SDL_Texture *fps_texture = SDL_CreateTextureFromSurface(global_state->rend, fps_surf);
        if (fps_texture == NULL) {
            printf("%s\n", SDL_GetError());
            return -1;
        }

        // NOTE: We want to optimize this later to not free every frame
        SDL_FreeSurface(fps_surf);

        snprintf(buffer, 128, "%.2f UNBOUNDED FPS", raw_fps);

        SDL_Surface *rawfps_surf = TTF_RenderText_Solid(fps_font, buffer, (SDL_Color){.r=255});
        if (rawfps_surf == NULL) {
            printf("%s\n", TTF_GetError());
            return -1;
        }

        SDL_Texture *rawfps_texture = SDL_CreateTextureFromSurface(global_state->rend, rawfps_surf);
        if (rawfps_texture == NULL) {
            printf("%s\n", SDL_GetError());
            return -1;
        }

        // NOTE: We want to optimize this later to not free every frame
        SDL_FreeSurface(rawfps_surf);

        int txt_h, txt_w;
        SDL_QueryTexture(fps_texture, NULL, NULL, &txt_w, &txt_h);
        SDL_Rect fps_dest = (SDL_Rect){
                .x=WINDOW_WIDTH - txt_w,
                .y=WINDOW_HEIGHT - txt_h,
                .w=txt_w,
                .h=txt_h
        };
        SDL_RenderCopy(global_state->rend, fps_texture, NULL, &fps_dest);
        int yoffset = fps_dest.h;

        SDL_QueryTexture(rawfps_texture, NULL, NULL, &txt_w, &txt_h);
        SDL_Rect rawfps_dest = {
            .x=WINDOW_WIDTH - txt_w,
            .y=WINDOW_HEIGHT - yoffset - txt_h,
            .w=txt_w,
            .h=txt_h
        };
        SDL_RenderCopy(global_state->rend, rawfps_texture, NULL, &rawfps_dest);

        SDL_RenderPresent(global_state->rend);

        SDL_DestroyTexture(fps_texture);
        SDL_DestroyTexture(rawfps_texture);

        fps_texture = NULL;
        rawfps_texture = NULL;

        /*********************************************************************
         * Maintenance calculations
         ********************************************************************/

        elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        raw_fps = (1.0 / elapsed);


        while (elapsed < TARGET_SPF) {
            // Perform maintenance sorts of tasks here...
            elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        }

        // calculate actual fps
        elapsed = (double)(clock() - frame_start) / CLOCKS_PER_SEC;
        actual_fps = (1.0 / elapsed);

        if (frame_counter >= TARGET_FPS) {
            frame_counter = 0;
        }

    }

    // NOTE: This also handles SDL_Quit and TTF_Quit and
    // all other such related things. I don't know if this
    // is the best way to do it, but it's how I'm doing it
    // for now.
    
    StateRunner_deconstruct(state_runner);
    ApplicationState_deconstruct(global_state);
}
