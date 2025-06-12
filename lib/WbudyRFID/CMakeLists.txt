cmake_minimum_required(VERSION 3.10)
project(WbudyRFID)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library (STATIC or SHARED)
add_library(WbudyRFID STATIC
    src/WbudyRFID.cpp
)

target_link_libraries(WbudyRFID
    pico_stdlib
    hardware_spi
    hardware_gpio
    FreeRTOS-Kernel
)

# Specify include directories for consumers of the library
target_include_directories(WbudyRFID PUBLIC
    ${PROJECT_SOURCE_DIR}/../../include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../../freertos/FreeRTOS-Kernel/include
)