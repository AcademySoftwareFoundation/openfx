<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Building OpenFX: Libs and Plugins

OpenFX itself is only a set of C header files, the ones in
[include](include). This repo also includes the C++ support lib,
giving a C++ API on top of the basic C, and two sets of example
plugins; one set with the support lib, one set using the raw C API.
There is also a host support lib for use when creating a new OpenFX
host. These instructions show how to build the support libs and all
the plugins, and install them into your plugin folder.

## Prerequisites

OpenFX uses [cmake](https://cmake.org) and [conan](https://conan.io)
to build. Other dependencies are fetched by conan. The build requires
Conan 2.1.0 or later, and CMake 3.28 or later.

Install cmake:
- Mac: `brew install cmake`
- Windows: `choco install cmake`
- Linux: `apt install cmake`

Install conan (version >= 2.1.0 recommended) using pip (and python3)
- `python3 -mpip install 'conan>=2.1.0'`

# Standard Builds

To build and install everything use [scripts/build-cmake.sh](scripts/build-cmake.sh). 

This should build with the default Visual Studio on Windows and
Makefiles with gcc on Linux and Mac, and install the plugins into the
standard location.

On Windows, you'll need either mingw or git bash to run the shell
script, but all builds are done using the MSVC compiler.


The `build-cmake.sh` script takes a few args:

```
  [-v|-C] [-G Generator] BUILDTYPE CMAKE-ARGS...
  -v: be verbose
  -C: Build examples with OpenCL support
  -G: Use a cmake generator other than the default (e.g. Ninja)
  BUILDTYPE may be Debug or Release (default: Release)
  The rest of the args are passed to cmake.
```

# Manual builds with CMake

The `scripts/build-cmake.sh` script does something like this, if you want to do it manually:

```sh
% cd $TOPLEVEL # where CMakeLists.txt is located
# Install dependencies from conanfile.py
% conan install -s build_type=Release -pr:b=default --build=missing .
# Configure cmake to build into Build folder, and build example plugins
% cmake --preset conan-release -DBUILD_EXAMPLE_PLUGINS=TRUE
# Do the build
% cmake --build build/Release --config Release --parallel
# Install the plugins locally (may require root privs)
% cmake --build build/Release --target install --config Release
```

The exact preset names and build dirs may vary depending on your OS. Check the build script for details.

Here are some useful parameters you can pass to `cmake` with `-D<parameter>=<value>`:

- `BUILD_EXAMPLE_PLUGINS`: enable/disable building of example plugins (default: OFF)
- `OFX_SUPPORTS_OPENGLRENDER`: enable/disable OpenGL render support (default: ON)
- `OFX_SUPPORTS_OPENCLRENDER`: enable/disable OpenGL render support (default: OFF)
- `OFX_SUPPORTS_CUDARENDER`: enable/disable OpenGL render support (default: OFF)

# CI build script
You may also want to look at the [CI build script](.github/workflows/build.yml)
which builds on a wide variety of OSes. It uses ninja for fast builds
and accounts for various special cases, so if you can't get your build
going, check in there.


_Linux dependencies you may need (but conan should provide these):_
```sh
sudo apt install pkg-config libgl-dev
```



