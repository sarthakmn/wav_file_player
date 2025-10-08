#include "main.h"

void save_file(int16_t *buffer, size_t bytes_read) {
    static FILE *fptr = NULL;

    if (fptr == NULL) {
        fptr = fopen("../saved_audio.raw", "wb");
        if (!fptr) {
            perror("Error opening file");
            return;
        }
    }

    // Ensure we write 16-bit PCM samples
    size_t samples = bytes_read / sizeof(int16_t);

    // Write audio data
    size_t written = fwrite(buffer, sizeof(int16_t), samples, fptr);
    if (written != samples) {
        fprintf(stderr, "Error writing audio data: written %zu / %zu samples\n", written, samples);
    }

    fflush(fptr);
}