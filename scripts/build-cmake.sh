#!/bin/bash

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
GENERATOR=${1:-"$DEFAULT_GENERATOR"}      # "Ninja", "Unix Makefiles", "Xcode"

echo "Building OpenFX $BUILDTYPE in $BUILDDIR with $GENERATOR"

set -x
conan install -if $BUILDDIR -of $BUILDDIR -s build_type="$BUILDTYPE" .

cmake -S. -B$BUILDDIR \
      -G"$GENERATOR" \
      -DCMAKE_BUILD_TYPE=$BUILDTYPE \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=1

if [[ $GENERATOR = Ninja ]]; then
    ninja -C $BUILDDIR
elif [[ $GENERATOR = 'Unix Makefiles' ]]; then
    make -C $BUILDDIR -j8
elif [[ $GENERATOR = 'Xcode' ]]; then
    (cd Build; xcodebuild)
else
    echo "ERROR: Unknown build system $GENERATOR"
    exit 1
fi

echo "Build complete; libs are in $BUILDDIR/lib"
