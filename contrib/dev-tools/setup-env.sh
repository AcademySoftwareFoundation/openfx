#!/bin/bash

# OpenFX Development Environment Setup Script
#
# This script automates the initial setup of the OpenFX development environment
# Supports Linux, macOS, and Windows (Git Bash/MSYS2)

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect platform
detect_platform() {
    case "$OSTYPE" in
        linux-gnu*) PLATFORM="linux" ;;
        darwin*)    PLATFORM="macos" ;;
        msys*|cygwin*|mingw*) PLATFORM="windows" ;;
        *)
            log_error "Unsupported platform: $OSTYPE"
            exit 1
            ;;
    esac
    log_success "Detected platform: $PLATFORM"
}

# Check for required tools
check_prerequisites() {
    log_info "Checking prerequisites..."
    
    # Check for Git
    if ! command -v git &> /dev/null; then
        case "$PLATFORM" in
            macos)
                log_error "Git not found. Please install Xcode Command Line Tools:"
                echo "xcode-select --install"
                ;;
            linux)
                log_error "Git not found. Please install git:"
                echo "sudo apt-get install git   # Ubuntu/Debian"
                echo "sudo yum install git      # RHEL/CentOS"
                echo "sudo dnf install git      # Fedora"
                ;;
            windows)
                log_error "Git not found. Please install Git for Windows:"
                echo "https://git-scm.com/download/win"
                ;;
        esac
        exit 1
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        case "$PLATFORM" in
            macos)
                log_warning "CMake not found. Install with: brew install cmake"
                read -p "Install CMake now? (y/n): " -n 1 -r
                echo
                if [[ $REPLY =~ ^[Yy]$ ]]; then
                    if command -v brew &> /dev/null; then
                        brew install cmake
                    else
                        log_error "Homebrew not found. Please install CMake manually."
                        exit 1
                    fi
                else
                    exit 1
                fi
                ;;
            linux)
                log_error "CMake not found. Please install cmake:"
                echo "sudo apt-get install cmake   # Ubuntu/Debian"
                echo "sudo yum install cmake       # RHEL/CentOS"
                echo "sudo dnf install cmake       # Fedora"
                exit 1
                ;;
            windows)
                log_error "CMake not found. Please install CMake:"
                echo "https://cmake.org/download/"
                exit 1
                ;;
        esac
    fi
    
    # Check for Python 3
    if ! command -v python3 &> /dev/null; then
        log_error "Python 3 not found. Please install Python 3.8+"
        exit 1
    fi
    
    log_success "Prerequisites check passed"
}

# Install Conan
setup_conan() {
    log_info "Setting up Conan package manager..."
    
    # Check if Conan is already installed
    if command -v conan &> /dev/null; then
        CONAN_VERSION=$(conan --version | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')
        log_info "Conan $CONAN_VERSION already installed"
    else
        log_info "Installing Conan..."
        python3 -m pip install 'conan>=2.1.0'
        
        # Find Conan installation path
        CONAN_PATH=$(python3 -c "import sys; import os; print(os.path.join(sys.prefix, 'bin'))")
        export PATH="$PATH:$CONAN_PATH"
        
        if ! command -v conan &> /dev/null; then
            # Try alternative paths
            for potential_path in ~/.local/bin ~/Library/Python/*/bin ~/.pyenv/versions/*/bin; do
                if [[ -f "$potential_path/conan" ]]; then
                    export PATH="$PATH:$potential_path"
                    break
                fi
            done
        fi
    fi
    
    # Create default profile
    if [[ ! -f ~/.conan2/profiles/default ]]; then
        log_info "Creating Conan default profile..."
        conan profile detect
        log_success "Conan profile created"
    else
        log_info "Conan profile already exists"
    fi
}

# Get platform-specific plugin paths
get_plugin_paths() {
    case "$PLATFORM" in
        macos)
            USER_PLUGIN_DIR="$HOME/Library/OFX/Plugins"
            DEV_PLUGIN_DIR="$HOME/OFX/Plugins"
            ;;
        linux)
            USER_PLUGIN_DIR="$HOME/.OFX/Plugins"
            DEV_PLUGIN_DIR="$HOME/OFX/Plugins"
            ;;
        windows)
            USER_PLUGIN_DIR="$HOME/AppData/Roaming/OFX/Plugins"
            DEV_PLUGIN_DIR="$HOME/OFX/Plugins"
            ;;
    esac
}

# Setup directories
setup_directories() {
    log_info "Setting up directories..."

    get_plugin_paths

    # Create user and development plugin directories
    mkdir -p "$USER_PLUGIN_DIR"
    mkdir -p "$DEV_PLUGIN_DIR"

    log_success "Plugin directories created:"
    log_info "  User: $USER_PLUGIN_DIR"
    log_info "  Development: $DEV_PLUGIN_DIR"
}

# Build OpenFX framework
build_openfx() {
    log_info "Building OpenFX framework..."
    
    # Ensure we're in the OpenFX root directory
    if [[ ! -f "CMakeLists.txt" ]] || [[ ! -d "include" ]]; then
        log_error "Not in OpenFX root directory. Please run from the openfx directory."
        exit 1
    fi
    
    # Build with examples
    get_plugin_paths
    if ./scripts/build-cmake.sh Release -DPLUGIN_INSTALLDIR="$DEV_PLUGIN_DIR"; then
        log_success "OpenFX framework built successfully"
        log_info "Built plugins are in: $DEV_PLUGIN_DIR"
        log_info "Build output is in: build/Release/"
    else
        log_error "OpenFX build failed"
        exit 1
    fi
}

# Setup environment variables
setup_env_vars() {
    log_info "Setting up environment variables..."
    
    SHELL_RC=""
    if [[ -f ~/.zshrc ]]; then
        SHELL_RC=~/.zshrc
    elif [[ -f ~/.bash_profile ]]; then
        SHELL_RC=~/.bash_profile
    elif [[ -f ~/.bashrc ]]; then
        SHELL_RC=~/.bashrc
    else
        log_warning "No shell RC file found. Creating ~/.zshrc"
        touch ~/.zshrc
        SHELL_RC=~/.zshrc
    fi
    
    # Check if OpenFX environment is already configured
    if grep -q "OPENFX_ROOT" "$SHELL_RC"; then
        log_info "OpenFX environment variables already configured"
    else
        log_info "Adding OpenFX environment variables to $SHELL_RC"
        
        cat >> "$SHELL_RC" << EOF

# OpenFX Development Environment
export OPENFX_ROOT="$(pwd)"
export OFX_PLUGIN_PATH="$USER_PLUGIN_DIR"
export OFX_DEV_PLUGIN_PATH="$DEV_PLUGIN_DIR"

# Add Conan to PATH
export PATH="\$PATH:\$(python3 -c 'import sys; import os; print(os.path.join(sys.prefix, "bin"))')"

# Helper aliases
alias ofx-build="\$OPENFX_ROOT/scripts/build-cmake.sh"
alias ofx-plugins="find \$OFX_PLUGIN_PATH -maxdepth 1 -name '*.ofx.bundle' -type d -exec ls -ld {} + 2>/dev/null || echo 'No plugins found'"
alias ofx-dev-build="\$OPENFX_ROOT/contrib/dev-tools/build-plugin.sh"
alias ofx-create-plugin="\$OPENFX_ROOT/contrib/dev-tools/create-plugin.sh"

EOF
        log_success "Environment variables added to $SHELL_RC"
        log_warning "Please run: source $SHELL_RC"
    fi
}

# Verify installation
verify_setup() {
    log_info "Verifying installation..."
    
    # Check if example plugins were built
    get_plugin_paths
    EXAMPLE_COUNT=$(find "$DEV_PLUGIN_DIR" -name "*.ofx.bundle" -type d 2>/dev/null | wc -l)
    if [[ $EXAMPLE_COUNT -gt 0 ]]; then
        log_success "Found $EXAMPLE_COUNT example plugins in $DEV_PLUGIN_DIR"
    else
        log_warning "No example plugins found. Build may have failed."
    fi
    
    # Check support libraries
    if [[ -f "build/Release/Support/Library/libOfxSupport.a" ]]; then
        log_success "OpenFX Support library built"
    else
        log_warning "OpenFX Support library not found"
    fi
    
    # Check host support
    if [[ -f "build/Release/HostSupport/libOfxHost.a" ]]; then
        log_success "OpenFX Host library built"
    else
        log_warning "OpenFX Host library not found"
    fi
    
    log_success "Setup verification completed"
}

# Print next steps
print_next_steps() {
    log_success "OpenFX development environment setup complete!"
    echo
    echo "Next steps:"
    echo "1. Reload your shell: source ~/.zshrc  (or ~/.bash_profile)"
    echo "2. Create your first plugin: ofx-create-plugin MyPlugin"
    echo "3. Build your plugin: ofx-dev-build MyPlugin"
    get_plugin_paths
    echo "4. Find plugins in: $USER_PLUGIN_DIR"
    echo
    echo "Available commands:"
    echo "  ofx-build            - Build OpenFX framework"
    echo "  ofx-plugins          - List installed plugins"
    echo "  ofx-dev-build        - Build specific plugin"
    echo "  ofx-create-plugin    - Create new plugin from template"
    echo "  ofx-plugins          - List installed plugins"
    echo
    echo "Documentation: OPENFX_DEVELOPMENT_GUIDE.md"
}

# Main execution
main() {
    echo "OpenFX Development Environment Setup"
    echo "====================================="
    echo
    
    detect_platform
    check_prerequisites
    setup_conan
    setup_directories
    build_openfx
    setup_env_vars
    verify_setup
    print_next_steps
}

# Run main function
main "$@"