#include <string.h>
#include <limits.h>

#include "hiscores.h"
#include "sirtet.h"  // May be removed if used later as a general-purpose tool


int parseInt(char* txt, size_t len, int *out_num) {

    int factor = 1;
    int result = 0;
    if (txt[0] == '-') {
        for (int idx = len - 1; idx > 0; idx--) {
            char c = txt[idx];
            if (c < 48 || c > 57) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "praseInt encountered an unknown digit %c\n", c
                );
                Sirtet_setError(buff);
                return -1;
            }

            int to_sub = factor * (c - 48);
            if (result < INT_MIN + to_sub) {

                char buff[128];
                snprintf(
                    buff, 128,
                    "parseInt cannot parse a number smaller than %d. (got %s)",
                    INT_MAX, txt
                );
                Sirtet_setError(buff);
                return -1;
            }

            result -= to_sub;
            factor *= 10;
        }
    }
    else {
        for (int idx = len - 1; idx >= 0; idx--) {
            char c = txt[idx];
            if (c < 48 || c > 57) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "praseInt encountered an unknown digit %c\n", c
                );
                Sirtet_setError(buff);
                return -1;
            }

            int to_add = factor * (c - 48);
            if (to_add > INT_MAX - result) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "parseInt cannot parse a number larger than %d. (got %s)",
                    INT_MAX, txt
                );
                Sirtet_setError(buff);
                return -1;
            }

            result += to_add;
            factor *= 10;
        }
    }


    *out_num = result;
    return 0;
}
