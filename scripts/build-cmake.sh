#!/bin/bash

set -e

# Build everything with Conan and CMake

BUILDDIR=Build
BUILDTYPE=Release               # "Release" or "Debug"

if command -v ninja; then
    DEFAULT_GENERATOR="Ninja"
elif command -v xcodebuild; then
    DEFAULT_GENERATOR="Xcode"
else
    DEFAULT_GENERATOR="Unix Makefiles"
fi
if [[ $1 = "-v" ]]; then
    VERBOSE="--verbose"; shift
fi
GENERATOR=${1:-"$DEFAULT_GENERATOR"}      # "Ninja", "Unix Makefiles", "Xcode"

echo "Building OpenFX $BUILDTYPE in $BUILDDIR with $GENERATOR"

set -x
# Install dependencies and set up build dir
conan install -if $BUILDDIR -of $BUILDDIR -s build_type="$BUILDTYPE" .

# Generate the build files
cmake -S. -B$BUILDDIR \
      -G"$GENERATOR" \
      -DBUILD_EXAMPLE_PLUGINS=TRUE \
      -DCMAKE_BUILD_TYPE=$BUILDTYPE \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=1

# Do the build
cmake --build $BUILDDIR $VERBOSE

# install (or you could just do "cmake --build $BUILDDIR --target install")
cmake --install $BUILDDIR
