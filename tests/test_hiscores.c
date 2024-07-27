
#include <EWENIT.h>
#include "hiscores.h"


// int parseNum(char* txt, size_t len, int *out_num) {
void testParseInt() {

    int num;
    int retval;


    /*** Good inputs ***/

    num = 0;
    retval = parseInt("123", 3, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, 123);

    // longer but with length specified
    num = 0;
    retval = parseInt("456__", 3, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, 456);

    // negative
    num = 0;
    retval = parseInt("-789", 4, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, -789);

    // maxvalue
    num = 0;
    retval = parseInt("2147483647", 10, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, 2147483647);

    // minvalue
    num = 0;
    retval = parseInt("-2147483648", 11, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, -2147483648);

    // shorter than input
    num = 0;
    retval = parseInt("1234", 2, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, 12);

    // leading zeroes
    num = 0;
    retval = parseInt("0123", 4, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, 123);

    // negative zero
    num = 0;
    retval = parseInt("-0", 2, &num);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_EQUAL_INT(num, 0);




    /*** Bad inputs ***/

    // too short
    num = 0;
    retval = parseInt("123", 4, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // unrecognized char
    num = 0;
    retval = parseInt("12a3", 3, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);


    // early negative
    num = 0;
    retval = parseInt("--123", 5, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // too big
    retval = parseInt("2147483648", 10, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);
    
    // way too big
    retval = parseInt("9999999999999999999999999999999999999999", 40, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // too small
    num = 0;
    retval = parseInt("-2147483649", 11, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

    // way too small
    retval = parseInt("-999999999999999999999999999999999999999", 40, &num);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(num, 0);

}

int main() {
    EWENIT_START;
    ADD_CASE(testParseInt);
    EWENIT_END;
}
