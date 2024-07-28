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

#include <stdlib.h>


// Parse a character string of the provided length for an integer
// Return 0 on success, or -1 on error
int parseInt(char* txt, int *out_num);


typedef struct {
    size_t len;
    size_t size;
    size_t namelen;
    char *names;
    int *scores;
} ScoreList;


// ScoreList *ScoreList_initFromFile(FILE *f);
// void ScoreList_deconstruct(ScoreList *self);
// void ScoreList_toFile(ScoreList *self, FILE *f);

#endif

