#include <assert.h>
#include <int_assertions.h>
#include <string.h>
#include "block.h"
#include "grid.h"
#include "EWENIT.h"


void testGameGridClear() {
    // GameGrid_clear

    GameGrid *grid = GameGrid_init(2, 1);
    
    // IDs stored should be made invalid
    
    ASSERT_GREATER_THAN_INT(0, grid->contents[0]);
    ASSERT_GREATER_THAN_INT(0, grid->contents[1]);

    GameGrid_deconstruct(grid);
}

void testGameGridReset() {

    BlockDb *db = BlockDb_init(8);

    // BlockDb_createBlock(BlockDb *self, int size, long contents, Point position)
    int id1 = BlockDb_createBlock(db, 1, 0b1, (Point){0, 0}, (SDL_Color){});
    int id2 = BlockDb_createBlock(db, 1, 0b1, (Point){0, 0}, (SDL_Color){});

    const int width = 2;
    const int height = 4;
    GameGrid *grid = GameGrid_init(width, height);

    grid->contents[0] = id1;
    grid->contents[1] = id2;

    GameGrid_reset(grid, db);


    for (int i = 0; i < 8; i++) {
        INFO_FMT("Index %d", i);
        ASSERT_EQUAL_INT((grid->contents)[i], -1);
    }

    ASSERT_EQUAL_INT(BlockDb_getCellCount(db, id1), 0);
    ASSERT_EQUAL_INT(BlockDb_getCellCount(db, id2), 0);

    ASSERT_FALSE(BlockDb_doesBlockExist(db, id1));
    ASSERT_FALSE(BlockDb_doesBlockExist(db, id2));

    GameGrid_deconstruct(grid);
    BlockDb_deconstruct(db);
}


void testGameGridCanBlockInfoExist() {

    GameGrid *grid = GameGrid_init(4, 4);

    int block_size = 4;
    // center of 4 = 5, 6, 9, 10
    long block_contents = ( 1L << 5 | 1L << 6 | 1L << 9 | 1L << 10 );
    bool result;

    // even sized grid

    // nw corner
    INFO("NW");
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=0, .y=0});
    ASSERT_FALSE(result);

    // ne corner
    INFO("NE");
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=4, .y=0});
    ASSERT_FALSE(result);

    // sw corner
    INFO("SW");
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=0, .y=4});
    ASSERT_FALSE(result);

    // s3 corner
    INFO("SE");
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=4, .y=4});
    ASSERT_FALSE(result);

    // center
    INFO("Center");
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=2, .y=2});
    ASSERT_TRUE(result);

    // nw corner, overall block scope can overlap but actualized content doesn't
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=1, .y=1});
    ASSERT_TRUE(result);

    // overlap
    grid->contents[5] = 1;
    result = GameGrid_canBlockInfoExist( grid, block_size, block_contents, (Point){.x=2, .y=2});
    ASSERT_FALSE(result);

    GameGrid_deconstruct(grid);
}

void testGameGridCanBlockExist() {

    GameGrid *grid = GameGrid_init(4, 4);
    BlockDb *db = BlockDb_init(8);

    long content_mask = ( 1L << 5 | 1L << 6 | 1L << 9 | 1L << 10 );

    int block_id = BlockDb_createBlock(
        db, 4, content_mask, (Point){0, 0}, (SDL_Color){0, 0, 0, 0}
    );

    ASSERT_GREATER_THAN_INT(block_id, INVALID_BLOCK_ID);

    bool result;
    // even sized grid
    // nw corner
    INFO("NW");
    BlockDb_setBlockPosition(db, block_id, (Point){0, 0});
    result = GameGrid_canBlockExist(grid, db, block_id);
    ASSERT_FALSE(result);

    // ne corner
    INFO("NE");
    BlockDb_setBlockPosition(db, block_id, (Point){4, 0});
    result = GameGrid_canBlockExist(grid, db, block_id);
    ASSERT_FALSE(result);


    // sw corner
    INFO("SW");
    BlockDb_setBlockPosition(db, block_id, (Point){0, 4});
    result = GameGrid_canBlockExist(grid, db, block_id);
    ASSERT_FALSE(result);

    // s3 corner
    INFO("SE");
    ASSERT_TRUE(BlockDb_doesBlockExist(db, block_id));
    BlockDb_setBlockPosition(db, block_id, (Point){4, 4});
    result = GameGrid_canBlockExist(grid, db, block_id);
    ASSERT_FALSE(result);

    // center
    INFO("Center");
    ASSERT_TRUE(BlockDb_doesBlockExist(db, block_id));
    BlockDb_setBlockPosition(db, block_id, (Point){2, 2});
    result = GameGrid_canBlockExist(grid, db, block_id);
    ASSERT_TRUE(result);

    // overlap
    INFO("Overlapping");
    grid->contents[5] = 1;

    BlockDb_setBlockPosition(db, block_id, (Point){2, 2});
    result = GameGrid_canBlockExist(grid, db, block_id);
    ASSERT_FALSE(result);

    BlockDb_deconstruct(db);
    GameGrid_deconstruct(grid);
}

void testGameGridResolveRows() {

    BlockDb *db = BlockDb_init(8);

    // int id_arr[2] = {4, 4};
    // BlockIds ids = {.max_ids=2, .id_array=id_arr, .head=0};
    int id1 = BlockDb_createBlock(db, 2, 0b1111, (Point){0, 0}, (SDL_Color){});
    int id2 = BlockDb_createBlock(db, 2, 0b1111, (Point){0, 0}, (SDL_Color){});


    GameGrid *grid = GameGrid_init(4, 4);
    int grid_contents[16] = {
        -1,     -1,   -1,   -1, 
         id1,  id1,   -1,   -1,
         id1,  id1,  id2,  id2,
        -1,     -1,  id2,  id2
    };
    memcpy(grid->contents, grid_contents, 16 * sizeof(int));

    int result = GameGrid_resolveRows(grid, db);

    ASSERT_EQUAL_INT(result, 1); // one row resolved

    ASSERT_EQUAL_INT(BlockDb_getCellCount(db, id1), 2);
    ASSERT_EQUAL_INT(BlockDb_getCellCount(db, id2), 2);


    /* final should be as below.
     * Notice the floating 0s - this is an official tetris quirk
        -1, -1, -1, -1, 
        -1, -1, -1, -1, 
         id1,  id1, -1, -1, 
        -1, -1,  id2, id2 
    */

    int height = grid->height;
    int width = grid->width;

    for (int grid_y = 0; grid_y < height; grid_y++) {
        for (int grid_x = 0; grid_x < width; grid_x++) {
            INFO_FMT("(%d, %d)", grid_x, grid_y);
            int grid_idx = grid_x + (grid_y * width);
            int grid_cell_val = grid->contents[grid_idx];

            if (8 == grid_idx || 9 == grid_idx) {
                ASSERT_EQUAL_INT(grid_cell_val, id1);
            }
            else if (14 == grid_idx || 15 == grid_idx) {
                ASSERT_EQUAL_INT(grid_cell_val, id2);
            }
            else {
                ASSERT_GREATER_THAN_INT(0, grid_cell_val);  // invalid id
            }
        }
    }
    BlockDb_deconstruct(db);
}

void testGameGridCommitBlock() {

    GameGrid *grid = GameGrid_init(4, 4);

    /*
     * 0 0 0 0
     * 0 1 1 0
     * 0 1 1 0
     * 0 0 0 0
     */
    long content_mask = ( 1L << 5 | 1L << 6 | 1L << 9 | 1L << 10 );

    BlockDb *db = BlockDb_init(8);

    int goodblock_id = BlockDb_createBlock(db, 4, content_mask, (Point){2, 2}, (SDL_Color){});
    int badblock_id = BlockDb_createBlock(db, 4, content_mask, (Point){0, 0}, (SDL_Color){});
    // bad because would need to commit to negative indices

    INFO_FMT("goodblock_id is %d\n", goodblock_id);
    INFO_FMT("badblock_id is %d\n", badblock_id);

    int good_result = GameGrid_commitBlock(grid, db, goodblock_id);

    ASSERT_EQUAL_INT(good_result, 0);
    ASSERT_EQUAL_INT(grid->contents[5], goodblock_id);
    ASSERT_EQUAL_INT(grid->contents[6], goodblock_id);
    ASSERT_EQUAL_INT(grid->contents[9], goodblock_id);
    ASSERT_EQUAL_INT(grid->contents[10], goodblock_id);

    // ASSERT_EQUAL_LONG(good_block.contents, 0L);  
    // Committed blocks have no more contents
    ASSERT_EQUAL_LONG(BlockDb_getBlockContents(db, goodblock_id), 0L);

    GameGrid_clear(grid);

    int bad_result = GameGrid_commitBlock(grid, db, badblock_id);
    ASSERT_EQUAL_INT(bad_result, -1);
    for (int grid_cell = 0; grid_cell < 16; grid_cell++) {
        ASSERT_EQUAL_INT(grid->contents[grid_cell], INVALID_BLOCK_ID);
    }

    ASSERT_EQUAL_LONG(BlockDb_getBlockContents(db, badblock_id), content_mask);  // nochange

    BlockDb_deconstruct(db);
}


void testGameGridAssessScore() {
    // Assess scoring of game grid based on states
    

    // Tests will perform on a standard 10w x 24h tetris grid,
    // but the logic used should apply
    GameGrid *grid = GameGrid_init(10, 24);

    int score;

    // completely empty
    score = GameGrid_assessScore(grid, 0);
    ASSERT_EQUAL_INT(score, 0);

    // some blocks but nothing filled
    grid->contents[0 + 10 * 23] = 0;
    grid->contents[1 + 10 * 23] = 0;
    grid->contents[0 + 9 * 22] = 0;
    grid->contents[1 + 9 * 22] = 0;
    score = GameGrid_assessScore(grid, 0);
    ASSERT_EQUAL_INT(score, 0);

    // one row
    for (int x = 0; x < grid->width; x++) {
        grid->contents[x + 10 * 23] = 0;
    }
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 0), 40);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 1), 80);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 2), 120);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 4), 200);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 8), 360);

    // two rows
    for (int x = 0; x < grid->width; x++) {
        grid->contents[x + 10 * 22] = 0;
    }
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 0), 100);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 1), 200);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 2), 300);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 4), 500);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 8), 900);

    // three rows
    for (int x = 0; x < grid->width; x++) {
        grid->contents[x + 10 * 21] = 0;
    }
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 0), 300);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 1), 600);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 2), 900);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 4), 1500);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 8), 2700);

    // four rows
    for (int x = 0; x < grid->width; x++) {
        grid->contents[x + 10 * 20] = 0;
    }
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 0), 1200);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 1), 2400);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 2), 3600);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 4), 6000);
    ASSERT_EQUAL_INT(GameGrid_assessScore(grid, 8), 10800);
}


void testGameGridAnimation() {

    GameGrid *grid = GameGrid_init(4, 8);
    int retval;

    retval = GameGrid_prepareAnimation(grid, 10);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_FALSE(grid->is_animating);

    // not yet animating
    retval = GameGrid_runAnimationFrame(grid);
    ASSERT_EQUAL_INT(retval, -1);


    for (int col = 0; col < grid->width; col++) {
        grid->contents[col] = 0;
    }
    retval = GameGrid_prepareAnimation(grid, 10);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_TRUE(grid->is_animating);

    // 4 to remove, 10 frames per removal
    for (int i = 0; i < 39; i++) {
        retval = GameGrid_runAnimationFrame(grid);
        ASSERT_EQUAL_INT(retval, 0);
        ASSERT_TRUE(grid->is_animating);
    }

    // #40, now done
    retval = GameGrid_runAnimationFrame(grid);
    ASSERT_EQUAL_INT(retval, 0);
    ASSERT_FALSE(grid->is_animating);

    retval = GameGrid_runAnimationFrame(grid);
    ASSERT_EQUAL_INT(retval, -1);



    GameGrid_deconstruct(grid);
}


int main() {
    EWENIT_START;
    ADD_CASE(testGameGridReset);
    ADD_CASE(testGameGridClear);
    ADD_CASE(testGameGridCanBlockInfoExist);
    ADD_CASE(testGameGridCanBlockExist);
    ADD_CASE(testGameGridResolveRows);
    ADD_CASE(testGameGridCommitBlock);

    ADD_CASE(testGameGridAssessScore);
    ADD_CASE(testGameGridAnimation);


    // EWENIT_END_COMPACT;
    EWENIT_END;
    return 0;
}
