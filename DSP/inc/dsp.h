#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "arm_math.h"

void init_biquad_filter();
void apply_biquad_filter(float32_t *input, float32_t *output, uint32_t length);