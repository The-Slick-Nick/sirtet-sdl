/* As this test is written within the library itself,
*  its include declaration and makefile will be slightly
*  different from what is presented in README 
*/
#include "../EWENIT/EWENIT.h"

int intFunc() {
    return 4;
}


char* strFunc() {
    return "Good result";
}


void test1() {
    int intResult = intFunc();
    ASSERT_EQUAL_INT(intResult, 4);
}

void test2() {
    char* strResult = strFunc();
    ASSERT_NOT_EQUAL_STR(strResult, "Bad result");
}

void test3() {
    int intResult = intFunc();
    ASSERT_GREATER_THAN_INT(intResult, 10);
}

void test4() {
    char* strResult = strFunc();
    ASSERT_EQUAL_STR(strResult, "Another bad one");
    ASSERT_NOT_EQUAL_STR(strResult, "Bad one again");
}


int main() {
    EWENIT_START;
    ADD_CASE(test1);
    ADD_CASE_CUSTOM(test2, "String Test");
    ADD_CASE(test3);
    ADD_CASE(test4);

/*     EWENIT_END; */
    EWENIT_END_COMPACT;
/*     EWENIT_END_VERBOSE; */
}


