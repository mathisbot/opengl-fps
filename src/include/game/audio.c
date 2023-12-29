#include "audio.h"


static char path[256];

Sound* createSound(char* filename, int channel, int volume) {
    Sound* sound = malloc(sizeof(Sound));
    if (sound == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for Sound object\n");
        return NULL;
    }
    sprintf(path, "%s%s", AUDIOPATH, filename);
    sound->chunk = Mix_LoadWAV(path);
    if (sound->chunk == NULL) {
        fprintf(stderr, "Error: Could not load sound file %s\n", path);
        free(sound);
        return NULL;
    }
    sound->channel = channel;
    sound->volume = volume;
    Mix_VolumeChunk(sound->chunk, volume);
    return sound;
}

void freeSound(Sound* sound) {
    Mix_FreeChunk(sound->chunk);
    free(sound);
}


void playSound(Sound* sound, unsigned int loops) {
    Mix_PlayChannel(sound->channel, sound->chunk, loops);
}