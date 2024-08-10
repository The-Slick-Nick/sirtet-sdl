
#include "application_state.h"
#include "sirtet.h"
#include <SDL2/SDL_mixer.h>


/******************************************************************************
 * Start/end
******************************************************************************/

int SirtetAudio_start() {
    int errcode;

    errcode = Mix_Init(0);

    if (errcode != 0) {
        char errbuff[ERRMSG_SZ];
        snprintf(
            errbuff, ERRMSG_SZ,
            "Error initializing SirtetAudio:    %s\n",
            Mix_GetError()
        );
        Sirtet_setError(errbuff);
        return errcode;
    }

    errcode = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 512) != 0;
    if (errcode != 0) {
        char errbuff[ERRMSG_SZ];
        snprintf(
            errbuff, ERRMSG_SZ,
            "Error initializing SirtetAudio:    %s\n",
            Mix_GetError()
        );
        Sirtet_setError(errbuff);
        return errcode;
    }

    return 0;
}


int SirtetAudio_end() {
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}


/******************************************************************************
 * Content management
******************************************************************************/

bool SirtetAudio_soundInvalid(SirtetAudio_sound sound) {
    return sound == NULL;
}

bool SirtetAudio_musicInvalid(SirtetAudio_music music) {
    return music == NULL;
}

SirtetAudio_sound SirtetAudio_loadSound(const char* path) {

    // TODO: Some kind of clever magic to determine which file load function
    // to call based on path argument 

    Mix_Chunk *sound = Mix_LoadWAV(path);

    if (sound == NULL) {
        char errmsg[ERRMSG_SZ];
        snprintf(
            errmsg, ERRMSG_SZ,
            "Error loading sound: %s\n",
            Mix_GetError()
        );
    }
    return sound;
}


int SirtetAudio_unloadSound(SirtetAudio_sound sound) {
    
    if ( !SirtetAudio_soundInvalid(sound) ) {
        Mix_FreeChunk(sound);
    }

    return 0;
}


SirtetAudio_music SirtetAudio_loadMusic(const char *path) {
    Sirtet_setError("Music loading not yet implemented\n");
    return NULL;
}



/******************************************************************************
 * Content usage
******************************************************************************/

int SirtetAudio_playSound(SirtetAudio_sound sound) {

    Mix_PlayChannel(-1, sound, 1);
    return 0;

}


int SirtetAudio_playMusic(SirtetAudio_music music) {
    Sirtet_setError("Music playing not yet implemented\n");
    return -1;
}

