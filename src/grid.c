#include <stdbool.h>
#include "grid.h"
#include "block.h"


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
bool GameGrid_canBlockExist(GameGrid *self, Block *block) {

    return GameGrid_canBlockInfoExist(
        self, block->size, block->contents, block->position
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
        int gridarr_idx = grid_coords.x + (self->width * grid_coords.y); // 2d access
        if (*(self->contents + gridarr_idx) >= 0) {
            return false;
        }

    }
    return true;
}

// add a block's cells to the grid. ModifyModifies  provided grid and block in place
int GameGrid_commitBlock(GameGrid* self, Block* block) {
    return -1;
}

// Reset all of a grid's contents
void GameGrid_clear(GameGrid* grid) {
    for (int idx = 0; idx < grid->width * grid->height; idx++) {
         grid->contents[idx] = -1;
    };
}  

// Reset a grid's contents, clearing encountered blocks
void GameGrid_reset(GameGrid* grid, BlockIds* ids) {
}  

// clears full rows of committed blocks
int GameGrid_resolveRows(GameGrid* grid, BlockIds* ids) {
    return 0;
}
