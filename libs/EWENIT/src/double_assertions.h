/*========================================================================================
double_assertions.h

Assertion functions revolving around doubleing point numbers
========================================================================================*/

#ifndef INCLUDE_GUARD_DOUBLE_ASSERTIONS
#define INCLUDE_GUARD_DOUBLE_ASSERTIONS

#include "core.h"
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define ALMOST_EQUAL_DOUBLE(val1, val2) \
    (fabs(val1 - val2) <= (\
    (fabs(val1) < fabs(val2) ? fabs(val2) : fabs(val1)) * DBL_EPSILON\
    ))


// Compares two doubles for exact equality
int assert_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num);

// Compare two doubles to assess their inequality
int assert_not_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num);

// Compare two doubles to assess if they are within DBL_EPSILON
int assert_almost_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num);

// Compare two doubles to assess if they are not within DBL_EPSILON
int assert_not_almost_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num);

// Compare two doubles and assert that val1 > val2
int assert_greater_than_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num);


#endif
