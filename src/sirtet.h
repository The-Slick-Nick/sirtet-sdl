#ifndef RUN_GAME_H
#define RUN_GAME_H

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdbool.h>


/******************************************************************************
 * Config macros/constants
******************************************************************************/


/*** High Score Settings ***/

#define HISCORES_MAX_SIZE 100
#define HISCORES_NAME_LEN 3


/*** Game Settings / Internal Configuration ***/

#define MAX_TILE_SIZE 5
#define MIN_TILE_SIZE 3
#define INIT_TILE_SIZE 4

#define MIN_LEVEL 0
#define MAX_LEVEL 10

#define INVALID_BLOCK_ID -1


/*** Color/Drawing configuration ***/
// menu
#define MENUCOL_ACTIVE ((SDL_Color){0, 0, 0})
#define MENUCOL_INACTIVE ((SDL_Color){100, 100, 100})
#define BACKGROUNDCOL ((SDL_Color){155, 155, 155})

// Draw weights in main game area
#define GAMEAREA_WEIGHT_W 3
#define SIDEBAR_WEIGHT_W 1
#define TOTAL_WEIGHT_W (GAMEAREA_WEIGHT_W + SIDEBAR_WEIGHT_W)

// Color in main game state
#define OUTSET_COL (SDL_Color){0, 0, 0, 255}
#define INSET_COL (SDL_Color){50, 50, 50, 255}
#define BORDER_SIZE 24


/*** other things ***/

// Max array/size for stack-declared arrays used in functions that require
// supplemental memory, that will otherwise allocate from heap 
// baesd on size
#define STATIC_ARRMAX 128
// TODO: Search out examples of this being used


/******************************************************************************
 * High-level prototypes
******************************************************************************/

int run();
void Sirtet_setError(const char *errmsg);
char* Sirtet_getError();


#endif
