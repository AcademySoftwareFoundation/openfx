<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->

# OpenFX Community Contributions

This directory contains community-contributed enhancements and tools for OpenFX plugin development.

## Directory Structure

### `dev-tools/`

Enhanced development tools and scripts:

- `build-plugin.sh` - Generalized plugin builder with automatic bundling
- `create-plugin.sh` - Plugin template generator for rapid development
- `setup-env.sh` - Automated development environment setup
- `README.md` - Comprehensive tool documentation

### `plugins/`

Example plugin implementations and development workspace:

- `MyFirstPlugin/` - Complete brightness plugin example with modern patterns
- `CleanFilter/` - Simple filter plugin template
- Plugin workspace with automatic CMake discovery

### `docs/`

Additional development documentation:

- `development-guide.md` - Comprehensive plugin development guide

## Quick Start

### 1. Environment Setup

```bash
# Automated setup (macOS)
./contrib/dev-tools/setup-env.sh
```

### 2. Create New Plugin

```bash
# Generate plugin template (filter, generator, or transition)
./contrib/dev-tools/create-plugin.sh MyEffect -t filter -c "Color" -d "My effect description"
```

### 3. Build Plugin

```bash
# Build and install automatically
./contrib/dev-tools/build-plugin.sh MyEffect MyEffect-support
```

## Integration with Main Repository

These tools are designed to:

- Complement the existing OpenFX build system
- Work alongside standard scripts in `scripts/`
- Maintain compatibility with upstream changes
- Provide enhanced development workflow without modifying core files

## Usage in Development

Add these aliases to your shell profile:

```bash
# OpenFX contrib tools
export OPENFX_ROOT="$HOME/src/openfx"
alias ofx-create="$OPENFX_ROOT/contrib/dev-tools/create-plugin.sh"
alias ofx-build="$OPENFX_ROOT/contrib/dev-tools/build-plugin.sh"
alias ofx-setup="$OPENFX_ROOT/contrib/dev-tools/setup-env.sh"
```

## Contributing

When contributing enhancements:

1. Keep tools in `contrib/` to avoid conflicts with upstream
2. Follow existing OpenFX code patterns and conventions
3. Maintain backward compatibility
4. Document all new features thoroughly

See [CONTRIBUTING.md](../CONTRIBUTING.md) for general contribution guidelines.
