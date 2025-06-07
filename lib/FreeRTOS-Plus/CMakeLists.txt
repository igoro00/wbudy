cmake_minimum_required(VERSION 3.10)
project(FreeRTOS-Plus)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library (STATIC or SHARED)
add_library(FreeRTOS-Plus STATIC
    src/FreeRTOS_CLI.c
)

target_link_libraries(FreeRTOS-Plus PUBLIC
    FreeRTOS-Kernel
    pico_stdlib
)

# Specify include directories for consumers of the library
target_include_directories(FreeRTOS-Plus PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../../include
    ${PROJECT_SOURCE_DIR}/../../freertos/FreeRTOS-Kernel/include
)