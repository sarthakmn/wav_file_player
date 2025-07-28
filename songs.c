#include "main.h"

// Function to scan directory and store .wav files in Songs struct
Songs list_Songs(const char *directory) {
    Songs songs;
    songs.count = 0;

    DIR *dir = opendir(directory);
    struct dirent *entry;

    if (dir == NULL) {
        perror("opendir");
        return songs;
    }

    while ((entry = readdir(dir)) != NULL && songs.count < MAX_FILES) {
        if (entry->d_type == DT_REG) {
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".wav") == 0) {
                songs.Songs[songs.count] = malloc(strlen(entry->d_name) + 1);
                if (songs.Songs[songs.count]) {
                    strcpy(songs.Songs[songs.count], entry->d_name);
                    songs.count++;                }
            }
        }
    }
    closedir(dir);
    return songs;
}

// Function to print the list of .wav files
void print_Songs(Songs *songs) {
    printf("List of Songs\n");
    for (int i = 0; i < songs->count; i++) {
        printf("%d. %s\n", i + 1, songs->Songs[i]);

    }
}

char* next_song(Songs *songs) {
    printf("count: %d, size: %d\n", songs->count, songs->size);
    if(songs->count != songs->size-1) {
        songs->count++;
    }
    else{
        songs->count = 0;
    }
    printf("Next song: %s count : %d\n", songs->Songs[songs->count],songs->count);
    return songs->Songs[songs->count];
}
char* prev_song(Songs *songs) {
    printf("count: %d, size: %d\n", songs->count, songs->size);
    if(songs->count != 0) {
        songs->count--;
    }
    else{
        songs->count = songs->size-1;
    }
    printf("Previous song: %s count : %d\n", songs->Songs[songs->count] ,songs->count);
    return songs->Songs[songs->count];
}

// Free memory allocated for song names
void free_Songs(Songs *songs) {
    for (int i = 0; i < songs->count; i++) {
        free(songs->Songs[i]);
    }
}

