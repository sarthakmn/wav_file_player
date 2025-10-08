#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "arm_math.h"

// EQ Control Structure
typedef struct {
    float bass_gain;    // -12dB to +12dB
    float mid_gain;     // -12dB to +12dB  
    float treble_gain;  // -12dB to +12dB
} eq_settings_t;

void init_biquad_filter();
void apply_biquad_filter(float32_t *input, float32_t *output, uint32_t length);

// 3-Band EQ Functions
void init_eq_filters();
void apply_eq(int16_t *buffer, uint32_t samples, eq_settings_t *eq);
void set_bass_gain(float gain_db);
void set_mid_gain(float gain_db);
void set_treble_gain(float gain_db);
eq_settings_t* get_eq_settings();