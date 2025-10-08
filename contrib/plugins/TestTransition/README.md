# TestTransition Plugin

Test transition plugin

## What to modify

1. **Algorithm**: In `testtransition.cpp`, look for the `MODIFY_HERE` comment in the `multiThreadProcessImages` method
2. **Parameters**: Look for `ADD_PARAMETERS` comments to add your own parameters
3. **Plugin Info**: Update the plugin identifier and other metadata

## Building

```bash
./contrib/dev-tools/build-plugin.sh contrib/plugins/TestTransition TestTransition-support
```

## Installation

The plugin will be automatically installed to your development plugin directory when built.
