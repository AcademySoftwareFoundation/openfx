# Host-Extensible Property System

This directory contains examples showing how OpenFX hosts can define custom properties that plugins can access with full type safety, without modifying OpenFX core code.

## Overview

The OpenFX C++ property system now supports **host-extensible properties** through C++17 `auto` template parameters and argument-dependent lookup (ADL). This allows hosts like MyHost, Resolve, Flame, etc. to:

- Define custom properties in their own namespace
- Provide the same type safety as standard OpenFX properties
- Maintain zero coupling with OpenFX core code
- Give plugins optional access to host-specific features

## How It Works

### Architecture

```
┌─────────────────────────────────────────────────────────────┐
│  OpenFX Core (openfx namespace)                             │
│  - PropId enum (standard properties)                        │
│  - PropTraits<PropId> (metadata)                            │
│  - PropertyAccessor with template<auto id>                  │
│  - prop_traits_helper() for ADL                             │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ No coupling - hosts extend independently
                              │
┌─────────────────────────────────────────────────────────────┐
│  Host Extension (e.g., myhost namespace)                      │
│  - PropId enum (host properties)                            │
│  - PropTraits<PropId> (metadata)                            │
│  - prop_traits_helper() for ADL                             │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ Plugins include host headers
                              │
┌─────────────────────────────────────────────────────────────┐
│  Plugin Code                                                │
│  - props.get<openfx::PropId::OfxPropLabel>()               │
│  - props.get<myhost::PropId::MyHostViewerProcess>()            │
└─────────────────────────────────────────────────────────────┘
```

### Key Mechanism: ADL (Argument-Dependent Lookup)

The `PropertyAccessor` uses `template<auto id>` to accept any enum type:

```cpp
template<auto id>
auto get() {
  using Traits = PropTraits_t<id>;  // Uses ADL to find correct namespace
  // ...
}
```

The `PropTraits_t` type alias uses decltype + ADL:

```cpp
template<auto id>
using PropTraits_t = decltype(
  prop_traits_helper(std::integral_constant<decltype(id), id>{})
);
```

Each namespace provides a `prop_traits_helper` function for its `PropId` enum:

```cpp
// In openfx namespace:
template<PropId id>
properties::PropTraits<id> prop_traits_helper(std::integral_constant<PropId, id>);

// In myhost namespace:
template<PropId id>
properties::PropTraits<id> prop_traits_helper(std::integral_constant<PropId, id>);
```

When the compiler sees `prop_traits_helper(std::integral_constant<myhost::PropId, ...>{})`, ADL finds the `myhost::prop_traits_helper` function because the argument type (`std::integral_constant<myhost::PropId, ...>`) is associated with the `nuke` namespace.

## For Hosts: Defining Custom Properties

### Step 1: Define Properties in YAML (Recommended)

Create a YAML file following the OpenFX property schema:

```yaml
# myhost-props.yml
properties:
  MyHostCustomProperty:
    name: "com.mycompany.myhost.CustomProperty"
    type: string
    dimension: 1
    description: "Description of the custom property"
    valid_for:
      - "Effect Descriptor"
      - "Effect Instance"
```

**Naming Convention**: Use reverse-DNS notation:
- `com.company.product.PropertyName`
- Examples: `com.foundry.myhost.ViewerProcess`, `com.blackmagic.resolve.Timeline`

### Step 2: Generate Metadata Header

Use the OpenFX `gen-props.py` script (or create your own generator):

```bash
./scripts/gen-props.py \
  --input myhost-props.yml \
  --output myhost/myhostPropsMetadata.h \
  --namespace myhost
```

Or create the header manually (see `nuke/nukePropsMetadata.h` for an example).

### Step 3: Distribute Header to Plugin Developers

Provide the generated header file to plugin developers in your SDK:

```
MyHostSDK/
  include/
    myhost/
      myhostPropsMetadata.h
```

## For Plugin Developers: Using Host Properties

### Step 1: Include Host Header

```cpp
#include <openfx/ofxPropsAccess.h>     // OpenFX core
#include <myhost/myhostPropsMetadata.h> // Host-specific
```

### Step 2: Use Properties with Type Safety

```cpp
using namespace openfx;

void describe(OfxImageEffectHandle effect, const SuiteContainer& suites) {
  PropertyAccessor props(effect, suites);

  // Standard OpenFX property
  props.set<PropId::OfxPropLabel>("My Effect");

  // Host-specific property (fully qualified)
  try {
    auto value = props.get<myhost::PropId::MyHostCustomProperty>(0, false);
    Logger::info("Host property value: {}", value);
  } catch (const PropertyNotFoundException&) {
    // Not running in MyHost, or property not supported
  }
}
```

### Step 3: Handle Missing Properties Gracefully

Host properties may not exist when running in other hosts:

```cpp
// Method 1: Use error_if_missing=false
auto value = props.get<myhost::PropId::CustomProp>(0, false);
if (value) {
  // Use the value
}

// Method 2: Use try/catch
try {
  auto value = props.get<myhost::PropId::CustomProp>();
  // Use the value
} catch (const PropertyNotFoundException&) {
  // Handle missing property
}
```

## Examples

### MyHost Example

See `nuke/` directory for a complete example showing:
- YAML property definition (`nuke-props.yml`)
- Generated metadata header (`nukePropsMetadata.h`)
- Example plugin usage (`example-plugin.cpp`)

Properties defined:
- `MyHostViewerProcess`: OCIO viewer process name
- `MyHostOCIOConfig`: Path to OCIO config
- `MyHostScriptPath`: Current MyHost script path
- `MyHostNodeName`: Name of the containing node
- `MyHostNodeColor`: RGB color of the node (3 ints)

## Benefits

### For Hosts
- **Zero Coupling**: No need to modify OpenFX core
- **Version Control**: Version your property definitions independently
- **Documentation**: Generate docs from YAML files
- **Flexibility**: Add properties without coordinating with OpenFX releases

### For Plugin Developers
- **Type Safety**: Compile-time checking for all properties
- **IDE Support**: Autocomplete and documentation tooltips
- **Discoverability**: Browse available host properties with IDE
- **Optional**: Ignore host extensions if not needed
- **Backward Compatible**: Existing code continues to work

## Technical Details

### Requirements
- C++17 or later (for `template<auto>` and structured bindings)
- Standard OpenFX C++ headers (`openfx-cpp`)

### Compiler Compatibility
Tested with:
- GCC 7+ (C++17)
- Clang 5+ (C++17)
- MSVC 2017+ (C++17)

### Performance
- **Zero runtime overhead**: All property lookups are template-based
- **Compile-time**: Type checking happens at compile time
- **Same performance** as direct property suite calls

### Limitations
- Host properties must use reverse-DNS naming (`com.company.product.*`) to avoid string name collisions
- PropId enum values only need to be unique within each host's own namespace (used for array indexing)
- ADL requires proper namespace organization (each host defines `prop_traits_helper` in their namespace)

## Migration Guide

### Existing Code
No changes needed! Existing code using `PropId` continues to work:

```cpp
// This still works exactly as before
props.get<PropId::OfxPropLabel>();
props.set<PropId::OfxImageEffectPropComponents>("OfxImageComponentRGBA");
```

### New Code
Take advantage of host extensions:

```cpp
// Mix standard and host properties seamlessly
props.set<openfx::PropId::OfxPropLabel>("My Effect")
     .set<myhost::PropId::MyHostViewerProcess>("sRGB")
     .setAll<myhost::PropId::MyHostNodeColor>({128, 200, 255});
```

## FAQ

**Q: Do I need to modify OpenFX core to add host properties?**
A: No! That's the whole point. Hosts define properties in their own namespace.

**Q: What if two hosts define the same property name?**
A: Use reverse-DNS naming (`com.company.product.*`) to avoid collisions. Even if names collide, they're in different C++ namespaces.

**Q: Can I still use `getRaw()` for dynamic properties?**
A: Yes! The "escape hatch" methods (`getRaw`, `setRaw`) still work for truly dynamic property access.

**Q: Do host properties work with older OpenFX versions?**
A: Host properties require the C++17-based property system (OpenFX 1.5+). For older versions, use `getRaw()`.

**Q: How do I discover what host properties are available?**
A: Include the host's metadata header and use IDE autocomplete on `hostname::PropId::` to see all available properties.

**Q: Can hosts add new properties without breaking plugins?**
A: Yes! New properties are opt-in. Plugins that don't know about them can simply not access them.

## Support

For questions or issues:
- OpenFX discussion forum: https://github.com/AcademySoftwareFoundation/openfx/discussions
- OpenFX issue tracker: https://github.com/AcademySoftwareFoundation/openfx/issues

## See Also

- [OpenFX Property System Documentation](../../Documentation/properties.md)
- [PropertyAccessor API Reference](../../openfx-cpp/include/openfx/ofxPropsAccess.h)
- [Property Generation Script](../../scripts/gen-props.py)
