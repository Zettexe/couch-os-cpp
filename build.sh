#!/bin/bash

set -e

# Remove the "build" directory if it exists
if [ -d "tmp" ]; then
  rm -rf tmp
fi

mkdir tmp

# Set a trap to execute the following block of code if the clang++ command fails
trap 'if [ -d "tmp" ]; then rm -rf tmp; fi' ERR

# Compile the code
clang++ -pthread -std=c++17 -Wall -Wextra -g ./musicos/*.cpp ./musicos/**/*.cpp -o tmp/couch-os -I./include -L/usr/lib/x86_64-linux-gnu/debug -ldpp -lcurl -lcurlpp -lfmt -lspdlog -lboost_regex -Wl,-rpath,/lib/x86_64-linux-gnu/debug

# Remove the "build" directory if it exists
if [ -d "build" ]; then
  rm -rf build
fi

# Create the "build" directory
mkdir build

cp -r tmp/* build

rm -rf tmp

cp config.json build
cp -r data build/data
