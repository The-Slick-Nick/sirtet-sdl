/* application_state.h
*
* Definition for structs and methods handling global application state
*/

#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

#include <SDL2/SDL.h>

// Higher-level status/state of hardware and such
// to pass to lower-level virtual states
typedef struct {
    int *hardware_states;  // Array, indexed by SDL_Scancode, indicating # of frames a hardware input has been pressed (or released)
    SDL_Renderer *rend;     // Pointer to renderer struct in use by the application
    SDL_Window *wind;
} ApplicationState;



ApplicationState* ApplicationState_init();
int ApplicationState_deconstruct(ApplicationState* self);

#endif
