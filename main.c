#include "run_game.h"
#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#include "inputs.h"
#include "run_game.h"

// https://www.libsdl.org/release/SDL-1.2.15/docs.html/sdlkey.html

int main(int argc, char* argv[]) {


    run();
    return 0;

    // SDL_Init(SDL_INIT_VIDEO);
    // SDL_Quit();
    // return 0;
}
