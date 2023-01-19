#!/bin/bash

# find musicos -name '*.cpp' -or -name '*.h' | xargs iwyu -std=c++17 -Iinclude -I/usr/lib/clang/14.0.0/include
# find include/musicos -name '*.cpp' -or -name '*.h' | xargs iwyu -std=c++17 -Iinclude -I/usr/lib/clang/14.0.0/include

# Set the directory to the first argument passed to the script
dir="musicos"

# Loop through all the files in the directory
for file in $dir/*
do
  # Check if the file is a C++ source file
  if [[ $file == *.cpp ]] || [[ $file == *.h ]] || [[ $file == *.c ]]
  then
    # Run iwyu on the file
    iwyu -std=c++17 -Iinclude -I/usr/lib/clang/14.0.0/include $file
  fi
done

# Set the directory to the first argument passed to the script
dir="include/musicos"

# Loop through all the files in the directory
for file in $dir/*
do
  # Check if the file is a C++ source file
  if [[ $file == *.cpp ]] || [[ $file == *.h ]] || [[ $file == *.c ]]
  then
    # Run iwyu on the file
    iwyu -std=c++17 -Iinclude -I/usr/lib/clang/14.0.0/include $file
  fi
done
