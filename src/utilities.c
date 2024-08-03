/******************************************************************************
* utilities.c
*
* Implmentation of utility functions
******************************************************************************/
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include "sirtet.h"


int parseInt(char* txt, int *out_num) {

    char *cur = txt;

    const bool neg = (*cur == '-');
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


int parseName(const char* txt, char *out_str, size_t maxlen) {

    size_t idx = 0;
    char cur;
    while (idx <= maxlen) {

        cur = txt[idx];

        if (cur == EOF) {
            return -1;
        }

        if (cur == ' ') {
            out_str[idx] = '\0';
            return idx;
        }


        out_str[idx++] = cur;
    }

    // exceeded max len
    return -1;
}



// Populate sort order of `basis` into `indices`
void sortByBasisDesc(size_t len, int *indices, const int *basis) {

    int st_supp[128];
    int *supp = (len > 128 ? (int*)malloc(len * sizeof(int)) : st_supp);

    for (int i = 0; i < len; i++) {
        indices[i] = i;
        supp[i] = i;
    }

    int *hold;
    int *tosort = supp; // better naming
    int *result = indices;

    size_t bucket_sz = 1;
    while (bucket_sz < len) {

        hold = tosort;
        tosort = result;
        result = hold;

        size_t placeidx = 0;

        for (size_t leftstart = 0; leftstart < len; leftstart += 2 * bucket_sz) {
            size_t leftend = leftstart + (bucket_sz - 1);
            leftend = (leftend >= len ? len - 1 : leftend);

            size_t rightstart = leftend + 1; 
            size_t rightend = rightstart + (bucket_sz - 1);
            rightend = (rightend >= len ? len - 1 : rightend);

            size_t ileft = leftstart;
            size_t iright = rightstart;

            while (ileft <= leftend || iright <= rightend) {

                if (iright > rightend) {
                    size_t n = leftend - ileft + 1;
                    memcpy(result + placeidx, tosort + ileft, sizeof(int) * n);
                    placeidx += n;
                    ileft += n;
                    continue;
                }

                if (ileft > leftend) {
                    size_t n = rightend - iright + 1;
                    memcpy(result + placeidx, tosort + iright, sizeof(int) * n);
                    placeidx += n;
                    iright += n;
                    continue;
                }

                if (basis[tosort[ileft]] > basis[tosort[iright]]) {
                    result[placeidx++] = tosort[ileft++];
                }
                else {
                    result[placeidx++] = tosort[iright++];
                }
            }
        }
        bucket_sz *= 2;
    }

    // in case we've ended up unswapped
    memcpy(indices, result, sizeof(int) * len);

    if (len > 128) {
        free(supp);
    }
}


void sortByOrder(
    void *tosort, const int *order, size_t elem_sz, size_t elem_n) {

    int st_curorder[128];
    int *curorder = (
        elem_n > 128 ?
        (int*)malloc(elem_n * sizeof(int)) :
        st_curorder);

    char st_hold[128];
    void *hold = (elem_sz > 128 ? malloc(elem_sz) : (void*)st_hold);


    for (int i = 0; i < elem_n; i++) {
        curorder[i] = i;
    }


    for (int i = 0; i < elem_n; i++) {

        // already correct
        if (curorder[i] == order[i]) {
            continue;
        }

        int searchi = order[i];
        while (curorder[searchi] != order[i]) {
            searchi = curorder[searchi];
        }

        // swap time baby
        size_t scale = (elem_sz / sizeof(char));
        char *swap1 = (char*)tosort + (scale * i);
        char *swap2 = (char*)tosort + (scale * searchi);

        memcpy(hold, swap1, elem_sz);
        memcpy(swap1, swap2, elem_sz);
        memcpy(swap2, hold, elem_sz);

        int idxhold = curorder[i];
        curorder[i] = curorder[searchi];
        curorder[searchi] = idxhold;
    }

    if (curorder != st_curorder) {
        free(curorder);
    }

    if (hold != st_hold) {
        free(hold);
    }

}
