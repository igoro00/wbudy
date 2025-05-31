#!/bin/bash

if [ -d build/cppcheck ]; then
    echo "Removing old build/cppcheck directory..."
    rm -rf build/cppcheck
fi

mkdir -p build/cppcheck

cwd=$(pwd)
jq --arg cwd "$cwd" \
    -c '[.[] | select((.file | startswith($cwd + "/src")) or (.file | startswith($cwd + "/lib")))]' \
    build/compile_commands.json > build/cppcheck/compile_commands.json

function check {
    cppcheck \
        --project=build/cppcheck/compile_commands.json \
        -j24 \
        --cppcheck-build-dir=build/cppcheck \
        --enable=all \
        --language=c++ \
        --std=c++17 \
        --addon=.github/scripts/misra.json \
        --force \
        --inline-suppr \
        --xml \
        -v \
        2> build/cppcheck-result.xml
}

export -f check

# unbuffer bash -c check | awk -F':' '{print $1}'
check

echo "Check completed."
echo "Parsing cppcheck results..."

python .github/scripts/cppcheck_parser.py build/cppcheck-result.xml
