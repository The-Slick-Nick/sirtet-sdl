/******************************************************************************
 * sirtet_audio.h
 *
 * This header/module will serve as a wrapper for this project's audio needs.
 * As the process of beginning to implement audio introduced hurdles that I am
 * still figuring
 * out and may be improved, I wanted to add a sort of wrapper api to allow
 * further development using basic audio-related functionality while also
 * providing the freedom to change certain audio settings and even swap out
 * different libraries without needing to rewrite a ton of implementation code
******************************************************************************/

#ifndef SIRTET_AUDIO_H
#define SIRTET_AUDIO_H


/*** Current configuration is for SDL_Mixer ***/
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>


typedef Mix_Chunk* SirtetAudio_sound;
typedef Mix_Music* SirtetAudio_music;


// Initialize audio
int SirtetAudio_start();

// End/close audio
int SirtetAudio_end();

// Load and return the sound effect resource at provided file path
SirtetAudio_sound SirtetAudio_loadSound(const char* path);

// Load and return the music resource at provided file path
SirtetAudio_music SirtetAudio_loadMusic(const char* path);

// Close/free loaded sound effect resource
int SirtetAudio_unloadSound(SirtetAudio_sound sound);

// Close/free loaded music resource
int SirtetAudio_unloadMusic(SirtetAudio_music music);

// Detects if sound resource is in some kind of unusable state (i.e. NULL)
bool SirtetAudio_soundInvalid(SirtetAudio_sound sound);

// Detects if music resource is in some kind of unusable state (i.e. NULL)
bool SirtetAudio_musicInvalid(SirtetAudio_music music);

int SirtetAudio_playSound(SirtetAudio_sound sound);
int SirtetAudio_playMusic(SirtetAudio_music music);

#endif
