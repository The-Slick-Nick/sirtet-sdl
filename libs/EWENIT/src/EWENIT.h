/*========================================================================================
EWENIT.c
Version 2.0.1

⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣐⡙⠶⣿⣷⣌⡳⣿⡿⠿⢷⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣠⡴⠮⠁⠀⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠙⠟⢿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢠⡾⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⣀⡘⠿⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣿⠃⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠻⣅⠀⠀⠀⠀⠀⢈⣹⣄⣼⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣿⠓⠒⠂⠀⣿⣰⣠⡀⠀⠀⢰⣦⣾⠷⠶⢤⣾⡦⠾⠉⠉⠉⠙⠿⢶⣦⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣿⣤⣤⣀⢀⣠⠄⠉⣻⡷⠾⠛⠁⠀⠀⠀⠀⠀⠝⢦⠀⠀⠀⠀⠀⠀⠀⠀⠀⣻⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡄⢀⣀⣠⡴⢿⣤⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠻⣧⣀⣌⠁⢀⡶⠞⠁⠀⠀⠀⣀⣤⣤⣄⠀⠀⢀⡾⠀⠀⠀⠀⠀⣠⡾⠿⠛⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡼⠛⠋⠉⠀⠀⠀⠀⠀⠉⠉⠙⠳⣄⡀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣠⣿⡿⠉⠉⠉⠀⠀⠀⠀⣰⣾⣿⣿⠾⠋⠀⠀⠈⢹⡷⠶⠶⠶⠿⠟⣷⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣴⠟⠒⠛⠉⠛⠒⠦⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠙⢦⡀⠀⠀⠀
⠀⠀⢀⣼⡿⠋⠀⠀⠀⠀⠀⠀⠀⠘⠉⠀⠀⠀⠀⠀⠀⠀⠀⣸⠇⠀⠀⠀⠀⠀⠹⣷⣶⠶⠶⠤⠶⣶⣟⡋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠑⢦⠀⠀⠀⠀⠀⠀⠀⠀⢻⡄⠀⠀
⠀⣠⣾⡟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⠏⠀⠀⠀⠀⠀⠀⠀⠙⣷⡄⠀⠀⠀⠀⠀⠙⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⡄⠀
⣸⡿⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠰⣏⢿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣄
⢿⣇⠀⢀⣶⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⡾⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠋⠘⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⢺
⢸⣿⢿⡏⠀⠀⠀⣀⡤⠀⢀⣀⣀⣠⣤⡶⠶⠟⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠟⠀⠾⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸
⠀⠙⢿⡟⠓⠛⠋⣁⣀⣴⣿⣿⣿⡉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠶⠋⠀⠀⣤⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀
⠀⠀⠀⠙⠻⠟⠛⠛⠉⠉⠀⠀⢹⣿⠦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣆⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡟⠛⠓⠦⢤⠆⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⠖⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡗⠢⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠟⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣶⢤⣤⣀⣀⡀⠀⢀⣴⠴⠚⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡼⡀⠀⠀⠀⠀⢸⡏
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⡀⣈⠉⠉⠉⡟⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢐⡏⢰⡇⠀⠀⠀⠀⣾⡇
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⠓⠶⣞⣁⡀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣹⠀⠀⠀⠀⠀⠀⣰⠟⢐⡿⠃⠀⠀⠀⠀⣸⠃
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣧⠀⠀⠀⠀⠙⠛⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⡴⠋⠀⠀⠀⢠⣤⠞⠁⢀⡿⠃⠀⠀⣠⠟⣴⡟⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡛⠲⢦⣄⣀⣠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⣀⣀⢀⡴⠋⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣁⣀⣠⣞⣡⣼⡿⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣷⣄⣀⣈⡉⣁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠀⠀⠉⠉⢹⡇⠀⠀⠀⠀⠀⣀⣰⣾⠛⣻⡟⠈⠉⠉⠉⢿⣿⠁⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣌⣉⠉⠉⣡⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⠞⠁⠀⠀⢀⣴⡿⣤⣀⣴⠶⡴⠏⠉⢹⣿⠀⣿⡇⠀⠀⠀⠀⠈⣿⡄⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣧⡉⠓⠆⠀⠙⣦⡀⠀⠀⣀⡀⠀⣀⣤⠴⠛⠁⠀⠀⠀⠀⢼⡏⠀⠀⠀⣿⡇⠀⠀⠀⢸⣿⠀⢿⣇⠀⠀⠀⠀⣰⣿⠃⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣷⠤⢤⣄⣀⣈⣙⣻⡿⠋⠉⠻⣿⣀⠀⠀⠀⢀⣀⣤⣶⠏⠀⠀⠀⠀⣿⡇⠀⠀⠀⢸⣿⠀⠘⣿⡄⠀⠀⠀⣿⡏⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣇⠀⠀⠈⠉⠉⢿⣿⠃⠀⠀⠀⢻⣿⡟⠃⠀⠈⠉⢹⣿⠀⠀⠀⠀⢀⣿⠁⠀⠀⠀⠘⣿⠀⠀⢹⡇⠀⠀⠀⣿⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡀⠀⠀⠀⠀⢸⣿⠀⠀⠀⠀⠈⣿⡇⠀⠀⠀⠀⢸⡟⠀⠀⠀⠀⢸⣿⠀⠀⠀⠀⢀⣿⡇⠀⢸⡇⠀⠀⠀⣿⡆⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣇⠀⠀⠀⠀⣿⡏⠀⠀⠀⠀⠀⢻⣇⠀⠀⠀⠀⢸⣧⠀⠀⠀⠀⢸⣿⠀⢀⡀⠀⣾⣿⠁⠀⣼⡇⠀⠀⠀⠘⣿⡀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⠀⠀⢀⣿⡇⠀⠀⠀⠀⠀⠀⣿⡀⠀⠀⠀⢀⣿⡄⠀⠀⠀⣿⣏⣀⣾⣀⣰⡿⠃⠀⣰⣿⠀⠀⠀⠀⠀⣼⡇⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠃⠀⠀⢸⣿⡀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⣼⣿⠇⠀⠀⠀⠉⠉⠉⠉⠉⠉⠀⠀⠀⣿⠇⠀⢀⡀⠀⢰⣿⠁⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⡏⠀⠀⠀⠀⢿⣇⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠀⠀⣼⡇⢀⣿⡿⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡟⠀⠀⠀⠀⢀⣾⣿⠂⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⠷⠿⠿⠟⠛⠋⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⠀⠀⠀⠀⠀⣼⣿⠁⠀⠀⠀⠀⠀⣠⣿⠀⠀⠀⠀⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠇⢀⡆⠀⠀⣠⣿⡏⠀⠀⠀⠀⠀⣠⣿⠇⠀⠀⠀⠀⢹⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠿⠶⠛⠻⠿⠟⠛⠋⠀⠀⠀⠀⠀⢀⣿⠏⠀⠀⠀⠀⢀⣾⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⠋⢀⡄⠀⠀⣰⣿⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣧⣤⣾⣷⡤⠾⢟⢁⠀⠀⠀⠀

========================================================================================*/

#ifndef INCLUDE_GUARD_EWENIT
#define INCLUDE_GUARD_EWENIT

#include "core.h"
#include "int_assertions.h"
#include "str_assertions.h"
#include "float_assertions.h"
#include "double_assertions.h"


TestSuite* ts;

#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    #define OPTIONAL_VARIADIC_SUPPORTED         1
#else
    #define OPTIONAL_VARIADIC_SUPPORTED         0
#endif

#define ERROR_CHECK(x)                          if (x < 0) do \
                                                   {printf("ASSERTION UNSUCCESSFUL\n");}\
                                                   while (0)


#define ASSERT_EQUAL_INT(val1, val2)            ERROR_CHECK(assert_equal_int(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_NOT_EQUAL_INT(val1, val2)        ERROR_CHECK(assert_not_equal_int(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_GREATER_THAN_INT(val1, val2)     ERROR_CHECK(assert_greater_than_int(ts, val1, val2, __FILE__, __LINE__))

#define ASSERT_TRUE(val1)                       ERROR_CHECK(assert_true(ts, val1, __FILE__, __LINE__))
#define ASSERT_FALSE(val1)                      ERROR_CHECK(assert_false(ts, val1, __FILE__, __LINE__))

#define ASSERT_EQUAL_STR(val1, val2)            ERROR_CHECK(assert_equal_str(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_NOT_EQUAL_STR(val1, val2)        ERROR_CHECK(assert_not_equal_str(ts, val1, val2, __FILE__, __LINE__))


#define ASSERT_EQUAL_FLOAT(val1, val2)          ERROR_CHECK(assert_equal_float(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_NOT_EQUAL_FLOAT(val1, val2)      ERROR_CHECK(assert_not_equal_float(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_GREATER_THAN_FLOAT(val1, val2)   ERROR_CHECK(assert_greater_than_float(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_ALMOST_EQUAL_FLOAT(val1, val2)   ERROR_CHECK(assert_almost_equal_float(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_NOT_ALMOST_EQUAL_FLOAT(val1, val2) \
                                                ERROR_CHECK(assert_not_almost_equal_float(ts, val1, val2, __FILE__, __LINE__))


#define ASSERT_EQUAL_DOUBLE(val1, val2)         ERROR_CHECK(assert_equal_double(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_NOT_EQUAL_DOUBLE(val1, val2)     ERROR_CHECK(assert_not_equal_double(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_GREATER_THAN_DOUBLE(val1, val2)  ERROR_CHECK(assert_greater_than_double(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_ALMOST_EQUAL_DOUBLE(val1, val2)  ERROR_CHECK(assert_almost_equal_double(ts, val1, val2, __FILE__, __LINE__))
#define ASSERT_NOT_ALMOST_EQUAL_DOUBLE(val1, val2) \
                                                ERROR_CHECK(assert_not_almost_equal_double(ts, val1, val2, __FILE__, __LINE__))



#define EWENIT_START                            ts = TestSuite_init();
#define TEST_START                              EWENIT_START
#define EWENIT_END                              TestSuite_commitCase(ts);\
                                                TestSuite_print(ts);\
                                                TestSuite_deconstruct(ts)
#define TEST_END                                EWENIT_END

// Support a compact version of final error reporting
#define EWENIT_END_COMPACT                      TestSuite_commitCase(ts);\
                                                TestSuite_printCompact(ts);\
                                                TestSuite_deconstruct(ts);
#define TEST_END_COMPACT                        EWENIT_END_COMPACT

#define EWENIT_END_VERBOSE                      TestSuite_commitCase(ts);\
                                                TestSuite_printVerbose(ts);\
                                                TestSuite_deconstruct(ts);
#define TEST_END_VERBOSE                        EWENIT_END_VERBOSE

#define ADD_CASE(func)                          TestSuite_newCase(ts, #func); func()
#define ADD_CASE_CUSTOM(func, name)             TestSuite_newCase(ts, name); func()

// Allow manual PASS/FAIL macros to use formatted arguments if compiler supports it
#if OPTIONAL_VARIADIC_SUPPORTED

    #define TEST_PASS(msg, ...)                 TestSuite_pass(ts, __FILE__, __LINE__, msg, ##__VA_ARGS__)
    #define TEST_FAIL(msg, ...)                 TestSuite_fail(ts, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
    #define TEST_PASS(msg)                      TestSuite_pass(ts, __FILE__, __LINE__, msg)
    #define TEST_FAIL(msg)                      TestSuite_fail(ts, __FILE__, __LINE__, msg)
#endif

// Provide a formatting-supported version of the macro regardless
#define TEST_PASS_FMT(msg, ...)                 TestSuite_pass(ts, __FILE__, __LINE__, msg, __VA_ARGS__)
#define TEST_FAIL_FMT(msg, ...)                 TestSuite_fail(ts, __FILE__, __LINE__, msg, __VA_ARGS__)




#endif
