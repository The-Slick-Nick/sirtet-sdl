/******************************************************************************
* utilities.h
*
* General purpose utilities and such
******************************************************************************/

// TODO: Separate these out into specific `utilities_xxx.h` files, when there
// are enough here

#include <stdint.h>
#include <stdlib.h>

/******************************************************************************
* Macros
******************************************************************************/

#define MIN2(a, b) ((a) > (b) ? (b) : (a))
#define MAX2(a, b) ((a) < (b) ? (b) : (a))

#define MIN3(a, b, c) (MIN2(MIN2(a, b), (c)))
#define MAX3(a, b, c) MAX2(MAX2(a, b), (c))


/******************************************************************************
* Functions
******************************************************************************/

// Parse an integer out of a string of text, returning the number of characters
// parsed
int parseInt(char* txt, int *out_num);

// Parse a string for characters into out_str, returning the number of 
// charactesr parsed
int parseName(const char* txt, char *out_str, size_t maxlen);


// Populate sort order of `basis` into `indices`
void sortByBasisDesc(size_t len, int *indices, const int *basis);


// Sort `tosort` given the index order in `order`
void sortByOrder(void *tosort, const int *order, size_t elem_sz, size_t elem_n);

