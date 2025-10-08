#include "../inc/dsp.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUM_STAGES 1  // Number of biquad stages (1 for simple low-pass)
#define SAMPLE_RATE 44100.0f  // Sample rate

// Original simple filter
arm_biquad_casd_df1_inst_f32 S;  // Filter instance
float32_t biquadCoeffs[5 * NUM_STAGES] = {
    0.00019897, 0.00039794, 0.00019897, 1.95970703, -0.96050292
};  // Coefficients for a low-pass filter
float32_t biquadState[2 * NUM_STAGES] = {0}; // State buffer

// 3-Band EQ Implementation
#define EQ_STAGES 3  // Bass, Mid, Treble

// EQ Filter instances
arm_biquad_casd_df1_inst_f32 bass_filter;
arm_biquad_casd_df1_inst_f32 mid_filter;  
arm_biquad_casd_df1_inst_f32 treble_filter;

// EQ Coefficients and states
float32_t bass_coeffs[5] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};    // Will be calculated
float32_t mid_coeffs[5] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};     // Will be calculated  
float32_t treble_coeffs[5] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};  // Will be calculated

float32_t bass_state[4] = {0};    // 2 * numStages for each filter
float32_t mid_state[4] = {0};
float32_t treble_state[4] = {0};

// EQ Settings
static eq_settings_t eq_settings = {0.0f, 0.0f, 0.0f};  // 0dB gain initially

// Helper function to calculate biquad coefficients for peaking EQ
void calculate_peaking_eq(float *coeffs, float freq, float gain_db, float q) {
    float A = powf(10.0f, gain_db / 40.0f);  // Convert dB to linear gain
    float w0 = 2.0f * M_PI * freq / SAMPLE_RATE;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * q);
    
    float b0 = 1.0f + alpha * A;
    float b1 = -2.0f * cos_w0;
    float b2 = 1.0f - alpha * A;
    float a0 = 1.0f + alpha / A;
    float a1 = -2.0f * cos_w0;
    float a2 = 1.0f - alpha / A;
    
    // Normalize coefficients
    coeffs[0] = b0 / a0;  // b0
    coeffs[1] = b1 / a0;  // b1
    coeffs[2] = b2 / a0;  // b2
    coeffs[3] = -a1 / a0; // -a1 (note the sign change for CMSIS-DSP)
    coeffs[4] = -a2 / a0; // -a2 (note the sign change for CMSIS-DSP)
}

// Helper function for shelving filters (bass/treble)
void calculate_low_shelf(float *coeffs, float freq, float gain_db) {
    float A = powf(10.0f, gain_db / 40.0f);
    float w0 = 2.0f * M_PI * freq / SAMPLE_RATE;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float S = 1.0f;  // Shelf slope
    float alpha = sin_w0 / 2.0f * sqrtf((A + 1.0f/A) * (1.0f/S - 1.0f) + 2.0f);
    float sqrt_A = sqrtf(A);
    
    float b0 = A * ((A + 1.0f) - (A - 1.0f) * cos_w0 + 2.0f * sqrt_A * alpha);
    float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos_w0);
    float b2 = A * ((A + 1.0f) - (A - 1.0f) * cos_w0 - 2.0f * sqrt_A * alpha);
    float a0 = (A + 1.0f) + (A - 1.0f) * cos_w0 + 2.0f * sqrt_A * alpha;
    float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cos_w0);
    float a2 = (A + 1.0f) + (A - 1.0f) * cos_w0 - 2.0f * sqrt_A * alpha;
    
    coeffs[0] = b0 / a0;
    coeffs[1] = b1 / a0;
    coeffs[2] = b2 / a0;
    coeffs[3] = -a1 / a0;
    coeffs[4] = -a2 / a0;
}

void calculate_high_shelf(float *coeffs, float freq, float gain_db) {
    float A = powf(10.0f, gain_db / 40.0f);
    float w0 = 2.0f * M_PI * freq / SAMPLE_RATE;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float S = 1.0f;
    float alpha = sin_w0 / 2.0f * sqrtf((A + 1.0f/A) * (1.0f/S - 1.0f) + 2.0f);
    float sqrt_A = sqrtf(A);
    
    float b0 = A * ((A + 1.0f) + (A - 1.0f) * cos_w0 + 2.0f * sqrt_A * alpha);
    float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos_w0);
    float b2 = A * ((A + 1.0f) + (A - 1.0f) * cos_w0 - 2.0f * sqrt_A * alpha);
    float a0 = (A + 1.0f) - (A - 1.0f) * cos_w0 + 2.0f * sqrt_A * alpha;
    float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cos_w0);
    float a2 = (A + 1.0f) - (A - 1.0f) * cos_w0 - 2.0f * sqrt_A * alpha;
    
    coeffs[0] = b0 / a0;
    coeffs[1] = b1 / a0;
    coeffs[2] = b2 / a0;
    coeffs[3] = -a1 / a0;
    coeffs[4] = -a2 / a0;
}

// Original functions
void init_biquad_filter() {
    arm_biquad_cascade_df1_init_f32(&S, NUM_STAGES, biquadCoeffs, biquadState);
}

void apply_biquad_filter(float32_t *input, float32_t *output, uint32_t length) {
    arm_biquad_cascade_df1_f32(&S, input, output, length);
}

// 3-Band EQ Functions
void init_eq_filters() {
    // Initialize with flat response (0dB gain)
    calculate_low_shelf(bass_coeffs, 100.0f, 0.0f);     // Bass: 100 Hz low shelf
    calculate_peaking_eq(mid_coeffs, 1000.0f, 0.0f, 1.0f); // Mid: 1kHz peaking, Q=1
    calculate_high_shelf(treble_coeffs, 8000.0f, 0.0f); // Treble: 8kHz high shelf
    
    // Initialize filter instances
    arm_biquad_cascade_df1_init_f32(&bass_filter, 1, bass_coeffs, bass_state);
    arm_biquad_cascade_df1_init_f32(&mid_filter, 1, mid_coeffs, mid_state);
    arm_biquad_cascade_df1_init_f32(&treble_filter, 1, treble_coeffs, treble_state);
}

void apply_eq(int16_t *buffer, uint32_t samples, eq_settings_t *eq) {
    // Convert to float for processing
    static float32_t temp_buffer[4096];  // Adjust size as needed
    static float32_t temp_buffer2[4096];
    
    // Limit samples to buffer size
    if (samples > 4096) samples = 4096;
    
    // Convert int16 to float32
    for (uint32_t i = 0; i < samples; i++) {
        temp_buffer[i] = (float32_t)buffer[i] / 32768.0f;
    }
    
    // Apply filters in series: Bass -> Mid -> Treble
    arm_biquad_cascade_df1_f32(&bass_filter, temp_buffer, temp_buffer2, samples);
    arm_biquad_cascade_df1_f32(&mid_filter, temp_buffer2, temp_buffer, samples);
    arm_biquad_cascade_df1_f32(&treble_filter, temp_buffer, temp_buffer2, samples);
    
    // Convert back to int16
    for (uint32_t i = 0; i < samples; i++) {
        float sample = temp_buffer2[i] * 32768.0f;
        
        // Clamp to prevent overflow
        if (sample > 32767.0f) sample = 32767.0f;
        if (sample < -32768.0f) sample = -32768.0f;
        
        buffer[i] = (int16_t)sample;
    }
}

void set_bass_gain(float gain_db) {
    // Limit gain to ±12dB
    if (gain_db > 12.0f) gain_db = 12.0f;
    if (gain_db < -12.0f) gain_db = -12.0f;
    
    eq_settings.bass_gain = gain_db;
    calculate_low_shelf(bass_coeffs, 100.0f, gain_db);
    arm_biquad_cascade_df1_init_f32(&bass_filter, 1, bass_coeffs, bass_state);
}

void set_mid_gain(float gain_db) {
    if (gain_db > 12.0f) gain_db = 12.0f;
    if (gain_db < -12.0f) gain_db = -12.0f;
    
    eq_settings.mid_gain = gain_db;
    calculate_peaking_eq(mid_coeffs, 1000.0f, gain_db, 1.0f);
    arm_biquad_cascade_df1_init_f32(&mid_filter, 1, mid_coeffs, mid_state);
}

void set_treble_gain(float gain_db) {
    if (gain_db > 12.0f) gain_db = 12.0f;
    if (gain_db < -12.0f) gain_db = -12.0f;
    
    eq_settings.treble_gain = gain_db;
    calculate_high_shelf(treble_coeffs, 8000.0f, gain_db);
    arm_biquad_cascade_df1_init_f32(&treble_filter, 1, treble_coeffs, treble_state);
}

eq_settings_t* get_eq_settings() {
    return &eq_settings;
}