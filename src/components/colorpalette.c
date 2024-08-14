#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "colorpalette.h"
#include "sirtet.h"

ColorPalette* ColorPalette_init(
    const char *name, size_t size, SDL_Color *src) {
    // memory structure:
    // [] size
    // [] name (prt)
    // [] colors (ptr)
    // [] name (char values)
    // ...
    // [] /0
    // [] colors (color values)
    // []
    // ...
    // []
    
    size_t nbytes = 0;
    size_t namelen = strlen(name);

    // use the struct size itself in case of padding
    nbytes += sizeof(ColorPalette);
    // Array values packed at end of same memory block
    nbytes += (strlen(name) + 1) * sizeof(char); 
    nbytes += sizeof(SDL_Color) * size;

    size_t name_offset = sizeof(ColorPalette);
    size_t colors_offset = name_offset + (sizeof(char) * (namelen + 1));

    void *mem = malloc(nbytes);
    if (mem == NULL) {
        Sirtet_setError("Error allocating memory for ColorPalette\n");
        return NULL;
    }
    memset(mem, '\0', nbytes);

    ColorPalette *retval = (ColorPalette*)mem;
    *retval = (ColorPalette){
        .size=size,
        .name=(char*)((char*)mem + name_offset),
        .colors=(SDL_Color*)((char*)mem + colors_offset)
    };
    strcpy(retval->name, name);

    // memcpy(void *, const void *, unsigned long)
    memcpy(retval->colors, src, sizeof(SDL_Color) * size);
    return retval;
}

ColorPalette* ColorPalette_initVa(const char *name, size_t size, ...) {

    // memory structure:
    // [] size
    // [] name (prt)
    // [] colors (ptr)
    // [] name (char values)
    // ...
    // [] /0
    // [] colors (color values)
    // []
    // ...
    // []

    SDL_Color colarr[STATIC_ARRMAX];
    SDL_Color *cparr;
    
    if (size <= STATIC_ARRMAX) {
        cparr = colarr;
    }
    else {
        cparr = (SDL_Color*)calloc(size, sizeof(SDL_Color));
    }

    va_list argptr;
    va_start(argptr, size);

    for (int colidx = 0; colidx < size; colidx++) {
        cparr[colidx] = va_arg(argptr, SDL_Color);
    }

    va_end(argptr);

    ColorPalette *retval = ColorPalette_init(name, size, cparr);

    if (size > STATIC_ARRMAX) {
        free(cparr);
    }
    return retval;
}


ColorPalette* ColorPalette_initCopy(ColorPalette *src) {
    return ColorPalette_init(src->name, src->size, src->colors);
}


void ColorPalette_deconstruct(ColorPalette *self) {
    free(self);
}

int ColorPalette_getColor(ColorPalette *self, size_t idx, SDL_Color *outcol) {

    if (self == NULL) {
        Sirtet_setError( "ColorPalette_getColor passed a NULL ColorPalette\n");
        return -1;
    }

    if (outcol == NULL) {
        Sirtet_setError(
            "ColorPalette_getColor passed a NULL pointer for outcol \n"
        );
        return -1;
    }

    if (idx >= self->size) {
        char buff[64];
        snprintf(buff, 64, "ColorPalette index %ld out of bounds\n", idx);
        Sirtet_setError(buff);
        return -1;
    }

    *outcol = self->colors[idx];
    return 0;
}

SDL_Color* ColorPalette_getColorPtr(ColorPalette *self, size_t idx) {

    if (self == NULL) {
        Sirtet_setError( "ColorPalette_getColor passed a NULL ColorPalette\n");
        return NULL;
    }

    if (idx >= self->size) {
        char buff[64];
        snprintf(buff, 64, "ColorPalette index %ld out of bounds\n", idx);
        Sirtet_setError(buff);
        return NULL;
    }

    return (self->colors + idx);

}


