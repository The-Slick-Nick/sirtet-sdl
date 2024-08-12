#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif


#include "sirtet.h"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <limits.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    run();
    return 0;
}
