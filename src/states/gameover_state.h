
/******************************************************************************
 * gameover_state.h
 *
 * Defines a state to show when the game is lost - showing the player's
 * score among the high scores and prompting for a name
******************************************************************************/

#ifndef GAMEOVER_STATE_H
#define GAMEOVER_STATE_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "hiscores.h"
#include "state_runner.h"
#include "inputs.h"


typedef struct {


    // for the top 10
    size_t n_lbls;
    SDL_Texture **name_lbls; // array of name lbl pointers
    SDL_Texture **score_lbls; // array of score lbl pointers
    
    // store ref
    ScoreList *hiscores;

    // Player data & whatnot
    SDL_Texture *pname_lbl;  // Player's name
    SDL_Texture *pscore_lbl;  // Player's score

    size_t name_idx;  // for name entry
    char *player_name;
    int player_score;
    int player_rank;  // Where player will be once added to hiscores

    bool *menucode_states;
    MenucodeMap *mcodes;

} GameoverState;


GameoverState* GameoverState_init(
    SDL_Renderer *rend, TTF_Font *lbl_font,
    int player_score, ScoreList *hiscores
);

int GameoverState_deconstruct(void *self);



/******************************************************************************
 *
******************************************************************************/




/******************************************************************************
 * State running
******************************************************************************/
int GameoverState_run(StateRunner *runner, void *app_data, void *state_data);

#endif
