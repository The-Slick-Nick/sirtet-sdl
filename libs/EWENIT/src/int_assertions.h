/*========================================================================================
int_assertions.h

Assertion functions revolving around integers
Also includes boolean checks
========================================================================================*/
#ifndef INCLUDE_GUARD_INT_ASSERTIONS
#define INCLUDE_GUARD_INT_ASSERTIONS

#include "core.h"

// Compares two ints for exact equality
int assert_equal_int(TestSuite* ts, int val1, int val2, char* file_name, long line_num);
// Compare two ints to assess their inequality
int assert_not_equal_int(TestSuite* ts, int val1, int val2, char* file_name, long line_num);

// Compare two ints and assert that val1 > val2
int assert_greater_than_int(TestSuite* ts, int val1, int val2, char* file_name, long line_num);

// Ensure value is true
int assert_true(TestSuite* ts, int val1, char* file_name, long line_num);

// Ensure value is false
int assert_false(TestSuite* ts, int val1, char* file_name, long line_num);

#endif