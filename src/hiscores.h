/* hiscores.h
 *
 * This header is meant to provide a way to read, write, and store
 * high scores from the game.
 *
 * (at least for now) it includes both Sirtet-integrated methods
 * and its own tooling for parsing high score files & numbers & whatnot.
 *
 */

#include <stdlib.h>


// Parse a character string of the provided length for an integer
// Return 0 on success, or -1 on error
int parseInt(char* txt, size_t len, int *out_num);
