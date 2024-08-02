#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include "hiscores_state.h"
#include "application_state.h"
#include "hiscores.h"
#include "inputs.h"
#include "state_runner.h"
#include "sirtet.h"




/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/

// TODO: Generalize highscores drawing method for use in this
// state and GameoverState

HiscoresState* HiscoresState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font, ScoreList *hiscores) {

    HiscoresState *retval = calloc(1, sizeof(HiscoresState));
    if (retval == NULL) {
        Sirtet_setError("Error allocating memory for HiscoresState\n");
        return NULL;
    }


    /*** Labels ***/

    retval->n_lbls = (hiscores->len <= 10 ? hiscores->len : 10);
    retval->name_lbls = malloc(hiscores->len * sizeof(SDL_Texture*));
    retval->score_lbls = malloc(hiscores->len * sizeof(SDL_Texture*));

    char *namebuff = calloc(hiscores->namelen + 1, sizeof(char));
    char scorebuff[12];
    int score;
    for (int i = 0; i < retval->n_lbls; i++) {
        if (ScoreList_get(hiscores, i, namebuff, &score) != 0) {
            free(retval);
            return NULL;
        }
        snprintf(scorebuff, 12, "%d", score);

        SDL_Surface *namesurf = TTF_RenderText_Solid(lbl_font, namebuff, (SDL_Color){50, 50, 200, 255});
        SDL_Surface *scoresurf = TTF_RenderText_Solid(lbl_font, scorebuff, (SDL_Color){50, 50, 200, 255});

        retval->name_lbls[i] = SDL_CreateTextureFromSurface(rend, namesurf);
        retval->score_lbls[i] = SDL_CreateTextureFromSurface(rend, scoresurf);

        SDL_FreeSurface(namesurf);
        SDL_FreeSurface(scoresurf);
    }

    free(namebuff);


    /*** Inputs ***/

    retval->menucode_states = calloc(NUM_MENUCODES, sizeof(bool));
    retval->mcodes = MenucodeMap_init(MAX_MENUCODE_MAPS);
    MenucodePreset_standard(retval->mcodes, 1, 1, 1);


    return retval;
}


int HiscoresState_deconstruct(void *self) {

    HiscoresState *hs = (HiscoresState*)self;


    for (int i = 0; i < hs->n_lbls; i++) {
        SDL_DestroyTexture(hs->score_lbls[i]);
        SDL_DestroyTexture(hs->name_lbls[i]);
    }
    free(hs->score_lbls);
    free(hs->name_lbls);


    MenucodeMap_deconstruct(hs->mcodes);
    free(hs->menucode_states);

    free(self);
    return 0;

}


/******************************************************************************
 * State running
 * typedef int (*state_func_t)(StateRunner*, void*, void*);
******************************************************************************/

int HiscoresState_run(StateRunner *runner, void *app_data, void *state_data) {


    /*** Recasting ***/

    ApplicationState *app_state = (ApplicationState*)app_data;
    HiscoresState *hs_state = (HiscoresState*)state_data;


    /*** Unpacking ***/
    SDL_Renderer *rend = app_state->rend;


    /*** Process Inputs ***/

    processMenucodes(hs_state->menucode_states, app_state->hardware_states, hs_state->mcodes);

    // should be mapped to ESC
    if (Menucode_pressed(hs_state->menucode_states, MENUCODE_EXIT)) {

        // StateRunner_setPopCount(StateRunner *self, int count)
        StateRunner_setPopCount(runner, 1);
    }


    /*** DRAW ***/

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);

    int yoffset = 0;
    for (int lbli = 0; lbli < hs_state->n_lbls; lbli++) {
        SDL_Texture *name_lbl = hs_state->name_lbls[lbli];
        SDL_Texture *score_lbl = hs_state->score_lbls[lbli];

        int name_h, name_w, score_h, score_w;

        SDL_QueryTexture(name_lbl, NULL, NULL, &name_w, &name_h);
        SDL_QueryTexture(score_lbl, NULL, NULL, &score_w, &score_h);

        int draw_h = name_h > score_h ? name_h : score_h;

        SDL_Rect namedst = {.x=0, .y=yoffset, .w=name_w, .h=draw_h};
        SDL_Rect scoredst = {.x=wind_w / 2 - score_w, .y=yoffset, .w=score_w, .h=draw_h};

        SDL_RenderCopy(rend, name_lbl, NULL, &namedst);
        SDL_RenderCopy(rend, score_lbl, NULL, &scoredst);

        yoffset += draw_h;
    }

    return 0;
}
    



