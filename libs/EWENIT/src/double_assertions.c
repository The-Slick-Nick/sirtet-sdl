#include "double_assertions.h"

#include "core.h"
#include "assertion_constants.h"
#include <stdbool.h>
#include <float.h>
#include <math.h>


// Compares two doubles for exact equality
int assert_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (val1 == val2) {
        snprintf(msg, MSG_BUFF_SIZE, "%f == %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }

    snprintf(msg, MSG_BUFF_SIZE, "%f != %f", val1, val2);
    return TestSuite_fail(ts, file_name, line_num, msg);
}

// Compare two doubles to assess their inequality
int assert_not_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num)
{

    char msg[MSG_BUFF_SIZE];

    if (val1 != val2) {
        snprintf(msg, MSG_BUFF_SIZE, "%f != %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }

    snprintf(msg, MSG_BUFF_SIZE, "%f == %f", val1, val2);
    return TestSuite_fail(ts,  file_name, line_num, msg);
}

// Compare two doubles to assess if they are within DBL_EPSILON
int assert_almost_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (ALMOST_EQUAL_DOUBLE(val1, val2)) {
        snprintf(msg, MSG_BUFF_SIZE, "%f ~= %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }

    snprintf(msg, MSG_BUFF_SIZE, "%f !~= %f", val1, val2);
    return TestSuite_fail(ts, file_name, line_num, msg);
}

// Compare two doubles to assess if they are not within DBL_EPSILON
int assert_not_almost_equal_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (!ALMOST_EQUAL_DOUBLE(val1, val2)) {
        snprintf(msg, MSG_BUFF_SIZE, "%f !~= %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }

    snprintf(msg, MSG_BUFF_SIZE, "%f ~= %f", val1, val2);
    return TestSuite_fail(ts, file_name, line_num, msg);
}

// Compare two doubles and assert that val1 > val2
int assert_greater_than_double(TestSuite* ts, double val1, double val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (val1 > val2) {
        snprintf(msg, MSG_BUFF_SIZE, "%f > %f",val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }

    snprintf(msg, MSG_BUFF_SIZE, "%f <= %f", val1, val2);
    return TestSuite_fail(ts, file_name, line_num, msg);
}
