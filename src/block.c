#include "block.h"


/* Bit representation per block
* [00] [01] [02] [03]
* [04] [05] [06] [07]
* [08] [09] [10] [11]
* [12] [13] [14] [15]
*/

Block Block_rotateCw90(Block block) {
   /* represent in quadrants  in the complex plane
    * (-i + 1) | ( i + 1)
    * ---------------------
    * (-i - 1) | ( i - 1)
    *
    * To rotate 90deg clockwise, multiply by i
    * To rotate 90deg counterclockwise, multiply by -i
    * To rotate 180deg, multiply by -1
    *
    *  Perform two rotations, first rotating 2x2 blocks 
    *  around the overall center, than performing that
    *  same rotation within each 2x2.
    */

    return block;
    
}
Block Block_rotateCcw90(Block block);
Block Block_rotate180(Block block);
