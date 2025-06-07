# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(default_build_type "Debug")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -DPICO_FLASH_SAFE_EXECUTE_SUPPORT_FREERTOS_SMP
    -I${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel/include
    -I${CMAKE_CURRENT_LIST_DIR}/include
    -I${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include
)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
set(PICO_FLASH_SIZE 0x1F8000) # 2MB - 32K for game data
include(pico_sdk_import.cmake)

project(wbudy C CXX ASM)

set(PICO_CXX_ENABLE_RTTI 1)

set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel)
set(FREERTOS_PORT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(wbudy
    src/main.cpp
    src/tSound.cpp
    src/sMain.cpp
    src/sLobby.cpp
    src/sGame.cpp
    src/Game.cpp
    src/rtoshooks.cpp
)

pico_set_program_name(wbudy "wbudy")
pico_set_program_version(wbudy "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(wbudy 0)
pico_enable_stdio_usb(wbudy 1)

include(FreeRTOS_Kernel_import.cmake)

# Add the standard include files to the build
target_include_directories(wbudy PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/WbudyLCD)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/WbudyRFID)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/WbudyLED)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/WbudyBUTTON)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Plus)
# Add any user requested libraries
target_link_libraries(wbudy 
    pico_stdlib
    pico_platform
    pico_multicore
    pico_runtime    
    hardware_spi
    hardware_i2c
    hardware_timer
    hardware_flash
    pico_cyw43_arch_lwip_threadsafe_background
    # pico_lwip_http
    FreeRTOS-Kernel-Heap4
    WbudyLCD
    WbudyRFID
    WbudyLED
    WbudyBUTTON
    FreeRTOS-Plus
)

pico_add_extra_outputs(wbudy)

