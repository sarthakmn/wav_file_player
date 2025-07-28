project(player)
cmake_minimum_required(VERSION 2.8)
add_subdirectory(DSP)
include_directories(${CMAKE_SOURCE_DIR}/DSP/inc)
include_directories(${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/Include)
include_directories(${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/PrivateInclude)
add_library(CMSIS_DSP STATIC
    ${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/Source/BasicMathFunctions/arm_scale_q15.c
    ${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/Source/SupportFunctions/arm_q15_to_float.c
    ${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/Source/SupportFunctions/arm_float_to_q15.c
    ${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
    ${CMAKE_SOURCE_DIR}/DSP/CMSIS-DSP-main/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
)
add_executable(player main.c alsa.c save_file.c songs.c)
target_link_libraries(player DSP CMSIS_DSP asound m)
