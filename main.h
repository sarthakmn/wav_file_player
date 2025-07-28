#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include <stdint.h>
#include "arm_math.h" // CMSIS-DSP
#include "DSP/inc/dsp.h"
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>

#pragma pack(1)  // Ensure no padding in struct

// WAV file header structure (first 44 bytes)
typedef struct {
    char riff[4];          // "RIFF"
    uint32_t chunkSize;    // Size of the WAV file
    char wave[4];          // "WAVE"
    char fmt[4];           // "fmt "
    uint32_t subchunk1Size;// Size of the format chunk (usually 16 for PCM)
    uint16_t audioFormat;  // PCM = 1
    uint16_t numChannels;  // Mono = 1, Stereo = 2
    uint32_t sampleRate;   // 44100, 48000, etc.
    uint32_t byteRate;     // SampleRate * NumChannels * BitsPerSample/8
    uint16_t blockAlign;   // NumChannels * BitsPerSample/8
    uint16_t bitsPerSample;// 8, 16, 24, 32 bits
    char dataHeader[4];    // "data"
    uint32_t dataSize;     // Size of audio data in bytes
} WAVHeader;

extern FILE *fptr;
extern pthread_t pt;
extern volatile int state;
enum state{PLAY=1,PAUSE,STOP,PLUSE,MINUS,NEXT,PREV,FADE_IN,FADE_OUT};
void play(FILE *fptr,WAVHeader header);
void save_file(int16_t *buffer, size_t samples);
extern int nxt_prv;
extern double total_seconds;
extern double timestamp;

#define MAX_FILES 100
#define MAX_FILENAME 256
typedef struct {
    char *Songs[MAX_FILES];
    int count;
    int size;
} Songs;
void free_Songs(Songs *songs);
Songs list_Songs(const char *directory);
void print_Songs(Songs *songs);
char* next_song(Songs *songs);
char* prev_song(Songs *songs);
extern char path[256];

#endif