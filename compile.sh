#!/bin/bash
output=$(gcc -Wall $1 2>&1)
errors=$(echo "$output" | grep -c "error:")
warnings=$(echo "$output" | grep -c "warning:")
echo "$errors,$warnings"  # This line will be read by the parent process
echo "Errors:$errors Warnings:$warnings">&2 # This will output the number of errors and warnings on the standard output