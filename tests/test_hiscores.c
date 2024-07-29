
#include <EWENIT.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hiscores.h"
#include "sirtet.h"


/******************************************************************************
 * Tests on general/helper functions
******************************************************************************/


void testSortByBasis() {
    // sorting algo test

    int indices[8];
    int basis[8] =    {8, 1, 7, 2, 6, 3, 5, 4};
    //                 0, 1, 2, 3, 4, 5, 6, 7
    int expected[8] = {0, 2, 4, 6, 7, 5, 3, 1};

    sortByBasisDesc(8, indices, basis);
    for (int i = 0; i < 8; i++) {
        INFO_FMT("i = %d", i);
        ASSERT_EQUAL_INT(indices[i], expected[i]);
    }

    int *bigindices = (int*)malloc(10000 * sizeof(int));
    int *bigbasis = (int*)malloc(10000 * sizeof(int));


    for (int i = 0; i < 10000; i++) {
        bigbasis[i] = rand();
    }

    sortByBasisDesc(10000, bigindices, bigbasis);

    for (int i = 1; i < 10000; i++) {
        int leftval = bigbasis[bigindices[i - 1]];
        int rightval = bigbasis[bigindices[i]];
         if (leftval >= rightval) {
            TEST_PASS("Sort order holds");
         }
         else {
            TEST_FAIL_FMT(
                "Sort order violated: %d at %d vs %d at %d",
                leftval, i - 1, rightval, i
            );
         }
    }


    free(bigbasis);
    free(bigindices);

}

void testSortByOrder() {

    int arr[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int order[8] = {7, 6, 5, 4, 3, 2, 1, 0};
    int expected[8] = {8, 7, 6, 5, 4, 3, 2, 1};


    sortByOrder(arr, order, sizeof(int), 8);

    for (int i = 0; i < 8; i++) {
        ASSERT_EQUAL_INT(expected[i], arr[i]);
    }


    // large array
    size_t n = 1000;

    int *bigarr = (int*)malloc(n * sizeof(int));
    int *bigsorted = (int*)malloc(n * sizeof(int));
    int *bigorder = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        bigarr[i] = rand();
        bigorder[i] = i;
    }
    memcpy(bigsorted, bigarr, n * sizeof(int));

    // scramble
    for (int i = 0; i < n; i++) {
        size_t swapidx = i + rand() % (n - i);
        int hold = bigorder[i];
        bigorder[i] = bigorder[swapidx];
        bigorder[swapidx] = hold;
    }

    sortByOrder(bigsorted, bigorder, sizeof(int), n);

    for (int i = 0;i < n; i++) {
        ASSERT_EQUAL_INT(bigsorted[i], bigarr[bigorder[i]]);
    }


    free(bigorder);
    free(bigsorted);
    free(bigarr);

}

void testParseInt() {

    int num;
    int retval;


    /*** Good inputs ***/

    num = 0;
    retval = parseInt("123", &num);
    ASSERT_EQUAL_INT(retval, 3);
    ASSERT_EQUAL_INT(num, 123);

    // longer but with length specified
    num = 0;
    retval = parseInt("456__", &num);
    ASSERT_EQUAL_INT(retval, 3);
    ASSERT_EQUAL_INT(num, 456);

    // negative
    num = 0;
    retval = parseInt("-789", &num);
    ASSERT_EQUAL_INT(retval, 4);
    ASSERT_EQUAL_INT(num, -789);

    // maxvalue
    num = 0;
    retval = parseInt("2147483647", &num);
    ASSERT_EQUAL_INT(retval, 10);
    ASSERT_EQUAL_INT(num, 2147483647);

    // minvalue
    num = 0;
    retval = parseInt("-2147483648", &num);
    ASSERT_EQUAL_INT(retval, 11);
    ASSERT_EQUAL_INT(num, -2147483648);

    // leading zeroes
    num = 0;
    retval = parseInt("0123", &num);
    ASSERT_EQUAL_INT(retval, 4);
    ASSERT_EQUAL_INT(num, 123);

    // negative zero
    num = 0;
    retval = parseInt("-0", &num);
    ASSERT_EQUAL_INT(retval, 2);
    ASSERT_EQUAL_INT(num, 0);

    // broken-up number (not a fail)
    num = 0;
    retval = parseInt("12a3", &num);
    ASSERT_EQUAL_INT(retval, 2);
    ASSERT_EQUAL_INT(num, 12);

    /*** Bad inputs ***/

    // early negative
    num = 0;
    retval = parseInt("--123", &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // too big
    retval = parseInt("2147483648", &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);
    
    // way too big
    retval = parseInt("9999999999999999999999999999999999999999", &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // too small
    num = 0;
    retval = parseInt("-2147483649", &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // way too small
    retval = parseInt("-999999999999999999999999999999999999999", &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);
}


void testParseName() {

    int retval;
    char out[64];

    retval = parseName("hello 123", out, 64);
    ASSERT_EQUAL_INT(retval, 5);
    ASSERT_EQUAL_STR(out, "hello");

    retval = parseName("hithere 8", out, 64);
    ASSERT_EQUAL_INT(retval, 7);
    ASSERT_EQUAL_STR(out, "hithere");

}


/******************************************************************************
 * ScoreList tests
******************************************************************************/

void testScoreList() {

    ScoreList *sl = ScoreList_init(10, 3);

    ASSERT_EQUAL_INT(sl->len, 0);
    ASSERT_EQUAL_INT(sl->size, 10);


    int score;
    char name[16];
    int retval;

    // Nothing added yet
    score = 0;
    strcpy(name, "___");
    retval = ScoreList_pop(sl, name, &score);
    ASSERT_EQUAL_INT(retval, -1);
    retval = ScoreList_get(sl, 0, name, &score);
    ASSERT_EQUAL_INT(retval, -1);

    retval = ScoreList_add(sl, "BOB", 123);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(sl->len, 1);

    score = 0;
    strcpy(name, "___");
    retval = ScoreList_get(sl, 0, name, &score);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(sl->len, 1);
    ASSERT_EQUAL_STR(name, "BOB");
    ASSERT_EQUAL_INT(score, 123);

    score = 0;
    strcpy(name, "___");
    retval = ScoreList_pop(sl, name, &score);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(sl->len, 0);
    ASSERT_EQUAL_STR(name, "BOB");
    ASSERT_EQUAL_INT(score, 123);
    
    // cannot add too large a name
    retval = ScoreList_add(sl, "A REALLY LONG NAME", 10);
    ASSERT_EQUAL_INT(retval, -1);

    // Cannot add more than SIZE
    for (int i = 0; i < 10; i++) {
        snprintf(name, 4, "P%d", i);
        retval = ScoreList_add(sl, name, score);
        ASSERT_EQUAL_INT(retval, 0);
    }
    retval = ScoreList_add(sl, "BAD", 10);
    ASSERT_EQUAL_INT(retval, -1);


    ScoreList_deconstruct(sl);
}




void testScoreListSort() {
    // Ensure sorting works correctly
    ScoreList *sl = ScoreList_init(10, 5);


    ScoreList_add(sl, "bob", 10);
    ScoreList_add(sl, "alice", -15);
    ScoreList_add(sl, "zack", 100);
    ScoreList_add(sl, "john", 5);


    ScoreList_sort(sl);
    int score;
    char name[6];
    int retval;

    retval = ScoreList_get(sl, 0, name, &score);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_STR(name, "zack");
    ASSERT_EQUAL_INT(score, 100);

    retval = ScoreList_get(sl, 1, name, &score);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_STR(name, "bob");
    ASSERT_EQUAL_INT(score, 10);
    
    retval = ScoreList_get(sl, 2, name, &score);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_STR(name, "john");
    ASSERT_EQUAL_INT(score, 5);

    retval = ScoreList_get(sl, 3, name, &score);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_STR(name, "alice");
    ASSERT_EQUAL_INT(score, -15);


    ScoreList_deconstruct(sl);
}

static inline void makefile(const char* content) {
    FILE *f_write = fopen("test.txt", "w");
    if (f_write == NULL) {
        TEST_FAIL("File failed to open for writing");
    }
    fprintf(f_write, "%s", content);
    fclose(f_write);
}

void testFileInteraction() {

    Sirtet_setError("");

    /*** GOOD ***/

    makefile("ABC 10\nDEF 45\n");

    FILE *f_read = fopen("test.txt", "r");
    ScoreList *sl = ScoreList_init(10, 3);
    ScoreList_readFile(sl, f_read);


    int outscore;
    char outname[16];
    int retval;

    ASSERT_EQUAL_INT(sl->len, 2);
    retval = ScoreList_get(sl, 0, outname, &outscore);
    ASSERT_EQUAL_INT(retval, 0);
    if (retval != 0) {
        char *errmsg = Sirtet_getError();
        INFO(errmsg);
    }
    ASSERT_EQUAL_INT(outscore, 10);
    ASSERT_EQUAL_STR(outname, "ABC");


    retval = ScoreList_get(sl, 1, outname, &outscore);
    ASSERT_EQUAL_INT(retval, 0);
    if (retval != 0) {
        char *errmsg = Sirtet_getError();
        INFO(errmsg);
    }
    ASSERT_EQUAL_INT(outscore, 45);
    ASSERT_EQUAL_STR(outname, "DEF");

    fclose(f_read);
    ScoreList_deconstruct(sl);


    /*** OVERFLOW ON NAME ***/


    makefile("VERYLONGNAME 10\nANOTHERREALLYLONGNAMELONGER 25\n");
    f_read = fopen("test.txt", "r");

    sl = ScoreList_init(10, 3);
    retval = ScoreList_readFile(sl, f_read);
    ASSERT_EQUAL_INT(retval, -1);

    fclose(f_read);
    ScoreList_deconstruct(sl);


    /*** OVERFLOW ON ENTRIES ***/

    makefile("abc 10\ndef 20\nghi 30\n");
    sl = ScoreList_init(2, 3);

    f_read = fopen("test.txt", "r");

    retval = ScoreList_readFile(sl, f_read);
    ASSERT_EQUAL_INT(retval, -1);

    fclose(f_read);
    ScoreList_deconstruct(sl);


    /*** Integer  Limits ***/

    INT_MAX; //  2147483647
    INT_MIN; // -2147483648
    makefile("name 2147483647\n name2 -2147483648\n");
    f_read = fopen("test.txt", "r");
    sl = ScoreList_init(10, 10);
    retval = ScoreList_readFile(sl, f_read);

    ASSERT_EQUAL_INT(retval, 0);
    if (retval != 0) {
        INFO(Sirtet_getError());
    }
    ASSERT_EQUAL_INT(sl->len, 2);

    ScoreList_get(sl, 0, outname, &outscore);
    ASSERT_EQUAL_INT(outscore, INT_MAX);

    ScoreList_get(sl, 1, outname, &outscore);
    ASSERT_EQUAL_INT(outscore, INT_MIN);

    fclose(f_read);
    ScoreList_deconstruct(sl);

    /*** Integer  Overflows ***/
    makefile("name 2147483648");
    f_read = fopen("test.txt", "r");
    sl = ScoreList_init(10, 10);

    retval = ScoreList_readFile(sl, f_read);
    ASSERT_EQUAL_INT(retval, -1);

    fclose(f_read);
    makefile("name -2147483649");

    f_read = fopen("test.txt", "r");
    sl = ScoreList_init(10, 10);

    retval = ScoreList_readFile(sl, f_read);
    ASSERT_EQUAL_INT(retval, -1);

    ScoreList_deconstruct(sl);
    fclose(f_read);

}



int main() {
    EWENIT_START;
    ADD_CASE(testParseInt);
    ADD_CASE(testParseName);
    ADD_CASE(testSortByBasis);
    ADD_CASE(testSortByOrder);



    ADD_CASE(testScoreList);
    ADD_CASE(testScoreListSort);
    ADD_CASE(testFileInteraction);
    EWENIT_END;
}
