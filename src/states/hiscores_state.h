/******************************************************************************
 * hiscores_state.h
 *
 * Defines a state acting as a menu reporting on the highscores (
 * as stored in global ApplicationState)
******************************************************************************/

#ifndef HISCORES_STATE_H
#define HISCORES_STATE_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "hiscores.h"
#include "state_runner.h"
#include "inputs.h"


// NOTE: ScoreList *hiscores is in ApplicationState, so is not stored here
typedef struct {


    ScoreDisplay *labels;
    // size_t n_lbls;
    // SDL_Texture **name_lbls; // array of name lbl pointers
    // SDL_Texture **score_lbls; // array of score lbl pointers

    bool *menucode_states;
    MenucodeMap *mcodes;


} HiscoresState;


HiscoresState* HiscoresState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font, ScoreList *hiscores);

int HiscoresState_deconstruct(void *self);




/******************************************************************************
 *
******************************************************************************/




/******************************************************************************
 * State running
******************************************************************************/
int HiscoresState_run(StateRunner *runner, void *app_data, void *state_data);

#endif
