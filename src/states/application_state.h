/* application_state.h
*
* Definition for structs and methods handling global application state
*/

#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_surface.h>

#include "hiscores.h"

struct fontlib {
    TTF_Font *lekton_12;
    TTF_Font *lekton_24;
    TTF_Font *vt323_12;
    TTF_Font *vt323_24;
};

struct imglib {
    SDL_Texture *logo;

    SDL_Texture *bg_topright;
    SDL_Texture *bg_topleft;
    SDL_Texture *bg_bottomleft;
    SDL_Texture *bg_bottomright;
};


// sounds & music
struct soundlib {

};

    

// Higher-level status/state of hardware and such
// to pass to lower-level virtual states
typedef struct {

    int *hardware_states;  // Array, indexed by SDL_Scancode, indicating # of frames a hardware input has been pressed (or released)
    SDL_Renderer *rend;     // Pointer to renderer struct in use by the application
    SDL_Window *wind;       // Pointer to SDL_Window struct

    ScoreList *hiscores;

    struct fontlib fonts;
    struct imglib images;

} ApplicationState;



ApplicationState* ApplicationState_init(char *asset_folder);
int ApplicationState_deconstruct(ApplicationState* self);

#endif
