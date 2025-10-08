#!/bin/bash

# Simple OpenFX Plugin Creator
# Uses working example files with simple sed replacements

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OPENFX_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

function usage() {
    cat << EOF
Usage: $0 <plugin-name> [options]

Simple plugin creator that copies working examples and does basic replacements.

Arguments:
  plugin-name         Name of the plugin (e.g., "ColorCorrector")

Options:
  -t, --type TYPE     Plugin type [filter|generator|transition] (default: filter)
  -c, --category CAT  Plugin category (default: "Color")
  -d, --description DESC  Plugin description
  -a, --author AUTHOR Plugin author (default: "Your Name")
  -h, --help          Show this help

Examples:
  $0 ColorCorrector
  $0 NoiseGenerator -t generator -c "Generators"
  $0 CrossFade -t transition

The created plugin will have clear comments showing you what to modify.
EOF
}

# Default values
PLUGIN_TYPE="filter"
PLUGIN_CATEGORY="Color"
PLUGIN_AUTHOR="Your Name"
PLUGIN_DESCRIPTION=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--type)
            PLUGIN_TYPE="$2"
            shift 2
            ;;
        -c|--category)
            PLUGIN_CATEGORY="$2"
            shift 2
            ;;
        -d|--description)
            PLUGIN_DESCRIPTION="$2"
            shift 2
            ;;
        -a|--author)
            PLUGIN_AUTHOR="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -*)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
        *)
            if [[ -z "$PLUGIN_NAME" ]]; then
                PLUGIN_NAME="$1"
            else
                echo "Too many arguments"
                usage
                exit 1
            fi
            shift
            ;;
    esac
done

if [[ -z "$PLUGIN_NAME" ]]; then
    echo "Error: Plugin name is required"
    usage
    exit 1
fi

# Validate plugin type
if [[ "$PLUGIN_TYPE" != "filter" && "$PLUGIN_TYPE" != "generator" && "$PLUGIN_TYPE" != "transition" ]]; then
    echo "Error: Plugin type must be filter, generator, or transition"
    exit 1
fi

# Generate names
PLUGIN_NAME_LOWER=$(echo "$PLUGIN_NAME" | tr '[:upper:]' '[:lower:]')
PLUGIN_SOURCE_FILE="${PLUGIN_NAME_LOWER}.cpp"
PLUGIN_TARGET_NAME="${PLUGIN_NAME}-support"
PLUGIN_IDENTIFIER="com.reepost.${PLUGIN_NAME_LOWER}"

if [[ -z "$PLUGIN_DESCRIPTION" ]]; then
    PLUGIN_DESCRIPTION="$PLUGIN_NAME OpenFX plugin"
fi

# Create plugin directory in contrib/plugins
PLUGIN_DIR="$OPENFX_ROOT/contrib/plugins/$PLUGIN_NAME"

echo "OpenFX Simple Plugin Creator"
echo "============================="
echo "Creating: $PLUGIN_NAME ($PLUGIN_TYPE)"
echo ""

if [[ -d "$PLUGIN_DIR" ]]; then
    echo "Error: Directory $PLUGIN_DIR already exists"
    exit 1
fi

mkdir -p "$PLUGIN_DIR"

# Copy and modify the appropriate source file based on plugin type (using official OpenFX examples)
echo "Creating plugin source code..."
case "$PLUGIN_TYPE" in
    filter)
        # Use our clean filter template based on OpenFX examples
        cp "$SCRIPT_DIR/templates/filter.cpp" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
        ;;
    generator)
        # Use our clean generator template based on OpenFX examples
        cp "$SCRIPT_DIR/templates/generator.cpp" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
        ;;
    transition)
        # Use our clean transition template based on OpenFX examples
        cp "$SCRIPT_DIR/templates/transition.cpp" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
        ;;
    *)
        echo "Error: Unknown plugin type: $PLUGIN_TYPE"
        exit 1
        ;;
esac

# Customize the template with user-provided values
echo "Customizing plugin template..."
sed -i '' "s|com\.example\.\([a-z]*\)plugin|$PLUGIN_IDENTIFIER|g" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
sed -i '' "s|\"Filter\"|\"$PLUGIN_NAME\"|g" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
sed -i '' "s|\"Generator\"|\"$PLUGIN_NAME\"|g" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
sed -i '' "s|\"Transition\"|\"$PLUGIN_NAME\"|g" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
sed -i '' "s|setPluginGrouping(\"[^\"]*\")|setPluginGrouping(\"$PLUGIN_CATEGORY\")|g" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"

# Add author information to the header comment
sed -i '' "3a\\
// Author: $PLUGIN_AUTHOR\\
" "$PLUGIN_DIR/$PLUGIN_SOURCE_FILE"

# Copy and modify CMakeLists.txt (use MyFirstPlugin as template for all types)
echo "Creating CMakeLists.txt..."
cp "$OPENFX_ROOT/contrib/plugins/MyFirstPlugin/CMakeLists.txt" "$PLUGIN_DIR/CMakeLists.txt"

# Replace the specific content from MyFirstPlugin CMakeLists.txt
sed -i '' "s|MyFirstBrightness-support|$PLUGIN_TARGET_NAME|g" "$PLUGIN_DIR/CMakeLists.txt"
sed -i '' "s|brightness.cpp|$PLUGIN_SOURCE_FILE|g" "$PLUGIN_DIR/CMakeLists.txt"

# Create simple README
cat > "$PLUGIN_DIR/README.md" << EOF
# $PLUGIN_NAME Plugin

$PLUGIN_DESCRIPTION

## What to modify

1. **Algorithm**: In \`$PLUGIN_SOURCE_FILE\`, look for the \`MODIFY_HERE\` comment in the \`multiThreadProcessImages\` method
2. **Parameters**: Look for \`ADD_PARAMETERS\` comments to add your own parameters
3. **Plugin Info**: Update the plugin identifier and other metadata

## Building

\`\`\`bash
./contrib/dev-tools/build-plugin.sh contrib/plugins/$PLUGIN_NAME $PLUGIN_TARGET_NAME
\`\`\`

## Installation

The plugin will be automatically installed to your development plugin directory when built.
EOF

echo "✅ SUCCESS: Plugin template created!"
echo ""
echo "Files created:"
echo "  - $PLUGIN_DIR/$PLUGIN_SOURCE_FILE"
echo "  - $PLUGIN_DIR/CMakeLists.txt"
echo "  - $PLUGIN_DIR/README.md"
echo ""
echo "Next steps:"
echo "1. Edit $PLUGIN_DIR/$PLUGIN_SOURCE_FILE - this is a real working $PLUGIN_TYPE plugin you can modify"
echo "2. Build: ./contrib/dev-tools/build-plugin.sh contrib/plugins/$PLUGIN_NAME $PLUGIN_TARGET_NAME"
echo "3. Test in your OpenFX host application"
echo ""
echo "The generated code is a working $PLUGIN_TYPE plugin that you can use as-is or modify."