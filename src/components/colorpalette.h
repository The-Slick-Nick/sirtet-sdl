/******************************************************************************
 * colorpalette.h
 *
 * 
 * Definition for a color palette and associated logic.
 *
 * Makes use of SDL_Color
 * Not to be confused with SDL_Palette
 * 
 *
 *****************************************************************************/

#ifndef COLORPALETTE_H
#define COLORPALETTE_H


#include <SDL2/SDL.h>


typedef struct {
    size_t size;
    char *name;
    SDL_Color *colors;
} ColorPalette;

ColorPalette* ColorPalette_init(const char *name, size_t size, ...);
void ColorPalette_deconstruct(ColorPalette *self);


int ColorPalette_getColor(ColorPalette *self, size_t idx, SDL_Color *outcol);
SDL_Color* ColorPalette_getColorPtr(ColorPalette *self, size_t idx);



#endif

