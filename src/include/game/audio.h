#ifndef AUDIO_H
#define AUDIO_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#include <SDL2/SDL_mixer.h>


#define AUDIO_SAMPLERATE 96000  // Sample frequency in Hz
#define AUDIO_CHUNKSIZE 2048  // Buffer size
#define AUDIO_NUMCHANS 16  // Number of channels

#define AUDIOPATH "assets/audio/"


typedef struct {
    Mix_Chunk* chunk;
    int channel;
    int volume;
} Sound;


/**
 * @brief Creates a new Sound object
 * 
 * @param filename The name of the file to load
 * @param channel The channel to play the sound on
 * @return Sound* A pointer to the new Sound object
*/
Sound* createSound(char* filename, int channel, int volume);

/**
 * @brief Frees a Sound object
 * 
 * @param sound Pointer to the Sound object to free
*/
void freeSound(Sound* sound);


/**
 * @brief Plays a sound
 * 
 * @param sound Pointer to the Sound object to play
 * @param loops Number of times to loop the sound
*/
void playSound(Sound* sound, unsigned int loops);


#endif