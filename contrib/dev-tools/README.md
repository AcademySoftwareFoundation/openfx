<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->

# OpenFX Development Tools

Enhanced development tools for OpenFX plugin development.

## Available Tools

### create-plugin.sh - Plugin Template Generator

Generate complete plugin templates with customizable options.

**Usage:**
```bash
./contrib/dev-tools/create-plugin.sh <plugin-name> [options]
```

**Options:**
- `-t, --type TYPE` - Plugin type: filter, generator, transition (default: filter)
- `-c, --category CAT` - Plugin category (default: "Color")
- `-d, --description DESC` - Plugin description
- `-a, --author AUTHOR` - Plugin author (default: "Your Name")
- `-h, --help` - Show help

**Examples:**
```bash
# Create a simple filter plugin
./contrib/dev-tools/create-plugin.sh MyColorEffect

# Create a generator plugin with custom settings
./contrib/dev-tools/create-plugin.sh NoiseGenerator -t generator -c "Generators" -a "John Doe"

# Create a transition plugin
./contrib/dev-tools/create-plugin.sh CrossFade -t transition -d "Custom transition effect"
```

### build-plugin.sh - Plugin Builder

Build, bundle, and install individual OpenFX plugins.

**Usage:**
```bash
./contrib/dev-tools/build-plugin.sh <plugin-directory> [target-name] [options]
```

**Options:**
- `-d, --debug` - Debug build
- `-c, --clean` - Clean build (removes build directory first)
- `-v, --verbose` - Verbose output
- `--bundle-name NAME` - Custom bundle name
- `--install-dir DIR` - Custom installation directory

**Examples:**
```bash
# Build plugin with automatic detection
./contrib/dev-tools/build-plugin.sh contrib/plugins/MyPlugin

# Build with specific target name
./contrib/dev-tools/build-plugin.sh contrib/plugins/MyPlugin MyPlugin-support

# Debug build with verbose output
./contrib/dev-tools/build-plugin.sh contrib/plugins/MyPlugin MyPlugin-support -d -v
```

### setup-env.sh - Environment Setup

Automated setup of OpenFX development environment (macOS only).

**Usage:**
```bash
./contrib/dev-tools/setup-env.sh
```

**Features:**
- Installs Conan package manager
- Sets up plugin directories
- Configures development environment
- Builds OpenFX framework

## Quick Start Workflow

1. **Setup environment** (first time only):
   ```bash
   ./contrib/dev-tools/setup-env.sh
   ```

2. **Create a new plugin**:
   ```bash
   ./contrib/dev-tools/create-plugin.sh MyEffect -t filter -c "Color" -d "My custom effect"
   ```

3. **Build and install**:
   ```bash
   ./contrib/dev-tools/build-plugin.sh contrib/plugins/MyEffect MyEffect-support
   ```

4. **Test in your OpenFX host** (Flame, Nuke, etc.)

## Directory Structure

Created plugins follow this structure:
```
contrib/plugins/MyEffect/
├── myeffect.cpp          # Plugin source code
├── CMakeLists.txt        # Build configuration
└── README.md             # Plugin documentation
```

Built plugins are installed to:
- **Development**: `~/OFX/Plugins/`
- **User**: `~/Library/OFX/Plugins/` (for host applications)

## Integration

All plugins in `contrib/plugins/` are automatically discovered by the build system. No manual CMakeLists.txt editing required.

## Troubleshooting

### Common Issues

**Build fails with missing dependencies:**
```bash
# Run setup script to install dependencies
./contrib/dev-tools/setup-env.sh
```

**Plugin not found in host application:**
```bash
# Check installation directories
ls ~/Library/OFX/Plugins/
ls ~/OFX/Plugins/
```

**CMake configuration errors:**
```bash
# Clean build and try again
./contrib/dev-tools/build-plugin.sh MyPlugin MyPlugin-support --clean
```

For more help, see the main [OpenFX documentation](../../Documentation/) or [CLAUDE.md](../../CLAUDE.md).