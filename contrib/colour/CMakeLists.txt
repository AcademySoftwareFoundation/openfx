# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause
#
# ofxColourConvert: a small, dependency-free, header-only C++17 library for
# converting RGB triplets between the OFX native colourspaces and ACES2065-1.
# It is a convenience for plug-in and host authors and is NOT a replacement
# for OpenColorIO (see ofxColourConvert.h and README.md).

add_library(OfxColourConvert INTERFACE)
add_library(OpenFX::ColourConvert ALIAS OfxColourConvert)

target_compile_features(OfxColourConvert INTERFACE cxx_std_17)
target_include_directories(OfxColourConvert INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# The header is distributed via the Conan package (conanfile.py) and the CI
# release tarball, matching the rest of the OFX headers; it is not installed by
# CMake (the project's `install` target only installs example plugins).

# The validation tests compare against OpenColorIO (heavy to build) and are
# driven by pcons, which fetches OpenColorIO and GoogleTest via Conan. They are
# off by default; enable with -DOFX_BUILD_COLOUR_TESTS=ON. The CTest entry just
# delegates to pcons (run via uvx, so no install needed); see tests/README.md.
option(OFX_BUILD_COLOUR_TESTS
       "Enable the contrib/colour OpenColorIO validation tests (run via pcons)" OFF)
if(OFX_BUILD_COLOUR_TESTS)
  # A fixture builds the test with pcons (which fetches OpenColorIO + GoogleTest
  # via Conan); the test itself then just runs it. CTest runs the setup first.
  add_test(NAME colour_convert_build
           COMMAND uvx pcons
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  set_tests_properties(colour_convert_build PROPERTIES FIXTURES_SETUP colour_convert)
  add_test(NAME colour_convert_vs_ocio
           COMMAND uvx pcons test
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  set_tests_properties(colour_convert_vs_ocio PROPERTIES FIXTURES_REQUIRED colour_convert)
endif()
