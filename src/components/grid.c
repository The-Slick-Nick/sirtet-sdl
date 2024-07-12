#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "grid.h"
#include "block.h"

/*************************************************************************************************
 * Initialization and deconstruction
*************************************************************************************************/


GameGrid *GameGrid_init(int width, int height) {

    GameGrid *retval = (GameGrid*)malloc(sizeof(GameGrid));
    if (retval == NULL) {
        return NULL;
    }
    retval->width = width;
    retval->height = height;

    retval->framerate = DEFAULT_GRID_FRAMERATE;
    retval->cooldown = 0;   
    retval->is_animating = false;


    retval->contents = (int*)malloc(width * height * sizeof(int));
    retval->to_remove = (int*)calloc(height, sizeof(int));
    retval->removed = (int*)calloc(height, sizeof(int));

    if (
        retval->contents == NULL
        || retval->to_remove == NULL
        || retval->removed == NULL
    ) {
        GameGrid_deconstruct(retval);
        return NULL;
    }

    GameGrid_clear(retval);
    return retval;
} 

int GameGrid_deconstruct(GameGrid *self) {

    free(self->contents);
    free(self->to_remove);
    free(self->removed);
    free(self);
    return 0;
}

/******************************************************************************
 * Content management
******************************************************************************/

// Convert a block's content bit to grid coordaintes
Point blockContentBitToGridCoords(
    int content_bit, int block_size, Point block_position) {


    // A self reminder:
    // flip sign of all y coordinates
    // if (block_size is even)
    //     correct positive x's down by 1
    //     correct positive y's down by 1
    // add origin point
    Point content_coords = contentBitToPoint(content_bit, block_size);

    content_coords = (Point){.x=content_coords.x, .y=-1 * content_coords.y};
    if ((block_size & 1) == 0) {
        content_coords = (Point){
            .x=content_coords.x > 0 ? content_coords.x - 1 : content_coords.x,
            .y=content_coords.y > 0 ? content_coords.y - 1 : content_coords.y
        };
    }
    return Point_translate(content_coords, block_position);
}

// Identify if the given block is compatible with the current grid
bool GameGrid_canBlockExist(GameGrid *self, BlockDb *db, int block_id) {

    return GameGrid_canBlockInfoExist(
        self,
        BlockDb_getBlockSize(db, block_id),
        BlockDb_getBlockContents(db, block_id),
        BlockDb_getBlockPosition(db, block_id)
    );
}

// Identify if the provided block data is compatible with current grid
bool GameGrid_canBlockInfoExist(
    GameGrid *self, int block_size, long block_contents, Point block_position
) {

    for (int bit_num = 0; bit_num < block_size * block_size; bit_num++) {

        if ((block_contents & (1L << bit_num)) == 0) {
            continue;
        }
        Point grid_coords = blockContentBitToGridCoords(
            bit_num, block_size, block_position);

        if (grid_coords.x < 0 || grid_coords.x >= self->width) {
            return false;
        }

        if (grid_coords.y < 0 || grid_coords.y >= self->height) {
            return false;
        }

        // occupied
        // 2d access
        int gridarr_idx = grid_coords.x + (self->width * grid_coords.y); 
        if (*(self->contents + gridarr_idx) >= 0) {
            return false;
        }

    }
    return true;
}

/**
 * @brief Add a block's cells to the grid. Modifies provided grid and block in place.
 * @param self  Pointer to the GameGrid struct in question
 * @param block Pointer to the Block struct to commit.
*/
int GameGrid_commitBlock(GameGrid *self, BlockDb *db, int block_id) {
    
    if ( !GameGrid_canBlockExist(self, db, block_id) ) {
        return -1;
    }

    int block_size = BlockDb_getBlockSize(db, block_id);
    long block_contents = BlockDb_getBlockContents(db, block_id);
    Point block_pos = BlockDb_getBlockPosition(db, block_id);

    for (int bit_num = 0; bit_num < block_size * block_size; bit_num++) {
        if ( 0 == (block_contents & (1L << bit_num)) ) {
            continue;
        }

        Point grid_coords = blockContentBitToGridCoords(bit_num, block_size, block_pos);

        // 2d access
        int grid_idx = grid_coords.x + (self->width * grid_coords.y);
        self->contents[grid_idx] = block_id;
    }

    BlockDb_setBlockContents(db, block_id, 0L);
    return 0;
}

// Reset all of a grid's contents to baseline.
int GameGrid_clear(GameGrid* grid) {
    for (int idx = 0; idx < grid->width * grid->height; idx++) {
         grid->contents[idx] = INVALID_BLOCK_ID;
    };

    return 0;
}  

// Reset a grid's contents, clearing encountered blocks within BlockDb
int GameGrid_reset(GameGrid* grid, BlockDb *db) {

    for (int grid_idx = 0; grid_idx < grid->width * grid->height; grid_idx++) {
        if (grid->contents[grid_idx] == INVALID_BLOCK_ID) {
            continue;
        }

        if (BlockDb_decrementCellCount(db, grid->contents[grid_idx], 1) == -1) {
            return -1;
        }
        grid->contents[grid_idx] = INVALID_BLOCK_ID;
    }

    return 0;
}  

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid *self, BlockDb *db) {

    int read_ptr = self->height - 1;
    int num_full_rows = 0;
    for (int write_ptr = self->height - 1; write_ptr >= 0; write_ptr--) {

        bool row_full = true;
        while (read_ptr >= 0 && row_full) {

            row_full = true;
            for (int x = 0; x < self->width; x++) {

                int grid_idx = x + (self->width * read_ptr);
                if (self->contents[grid_idx] < 0) {
                    row_full = false;
                    break;
                }
            }

            if (row_full) {

                // must adjust BlockIds now, but only now since
                // we did not previously know this row was full
                for (int x = 0; x < self->width; x++) {
                    int grid_idx = x + (self->width * read_ptr);
                    int grid_cell_val = self->contents[grid_idx];
                    BlockDb_decrementCellCount(db, grid_cell_val, 1);
                }
                read_ptr--;
                num_full_rows++;
            }
        }

        for (int x = 0; x < self->width; x++) {
            // row pointers to actual indices
            int write_idx = x + (self->width * write_ptr);
            int read_idx;

            if (read_ptr < 0) {
                self->contents[write_idx] = -1;
            }
            else {
                read_idx = x + (self->width * read_ptr);
                self->contents[write_idx] = self->contents[read_idx];
            }
        }

        // both read and writes move
        if (read_ptr >= 0) {
            read_ptr--;
        }
    }
    return num_full_rows;
}

// Calculate how many points to award based on the current grid state
int GameGrid_assessScore(GameGrid *self, int level) {
    /**************************************************************************
     * The scoring system here will attempt to use the scoring system
     * from the original Tetris for NES
     *
     * https://tetris.wiki/Scoring
     *
     * Points are awarded based on the number of lines cleared at a time, with
     * a multiplier applied based on the current game level.
     *
     * score = (line_score) * (level + 1)
     * game level.
     *
     * Number of Lines      Score
     * ---------------------------
     *  1                     40
     *  2                    100
     *  3                    300
     *  4                   1200
     *
     * Also note that points will also be awarded when blocks are
     * "soft dropped" (sped up), though that will not be tabulated here.
     *
    **************************************************************************/

    assert(level >= 0);

    int num_rows = 0;
    int idx;
    bool is_full_row;
    for (int y = 0; y < self->height; y++) {
        is_full_row = true;
        for (int x = 0; x < self->width; x++) {
            idx = x + y * self->width;

            if (self->contents[idx] == INVALID_BLOCK_ID) {
                is_full_row = false;
                break;
            }
        }
        if (is_full_row) {
            num_rows++;
        }
    }

    return (level + 1) * (
        num_rows == 0 ? 0 :
        num_rows == 1 ? 40 :
        num_rows == 2 ? 100 :
        num_rows == 3 ? 300 :
        num_rows == 4 ? 1200 : -1
    );
}


/******************************************************************************
 * Display/animation management
******************************************************************************/


int GameGrid_prepareAnimation(GameGrid *self, int framerate) {

    // must clear existing animation before attempting a new one
    if (self->is_animating) {
        return -1;
    }

    for (int x = 0; x < self->height; x++) {

        self->to_remove[x] = 0;
        self->removed[x] = 0;

        bool row_full = true;
        for (int y = 0; y < self->width; y++) {
            if (self->contents[y + (self->width * x)] == INVALID_BLOCK_ID) {
                row_full = false;
                break;
            }
        }

        if (row_full) {
            self->to_remove[x] = self->width;
            self->is_animating = true;
        }
    }

    if (self->is_animating) {
        self->framerate = framerate;
        self->cooldown += framerate;
    }

    return 0;
}

int GameGrid_prepareAnimationAllRows(GameGrid *self, int framerate) {
    if (self->is_animating) {
        return -1;
    }

    for (int i = 0; i < self->height; i++) {
        self->to_remove[i] = self->width;
        self->is_animating = true;
    }

    self->framerate = framerate;
    self->cooldown = framerate;

    return 0;
}

// Run one frame of the current grid animation
int GameGrid_runAnimationFrame(GameGrid *self) {


    if (!self->is_animating) {
        return -1;
    }
    assert(self->cooldown > 0);

    if (--self->cooldown > 0) {
        return 0;
    }

    bool animation_complete = true;
    for (int x = 0; x < self->height; x++) {

        if (self->removed[x] == self->to_remove[x]) {
            self->removed[x] = 0;
            self->to_remove[x] = 0;
            continue;
        }

        animation_complete = false;
        self->removed[x]++;
    }

    if (animation_complete) {
        self->is_animating = false;
    }
    else {
        self->cooldown += self->framerate;
    }

    return 0;
}

