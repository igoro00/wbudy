#!/bin/bash

if [ -d build/cppcheck ]; then
    echo "Removing old build/cppcheck directory..."
    rm -rf build/cppcheck
fi

mkdir -p build/cppcheck

function check {
    cppcheck \
        -j24 \
        --cppcheck-build-dir=build/cppcheck \
        --enable=all \
        --language=c++ \
        --std=c++17 \
        --addon=.github/scripts/misra.json \
        --force \
        --inline-suppr \
        --xml \
        --suppress=cstyleCast \
        --suppress=*:misra/* \
        --suppress=*:freertos/* \
        -v \
        src \
        lib \
        -I include \
        -I lib/WbudyLCD/include \
        -I lib/WbudyRFID/include \
        -I lib/WbudyLED/include \
        -I lib/WbudyBUTTON/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2040/hardware_regs/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2040/hardware_structs/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_base/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_clocks/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_gpio/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_pwm/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_timer/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_spi/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/hardware_i2c/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/rp2_common/pico_cyw43_arch/include \
        -I misra/.pico-sdk/sdk/2.1.1/src/common/pico_stdlib_headers/include \
        -I misra/.pico-sdk/sdk/2.1.1/lib/lwip/src/include \
        -I misra/.pico-sdk/toolchain/14_2_Rel1/arm-none-eabi/include \
        -I misra/.pico-sdk/toolchain/14_2_Rel1/arm-none-eabi/include/c++/14.2.1 \
        -I freertos \
        -I freertos/FreeRTOS-Kernel/include \
        -I freertos/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include \
        2> build/cppcheck-result.xml
}

export -f check

# unbuffer bash -c check | awk -F':' '{print $1}'
check

echo "Check completed."
echo "Parsing cppcheck results..."

python .github/scripts/cppcheck_parser.py build/cppcheck-result.xml
