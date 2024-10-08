/* hiscores.h
 *
 * This header is meant to provide a way to read, write, and store
 * high scores from the game.
 *
 * (at least for now) it includes both Sirtet-integrated methods
 * and its own tooling for parsing high score files & numbers & whatnot.
 *
 */

#ifndef HISCORES_H
#define HISCORES_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


typedef struct {
    size_t len;
    size_t size;
    size_t namelen;
    char *names;
    int *scores;

    int *intbuff;
    char *strbuff;
} ScoreList;


// A struct representing a display of the above
typedef struct {

    size_t n_lbls;
    SDL_Texture **name_lbls;
    SDL_Texture **score_lbls;
    SDL_Texture **rank_lbls;


} ScoreDisplay; 


/******************************************************************************
 * ScoreList methods
******************************************************************************/

ScoreList* ScoreList_initFromFile(FILE *f, size_t size, size_t namelen);

// Initialize a ScoreList with the given parameters
ScoreList* ScoreList_init(size_t size, size_t namelen);

// Destroy a ScoreList and free all its memory
void ScoreList_deconstruct(ScoreList *self);

// Write to the given opened file
int ScoreList_toFile(ScoreList *self, FILE *f);

// Read contents of a text file into existing ScoreList
int ScoreList_readFile(ScoreList *self, FILE *f);

// Add an entry to score list, returning a status code
int ScoreList_add(ScoreList *self, char *name, int score);

// Remove the final element of the scorelist, returning a status code
int ScoreList_pop(ScoreList *self, char *out_name, int *out_score);

// Retrieve the entry at the given index
int ScoreList_get(ScoreList *self, size_t idx, char *out_name, int *out_score);

// Sort scoreList in descending order by score
int ScoreList_sort(ScoreList *self);



/******************************************************************************
 * ScoreDisplay methods
******************************************************************************/

/**
 * @brief Create a ScoreDisplay from an existing ScoreList
 * @param sl - ScoreList to use as basis
 * @param first - First index of ScoreList to read from
 * @param last - Final index of ScoreList to read from
 * @param rank_start - First rank number to show (in labels)
 */
ScoreDisplay *ScoreDisplay_init(
    ScoreList *sl, 
    int first, int last, int rank_start,
    SDL_Renderer *rend,
    TTF_Font *lbl_font, const SDL_Color *lbl_col
);

/**
 * @brief - Destroy a ScoreDisplay, freeing any memory it has reserved
 */
int ScoreDisplay_deconstruct(ScoreDisplay *self);

/**
 * @param self - ScoreDisplay to draw
 * @param n - Number of labels to draw. 0 or negative to draw all
 * @param rend - SDL Renderer pointer to draw with
 * @param draw_window - Pointer to dimensions to draw within
 * @param out_dim - Pointer to rectangle that will be written into
 *                  with the final overall drawn dimensions.
 */
int ScoreDisplay_draw(
    ScoreDisplay *self,
    int n,
    SDL_Renderer *rend,
    const SDL_Rect *draw_window,
    SDL_Rect *out_dim

);





#endif

