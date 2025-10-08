<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->

# OpenFX Plugin Development Guide

This guide provides comprehensive instructions for developing OpenFX plugins, from environment setup to advanced plugin creation techniques.

## Quick Start

### 1. Environment Setup

For automated setup on macOS:

```bash
# Run the setup script (installs dependencies, sets up directories)
./scripts/setup-env.sh
```

For manual setup, see the [install.md](install.md) instructions.

### 2. Create Your First Plugin

#### Method 1: Using Template Generator (Recommended)

```bash
# Create a basic filter plugin
./scripts/create-plugin.sh MyColorCorrector

# Create with parameters and GPU support  
./scripts/create-plugin.sh MyColorCorrector -p -g -c "Color"

# Create a generator plugin
./scripts/create-plugin.sh NoiseGenerator -t generator -p
```

#### Method 2: Manual Plugin Creation

Create plugin directory and source files manually, following the patterns in [Examples/](Examples/).

### 3. Build and Test

```bash
# Build your plugin
./scripts/build-plugin.sh MyColorCorrector MyColorCorrector-support

# Plugin is automatically installed to ~/Library/OFX/Plugins/ for testing
```

## Plugin Development Workflow

### Recommended Development Cycle

1. **Create Plugin**: `./scripts/create-plugin.sh PluginName [options]`
2. **Edit Source**: Modify the generated `.cpp` file with your effect logic  
3. **Build**: `./scripts/build-plugin.sh PluginName TargetName`
4. **Test**: Launch your OpenFX host (Flame, Nuke, etc.) and test the plugin
5. **Iterate**: Repeat steps 2-4 as needed

### Plugin Types Supported

- **Filter**: Process input images (most common)
- **Generator**: Create images from scratch  
- **Transition**: Blend between two images

## Advanced Plugin Development

### Plugin Architecture

OpenFX plugins consist of:
1. **Plugin Class**: Main effect implementation (inherits from `OFX::ImageEffect`)
2. **Processor Classes**: Handle pixel-level processing with templates
3. **Factory Class**: Describes plugin capabilities and creates instances
4. **Registration**: Exports plugin to OpenFX host

### Supported Features

- **Bit Depths**: 8-bit (UByte), 16-bit (UShort), 32-bit (Float)
- **Color Formats**: RGB, RGBA, Alpha-only
- **Processing**: Multi-threaded, tiled rendering
- **Parameters**: Various parameter types (Double, Integer, Boolean, Choice, etc.)

### Directory Structure

```
YourPlugin/
├── yourplugin.cpp        # Main plugin source
├── CMakeLists.txt        # Build configuration  
└── README.md            # Plugin documentation
```

## Build System Details

### Framework Build

```bash
# Build entire OpenFX framework
./scripts/build-cmake.sh Release
```

### Plugin-Specific Build  

```bash
# Build individual plugin with bundling and installation
./scripts/build-plugin.sh <plugin-dir> <target> [options]
```

Build script options:
- `-d, --debug`: Debug build mode
- `-c, --clean`: Clean build before building  
- `-i, --install`: Install to system directory (requires sudo)
- `-v, --verbose`: Verbose build output

## Plugin Installation

### Installation Paths

**macOS:**
- User: `~/Library/OFX/Plugins/` (recommended for development)
- System: `/Library/OFX/Plugins/` (requires admin privileges)

**Linux:**  
- System: `/usr/OFX/Plugins/`

**Windows:**
- System: `%COMMONPROGRAMFILES%/OFX/Plugins`

### Bundle Structure

```
MyPlugin.ofx.bundle/
└── Contents/
    └── MacOS/
        └── MyPlugin.ofx    # Universal binary (x86_64 + arm64)
```

## Development Environment

### Shell Aliases

Add to your shell profile (`~/.zshrc` or `~/.bash_profile`):

```bash
# OpenFX development aliases  
export OPENFX_ROOT="$HOME/src/openfx"
alias ofx-build="$OPENFX_ROOT/scripts/build-cmake.sh"
alias ofx-build-plugin="$OPENFX_ROOT/scripts/build-plugin.sh"
alias ofx-create-plugin="$OPENFX_ROOT/scripts/create-plugin.sh"
alias ofx-setup="$OPENFX_ROOT/scripts/setup-env.sh"
alias ofx-plugins="ls -la ~/Library/OFX/Plugins/"
```

## Troubleshooting

### Common Build Issues

- **Conan not found**: Check PATH includes conan binary location
- **Permission denied**: Use user directories instead of system paths  
- **Compilation errors**: Ensure all headers are included and syntax is correct
- **Plugin not visible**: Check bundle structure and installation paths

### Debug Information

```bash
# Enable verbose build output
./scripts/build-plugin.sh MyPlugin target -v

# Check plugin architecture
file ~/Library/OFX/Plugins/MyPlugin.ofx.bundle/Contents/MacOS/MyPlugin.ofx

# Expected output: Universal binary with x86_64 and arm64 architectures
```

## Code Style Guidelines

- Use the Support Library C++ wrapper classes over raw C API
- Follow existing code patterns in [Examples/](Examples/)
- Implement proper error handling and validation
- Use templates for different bit depths and pixel formats
- Enable multi-threading with `OFX::ImageProcessor` base class

## Testing

Test your plugins in OpenFX host applications:
- Autodesk Flame  
- Foundry Nuke
- Blackmagic DaVinci Resolve
- Other OFX-compatible applications

## Contributing

When contributing plugin development improvements:
1. Follow the existing code style and patterns
2. Update documentation as needed
3. Test across different host applications  
4. Submit pull requests following [CONTRIBUTING.md](CONTRIBUTING.md)

For more information about OpenFX:
- [OpenFX Documentation](https://openfx.readthedocs.io/en/latest)
- [Programming Guide By Example](https://openfx.readthedocs.io/en/latest/Guide)
- [OpenFX Reference](https://openfx.readthedocs.io/en/latest/Reference)