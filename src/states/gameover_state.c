
#include "hiscores.h"
#include "sirtet.h"
#include "gameover_state.h"
#include "application_state.h"
#include <SDL2/SDL_surface.h>


/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/


GameoverState* GameoverState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font,
    int player_score, ScoreList *hiscores
) {

    GameoverState *retval = calloc(1, sizeof(GameoverState));
    if (retval == NULL) {
        Sirtet_setError("Error allocating memory for GameoverState\n");
        return NULL;
    }


    // For good measure - likely already sorted but just in case
    ScoreList_sort(hiscores);

    // TODO: Deconstruct/free
    retval->player_name = calloc(hiscores->namelen + 1, sizeof(char));
    memset(retval->player_name, '_', 3 * sizeof(char));
    retval->name_idx = 0;
    retval->player_score = player_score;

    // NOTE: Could binary search, but likely not needed
    retval->player_rank = hiscores->len;
    for (int i = 0; i < hiscores->len; i++) {
        int loopscore;
        ScoreList_get(hiscores, i, NULL, &loopscore);

        if (player_score > loopscore) {
            retval->player_rank = i;
            break;
        }
    }


    /*** Labels ***/

    char scorebuff[12];
    char *namebuff = calloc(hiscores->namelen + 1, sizeof(char));
    memset(namebuff, '_', hiscores->namelen * sizeof(char));


    /** Player Score **/

    snprintf(scorebuff, 12, "%d", player_score);
    SDL_Surface *pscoresurf = TTF_RenderText_Solid(
        lbl_font, scorebuff, (SDL_Color){50, 50, 200, 255});
    SDL_Surface *pnamesurf = TTF_RenderText_Solid(
        lbl_font, namebuff, (SDL_Color){50, 50, 200, 255});

    if (pscoresurf == NULL || pnamesurf == NULL) {
        char buff[128];
        snprintf(
            buff, 128,
            "Error creating player labels in GameoverState:\n%s\n",
            TTF_GetError()
        );
        Sirtet_setError(buff);
        return NULL;
    }

    // TODO: Deconstruct/free
    retval->pname_lbl = SDL_CreateTextureFromSurface(rend, pnamesurf);
    retval->pscore_lbl = SDL_CreateTextureFromSurface(rend, pscoresurf);

    if (retval->pname_lbl == NULL || retval->pscore_lbl == NULL) {
        char buff[128];
        snprintf(
            buff, 128,
            "Error creating player labels in GameoverState:\n%s\n",
            SDL_GetError()
        );
        return NULL;
    }

    // TODO: Null check
    SDL_FreeSurface(pscoresurf);
    SDL_FreeSurface(pnamesurf);


    /** Existing High Scores **/

    // 1 for player labels
    retval->n_lbls = 1 + (hiscores->len <= 10 ? hiscores->len : 10);
    retval->name_lbls = malloc(retval->n_lbls * sizeof(SDL_Texture*));
    retval->score_lbls = malloc(retval->n_lbls * sizeof(SDL_Texture*));

    int score;
    int lbli = 0;
    int hiscore_i = 0;
    for (int i = 0; i < retval->n_lbls; i++) {

        if (i == retval->player_rank) {
            retval->name_lbls[lbli] = retval->pname_lbl;
            retval->score_lbls[lbli++] = retval->pscore_lbl;
            continue;
        }

        if (ScoreList_get(hiscores, hiscore_i, namebuff, &score) != 0) {
            free(retval);
            return NULL;
        }
        snprintf(scorebuff, 12, "%d", score);

        SDL_Surface *namesurf = TTF_RenderText_Solid(
            lbl_font, namebuff, (SDL_Color){50, 50, 200, 255});

        SDL_Surface *scoresurf = TTF_RenderText_Solid(
            lbl_font, scorebuff, (SDL_Color){50, 50, 200, 255});

        retval->name_lbls[lbli] = SDL_CreateTextureFromSurface(rend, namesurf);
        retval->score_lbls[lbli++] = SDL_CreateTextureFromSurface(rend, scoresurf);

        SDL_FreeSurface(namesurf);
        SDL_FreeSurface(scoresurf);
        hiscore_i++;
    }

    if (retval->player_rank > 10) {
        retval->name_lbls[lbli] = retval->pname_lbl;
        retval->score_lbls[lbli] = retval->pscore_lbl;
    }

    
    free(namebuff);

    /***/



    /*** Inputs ***/

    retval->menucode_states = calloc(NUM_MENUCODES, sizeof(bool));
    retval->mcodes = MenucodeMap_init(MAX_MENUCODE_MAPS);
    MenucodePreset_standard(retval->mcodes, 1, 1, 1);
    MenucodePreset_upperAlpha(retval->mcodes, 1, 1, 1);


    return retval;
}


int GameoverState_deconstruct(void *self) {

    GameoverState *hs = (GameoverState*)self;

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

int GameoverState_run(StateRunner *runner, void *app_data, void *state_data) {


    /*** Recasting ***/

    ApplicationState *app_state = (ApplicationState*)app_data;
    GameoverState *go_state = (GameoverState*)state_data;


    /*** Unpacking ***/
    SDL_Renderer *rend = app_state->rend;


    /*** Process Inputs ***/

    processMenucodes(
        go_state->menucode_states,
        app_state->hardware_states,
        go_state->mcodes
    );

    // should be mapped to ESC
    if (Menucode_pressed(go_state->menucode_states, MENUCODE_EXIT)) {
        StateRunner_setPopCount(runner, 1);
    }


    /*** DRAW ***/

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);

    int yoffset = 0;
    for (int lbli = 0; lbli < go_state->n_lbls; lbli++) {

        SDL_Texture *name_lbl;
        SDL_Texture *score_lbl;

        name_lbl = go_state->name_lbls[lbli];
        score_lbl = go_state->score_lbls[lbli];

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
