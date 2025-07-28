#include "../inc/dsp.h"

#define NUM_STAGES 1  // Number of biquad stages (1 for simple low-pass)

arm_biquad_casd_df1_inst_f32 S;  // Filter instance
float32_t biquadCoeffs[5 * NUM_STAGES] = {
    0.00019897, 0.00039794, 0.00019897, 1.95970703, -0.96050292
};  // Coefficients for a low-pass filter

float32_t biquadState[2 * NUM_STAGES] = {0}; // State buffer

void init_biquad_filter() {
    arm_biquad_cascade_df1_init_f32(&S, NUM_STAGES, biquadCoeffs, biquadState);
}

void apply_biquad_filter(float32_t *input, float32_t *output, uint32_t length) {
    arm_biquad_cascade_df1_f32(&S, input, output, length);
}