
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
    ASSERT_EQUAL_INT(grid.contents[0], 0);
    ASSERT_EQUAL_INT(grid.contents[1], 0);
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
    int block_contents = ( 1 << 5 | 1 << 6 | 1 << 9 | 1 << 10 );
    bool result;

    // even sized grid

    // nw corner
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=0, .y=0});
    ASSERT_FALSE(result);

    // ne corner
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=4, .y=0});
    ASSERT_FALSE(result);

    // sw corner
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=0, .y=4});
    ASSERT_FALSE(result);

    // s3 corner
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=4, .y=4});
    ASSERT_FALSE(result);

    // center
    result = GameGrid_canBlockInfoExist( &grid, block_size, block_contents, (Point){.x=2, .y=2});
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
    block.position = (Point){.x=0, .y=0};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // ne corner
    block.position = (Point){.x=4, .y=0};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // sw corner
    block.position = (Point){.x=0, .y=4};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // s3 corner
    block.position = (Point){.x=4, .y=4};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

    // center
    block.position = (Point){.x=2, .y=2};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_TRUE(result);

    // overlap
    grid.contents[5] = 1;
    block.position = (Point){.x=2, .y=2};
    result = GameGrid_canBlockExist(&grid, &block);
    ASSERT_FALSE(result);

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

    ASSERT_EQUAL_INT(result, 1);
    ASSERT_EQUAL_INT(ids.id_array[0], 2);
    ASSERT_EQUAL_INT(ids.id_array[1], 2);

    /* final should be
        -1, -1, -1, -1, 
        -1, -1, -1, -1, 
        -1, -1, -1, -1, 
        -1, -1, -1, -1, 
         0,  0,  1,  1
    */
}

int main() {
    EWENIT_START;
    ADD_CASE(testGameGridReset);
    ADD_CASE(testGameGridCanBlockInfoExist);
    ADD_CASE(testGameGridCanBlockExist);
    ADD_CASE(testGameGridResolveRows);
    EWENIT_END;
    // EWENIT_END_COMPACT;
    return 0;
}
