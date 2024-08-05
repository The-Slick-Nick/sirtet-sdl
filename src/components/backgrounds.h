/******************************************************************************
 * backgrounds.h
 *
 * This file/module is meant to define ways to have a pretty or cool
 * functioning background to draw
 *****************************************************************************/


#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include <SDL2/SDL.h>

// A struct to represent a slowly panning background
typedef struct {

    float xvel;
    float yvel;

    float xpos;
    float ypos;


    SDL_Texture *topleft;
    SDL_Texture *topright;
    SDL_Texture *bottomleft;
    SDL_Texture *bottomright;


} PanningBg;


// Initialize a PanningBg struct and return a pointer to it 
PanningBg *PanningBg_init(
    float xvel, float yvel,
    SDL_Texture *topleft, SDL_Texture *topright,
    SDL_Texture *bottomleft, SDL_Texture *bottomright
);


// Free memory allocated for a PanningBg struct through PanningBg_init
int PanningBg_deconstruct(PanningBg *self);


// Move center focus point of background by configured velocity
int PanningBg_move(PanningBg *self, SDL_Rect *containing_window);

// Draw the PanningBg at its current position
int PanningBg_draw(
    PanningBg *self, SDL_Renderer *rend, SDL_Rect *containing_window);



#endif
