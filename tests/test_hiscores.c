
#include <EWENIT.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hiscores.h"
#include "sirtet.h"
#include "utilities.h"


/******************************************************************************
 * Tests on general/helper functions
******************************************************************************/

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

// make a test_txt file in current directory with provided content
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
    ADD_CASE(testScoreList);
    ADD_CASE(testScoreListSort);
    ADD_CASE(testFileInteraction);
    EWENIT_END;
}
