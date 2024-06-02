
#include <string.h>
#include "core.h"
#include "str_assertions.h"

int assert_equal_str(TestSuite* ts, char* val1, char* val2, char* file_name, long line_num)
{
    int idx;
    int len1 = strlen(val1);
    int len2 = strlen(val2);
    char chr1, chr2;

    if (len1 != len2)
        return TestSuite_fail(
            ts, file_name, line_num,
            "String lengths differ: %d vs %d\n      %s\n      %s",
            len1, len2, val1, val2
        );

    for(idx = 0; idx < len1; idx++)
    {
        chr1 = *(val1 + idx);
        chr2 = *(val2 + idx);
        if(chr1 != chr2)
        {
            char* indicator = (char*)malloc((idx + 1) * sizeof(char));
            for(int i = 0; i < idx; i++)
            {
                *(indicator + i) = '-';
            }
            *(indicator + idx) = '^';

            int return_code = TestSuite_fail(
                ts, file_name, line_num,
                "Strings differ at index %d\n      %s\n      %s\n      %s",
                idx, val1, val2, indicator
            );
            free(indicator);
            return return_code;
        }
    }

    return TestSuite_pass(
        ts, file_name, line_num,
        "Strings equal\n      %s\n      %s", val1, val2
    );
}

int assert_not_equal_str(TestSuite* ts, char* val1, char* val2, char* file_name, long line_num)
{
    if (strcmp(val1, val2) != 0)
        return TestSuite_pass(
            ts, file_name, line_num, "Strings differ\n      %s\n      %s", val1, val2
        );
    else
        return TestSuite_fail(
            ts, file_name, line_num, "Strings equal\n      %s\n      %s", val1, val2
        );
}