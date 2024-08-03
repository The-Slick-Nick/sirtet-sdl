#include <SDL2/SDL_render.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "hiscores.h"
#include "sirtet.h"  // May be removed if used later as a general-purpose tool

// TODO: Put these from global utilities header & include from there
#ifndef MAX2
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MAX3
#define MAX3(a, b, c) (MAX2(MAX2(a, b), c))
#endif


/******************************************************************************
 * General Utilities
******************************************************************************/

// TODO: Send these two to a utilities.h/c file

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

/******************************************************************************
 * Scorelist Methods
******************************************************************************/


ScoreList* ScoreList_init(size_t size, size_t namelen) {

    ScoreList *retval = (ScoreList*)calloc(1, sizeof(ScoreList));
    if (retval == NULL) {
        Sirtet_setError("Error allocating memory for ScoreList\n");
        return NULL;
    }

    retval->len = 0;
    retval->namelen = namelen;
    retval->size = size;

    retval->names = (char*)calloc((namelen + 1) * size, sizeof(char));
    retval->scores = (int*)calloc(size, sizeof(int));

    size_t bufflen = (namelen + 1) > 32 ? (namelen + 1) : 32;
    retval->strbuff = calloc(bufflen, sizeof(char));
    retval->intbuff = calloc(size, sizeof(int));

    if (
        retval->names == NULL ||
        retval->scores == NULL ||
        retval->strbuff == NULL ||
        retval->intbuff == NULL
    ) {
        Sirtet_setError("Error allocating memory for ScoreList\n");
        return NULL;
    }

    return retval;
}


void ScoreList_deconstruct(ScoreList *self) {
    free(self->names);
    free(self->scores);
    free(self->strbuff);
    free(self->intbuff);
}


// Add an entry to score list, returning a status code
int ScoreList_add(ScoreList *self, char *name, int score) {

    if (self->len == self->size) {
        return -1;
    }

    size_t idx = self->len;
    size_t namelen = self->namelen;
    size_t inputlen = strlen(name);


    if (inputlen > namelen) {
         return -1;
        Sirtet_setError("Name too long to add to ScoreList\n");
    }
    self->scores[idx] = score;

    char *nameptr = (char*)(self->names + idx * (namelen + 1));
    memset(nameptr, 0, sizeof(char) * (namelen + 1));
    memcpy(nameptr, name, sizeof(char) * inputlen);

    self->len++;
    return 0;
}

// Remove the final element of the scorelist, returning a status code
int ScoreList_pop(ScoreList *self, char *out_name, int *out_score) {

    if (self->len == 0) {
        return -1;
    }

    // we store return val, then decrement
    const int retval = ScoreList_get(self, self->len - 1, out_name, out_score);
    if (retval != 0) {
        return retval;
    }
    self->len--;
    return retval;

}

// Retrieve the entry at the given index
int ScoreList_get(ScoreList *self, size_t idx, char *out_name, int *out_score) {

    if (idx >= self->len) {
        return -1;
    }

    if (out_name != NULL) {
        char *strptr = (char*)(self->names + (idx * (self->namelen + 1)));
        strcpy(out_name, strptr);
    }

    if (out_score != NULL) {
        *out_score = self->scores[idx];
    }

    return 0;
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



void sortByOrder(void *tosort, const int *order, size_t elem_sz, size_t elem_n) {

    int st_curorder[128];
    int *curorder = (elem_n > 128 ? (int*)malloc(elem_n * sizeof(int)) : st_curorder);

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

// Sort scoreList in descending order by score
int ScoreList_sort(ScoreList *self) {

    int *scores = self->scores;
    int *indices = self->intbuff;

    sortByBasisDesc(self->len, indices, self->scores);

    sortByOrder(self->scores, indices, sizeof(int), self->len);
    sortByOrder(self->names, indices, (1 + self->namelen) * sizeof(char), self->len);

    return 0;
}



enum HsParseState {
    AwaitingName,
    ParsingName,
    AwaitingNum,
    ParsingNum
};

// Read rows from a highscores file into provided ScoreList
int ScoreList_readFile(ScoreList *self, FILE *f) {

    if (self == NULL) {
        Sirtet_setError("ScoreList_readFile passed a NULL ScoreList\n");
        return -1;
    }

    if (f == NULL) {
        Sirtet_setError("ScoreList_readFile passed a NULL file pointer\n");
        return -1;
    }

    char *newname = self->strbuff;
    int score;
    bool neg;
    int nameidx = 0;
    char chr;
    enum HsParseState curstate = AwaitingName;

    do {
        chr = fgetc(f);

        switch (curstate) {
            case AwaitingName:
                if (chr != ' ' && chr != '\0' && chr != '\n' && chr != EOF) {
                    nameidx = 0;
                    newname[nameidx++] = chr;
                    curstate = ParsingName;
                }
                break;

            case ParsingName:
                if (chr == ' ' || chr == '\0' || chr == '\n' || chr == EOF) {
                    newname[nameidx] = '\0';
                    curstate = AwaitingNum;
                }
                else {
                    if (nameidx >= self->namelen) {
                        Sirtet_setError(
                            "Error parsing score file: Name too long\n"
                        );
                        return -1;
                    }
                    newname[nameidx++] = chr;
                }
                break;

            case AwaitingNum:
                if (chr == '-') {
                    neg = true;
                    score = 0;
                    curstate = ParsingNum;
                }
                else if (chr >= '0' && chr <= '9') {
                    neg = false;
                    score = (chr - '0');
                    curstate = ParsingNum;
                }
                break;

            case ParsingNum:
                if (chr >= '0' && chr <= '9') {
                    int digit = chr - '0';
                    if (neg) {
                        if (score < ((INT_MIN + digit) / 10)) {
                            Sirtet_setError(
                                "Error parsing score file: "
                                "Parsed score below MIN_INT value\n"
                            );
                            return -1;
                        }
                        score = (score * 10) - (chr - '0');
                    }
                    else {
                        if (score > ((INT_MAX - digit) / 10)) {
                            Sirtet_setError(
                                "Error parsing score file: "
                                "Parsed score exceeds INT_MAX\n"
                            );
                            return -1;
                        }
                        score = (score * 10) + (chr - '0');
                    }
                }
                else {
                    // end
                    if (ScoreList_add(self, newname, score) != 0) {
                        printf(":(");
                        return -1;
                    }
                    curstate = AwaitingName;
                }
                break;
        }
    } while (chr != EOF);
    return 0;
}


int ScoreList_toFile(ScoreList *self, FILE *f) {


    if (self == NULL) {
        Sirtet_setError("ScoreList_toFile passed a NULL ScoreList\n");
        return -1;
    }

    if (f == NULL) {
        Sirtet_setError("ScoreList_toFile passed a NULL file pointer\n");
        return -1;
    }

    int *scores = self->scores;
    char *names = self->names;

    size_t name_sz = ((sizeof(char) * (self->namelen + 1)));

    for (int i = 0; i < self->len; i++) {
        fprintf(f, "%s %d\n", names + (name_sz * i), scores[i]);
    }

    return 0;
}



/******************************************************************************
 * ScoreDisplay methods
******************************************************************************/


ScoreDisplay *ScoreDisplay_init(
    ScoreList *sl, 
    int first, int last, int rank_start,
    SDL_Renderer *rend,
    TTF_Font *lbl_font, const SDL_Color *lbl_col
) {

    ScoreDisplay *retval = malloc(sizeof(ScoreDisplay));

    if (
        first >= last ||
        first < 0 ||
        last >= sl->len ) {

        Sirtet_setError(
            "Invalid ScoreList indices passed in ScoreDisplay_init\n");
        return NULL;
    }
    retval->n_lbls = (last - first + 1);


    size_t arrsz = retval->n_lbls * sizeof(SDL_Texture*);
    retval->rank_lbls = malloc(arrsz);
    retval->name_lbls = malloc(arrsz);
    retval->score_lbls = malloc(arrsz);

    char rankbuff[12];
    char scorebuff[12];
    char *namebuff = calloc(sl->namelen + 1, sizeof(char));
    if (namebuff == NULL) {
        Sirtet_setError("Error allocating memory in ScoreDisplay_init\n");
        return NULL;
    }

    for (
        int lbl_i = 0, rank = rank_start, sl_i = first;
        lbl_i < retval->n_lbls && sl_i <= last;
        lbl_i++, rank++, sl_i++
    ) {

        int score;
        if (ScoreList_get(sl, sl_i, namebuff, &score) != 0) {
            free(retval);
            return NULL;
        }

        snprintf(scorebuff, 12, "%d", score);
        snprintf(rankbuff, 12, "%d", rank);

        SDL_Surface *namesurf = TTF_RenderText_Solid(
            lbl_font, namebuff, *lbl_col);

        SDL_Surface *scoresurf = TTF_RenderText_Solid(
            lbl_font, scorebuff, *lbl_col);

        SDL_Surface *ranksurf = TTF_RenderText_Solid(
            lbl_font, rankbuff, *lbl_col);

        if (
            namesurf == NULL ||
            scoresurf == NULL ||
            ranksurf == NULL
        ) {
            char errbuff[128];
            snprintf(
                errbuff, 128, 
                "Error creating labels for ScoreDisplay:\n%s\n",
                TTF_GetError()
            );
            return NULL;
        }

        retval->name_lbls[lbl_i] = SDL_CreateTextureFromSurface(
            rend, namesurf);
        retval->score_lbls[lbl_i] = SDL_CreateTextureFromSurface(
            rend, scoresurf);
        retval->rank_lbls[lbl_i] = SDL_CreateTextureFromSurface(
            rend, ranksurf);

        SDL_FreeSurface(namesurf);
        SDL_FreeSurface(scoresurf);
        SDL_FreeSurface(ranksurf);

        if (
            retval->name_lbls[lbl_i] == NULL ||
            retval->score_lbls[lbl_i] == NULL ||
            retval->rank_lbls[lbl_i] == NULL
        ) {
            char errbuff[128];
            snprintf(
                errbuff, 128, 
                "Error creating labels for ScoreDisplay:\n%s\n",
                SDL_GetError()
            );
            return NULL;
        }
    }
    free(namebuff);
    return retval;
}


int ScoreDisplay_deconstruct(ScoreDisplay *self) {

    for (int i = 0; i < self->n_lbls; i++) {
        SDL_DestroyTexture(self->name_lbls[i]);
        SDL_DestroyTexture(self->score_lbls[i]);
        SDL_DestroyTexture(self->rank_lbls[i]);
    }

    free(self->name_lbls);
    free(self->score_lbls);
    free(self->rank_lbls);

    free(self);

    return 0;
}


int ScoreDisplay_draw(
    ScoreDisplay *self,
    SDL_Renderer *rend,
    const SDL_Rect *draw_window,
    SDL_Rect *out_dim
) {

    int yoffset = 0;
    for (int lbl_i = 0; lbl_i < self->n_lbls; lbl_i++) {

        SDL_Rect rankdst = {
            .x=draw_window->x,
            .y=yoffset
        };
        SDL_QueryTexture(
            self->rank_lbls[lbl_i], NULL, NULL, &rankdst.w, &rankdst.h);


        SDL_Rect namedst = {
            .x=draw_window->x + rankdst.w,
            .y=yoffset
        };
        SDL_QueryTexture(
            self->name_lbls[lbl_i], NULL, NULL, &namedst.w, &namedst.h);


        SDL_Rect scoredst;
        SDL_QueryTexture(
            self->score_lbls[lbl_i], NULL, NULL, &scoredst.w, &scoredst.h);
        scoredst.x = (draw_window->x + draw_window->w - scoredst.w);
        scoredst.y = yoffset;

        SDL_RenderCopy(rend, self->rank_lbls[lbl_i], NULL, &rankdst);
        SDL_RenderCopy(rend, self->name_lbls[lbl_i], NULL, &namedst);
        SDL_RenderCopy(rend, self->score_lbls[lbl_i], NULL, &scoredst);

        yoffset += MAX3(rankdst.h, namedst.h, scoredst.h);
    }

    if (out_dim != NULL) {
        *out_dim = (SDL_Rect) {
            .x=draw_window->x,
            .y=draw_window->y,
            .w=draw_window->h,
            .h=yoffset
        };
    }

    return 0;
}


