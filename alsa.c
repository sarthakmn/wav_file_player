#include "main.h"

#define FADE_STEP 0.005f
#define MAX_VOLUME 1.0f
#define MIN_VOLUME 0.0f
#define BUFFER_SIZE 4096

static float volume = 0.0f;
static float temp_volume = 0.5f;
static float start_vol = 0.0f;
static float target_vol = 0.5f;
static int fade_out_var = 0;
int nxt_prv = 0;
double timestamp = 0.0;
q15_t scale_factor;

static void flush_silence(snd_pcm_t *handle, size_t frames) {
    int16_t *silence_buf = calloc(frames * 2, sizeof(int16_t));  // 2 channels
    for(int i=0;i<30;i++) {
        snd_pcm_writei(handle, silence_buf, frames);
    }
    free(silence_buf);
}

static int fade_in() {
    if (start_vol < temp_volume) {
        volume += FADE_STEP;
        if (volume > temp_volume) volume = temp_volume;
        scale_factor = (q15_t)(volume * 32768);
        start_vol = volume;
        return 0;
    }
    return 1;
}

static int fade_out() {
    if (start_vol > 0) {
        volume -= FADE_STEP;
        if (volume < 0.0f) volume = 0.0f;
        scale_factor = (q15_t)(volume * 32768);
        start_vol = volume;
        return 0;
    }
    state = FADE_OUT;
    return 1;
}

static void apply_balance(int16_t *buffer, size_t samples, float balance) {
    float left_gain = 1.0f - fmaxf(0.0f, balance);
    float right_gain = 1.0f + fminf(0.0f, balance);
    for (size_t i = 0; i < samples; i += 2) {
        buffer[i] *= left_gain;      // Left
        buffer[i + 1] *= right_gain; // Right
    }
}

static snd_pcm_t* init_pcm(unsigned int rate, int channels, int *err) {
    snd_pcm_t *handle;
    *err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (*err < 0) return NULL;

    *err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE,
                              SND_PCM_ACCESS_RW_INTERLEAVED,
                              channels, rate, 1, 500000);
    if (*err < 0) {
        snd_pcm_close(handle);
        return NULL;
    }

    snd_pcm_prepare(handle);
    return handle;
}

void play(FILE *file, WAVHeader header) {
    int err;
    int16_t *buffer = calloc(BUFFER_SIZE, 1);
    if (!buffer) return;

    float balance = 0.0f;
    if (balance < -1.0f) balance = -1.0f;
    if (balance > 1.0f) balance = 1.0f;

    snd_pcm_t *pcm_handle = init_pcm(header.sampleRate, header.numChannels, &err);
    if (!pcm_handle) {
        fprintf(stderr, "Error opening PCM device: %s\n", snd_strerror(err));
        free(buffer);
        return;
    }

    size_t frame_bytes = header.numChannels * (header.bitsPerSample / 8);
    size_t frames = BUFFER_SIZE / frame_bytes;
    
    init_biquad_filter();

    while (1) {
        switch (state) {
            case PLAY: {
                snd_pcm_pause(pcm_handle, 0);
                fade_out_var ? fade_out() : fade_in();
                size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);

                if (bytes_read < frame_bytes) {
                    break;
                }

                arm_scale_q15(buffer, scale_factor, 1, buffer, bytes_read / 2);
                apply_balance(buffer, bytes_read / 2, balance);
                
                err = snd_pcm_writei(pcm_handle, buffer, frames);
                if (err < 0) {
                    fprintf(stderr, "PCM write error: %s\n", snd_strerror(err));
                    snd_pcm_prepare(pcm_handle);
                }
                //save_file(buffer, bytes_read);
                break;
            }

            case PAUSE:
                fade_out_var = 1;
                temp_volume = volume;
                state = PLAY;
                break;

            case FADE_OUT:
                flush_silence(pcm_handle, frames);  
                snd_pcm_pause(pcm_handle, 1); // Pause PCM playback
                snd_pcm_drop(pcm_handle);    // Immediately stop PCM without playing remaining samples
                snd_pcm_prepare(pcm_handle); // Prepare PCM again for future use
                
                volume = 0.0f;
                fade_out_var = 0;
                start_vol = 0.0f;
                target_vol = temp_volume;
                if(nxt_prv){
                    nxt_prv = 0;
                    state = STOP;
                    return;
                }
                break;

            case STOP:
                snd_pcm_drain(pcm_handle);
                snd_pcm_close(pcm_handle);
                free(buffer);
                return;

            case PLUSE:
                volume = fminf(volume + 0.1f, MAX_VOLUME);
                scale_factor = (q15_t)(volume * 32768);
                temp_volume = volume;
                state = PLAY;
                break;

            case MINUS:
                volume = fmaxf(volume - 0.1f, MIN_VOLUME);
                scale_factor = (q15_t)(volume * 32768);
                temp_volume = volume;
                state = PLAY;
                break;
            default:
                break;
        }
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buffer);
}
