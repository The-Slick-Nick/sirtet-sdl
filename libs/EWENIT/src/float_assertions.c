

#include <stdbool.h>
#include <float.h>
#include <math.h>

#include "core.h"
#include "float_assertions.h"
#include "assertion_constants.h"


// Compares two floats for exact equality
int assert_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (val1 == val2) {
        snprintf(msg, MSG_BUFF_SIZE, "%f == %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }
    else {
        snprintf(msg, MSG_BUFF_SIZE, "%f != %f", val1, val2);
        return TestSuite_fail(ts, file_name, line_num, msg);
    }
}

// Compare two floats to assess their inequality
int assert_not_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (val1 != val2) {
        snprintf(msg, MSG_BUFF_SIZE, "%f != %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }
    else {
        snprintf(msg, MSG_BUFF_SIZE, "%f == %f", val1, val2);
        return TestSuite_fail(ts, file_name, line_num, msg);
    }
}

// Compares two floats assess if they are within FLT_EPSILON
int assert_almost_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (ALMOST_EQUAL_FLOAT(val1, val2)) {
        snprintf(msg, MSG_BUFF_SIZE, "%f ~= %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }
    else {
        snprintf(msg, MSG_BUFF_SIZE, "%f !~= %f", val1, val2);
        return TestSuite_fail(ts, file_name, line_num, msg);
    }
}

// Compares two floats to assess if they are not within FLT_EPSILON
int assert_not_almost_equal_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num)
{

    char msg[MSG_BUFF_SIZE];

    if (!ALMOST_EQUAL_FLOAT(val1, val2)) {
        snprintf(msg, MSG_BUFF_SIZE, "%f !~= %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }
    else {
        snprintf(msg, MSG_BUFF_SIZE, "%f ~= %f", val1, val2);
        return TestSuite_fail(ts, file_name, line_num, msg);
    }

}

// Compare two floats and assert that val1 > val2
int assert_greater_than_float(TestSuite* ts, float val1, float val2, char* file_name, long line_num)
{
    char msg[MSG_BUFF_SIZE];

    if (val1 > val2) {
        snprintf(msg, MSG_BUFF_SIZE, "%f > %f", val1, val2);
        return TestSuite_pass(ts, file_name, line_num, msg);
    }
    else {
        snprintf(msg, MSG_BUFF_SIZE, "%f <= %f", val1, val2);
        return TestSuite_fail(ts, file_name, line_num, msg);
    }
}
