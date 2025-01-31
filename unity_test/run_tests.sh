#!/bin/bash

# set the executable name
TEST_EXEC="test_chip8"

# clean up previous test binary
if [ -f "$TEST_EXEC" ]; then
    echo "Removing previous test binary..."
    rm "$TEST_EXEC"
fi

# compile the test file with Unity
echo "Compiling tests..."
gcc c8_test.c ../cpu.c unity.c -o "$TEST_EXEC"

# check if compilation succeeded
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# run the test executable
echo "Running tests..."
./"$TEST_EXEC"
