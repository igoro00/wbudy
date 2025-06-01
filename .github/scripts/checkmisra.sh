#!/bin/bash

if [ -d build/cppcheck ]; then
    echo "Removing old build/cppcheck directory..."
    rm -rf build/cppcheck
fi

mkdir -p build/cppcheck

cwd=$(pwd)
jq --arg cwd "$cwd" \
    -c '[.[] | select((.file | startswith($cwd + "/src")) or (.file | startswith($cwd + "/lib/Wbudy")))]' \
    build/compile_commands.json > build/cppcheck/compile_commands.json

# Add absolute -Imisra/include to each compile command
jq --arg cwd "$cwd" 'map(. + {command: (.command + " -I" + $cwd + "/misra/include -I" + $cwd + "/build/generated/pico_base -I" + $cwd + "/include")})' build/cppcheck/compile_commands.json > build/cppcheck/compile_commands_patched.json

function check {
    cppcheck \
        --project=build/cppcheck/compile_commands_patched.json \
        -j24 \
        --cppcheck-build-dir=build/cppcheck \
        --enable=all \
        --language=c++ \
        --std=c++17 \
        --force \
        --inline-suppr \
        --xml \
        -D PICO_CONFIG_RTOS_ADAPTER_HEADER="$cwd/lib/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include/freertos_sdk_config.h" \
        -U __CYGWIN__ \
        -D __GNUC__ \
        -D __arm__ \
        -D __ARMEL__ \
        -D PICO_RP2040 \
        -D __INTPTR_TYPE__=int -D __INT32_TYPE__="long int" -D __CHAR_BIT__=8 \
        -U __riscv -U __sparc__ -U __SPU__ -U __MSDOS__ -U __i386__ -U GO32 \
        -U CYW43_NETUTILS -U LWIP_PERF -U MEMP_USE_CUSTOM_POOLS -U MEMP_USE_CUSTOM_POOLS -U PACK_STRUCT_USE_INCLUDES \
        -U __CUSTOM_FILE_IO__ -U PICO_CYW43_ARCH_HEADER\
        --suppress=*:lib/FreeRTOS-Kernel/* \
        --suppress=*:misra/* \
        -v \
        2> build/cppcheck-result.xml
}

export -f check

# unbuffer bash -c check | awk -F':' '{print $1}'
check

echo "Check completed."
echo "Parsing cppcheck results..."

python .github/scripts/cppcheck_parser.py build/cppcheck-result.xml
