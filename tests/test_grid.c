
#include "block.h"
#include "grid.h"
#include "EWENIT.h"


void testGameGridClear() {
    // GameGrid_clear
    const int width = 1;
    const int height = 2;
    int grid_contents[1 * 2];

    GameGrid grid = {.width=width, .height=height, .contents=grid_contents};

    GameGrid_clear(&grid);

    // IDs stored should be made invalid
    ASSERT_GREATER_THAN_INT(0, grid.contents[0]);
    ASSERT_GREATER_THAN_INT(0, grid.contents[1]);
}

void testGameGridReset() {

    const int width = 2;
    const int height = 4;
    int grid_contents[8] = {
        -1, -1,
         0,  0,
        -1, -1,
         2,  2
    };

    int id_arr[4] = {2, 0, 2, 0};
    GameGrid grid = {.width=width, .height=height, .contents=grid_contents};
    BlockIds ids = {.max_ids=4, .id_array=id_arr, .head=0};

    GameGrid_reset(&grid, &ids);

    for (int i = 0; i < 8; i++) {
        INFO_FMT("Index %d", i);
        ASSERT_EQUAL_INT((grid.contents)[i], -1);
    }

    for (int i = 0; i < 4; i++) {
        INFO_FMT("Index %d", i);
        ASSERT_EQUAL_INT((ids.id_array)[i], 0);
    }
}


void testGameGridCanBlockInfoExist() {

    const int width = 4;
    const int height = 4;
    int grid_contents[16] = {-1};
    GameGrid grid = {.width=width, .height=height, .contents=grid_contents};
    GameGrid_clear(&grid);

    int block_size = 4;
    // center of 4 = 5, 6, 9, 10
    long block_contents = ( 1L << 5 | 1L << 6 | 1L << 9 | 1L << 10 );
    bool result;

    // even sized grid

    // nw corner
    INFO("NW");
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=0, .y=0});
    ASSERT_FALSE(result);

    // ne corner
    INFO("NE");
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=4, .y=0});
    ASSERT_FALSE(result);

    // sw corner
    INFO("SW");
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=0, .y=4});
    ASSERT_FALSE(result);

    // s3 corner
    INFO("SE");
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=4, .y=4});
    ASSERT_FALSE(result);

    // center
    INFO("Center");
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=2, .y=2});
    ASSERT_TRUE(result);

    // nw corner, overall block scope can overlap but actualized content doesn't
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=1, .y=1});
    ASSERT_TRUE(result);

    // overlap
    grid.contents[5] = 1;
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=2, .y=2});
    ASSERT_FALSE(result);


    // even-sized grid
}

void testGameGridCanBlockExist() {
    
    const int width = 4;
    const int height = 4;
    int grid_contents[16] = {-1};
    GameGrid grid = {.width=width, .height=height, .contents=grid_contents};
    GameGrid_clear(&grid);

    int block_size = 4;
    // center of 4 = 5, 6, 9, 10
    int block_contents = ( 1 << 5 | 1 << 6 | 1 << 9 | 1 << 10 );
    Block block = {.size=block_size, .contents=block_contents};
    bool result;

    // even sized grid

    // nw corner
    INFO("NW");
    block.position = (Point){.x=0, .y=0};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // ne corner
    INFO("NE");
    block.position = (Point){.x=4, .y=0};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // sw corner
    INFO("SW");
    block.position = (Point){.x=0, .y=4};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // s3 corner
    INFO("SE");
    block.position = (Point){.x=4, .y=4};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // center
    INFO("Center");
    block.position = (Point){.x=2, .y=2};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_TRUE(result);

    // overlap
    INFO("Overlapping");
    grid.contents[5] = 1;
    block.position = (Point){.x=2, .y=2};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);


    /* MANUAL EXAMPLES OF THINGS THAT FAILED BUT SHOULDNT HAVE */
    Block primary_block = {
        .id=1,
        .position=(Point){.x=5, .y=5},
        .contents=block_contents,
        .size=4
    };

    int new_grid_contents[100] = {-1};
    GameGrid ref_grid = {
        .width=10,
        .height=10,
        .contents=new_grid_contents
    };
    GameGrid_clear(&ref_grid);

    Block projected_block;
    projected_block = (Block){
        .size=primary_block.size,
        .contents=primary_block.contents,
        .id=-1,
        .position=(Point){.x=primary_block.position.x, .y=primary_block.position.y + 1}
    };

    result = GameGrid_canBlockExist(&ref_grid, &projected_block);
    ASSERT_TRUE(result);

}

void testGameGridResolveRows() {

    const int width = 4;
    const int height = 4;
    int grid_contents[16] = {
        -1, -1, -1, -1, 
         0,  0, -1, -1,
         0,  0,  1,  1,
        -1, -1,  1,  1
    };
    GameGrid grid = {.width=width, .height=height, .contents=grid_contents};

    int id_arr[2] = {4, 4};
    BlockIds ids = {.max_ids=2, .id_array=id_arr, .head=0};

    int result = GameGrid_resolveRows(&grid, &ids);

    ASSERT_EQUAL_INT(result, 1); // one row resolved
    ASSERT_EQUAL_INT(ids.id_array[0], 2);
    ASSERT_EQUAL_INT(ids.id_array[1], 2);

    /* final should be as below.
     * Notice the floating 0s - this is an official tetris quirk
        -1, -1, -1, -1, 
        -1, -1, -1, -1, 
         0,  0, -1, -1, 
        -1, -1,  1,  1
    */

    for (int grid_y = 0; grid_y < height; grid_y++) {
        for (int grid_x = 0; grid_x < width; grid_x++) {
            INFO_FMT("(%d, %d)", grid_x, grid_y);
            int grid_idx = grid_x + (grid_y * width);
            int grid_cell_val = grid.contents[grid_idx];

            if (8 == grid_idx || 9 == grid_idx) {
                ASSERT_EQUAL_INT(grid_cell_val, 0);
            }
            else if (14 == grid_idx || 15 == grid_idx) {
                ASSERT_EQUAL_INT(grid_cell_val, 1);
            }
            else {
                ASSERT_GREATER_THAN_INT(0, grid_cell_val);  // invalid id
            }
        }
    }
}


void testGameGridCommitBlock() {

    const int width = 4;
    const int height = 4;
    int grid_contents[16] = {-1};
    GameGrid grid = {.width=width, .height=height, .contents=grid_contents};

    GameGrid_clear(&grid);

    /*
     * 0 0 0 0
     * 0 1 1 0
     * 0 1 1 0
     * 0 0 0 0
     */
    long block_contents = ( 1L << 5 | 1L << 6 | 1L << 9 | 1L << 10 );

    int goodblock_id = 1;
    int badblock_id = 2;

    Block good_block = {
        .contents=block_contents,
        .id=goodblock_id,
        .size=4,
        .position=(Point){.x=2, .y=2}
    };

    Block bad_block = {
        .contents=block_contents,
        .id=badblock_id,
        .size=4,
        .position=(Point){.x=0, .y=0}
    };

    int good_result = GameGrid_commitBlock(&grid, &good_block);
    ASSERT_EQUAL_INT(good_result, 0);
    ASSERT_EQUAL_INT(grid.contents[5], goodblock_id);
    ASSERT_EQUAL_INT(grid.contents[6], goodblock_id);
    ASSERT_EQUAL_INT(grid.contents[9], goodblock_id);
    ASSERT_EQUAL_INT(grid.contents[10], goodblock_id);
    ASSERT_EQUAL_LONG(good_block.contents, 0L);  // Committed blocks have no more contents

    GameGrid_clear(&grid);

    int bad_result = GameGrid_commitBlock(&grid, &bad_block);
    ASSERT_EQUAL_INT(bad_result, -1);
    for (int grid_cell = 0; grid_cell < 16; grid_cell++) {
        // Id should be negative (though exact number doesn't matter)
        ASSERT_GREATER_THAN_INT(0, grid.contents[grid_cell]);
    }
    ASSERT_EQUAL_LONG(bad_block.contents, block_contents); // no change
}


int main() {
    EWENIT_START;
    ADD_CASE(testGameGridReset);
    ADD_CASE(testGameGridClear);
    ADD_CASE(testGameGridCanBlockInfoExist);
    ADD_CASE(testGameGridCanBlockExist);
    ADD_CASE(testGameGridResolveRows);
    ADD_CASE(testGameGridCommitBlock);
    EWENIT_END_VERBOSE;
    // EWENIT_END_COMPACT;
    return 0;
}
