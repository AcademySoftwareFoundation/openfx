# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

OpenFX is the industry-standard API specification for visual effects plug-ins used in video compositing, editing, and color-grading applications. This repository contains:

- **C Header Files**: Core OpenFX API definitions in `include/` (ofxCore.h, ofxImageEffect.h, etc.)
- **Support Library**: C++ wrapper classes that simplify plugin development (Support/)
- **Host Support Library**: C++ classes for implementing OpenFX hosts/applications (HostSupport/)
- **Example Plugins**: Reference implementations demonstrating various OpenFX features (Examples/)

## Build System

OpenFX uses **CMake** with **Conan 2.1+** for dependency management.

### Common Development Commands

**Build everything** (recommended for development):
```bash
./scripts/build-cmake.sh Release
```

**Build specific plugin** (using contrib tools):
```bash
./contrib/dev-tools/build-plugin.sh <plugin-directory> [target-name] [options]
```

**Build with OpenCL support**:
```bash
./scripts/build-cmake.sh -C Release
```

**Build with different generator**:
```bash
./scripts/build-cmake.sh -G Ninja Release
```

### Plugin-Specific Build Commands

**Create and build custom plugin**:
```bash
# Create plugin from template
./contrib/dev-tools/create-plugin.sh MyColorEffect -p -g

# Build custom plugin
./contrib/dev-tools/build-plugin.sh MyColorEffect MyColorEffect-support
```

**Build with options**:
```bash
# Debug build with verbose output
./contrib/dev-tools/build-plugin.sh MyColorEffect MyColorEffect-support -d -v

# Clean build
./contrib/dev-tools/build-plugin.sh MyColorEffect MyColorEffect-support -c

# Custom bundle name
./contrib/dev-tools/build-plugin.sh MyColorEffect MyColorEffect-support --bundle-name "My Effect"
```

**Manual build process**:
```bash
# Install dependencies
conan install -s build_type=Release -pr:b=default --build=missing .

# Configure cmake
cmake --preset conan-release -DBUILD_EXAMPLE_PLUGINS=TRUE

# Build
cmake --build build/Release --config Release --parallel

# Install plugins locally
cmake --build build/Release --target install --config Release
```

### Key CMake Options

- `BUILD_EXAMPLE_PLUGINS=TRUE/FALSE` - Build example plugins (default: OFF)
- `OFX_SUPPORTS_OPENGLRENDER=TRUE/FALSE` - OpenGL render support (default: ON)
- `OFX_SUPPORTS_OPENCLRENDER=TRUE/FALSE` - OpenCL render support (default: OFF)
- `OFX_SUPPORTS_CUDARENDER=TRUE/FALSE` - CUDA render support (default: OFF)

## Architecture

### Core Components

1. **OpenFX C API** (`include/`):
   - `ofxCore.h` - Core architectural definitions and host-plugin interface
   - `ofxImageEffect.h` - Main image processing API definitions
   - `ofxParam.h`, `ofxProperty.h` - Parameter and property system definitions

2. **Support Library** (`Support/`):
   - C++ wrapper classes around the C API
   - Simplifies plugin development with object-oriented design
   - Located in `Support/Library/` with headers in `Support/include/`

3. **Host Support Library** (`HostSupport/`):
   - C++ framework for building OpenFX host applications
   - Provides plugin caching, property management, and action handling
   - Includes XML-based plugin description caching via expat

4. **Examples** (`Examples/`):
   - Reference plugin implementations
   - Both raw C API examples and Support Library examples
   - Demonstrates contexts: filters, generators, transitions, etc.

### Plugin Installation

**Standard plugin directories:**
- **macOS**: `/Library/OFX/Plugins` (system-wide) or `~/Library/OFX/Plugins` (user)
- **Linux**: `/usr/OFX/Plugins`
- **Windows**: `%COMMONPROGRAMFILES%/OFX/Plugins`

**Custom plugin development directories:**
- Development builds: `~/OFX/Plugins/` (used by build-plugin.sh)
- User installs: `~/Library/OFX/Plugins/` (for Flame/host apps)

### Development Workflow

#### Custom Plugin Development (Recommended)

1. **Create plugin directory** (e.g., `MyFirstPlugin/`) in OpenFX root
2. **Add CMakeLists.txt** defining your plugin target
3. **Develop plugin code** using Support Library classes (inherit from `ImageEffect`, implement `render()`)
4. **Build and test**:
   ```bash
   ./contrib/dev-tools/build-plugin.sh MyFirstPlugin <target-name>
   ```
5. **Plugin is automatically bundled** and installed to `~/Library/OFX/Plugins/`
6. **Test in host application** (Flame, Nuke, etc.)

#### Traditional Development
1. **Plugin Development**: Use Support Library classes (inherit from `ImageEffect`, implement `render()`)
2. **Host Development**: Use HostSupport Library (derive from `Host` class, implement virtual methods)
3. **Testing**: Built plugins are installed to system OFX directory and can be tested with compliant host apps

## Dependencies

- **CMake 3.28+** and **Conan 2.1+** (primary build system)
- **expat** (XML parsing for host support)
- **OpenGL/CUDA/OpenCL** (optional, for GPU rendering support)
- **CImg, spdlog** (via Conan for examples)

## File Organization

- `include/` - Core OpenFX C API headers
- `Support/` - C++ plugin support library
- `HostSupport/` - C++ host application support library  
- `Examples/` - Reference plugin implementations
- `Documentation/` - API documentation (built with Sphinx/ReadTheDocs)
- `contrib/` - **Community contributions and enhanced development tools**
- `scripts/build-cmake.sh` - Primary build script (builds entire framework)

## Community Contributions (`contrib/`)

Our enhanced development tools are organized in the `contrib/` directory to:
- Avoid conflicts with upstream OpenFX repository changes
- Provide clear separation between core and contributed functionality
- Enable easy maintenance and updates

### Enhanced Development Tools

See [contrib/README.md](contrib/README.md) and [contrib/dev-tools/README.md](contrib/dev-tools/README.md) for complete documentation.

**Quick Reference**:
```bash
# Create plugin template
./contrib/dev-tools/create-plugin.sh MyEffect -p -g

# Build and install plugin  
./contrib/dev-tools/build-plugin.sh MyEffect MyEffect-support

# Environment setup (macOS)
./contrib/dev-tools/setup-env.sh
```

When developing plugins, prefer using the Support Library C++ wrapper classes over the raw C API for easier development and better error handling.

## Current Project Modifications

### Enhanced Development Tools (`contrib/dev-tools/`)

1. **build-plugin.sh** - Generalized plugin builder (410+ lines)
   - Works with any plugin directory structure (not hardcoded)
   - Automatic binary discovery and bundling
   - Universal macOS binary support (x86_64 + arm64)
   - Creates proper .ofx.bundle structure automatically

2. **create-plugin.sh** - Plugin template generator (780+ lines)
   - Creates complete plugin scaffolding from templates
   - Multiple plugin types: filter, generator, transition
   - Supports parameters, GPU processing, custom categories
   - Automatically updates build system integration

3. **setup-env.sh** - Environment setup automation
   - Automated macOS development environment setup
   - Conan package manager installation and configuration
   - Plugin directory structure creation

4. **README.md** - Comprehensive tool documentation
   - Usage examples and workflow comparisons
   - Troubleshooting guide and best practices
   - Integration instructions for existing projects

### Example Plugin Implementation (`contrib/plugins/`)

- **MyFirstPlugin/** - Complete brightness plugin example
  - `brightness.cpp` - Full OpenFX plugin implementation with Support Library
  - `CMakeLists.txt` - Build configuration
  - Demonstrates proper OFX patterns and multi-bit-depth support

### Documentation (`contrib/docs/`)

- **development-guide.md** - Comprehensive plugin development guide
- **README.md** - Overview of contrib directory structure
- All files include proper SPDX license headers following OpenFX conventions

### Bug Fixes

- **Support/Plugins/GPUGain/MetalKernel.mm** - Fixed Metal framework deprecation warning
  - Uses `mathMode` on macOS 15.0+ with backward compatibility
  - Proper compiler pragma to suppress warnings on older versions

### Build System Integration

- **CMakeLists.txt** - Added MyFirstPlugin to main build
- All new components integrate with existing Conan/CMake build system
- Maintains compatibility with CI/CD pipelines

## Development Status

**Working Features:**
- ✅ Generalized plugin building and bundling
- ✅ Plugin template generation with multiple types  
- ✅ Automatic environment setup
- ✅ Cross-architecture builds (universal binaries)
- ✅ Integration with existing build system
- ✅ Comprehensive documentation

**Testing:**
- ✅ Built and tested MyFirstBrightness-support plugin
- ✅ Successful installation to macOS OFX directories
- ✅ No build warnings or errors
- ✅ Universal binary generation confirmed

This enhanced development environment significantly streamlines OpenFX plugin development while maintaining full compatibility with the existing OpenFX ecosystem and repository conventions.