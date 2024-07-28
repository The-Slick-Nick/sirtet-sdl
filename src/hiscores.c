#include <string.h>
#include <limits.h>

#include "hiscores.h"
#include "sirtet.h"  // May be removed if used later as a general-purpose tool



    // TODO: Rework this to END on non-digit char, returning
    // length of parsed number (works better for my intended use)
int parseInt(char* txt, int *out_num) {

    char *cur = txt;

    bool neg = (*cur == '-');
    bool hasdig = false;

    if (*cur == '-' || *cur == '+') {
        cur++;
    }

    int result = 0;
    while (*cur >= '0' && *cur <= '9') {
        hasdig = true;

        int digit = *cur - '0';

        if (neg) {
            if (result < ((INT_MIN + digit) / 10) ) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "Cannot parse integer smaller than %d\n",
                    INT_MIN
                );
                Sirtet_setError(buff);
                return -1;
            }
            result = result * 10 - digit;
        }
        else {
            if (result > ((INT_MAX - digit) / 10) ) {
                char buff[128];
                snprintf(
                    buff, 128,
                    "Cannot parse integer larger than %d\n",
                    INT_MAX
                );
                return -1;
            }
            result = result * 10 + digit;
        }

        cur++;
    }

    if (!hasdig) {
        Sirtet_setError("No digits enounctered parsing text\n");
        return -1;
    }

    *out_num = result;
    return (int)(cur - txt);
}
