

#include <EWENIT.h>
#include <SDL2/SDL.h>
#include "colorpalette.h"

void testInitPalette() {
    // ColorPalette *mypalette = ColorPalette_init(const char *name, size_t size, ...)
    ColorPalette *mypalette = ColorPalette_init(
        "Test",
        3,
        (SDL_Color){155, 155, 155, 255},
        (SDL_Color){200, 200, 200, 255},
        (SDL_Color){55, 55, 55, 255}
    );

    ColorPalette *otherpalette = ColorPalette_init(
        "Test2", 1, (SDL_Color){10, 10, 10, 255}
    );

    ASSERT_EQUAL_INT(mypalette->size, 3);
    ASSERT_EQUAL_STR(mypalette->name, "Test");

    ASSERT_EQUAL_INT(otherpalette->size, 1);
    ASSERT_EQUAL_STR(otherpalette->name, "Test2");

    ColorPalette_deconstruct(mypalette);
}

#define MANY_PALETTE_COUNT 10000
void testManyPalettes() {
    // This test is mostly to check for leaks - I think va_args is 
    // possibly leaving an inconsequential amount of memory memory un-freed,
    // (whish is fine), so I'm testing if
    // the amount of accessible memory at program exit scales with
    // the number of palettes created or not


    ColorPalette *palettes[MANY_PALETTE_COUNT];

    for (int i = 0; i < MANY_PALETTE_COUNT; i++) {
         palettes[i] = ColorPalette_init(
            "PALETTE", 1, (SDL_Color){255, 255, 255, 255}
        );

    }

    // check separately from intialization to catch memory overflow
    // shenanigans
    for (int i = 0; i < MANY_PALETTE_COUNT; i++) {
        ASSERT_EQUAL_INT(palettes[i]->size, 1);
        ASSERT_EQUAL_STR(palettes[i]->name, "PALETTE");
    }


    for (int i = 0; i < MANY_PALETTE_COUNT; i++) {
         ColorPalette_deconstruct(palettes[i]);
    }
}

void testGetColor() {

    ColorPalette *cp = ColorPalette_init(
        "Test1", 2,
        (SDL_Color){1, 2, 3, 4},
        (SDL_Color){5, 6, 7, 8}
    );
    SDL_Color outcol;

    int retval;
    retval = ColorPalette_getColor(cp, 0, &outcol);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(outcol.r, 1);
    ASSERT_EQUAL_INT(outcol.g, 2);
    ASSERT_EQUAL_INT(outcol.b, 3);
    ASSERT_EQUAL_INT(outcol.a, 4);


    retval = ColorPalette_getColor(cp, 1, &outcol);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(outcol.r, 5);
    ASSERT_EQUAL_INT(outcol.g, 6);
    ASSERT_EQUAL_INT(outcol.b, 7);
    ASSERT_EQUAL_INT(outcol.a, 8);

    retval = ColorPalette_getColor(cp, 2, &outcol);
    ASSERT_EQUAL_INT(retval, -1);
    
    retval = ColorPalette_getColor(cp, 0, NULL);
    ASSERT_EQUAL_INT(retval, -1);

    retval = ColorPalette_getColor(NULL, 0, &outcol);
    ASSERT_EQUAL_INT(retval, -1);
}

void testGetColorPtr() {

    ColorPalette *cp = ColorPalette_init(
        "Test1", 2,
        (SDL_Color){1, 2, 3, 4},
        (SDL_Color){5, 6, 7, 8}
    );
    SDL_Color *retcol;

    retcol = ColorPalette_getColorPtr(cp, 0);
    ASSERT_EQUAL_INT(retcol->r, 1);
    ASSERT_EQUAL_INT(retcol->g, 2);
    ASSERT_EQUAL_INT(retcol->b, 3);
    ASSERT_EQUAL_INT(retcol->a, 4);

    retcol = ColorPalette_getColorPtr(cp, 1);
    ASSERT_EQUAL_INT(retcol->r, 5);
    ASSERT_EQUAL_INT(retcol->g, 6);
    ASSERT_EQUAL_INT(retcol->b, 7);
    ASSERT_EQUAL_INT(retcol->a, 8);

    // There is not yet a NULL assertion check
    retcol = ColorPalette_getColorPtr(cp, 2);
    if (retcol == NULL) {
        TEST_PASS("NULL return on OOB index");
    }
    else {
        TEST_FAIL("Expected NULL return on OOB index");
    }

    retcol = ColorPalette_getColorPtr(NULL, 0);
    if (retcol == NULL) {
        TEST_PASS("NULL return on NULL input");
    }
    else {
        TEST_FAIL("Expected NULL return on NULL input");
    }
}


int main() {
    EWENIT_START;
    ADD_CASE(testInitPalette);
    ADD_CASE(testManyPalettes);
    ADD_CASE(testGetColor);
    ADD_CASE(testGetColorPtr);
    EWENIT_END;
    return 0;
}
