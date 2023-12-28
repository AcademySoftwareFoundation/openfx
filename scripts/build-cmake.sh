#!/bin/bash

set -e

# Build everything with Conan and CMake

BUILDTYPE=Release               # "Release" or "Debug"

if [[ $1 = "-v" ]]; then
    VERBOSE="--verbose"; shift
fi

if [[ $1 = "-G" ]]; then
    GENERATOR=$1; shift
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
    PRESET_BASE_NAME=$(tr 'A-Z' 'a-z' <<< $BUILDTYPE)
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
echo === Running conan to install dependencies
conan install ${GENERATOR_OPTION} -s build_type=$BUILDTYPE -pr:b=default --build=missing .

echo === Running cmake
# Generate the build files
cmake --preset ${PRESET_NAME} -DBUILD_EXAMPLE_PLUGINS=TRUE "$@"

echo === Building plugins and support libs
cmake --build ${CMAKE_BUILD_DIR} --config $BUILDTYPE $VERBOSE

set +x
echo "=== Build complete."
echo "  Sample Plugins are in ${CMAKE_BUILD_DIR}/Examples/*/${BUILDTYPE}"
echo "  Plugin support lib and examples are in ${CMAKE_BUILD_DIR}/Support/{Library,Plugins}"
echo "  Host lib is in ${CMAKE_BUILD_DIR}/HostSupport/${BUILDTYPE}"
echo "=== To install the sample plugins to your OFX plugins folder, become root and then do:"
echo "  cmake --install ${CMAKE_BUILD_DIR} --config $BUILDTYPE"
