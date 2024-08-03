
#include <EWENIT.h>
#include "utilities.h"


/******************************************************************************
 * Test definition
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
 * Test running
******************************************************************************/

int main() {
    EWENIT_START;
    ADD_CASE(testParseInt);
    ADD_CASE(testParseName);
    ADD_CASE(testSortByBasis);
    ADD_CASE(testSortByOrder);
    EWENIT_END;
}
