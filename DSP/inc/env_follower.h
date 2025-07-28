#ifndef _ENV_FOLLOWER_H_
#define _ENV_FOLLOWER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct env_follower
{
    float fs;       // sample rate
    float peak_env; // peak envelope estimate
    float rms_env;  // rms envelope estimate
    float sil_env;  // silence envelope estimate

    float peak_up_time;   // smoothing factor for increasing peak envelope
    float peak_down_time; // smoothing factor for decreasing peak envelope

    float rms_up_time;   // smoothing factor for increasing rms envelope
    float rms_down_time; // smoothing factor for decreasing rms envelope

    float sil_down_time; // smoothing factor for decreasing silence envelope
    float sil_up_time;   // smoothing factor for increasing silence envelope

    int fast_init_flag;
}env_follower;


// parameter structure for env_follower block
// ** struct shared with Malt tuning tool (CongaBridge\inc\congabridge.h) ! **
struct env_follower_params {
    float peak_up_time_ms;
    float peak_down_time_ms;
    float rms_up_time_ms;
    float rms_down_time_ms;
};

// parameter IDs for env_follower. The xx_ALL_PARAMS sets all the parameters of the structure

enum env_follower_param_id {
    ENV_FOLLOWER_PEAK_UP_TIME  ,
    ENV_FOLLOWER_PEAK_DOWN_TIME,
    ENV_FOLLOWER_RMS_UP_TIME   ,
    ENV_FOLLOWER_RMS_DOWN_TIME ,
    ENV_FOLLOWER_ALL_PARAMS    ,
};

void env_follower_init(env_follower* handle, float fs);
void env_follower_process(env_follower* handle, float* in, int is_silence, float* peak_env,
                            float* rms_env, float* silence_env, int sampleframes);
int env_follower_set_parameters(env_follower* handle, void *pValue, enum env_follower_param_id param_id);

void env_follower_get_default_parameters(struct env_follower_params* params);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ENV_FOLLOWER_H_ */
