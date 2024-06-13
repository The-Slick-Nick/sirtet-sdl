#include "block.h"
#include <stdio.h>
#include <SDL2/SDL.h>

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1080

int main(int argc, char* argv[]) {

    SDL_Window *wind = SDL_CreateWindow(
        "Test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0
    );

    long before_rotation = 0b1000;
    long after_rotation = rotateBlockContents180(before_rotation, 4);
    printf("Before we rotated the value was %ld\n", before_rotation);
    printf("AFter we rotated the value was %ld\n", after_rotation);

    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}
