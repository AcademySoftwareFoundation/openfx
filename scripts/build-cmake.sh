#!/bin/bash

set -e

# Build everything with Conan and CMake

BUILDTYPE=Release               # "Release" or "Debug"
VERBOSE=""
GENERATOR=""
USE_OPENCL=""

usage() {
    echo "Usage: $0 [-v|-C] [-G Generator] BUILDTYPE CMAKE-ARGS..."
    echo " -v: verbose"
    echo " -C: Build examples with OpenCL support"
    echo " -G: Use a cmake generator other than the default (e.g. Ninja)"
    echo " BUILDTYPE may be Debug or Release"
    echo " the rest of the args are passed to cmake"
}

# Parse options
while getopts ":vG:C" opt; do
  case ${opt} in
    v )
      VERBOSE="--verbose"
      ;;
    C )
      USE_OPENCL=1
      ;;
    G )
      GENERATOR=${OPTARG}
      GENERATOR_OPTION="-c tools.cmake.cmaketoolchain:generator=${GENERATOR}"
      ;;
    \? )
      echo "Invalid option: $OPTARG" 1>&2
      usage
      exit 1
      ;;
    : )
      echo "Invalid option: $OPTARG requires an argument" 1>&2
      usage
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))

# First positional argument as BUILDTYPE
if [[ $# -gt 0 ]]; then
   BUILDTYPE=$1; shift
fi

ARGS="$@"

echo "Building OpenFX $BUILDTYPE in build/ with ${GENERATOR:-conan platform default generator}, $ARGS"

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

# Install dependencies, set up build dir, and generate build files.
echo === Running conan to install dependencies
[[ $USE_OPENCL ]] && conan_opts="$conan_opts -o use_opencl=True"
conan install ${GENERATOR_OPTION} -s build_type=$BUILDTYPE -pr:b=default --build=missing . $conan_opts

echo === Running cmake
# Generate the build files
[[ $USE_OPENCL ]] && cmake_opts="$cmake_opts -DOFX_SUPPORTS_OPENCLRENDER=TRUE"
cmake --preset ${PRESET_NAME} -DBUILD_EXAMPLE_PLUGINS=TRUE $cmake_opts $ARGS

echo === Building and installing plugins and support libs
cmake --build ${CMAKE_BUILD_DIR} --target install --config $BUILDTYPE --parallel $VERBOSE

set +x
echo "=== Build complete."
echo "  Sample Plugins are in ${CMAKE_BUILD_DIR}/Examples/*/${BUILDTYPE}"
echo "    and installed in the OFX plugin dir for your platform (or where specified by PLUGIN_INSTALLDIR)."
echo "  Plugin support lib and examples are in ${CMAKE_BUILD_DIR}/Support/{Library,Plugins}"
echo "  Host lib is in ${CMAKE_BUILD_DIR}/HostSupport/${BUILDTYPE}"
echo "=== To (re)install the sample plugins to your OFX plugins folder, become root if necessary, and then do:"
echo "  cmake --install ${CMAKE_BUILD_DIR}"
echo "  (pass -DINSTALLDIR=<path> to this script or cmake to install elsewhere than the standard OFX folder)"
