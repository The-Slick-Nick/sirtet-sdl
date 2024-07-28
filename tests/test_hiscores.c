
#include <EWENIT.h>
#include "hiscores.h"


// int parseNum(char* txt, size_t len, int *out_num) {
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

int main() {
    EWENIT_START;
    ADD_CASE(testParseInt);
    EWENIT_END;
}
