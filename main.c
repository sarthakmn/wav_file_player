#include "main.h"
FILE *fptr;
FILE *file;
pthread_t pt;
WAVHeader header;
int volatile state = 0;
char path[256] = "../songs/";
double total_seconds = 0.0;

void * play_thread(void *arg){
    play(file,header);
}

int main() {
    Songs songs = list_Songs("../songs");
    print_Songs(&songs);
    songs.size = songs.count;
    songs.count = 0;
    char *curr_song = songs.Songs[songs.count];
    while (1){
        if(state == STOP)
            break;
        strcpy(path, "../songs/");
        strcat(path,curr_song);
        print_Songs(&songs);
        printf("Current song: %s\n", path);

        file = fopen(path, "rb"); // Open file in binary mode
            
        if (!file) {
            perror("Error opening file");
            return 1;
        }

        // Read the WAV file header
        if (fread(&header, sizeof(WAVHeader), 1, file) != 1) {
            perror("Error reading WAV header");
            fclose(file);
            return 1;
        }

        timestamp = header.dataSize;
        /* Count Time Interval of a song */  
        int frame_size = header.numChannels * (header.bitsPerSample / 8);
        double total_frames = (double)((header.chunkSize)/frame_size);
        total_seconds = total_frames / header.sampleRate;
        int minutes = (int)(total_seconds / 60);
        int seconds = total_seconds - (minutes * 60);
        printf("Time = %d min %d sec\n", minutes, seconds);

        pthread_create(&pt,NULL,play_thread,NULL);        
        while(1){ 
            printf("\n******* WELCOME to Audio Player *******\n");
            printf("1. Play\n");
            printf("2. Pause\n");
            printf("3. Stop\n");
            printf("4. Volume +\n");
            printf("5. Volume -\n");
            printf("6. Next \n");
            printf("7. Prev \n");
            printf("Enter Input : ");
            scanf("%d",&state);
            if(state == STOP)
                break;
            else if(state == NEXT){
                nxt_prv = 1;
                state = PAUSE;
                while(nxt_prv){}
                curr_song = next_song(&songs);
                state = PLAY;
                break;
            }
            else if(state == PREV){
                nxt_prv = 1;
                state = PAUSE;
                while(nxt_prv){}
                curr_song = prev_song(&songs);
                state = PLAY;
                break;
            }
            else{}
        }
        pthread_join(pt,NULL);
    }
    fclose(file);
    free_Songs(&songs);
    return 0;
}
