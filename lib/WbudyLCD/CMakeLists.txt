cmake_minimum_required(VERSION 3.10)
project(WbudyLCD)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library (STATIC or SHARED)
add_library(WbudyLCD STATIC
    src/WbudyLCD.cpp
)

# Specify include directories for consumers of the library
target_include_directories(WbudyLCD PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)