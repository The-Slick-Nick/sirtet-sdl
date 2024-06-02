/*
str_assertions.h

Assertion functions revolving around strings
*/

#ifndef INCLUDE_GUARD_STR_ASSERTIONS
#define INCLUDE_GUARD_STR_ASSERTIONS

#include <string.h>
#include "core.h"

int assert_equal_str(TestSuite* ts, char* val1, char* val2, char* file_name, long line_num);
int assert_not_equal_str(TestSuite* ts, char* val1, char* val2, char* file_name, long line_num);

#endif