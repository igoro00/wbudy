cmake_minimum_required(VERSION 3.10)
project(WbudyLED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library (STATIC or SHARED)
add_library(WbudyLED STATIC
    src/WbudyLED.cpp
    src/WbudyRGB.cpp
)

target_link_libraries(WbudyLED PUBLIC
    hardware_pwm
    pico_stdlib
)

# Specify include directories for consumers of the library
target_include_directories(WbudyLED PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)