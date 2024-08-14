#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "hiscores.h"
#include "inputs.h"
#include "sirtet.h"
#include "utilities.h"
#include "gameover_state.h"
#include "application_state.h"
#include "state_runner.h"



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


    /** Player Score **/

    char scorebuff[12];
    char rankbuff[12];
    char *namebuff = malloc((hiscores->namelen + 1) * sizeof(char));
    memset(namebuff, '_', hiscores->namelen * sizeof(char));
    namebuff[hiscores->namelen] = '\0';

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


    /** General Scores **/

    int number_of_score_labels_to_print = MIN2(
        (retval->player_rank < 10 ? 9 : 10),
        hiscores->len
    );

    int top_n = MIN2(retval->player_rank, number_of_score_labels_to_print);
    int bot_n = number_of_score_labels_to_print - top_n;

    retval->top_labels = NULL;
    if (top_n > 0) {
        retval->top_labels = ScoreDisplay_init(
            hiscores,
            0,
            top_n - 1,
            1,
            rend, lbl_font, static_col
        );
    }

    retval->bottom_labels = NULL;
    if (bot_n > 0) {
        retval->bottom_labels = ScoreDisplay_init(
            hiscores,
            top_n,
            top_n + bot_n - 1,
            retval->player_rank + 2,
            rend, lbl_font, static_col
        );

        if (retval->bottom_labels == NULL) {
            printf("Error making bottom labels:\n    %s\n", Sirtet_getError());
            exit(1);
        }
    }

    /***/

    return retval;
}


int GameoverState_deconstruct(void *self) {

    GameoverState *hs = (GameoverState*)self;


    free(hs->player_name);

    SDL_DestroyTexture(hs->prank_lbl);
    SDL_DestroyTexture(hs->pscore_lbl);
    SDL_DestroyTexture(hs->pname_lbl);

    if (hs->top_labels != NULL) {
        ScoreDisplay_deconstruct(hs->top_labels);
    }

    if (hs->bottom_labels != NULL) {
        ScoreDisplay_deconstruct(hs->bottom_labels);
    }


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


    /*** Button Processing ***/

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


    if (
        Menucode_pressed(go_state->menucode_states, MENUCODE_EXIT)
        ||
        Menucode_pressed(go_state->menucode_states, MENUCODE_SELECT)
    ) {
        // submit score
        ScoreList_add(
            go_state->hiscores, go_state->player_name, go_state->player_score);

        ScoreList_sort(go_state->hiscores);
        StateRunner_setPopCount(runner, 1);
    }



    /*** DRAW ***/

    // top
    SDL_Rect drawdst = {.x=0, .y=0, .w=0, .h=0};
    SDL_GetWindowSize(app_state->wind, &drawdst.w, &drawdst.h);
    SDL_Rect outrect = {0, 0, drawdst.w, 0};

    if (go_state->top_labels != NULL) {
        ScoreDisplay_draw(
            go_state->top_labels, 0,
            rend, &drawdst,  &outrect
        );
    }
    drawdst.y += outrect.h;

    // player
    SDL_Rect prank_dst, pname_dst, pscore_dst;
    prank_dst.y = pname_dst.y = pscore_dst.y = drawdst.y;

    SDL_QueryTexture(
        go_state->pname_lbl, NULL, NULL, &pname_dst.w, &pname_dst.h);
    SDL_QueryTexture(
        go_state->prank_lbl, NULL, NULL, &prank_dst.w, &prank_dst.h);
    SDL_QueryTexture(
        go_state->pscore_lbl, NULL, NULL, &pscore_dst.w, &pscore_dst.h);

    int plab_h = MAX3(pname_dst.h, prank_dst.h, pscore_dst.h);
    pname_dst.h = prank_dst.h = pscore_dst.h = plab_h;

    prank_dst.x = drawdst.x;
    pname_dst.x = prank_dst.w;
    pscore_dst.x = drawdst.x + drawdst.w - pscore_dst.w;

    SDL_RenderCopy(rend, go_state->prank_lbl, NULL, &prank_dst);
    SDL_RenderCopy(rend, go_state->pname_lbl, NULL, &pname_dst);
    SDL_RenderCopy(rend, go_state->pscore_lbl, NULL, &pscore_dst);

    drawdst.y += plab_h;


    // bottom
    if (go_state->bottom_labels != NULL) {
        ScoreDisplay_draw(
            go_state->bottom_labels, 0,
            rend, &drawdst, &outrect
        );
    }

    return 0;
}
