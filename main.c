#include "run_game.h"
#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#include "inputs.h"


// https://www.libsdl.org/release/SDL-1.2.15/docs.html/sdlkey.html

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *wind = SDL_CreateWindow(
        "Test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        100, 100, 0
    );

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);

    if (!wind || !rend) {
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return -1;
    }


    clock_t start_t, report_start_t;
    double elapsed_total;
    double elapsed_report;  // time since time was reported

    start_t = clock();
    report_start_t = clock();
    elapsed_total = 0;

    GamecodeMap keymap;
    keymap.head = 0;
    addKeymap(&keymap, GAMECODE_ROTATE, SDL_SCANCODE_SPACE, 1, 1);


    int hardware_states[(int)SDL_NUM_SCANCODES] = {INT_MIN};
    bool gamecode_states[(int)NUM_GAMECODES] = {false};

    for (int i = 0; i < (int)SDL_NUM_SCANCODES; i++) { hardware_states[i] = INT_MIN; }
    for (int i = 0; i < (int)NUM_GAMECODES; i++) { gamecode_states[i] = false; }

    while (elapsed_total < 20) {


        processHardwareInputs(hardware_states);
        // processGamecodes(bool *gamecode_states, int *hardware_states, GamecodeMap *all_mappings)
        processGamecodes(gamecode_states, hardware_states, &keymap);

       
        if (gamecode_states[(int)GAMECODE_ROTATE]) {
            printf("Rotate!\n");
        }

        elapsed_total = (double)(clock() - start_t) / CLOCKS_PER_SEC;
        elapsed_report = (double)(clock() - report_start_t) / CLOCKS_PER_SEC;

        if (elapsed_report >= 1) {
            printf("%f seconds elapsed_total\n", elapsed_total);
            report_start_t = clock();
        }
    }


    SDL_Quit();
    return 0;
}
