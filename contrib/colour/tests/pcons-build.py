#!/usr/bin/env python3
# /// script
# requires-python = ">=3.11"
# dependencies = ["pcons>=0.20.1"]
# ///
# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause
"""pcons build for the ofxColourConvert.h unit tests.

Validates contrib/colour/ofxColourConvert.h against OpenColorIO (the reference
implementation), using GoogleTest as the harness. Both dependencies are
fetched with Conan.

This is a developer convenience; the canonical build path is CMake/CTest
(see ../CMakeLists.txt and the OFX_BUILD_COLOUR_TESTS option).

Usage::

    cd contrib/colour/tests
    pcons                 # conan install (first run) + build the test program
    pcons test            # build and run the tests
    ninja test            # same, via the generated ninja file
    pcons test --list     # show the tests without running them
"""

import os
from pathlib import Path

from pcons import Generator, Project, find_c_toolchain, get_variant
from pcons.configure.config import Configure
from pcons.packages.finders import ConanFinder

VARIANT = get_variant("release")

project_dir = Path(os.environ.get("PCONS_SOURCE_DIR", Path(__file__).parent))
build_dir = Path(os.environ.get("PCONS_BUILD_DIR", project_dir / "build"))
openfx_include = project_dir.parent            # contrib/colour, where ofxColourConvert.h lives

# --- Toolchain (configured once, cached) -----------------------------------
config = Configure(build_dir=build_dir)
toolchain = find_c_toolchain()
if not config.get("configured") or os.environ.get("PCONS_RECONFIGURE"):
    toolchain.configure(config)
    config.set("configured", True)
    config.save()

project = Project("ofx_colour_convert_tests", root_dir=project_dir, build_dir=build_dir)

# --- Conan dependencies (OpenColorIO + GoogleTest) -------------------------
conan = ConanFinder(
    config,
    conanfile=project_dir / "conanfile.txt",
    output_folder=build_dir / "conan",
)
conan.sync_profile(toolchain, build_type=VARIANT.capitalize(), cppstd="17")
conan.install()
project.add_package_finder(conan)

ocio = project.find_package("OpenColorIO")
gtest = project.find_package("gtest")

# --- Environment ------------------------------------------------------------
env = project.Environment(toolchain=toolchain)
env.set_variant(VARIANT)
env.cxx.flags.append("-std=c++17")
env.link.cmd = env.cxx.cmd  # link with the C++ driver

# --- Test program -----------------------------------------------------------
test_prog = project.Program(
    "test_colour_convert",
    env,
    sources=[project_dir / "test_colour_convert.cpp"],
)
test_prog.private.include_dirs.append(openfx_include)
test_prog.link(ocio)
test_prog.link(gtest)

project.Default(test_prog)

# One gtest binary; pass/fail is per-binary (gtest prints per-case detail).
project.Test("colour_convert.vs_ocio", test_prog, labels=["unit"])

Generator().generate(project)
print(f"Generated {build_dir}")
