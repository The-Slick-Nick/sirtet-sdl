#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <stdio.h>

#include "sirtet.h"
#include "backgrounds.h"

PanningBg *PanningBg_init(
    float xvel, float yvel,
    SDL_Texture *topleft, SDL_Texture *topright,
    SDL_Texture *bottomleft, SDL_Texture *bottomright
) {


    PanningBg *retval = calloc(1, sizeof(PanningBg));


    retval->xvel = xvel;
    retval->yvel = yvel;

    retval->xpos = 0;
    retval->ypos = 0;

    return retval;
}



int PanningBg_deconstruct(PanningBg *self) {
    free(self);
    return 0;
}

int PanningBg_move(PanningBg *self, SDL_Rect *containing_window) {


    float left_extrm = (float)containing_window->x;
    float right_extrm = left_extrm + (float)containing_window->w;

    float top_extrm = (float)containing_window->y;
    float bottom_extrm = top_extrm + (float)containing_window->h;

    self->xpos += self->xvel;
    self->ypos += self->yvel;

    int loopn = 0;

    SDL_Texture *swaptx;

    if (self->xpos > right_extrm) {

        while (self->xpos > right_extrm && loopn++ < 1000000) {
            self->xpos -= (right_extrm - left_extrm);
        }

        swaptx = self->topleft;
        self->topleft = self->topright;
        self->topright = swaptx;

        swaptx = self->bottomleft;
        self->bottomleft = self->bottomright;
        self->bottomright = swaptx;
    }

    if (self->xpos < left_extrm) {
        while (self->xpos < left_extrm && loopn++ < 1000000) {
            self->xpos += (right_extrm - left_extrm);
        }

        swaptx = self->topleft;
        self->topleft = self->topright;
        self->topright = swaptx;

        swaptx = self->bottomleft;
        self->bottomleft = self->bottomright;
        self->bottomright = swaptx;

    }

    if (self->ypos > bottom_extrm) {
        while (self->ypos > bottom_extrm && loopn++ < 1000000) {
            self->ypos -= (bottom_extrm - top_extrm);
        }

        swaptx = self->topleft;
        self->topleft = self->bottomleft;
        self->bottomleft = swaptx;


        swaptx = self->topright;
        self->topright = self->bottomright;
        self->bottomright = swaptx;
    }

    if (self->ypos < top_extrm) {
        while (self->ypos < top_extrm && loopn++ < 1000000) {
            self->ypos += (bottom_extrm - top_extrm);
        }

        swaptx = self->topleft;
        self->topleft = self->bottomleft;
        self->bottomleft = swaptx;


        swaptx = self->topright;
        self->topright = self->bottomright;
        self->bottomright = swaptx;
    }


    assert(loopn < 1000000);


    return 0;
}

int PanningBg_draw(
    PanningBg *self, SDL_Renderer *rend, SDL_Rect *containing_window) {

    float left_extrm = (float)containing_window->x;
    float right_extrm = left_extrm + (float)containing_window->w;

    float top_extrm = (float)containing_window->y;
    float bottom_extrm = top_extrm + (float)containing_window->h;

    if (
        self->xpos < left_extrm ||
        self->xpos > right_extrm ||
        self->ypos < top_extrm || 
        self->ypos > bottom_extrm ) {

        char errbuff[STATIC_ARRMAX];
        snprintf(
            errbuff, STATIC_ARRMAX,
            "Error drawing PanningBg: (%f, %f) out of bounds",
            self->xpos, self->ypos
        );
        Sirtet_setError(errbuff);
        return -1;
    }


    int orig_x = (int)self->xpos;
    int orig_y = (int)self->ypos;

    if (DEBUG_ENABLED) {

        SDL_Rect dbg_rect = {.x=orig_x - 2, .y=orig_y - 2, .w = 4, .h = 4};
        // SDL_RenderDrawRect(SDL_Renderer *renderer, const SDL_Rect *rect)
        // SDL_SetRenderDrawColor(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
        SDL_RenderDrawRect(rend, &dbg_rect);
    }



    int w, h;

    SDL_QueryTexture(self->topleft, NULL, NULL, &w, &h);
    SDL_Rect tl_dst = {.x=orig_x - w, .y=orig_y - h, .w = w, .h = h};

    SDL_QueryTexture(self->topright, NULL, NULL, &w, &h);
    SDL_Rect tr_dst = {.x=orig_x, .y=orig_y - h, .w = w, .h = h};

    SDL_QueryTexture(self->bottomleft, NULL, NULL, &w, &h);
    SDL_Rect bl_dst = {.x=orig_x - w, .y=orig_y, .w = w, .h = h};

    SDL_QueryTexture(self->bottomright, NULL, NULL, &w, &h);
    SDL_Rect br_dst = {.x=orig_x, .y=orig_y, .w = w, .h = h};

    SDL_RenderCopy(rend, self->topleft, NULL, &tl_dst);
    SDL_RenderCopy(rend, self->topright, NULL, &tr_dst);
    SDL_RenderCopy(rend, self->bottomleft, NULL, &bl_dst);
    SDL_RenderCopy(rend, self->bottomright, NULL, &br_dst);

    return 0;
}
