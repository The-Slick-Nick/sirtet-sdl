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




typedef struct {
    size_t len;
    size_t size;
    size_t namelen;
    char *names;
    int *scores;

    // TODO: Remove name_hld (and possibly scores_hld?)
    int *scores_hld;    // Supplementary array for use in sorting
    char *name_hld;     // Supplementary char buffer for use in sorting
} ScoreList;


/******************************************************************************
 * General Utilities
******************************************************************************/

// TODO: Perhaps move this into a general utilities file?

// Populate sort order of `basis` into `indices`
void sortByBasisDesc(size_t len, int *indices, const int *basis);


// Sort `tosort` given the index order in `order`
void sortByOrder(void *tosort, const int *order, size_t elem_sz, size_t elem_n);


// Return the number of character parsed in total
int parseInt(char* txt, int *out_num);


// Parse a string for characters into out_str
// Return the number of character parsed in total
int parseName(const char* txt, char *out_str, size_t maxlen);

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

#endif

