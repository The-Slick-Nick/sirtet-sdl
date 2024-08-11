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

HiscoresState* HiscoresState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font, ScoreList *hiscores) {

    HiscoresState *retval = calloc(1, sizeof(HiscoresState));
    if (retval == NULL) {
        Sirtet_setError("Error allocating memory for HiscoresState\n");
        return NULL;
    }



    /*** Labels ***/

    retval->labels = NULL;
    if (hiscores->len > 0) {

        SDL_Color lblcol = {50, 50, 200, 255};
        retval->labels = ScoreDisplay_init(
            hiscores, 0, hiscores->len - 1, 1, rend, lbl_font, &lblcol);

        if (retval->labels == NULL) {
            char buff[128];
            snprintf(
                buff, 128,
                "Error creating HiscoreState:\n    %s\n",
                Sirtet_getError()
            );
            Sirtet_setError(buff);
            return NULL;
        }
    }


    /*** Inputs ***/

    retval->menucode_states = calloc(NUM_MENUCODES, sizeof(bool));
    retval->mcodes = MenucodeMap_init(MAX_MENUCODE_MAPS);
    MenucodePreset_standard(retval->mcodes, 1, 1, 1);


    return retval;
}


int HiscoresState_deconstruct(void *self) {

    HiscoresState *hs = (HiscoresState*)self;

    ScoreDisplay_deconstruct(hs->labels);

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

    processMenucodes(
        hs_state->menucode_states, app_state->hardware_states,
        hs_state->mcodes
    );

    if (Menucode_pressed(hs_state->menucode_states, MENUCODE_EXIT)) {

        StateRunner_setPopCount(runner, 1);
    }


    /*** DRAW ***/

    if (hs_state->labels != NULL && hs_state->labels->n_lbls > 0) {
        SDL_Rect dstwind = {.x=0, .y=0};
        SDL_GetWindowSize(app_state->wind, &dstwind.w, &dstwind.h);
        ScoreDisplay_draw(hs_state->labels, 10, rend, &dstwind, NULL);
    }

    return 0;
}
    



