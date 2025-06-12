cmake_minimum_required(VERSION 3.10)
project(WbudyBUTTON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library (STATIC or SHARED)
add_library(WbudyBUTTON STATIC
    src/WbudyBUTTON.cpp
)

target_link_libraries(WbudyBUTTON PUBLIC
    pico_stdlib
    hardware_timer
    FreeRTOS-Kernel
)

# Specify include directories for consumers of the library
target_include_directories(WbudyBUTTON PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../../include
    ${PROJECT_SOURCE_DIR}/../../freertos/FreeRTOS-Kernel/include
)