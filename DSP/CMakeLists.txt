cmake_minimum_required(VERSION 3.10)

# Set the project name
project(DSP)

# Add source files
set(SOURCES
    src/dsp.c
)

# Add header files
set(HEADERS
    inc/arm_math.h
    inc/dsp.h
)

# Include directories
include_directories(inc)

# Create the library
add_library(DSP 
${SOURCES} 
${HEADERS})