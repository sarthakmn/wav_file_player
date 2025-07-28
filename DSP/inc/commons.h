/**
 ***************************************************************************************************
 * Copyright (c) 2022 Faurecia Clarion Electronics Europe SAS
 *
 ***************************************************************************************************
 */

#ifndef _COMMONS_H_
#define _COMMONS_H_

#include "arm_math.h"

#define MAX(x,y)    ( ((x) > (y)) ? (x) : (y) )

#define SAMPLING_FREQUENCY 48000

#define TRUE    1
#define FALSE   0

#define NB_CHAN_STEREO 2
#define GAIN_MUTE   0.0
#define GAIN_0DB    1.0

#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define REAR_LEFT 2
#define REAR_RIGHT 3

#define DB_TO_LINEAR_FLOAT(Value_dB)  (powf ( 10.f, Value_dB / 20.0f ))

#pragma pack(1)

/* Biquad filter Coefficients */

#define BIQUAD_Q31_POSTSHIFT    1   // aibi coefficients must be divide by 2 to be in range [-1,1] of Q31
// So postshift must apply to have same result as initial range [-2,2]

#define FLOAT_TO_Q31        (1 << (31-1))      // shift (31-1) : allows to have coeff between [-2, 2[ in Q31
#define BQ_COEFF_DOUBLE_2_Q31(val)  (q31_t)((val*FLOAT_TO_Q31) + ((val > 0) ? 0.5 : -0.5))  // convert float/double coeff calculated with biquad design to Q31

typedef struct biquad_coeff_float
{
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
} biquad_coeff_float;

typedef struct biquad_coeff_q31
{
    q31_t b0;
    q31_t b1;
    q31_t b2;
    q31_t a1;
    q31_t a2;
} biquad_coeff_q31;

#pragma pack()

void change_coeff_ai_sign(biquad_coeff_float * pcoeff, int nb_band_coeff);
void change_coeff_ai_sign_q31(biquad_coeff_q31* pcoeff, int nb_band_coeff);
void init_biquad_coeff_flat_Q31(biquad_coeff_q31* pCoeffs);

#endif /* _COMMONS_H_ */
