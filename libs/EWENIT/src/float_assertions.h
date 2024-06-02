/*========================================================================================
float_assertions.h

Assertion functions revolving around floating point numbers
========================================================================================*/

#ifndef INCLUDE_GUARD_FLOAT_ASSERTIONS
#define INCLUDE_GUARD_FLOAT_ASSERTIONS

#include <math.h>

// #define ALMOST_EQUAL_FLOAT(val1, val2) fabs(val1 - val2) < FLT_EPSILON
#define ALMOST_EQUAL_FLOAT(val1, val2) \
    (fabs(val1 - val2) <= (\
    (fabs(val1) < fabs(val2) ? fabs(val2) : fabs(val1)) * FLT_EPSILON\
    ))

// Compares two floats for exact equality
int assert_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num);
// Compare two floats to assess their inequality
int assert_not_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num);

// Compares two floats assess if they are within FLT_EPSILON
int assert_almost_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num);

// Compares two floats to assess if they are not within FLT_EPSILON
int assert_not_almost_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num);

// Compare two floats and assert that val1 > val2
int assert_greater_than_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num);

#endif