#!/bin/bash

set -e

# Build everything with Conan and CMake

BUILDTYPE=Release               # "Release" or "Debug"

if [[ $1 = "-v" ]]; then
    VERBOSE="--verbose"; shift
fi

if [[ -n ${1:-} ]]; then
    GENERATOR=$1
    GENERATOR_OPTION="-c tools.cmake.cmaketoolchain:generator=${GENERATOR}"
fi

echo "Building OpenFX $BUILDTYPE in build/ with ${GENERATOR:-conan platform default generator}"


CONAN_VERSION=$(conan -v | sed -e 's/Conan version //g')
CONAN_MAJOR_VERSION=${CONAN_VERSION:0:1}

PRESET_NAME=
PRESET_BASE_NAME=
CMAKE_BUILD_DIR=build
OS=$(uname -s)

if [[ ${OS:0:5} == "MINGW" && ${GENERATOR,,} != "ninja" ]]; then
    PRESET_BASE_NAME=default
else
    PRESET_BASE_NAME=${BUILDTYPE,,}
    CMAKE_BUILD_DIR=${CMAKE_BUILD_DIR}/${BUILDTYPE}
fi

if [[ ${CONAN_MAJOR_VERSION} == "1" ]]; then
    PRESET_NAME=${PRESET_BASE_NAME}
elif [[ ${CONAN_MAJOR_VERSION} == "2" ]]; then
    PRESET_NAME=conan-${PRESET_BASE_NAME}
else
    echo "Unexpected conan version ${CONAN_VERSION}"
    exit 1
fi

set -x

# Install dependencies, set up build dir, and generate build files.
conan install ${GENERATOR_OPTION} -s build_type=$BUILDTYPE --build=missing .

# Generate the build files
cmake --preset ${PRESET_NAME} -DBUILD_EXAMPLE_PLUGINS=TRUE

# Do the build
cmake --build ${CMAKE_BUILD_DIR} --config $BUILDTYPE $VERBOSE

# install (or you could just do "cmake --build $BUILDDIR --config $BUILDTYPE --target install")
#cmake --install build/ --config $BUILDTYPE