
#include <EWENIT.h>

#include "sirtet_audio.h"


void testNullConstants() {

    ASSERT_TRUE(SirtetAudio_soundInvalid(NULLSOUND));
    ASSERT_TRUE(SirtetAudio_musicInvalid(NULLMUSIC));

}

int main() {

    EWENIT_START;

    ADD_CASE(testNullConstants);

    EWENIT_END;

    return 0;
}
