#include "audio.h"


int loadSound(Sound *sound, char* filename, int volume)
{
    static char path[256];
    sprintf(path, "%s%s", AUDIOPATH, filename);
    sound->chunk = Mix_LoadWAV(path);
    if (sound->chunk == NULL)
    {
        fprintf(stderr, "Error: Could not load sound file %s\n", path);
        return -1;
    }
    sound->volume = volume;
    Mix_VolumeChunk(sound->chunk, volume);
    return 0;
}

void destroySound(Sound sound)
{
    Mix_FreeChunk(sound.chunk);
}

void playSound(Sound sound, unsigned int loops)
{
    Mix_PlayChannel(-1, sound.chunk, loops);
}

int initMixer(int numchans)
{
    if (Mix_OpenAudio(AUDIO_SAMPLERATE, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, AUDIO_CHUNKSIZE) < 0) return -1;
    else return 0;
    Mix_AllocateChannels(numchans);
}