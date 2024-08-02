
#include "hiscores.h"
#include "inputs.h"
#include "sirtet.h"
#include "gameover_state.h"
#include "application_state.h"
#include "state_runner.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#ifndef MAX2
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MAX3
#define MAX3(a, b, c) MAX2((a), MAX2((b), (c)))
#endif

#ifndef MIN2
#define MIN2(a, b) ((a) > (b) ? (b) : (a))
#endif

/******************************************************************************
 * State Struct creation & destruction
******************************************************************************/


GameoverState* GameoverState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font,
    int player_score, ScoreList *hiscores,
    const SDL_Color *static_col, const SDL_Color *dynamic_col
) {

    GameoverState *retval = calloc(1, sizeof(GameoverState));
    if (retval == NULL) {
        Sirtet_setError("Error allocating memory for GameoverState\n");
        return NULL;
    }


    // For good measure - likely already sorted but just in case
    retval->hiscores = hiscores;
    ScoreList_sort(hiscores);

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


    /*** General config ***/

    retval->menucode_states = calloc(NUM_MENUCODES, sizeof(bool));
    retval->mcodes = MenucodeMap_init(MAX_MENUCODE_MAPS);
    MenucodePreset_standard(retval->mcodes, 1, 1, 1);
    MenucodePreset_upperAlpha(retval->mcodes, 1, 1, 1);

    retval->static_col = *static_col;
    retval->dynamic_col = *dynamic_col;
    retval->lbl_font = lbl_font;


    /*** Labels ***/

    char scorebuff[12];
    char rankbuff[12];
    char *namebuff = malloc((hiscores->namelen + 1) * sizeof(char));
    memset(namebuff, '_', hiscores->namelen * sizeof(char));
    namebuff[hiscores->namelen] = '\0';


    /** Player Score **/

    snprintf(scorebuff, 12, "%d", player_score);
    snprintf(rankbuff, 12, "%d", retval->player_rank + 1);


    SDL_Surface *pscoresurf = TTF_RenderText_Solid(
        lbl_font, scorebuff, *dynamic_col);
    SDL_Surface *pnamesurf = TTF_RenderText_Solid(
        lbl_font, namebuff, *dynamic_col);
    SDL_Surface *pranksurf = TTF_RenderText_Solid(
        lbl_font, rankbuff, *dynamic_col);

    if (pscoresurf == NULL || pnamesurf == NULL || pranksurf == NULL) {
        char buff[128];
        snprintf(
            buff, 128,
            "Error creating player labels in GameoverState:\n%s\n",
            TTF_GetError()
        );
        Sirtet_setError(buff);
        return NULL;
    }

    retval->pname_lbl = SDL_CreateTextureFromSurface(rend, pnamesurf);
    retval->pscore_lbl = SDL_CreateTextureFromSurface(rend, pscoresurf);
    retval->prank_lbl = SDL_CreateTextureFromSurface(rend, pranksurf);


    if (
        retval->pname_lbl == NULL ||
        retval->pscore_lbl == NULL ||
        retval->prank_lbl == NULL
    ) {
        char buff[128];
        snprintf(
            buff, 128,
            "Error creating player labels in GameoverState:\n%s\n",
            SDL_GetError()
        );
        return NULL;
    }

    SDL_FreeSurface(pscoresurf);
    SDL_FreeSurface(pnamesurf);
    SDL_FreeSurface(pranksurf);


    /** Existing High Scores **/

    retval->n_lbls = (hiscores->len <= 10 ? hiscores->len : 10);
    size_t arrsz = retval->n_lbls * sizeof(SDL_Texture*);
    retval->rank_lbls = malloc(arrsz);
    retval->name_lbls = malloc(arrsz);
    retval->score_lbls = malloc(arrsz);

    int score;
    int lbli = 0;
    for (int i = 0; i < retval->n_lbls; i++) {

        if (ScoreList_get(hiscores, i, namebuff, &score) != 0) {
            free(retval);
            return NULL;
        }

        int rank = i + 1 + (retval->player_rank <= i);

        snprintf(scorebuff, 12, "%d", score);
        snprintf(rankbuff, 12, "%d", rank);

        SDL_Surface *namesurf = TTF_RenderText_Solid(
            lbl_font, namebuff, *static_col);

        SDL_Surface *scoresurf = TTF_RenderText_Solid(
            lbl_font, scorebuff, *static_col);

        SDL_Surface *ranksurf = TTF_RenderText_Solid(
            lbl_font, rankbuff, *static_col);

        retval->name_lbls[lbli] = SDL_CreateTextureFromSurface(rend, namesurf);
        retval->score_lbls[lbli] = SDL_CreateTextureFromSurface(rend, scoresurf);
        retval->rank_lbls[lbli++] = SDL_CreateTextureFromSurface(rend, ranksurf);

        SDL_FreeSurface(namesurf);
        SDL_FreeSurface(scoresurf);
        SDL_FreeSurface(ranksurf);
    }

    
    free(namebuff);

    /***/





    return retval;
}


int GameoverState_deconstruct(void *self) {

    GameoverState *hs = (GameoverState*)self;


    free(hs->player_name);

    SDL_DestroyTexture(hs->prank_lbl);
    SDL_DestroyTexture(hs->pscore_lbl);
    SDL_DestroyTexture(hs->pname_lbl);

    for (int i = 0; i < hs->n_lbls; i++) {
        SDL_DestroyTexture(hs->score_lbls[i]);
        SDL_DestroyTexture(hs->name_lbls[i]);
        SDL_DestroyTexture(hs->rank_lbls[i]);
    }
    free(hs->score_lbls);
    free(hs->name_lbls);


    MenucodeMap_deconstruct(hs->mcodes);
    free(hs->menucode_states);

    free(self);
    return 0;

}


/******************************************************************************
 * Helper funcs
******************************************************************************/


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


    /*** Respond to input ***/

    Menucode mcmap[26] = {
        MENUCODE_ALPHA_UC_A, MENUCODE_ALPHA_UC_B, MENUCODE_ALPHA_UC_C,
        MENUCODE_ALPHA_UC_D, MENUCODE_ALPHA_UC_E, MENUCODE_ALPHA_UC_F,
        MENUCODE_ALPHA_UC_G, MENUCODE_ALPHA_UC_H, MENUCODE_ALPHA_UC_I,
        MENUCODE_ALPHA_UC_J, MENUCODE_ALPHA_UC_K, MENUCODE_ALPHA_UC_L,
        MENUCODE_ALPHA_UC_M, MENUCODE_ALPHA_UC_N, MENUCODE_ALPHA_UC_O,
        MENUCODE_ALPHA_UC_P, MENUCODE_ALPHA_UC_Q, MENUCODE_ALPHA_UC_R,
        MENUCODE_ALPHA_UC_S, MENUCODE_ALPHA_UC_T, MENUCODE_ALPHA_UC_U,
        MENUCODE_ALPHA_UC_V, MENUCODE_ALPHA_UC_W, MENUCODE_ALPHA_UC_X,
        MENUCODE_ALPHA_UC_Y, MENUCODE_ALPHA_UC_Z
    };

    for (int i = 0; i < 26; i++) {
        Menucode mc = mcmap[i];

        if (Menucode_pressed(go_state->menucode_states, mc)) {
            char newchar = 'A' + i;

            size_t *nameidx = &go_state->name_idx;

            go_state->player_name[*nameidx] = newchar;

            *nameidx = (*nameidx + 1) % go_state->hiscores->namelen;

            // remake label
            SDL_DestroyTexture(go_state->pname_lbl);
            SDL_Surface *namesurf = TTF_RenderText_Solid(
                go_state->lbl_font,
                go_state->player_name,
                go_state->dynamic_col
            );

            if (namesurf == NULL) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "Error creating surface for new name:\n%s\n",
                    TTF_GetError()
                );
                Sirtet_setError(buff);
                return -1;
            }

            go_state->pname_lbl = SDL_CreateTextureFromSurface(rend, namesurf);
            SDL_FreeSurface(namesurf);
            if (go_state->pname_lbl == NULL) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "Error creating surface for new name:\n%s\n",
                    SDL_GetError()
                );
                Sirtet_setError(buff);
                return -1;
            }


        }
    }


    if (Menucode_pressed(go_state->menucode_states, MENUCODE_SELECT)) {
        // submit score

        ScoreList_add(
            go_state->hiscores, go_state->player_name, go_state->player_score);

        ScoreList_sort(go_state->hiscores);
        StateRunner_setPopCount(runner, 1);

    }


    /*** DRAW ***/

    int wind_w, wind_h;
    SDL_GetWindowSize(app_state->wind, &wind_w, &wind_h);

    int yoffset = 0;


    int max_lbli = MIN2(
        go_state->n_lbls,
        (go_state->player_rank <= 10 ? 9 : 10)
    );

    bool player_drawn = false;
    for (int lbli = 0; lbli < max_lbli; lbli++) {

        int name_h, name_w, score_h, score_w, rank_w, rank_h;

        if (lbli == go_state->player_rank) {
            SDL_QueryTexture(
                go_state->pname_lbl, NULL, NULL, &name_w, &name_h);
            SDL_QueryTexture(
                go_state->pscore_lbl, NULL, NULL, &score_w, &score_h);
            SDL_QueryTexture(
                go_state->prank_lbl, NULL, NULL, &rank_w, &rank_h);

            int draw_h = MAX3(name_h, score_h, rank_h);

            SDL_Rect namedst = {
                .x=rank_w * 2, .y=yoffset, .w=name_w, .h=draw_h
            };
            SDL_Rect scoredst = {
                .x=wind_w / 2 - score_w, .y=yoffset, .w=score_w, .h=draw_h};

            SDL_Rect rankdst = {
                .x=0, .y=yoffset, .w=rank_w, .h=draw_h};


            SDL_RenderCopy(rend, go_state->pname_lbl, NULL, &namedst);
            SDL_RenderCopy(rend, go_state->pscore_lbl, NULL, &scoredst);
            SDL_RenderCopy(rend, go_state->prank_lbl, NULL, &rankdst);
            yoffset += draw_h;
            player_drawn = true;
        }


        SDL_Texture *name_lbl;
        SDL_Texture *score_lbl;
        SDL_Texture *rank_lbl;

        name_lbl = go_state->name_lbls[lbli];
        score_lbl = go_state->score_lbls[lbli];
        rank_lbl = go_state->rank_lbls[lbli];


        SDL_QueryTexture(name_lbl, NULL, NULL, &name_w, &name_h);
        SDL_QueryTexture(score_lbl, NULL, NULL, &score_w, &score_h);
        SDL_QueryTexture(rank_lbl, NULL, NULL, &rank_w, &rank_h);

        int draw_h = MAX3(name_h, score_h, rank_h);


        SDL_Rect namedst = {.x=rank_w * 2, .y=yoffset, .w=name_w, .h=draw_h};
        SDL_Rect scoredst = {
            .x=wind_w / 2 - score_w, .y=yoffset, .w=score_w, .h=draw_h};
        SDL_Rect rankdst = {.x=0, .y=yoffset, .w=rank_w, .h=draw_h};

        SDL_RenderCopy(rend, name_lbl, NULL, &namedst);
        SDL_RenderCopy(rend, score_lbl, NULL, &scoredst);
        SDL_RenderCopy(rend, rank_lbl, NULL, &rankdst);

        yoffset += draw_h;
    }

    // Drawn at end (if not in top 10)
    if (!player_drawn) {

        int name_h, name_w, score_h, score_w, rank_w, rank_h;

        SDL_QueryTexture(
            go_state->pname_lbl, NULL, NULL, &name_w, &name_h);
        SDL_QueryTexture(
            go_state->pscore_lbl, NULL, NULL, &score_w, &score_h);
        SDL_QueryTexture(
            go_state->prank_lbl, NULL, NULL, &rank_w, &rank_h);

        int draw_h = MAX3(name_h, score_h, rank_h);

        SDL_Rect namedst = {
            .x=rank_w * 2, .y=yoffset, .w=name_w, .h=draw_h
        };
        SDL_Rect scoredst = {
            .x=wind_w / 2 - score_w, .y=yoffset, .w=score_w, .h=draw_h};

        SDL_Rect rankdst = {
            .x=0, .y=yoffset, .w=rank_w, .h=draw_h};


        SDL_RenderCopy(rend, go_state->pname_lbl, NULL, &namedst);
        SDL_RenderCopy(rend, go_state->pscore_lbl, NULL, &scoredst);
        SDL_RenderCopy(rend, go_state->prank_lbl, NULL, &rankdst);
        yoffset += draw_h;
    }

    return 0;
}
