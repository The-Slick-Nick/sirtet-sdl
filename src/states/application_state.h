/* application_state.h
*
* Definition for structs and methods handling global application state
*/

#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H

#define WINDOW_WIDTH 540 
#define WINDOW_HEIGHT 720

#define TARGET_FPS 60
#define TARGET_SPF (1.0 / TARGET_FPS)

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Higher-level status/state of hardware and such
// to pass to lower-level virtual states
typedef struct {

    int *hardware_states;  // Array, indexed by SDL_Scancode, indicating # of frames a hardware input has been pressed (or released)
    SDL_Renderer *rend;     // Pointer to renderer struct in use by the application
    SDL_Window *wind;       // Pointer to SDL_Window struct

    TTF_Font *menu_font;    // Font pointer to use for menu things

} ApplicationState;



ApplicationState* ApplicationState_init(char *asset_folder);
int ApplicationState_deconstruct(ApplicationState* self);

#endif
