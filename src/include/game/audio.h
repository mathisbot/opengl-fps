#ifndef AUDIO_H
#define AUDIO_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include <SDL2/SDL_mixer.h>

#include "logs.h"


#define AUDIO_SAMPLERATE 96000  // Sample frequency in Hz
#define AUDIO_CHUNKSIZE 2048  // Buffer size
#define AUDIO_NUMCHANS 16  // Number of channels

#define AUDIOPATH "assets/audio/"


/**
 * @brief Sound object
 * 
 * @param chunk Pointer to the Mix_Chunk object
 * @param volume The volume of the sound
*/
typedef struct {
    Mix_Chunk *chunk;
    int volume;
} Sound;


/**
 * @brief Loads an audio file
 * 
 * @param sound Pointer to the Sound object to create
 * @param filename The name of the file to load
 * @param volume The volume of the sound
 * @return int 0 on success, -1 on failure
*/
int loadSound(Sound *sound, char* filename, int volume);

/**
 * @brief Destroys a Sound object
 * 
 * @param sound Sound object to destroy
*/
void destroySound(Sound sound);

/**
 * @brief Plays a sound
 * 
 * @param sound Pointer to the Sound object to play
 * @param loops Number of times to loop the sound
*/
void playSound(Sound sound, unsigned int loops);


/**
 * @brief Initializes the audio mixer
 * 
 * @param numchans The number of channels to initialize
 * @return int 0 on success, -1 on failure
*/
int initMixer(int numchans);


#endif
