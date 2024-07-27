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

// Initialize a ColorPalette, passing colors as variadic arguments
ColorPalette* ColorPalette_initVa(const char *name, size_t size, ...);

// Initialize a ColorPalette, returning its pointer, or NULL if failed
ColorPalette* ColorPalette_init(const char *name, size_t size, SDL_Color *src);

// Initialize a ColorPalette as a copy of another one
ColorPalette* ColorPalette_initCopy(ColorPalette *src);

// Free color palette
void ColorPalette_deconstruct(ColorPalette *self);


int ColorPalette_getColor(ColorPalette *self, size_t idx, SDL_Color *outcol);
SDL_Color* ColorPalette_getColorPtr(ColorPalette *self, size_t idx);



#endif

