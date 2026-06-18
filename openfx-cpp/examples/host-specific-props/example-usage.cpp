// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
//
// Example: Using Host-Defined Properties
//
// NOTE: This file contains example CODE SNIPPETS for documentation purposes.
// It is NOT a complete, buildable OFX plugin. These examples demonstrate how
// a plugin would access both standard OpenFX properties and host-defined
// properties (in this case, MyHost properties) with full type safety.
//
// The myhost/myhostPropsMetadata.h header IS compile-tested as part of the
// TestProps example build, ensuring the example code is syntactically correct.
//
// For complete, buildable plugin examples, see the Examples/ directory at
// the project root (e.g., TestProps, Basic, etc.).

#include <openfx/ofxPropsAccess.h>
#include <openfx/ofxLog.h>
#include "myhost/myhostPropsMetadata.h"

using namespace openfx;

// Example plugin describe function
void describeMyHostAwarePlugin(OfxImageEffectHandle effect, const SuiteContainer& suites) {
  PropertyAccessor props(effect, suites);

  // =========================================================================
  // Standard OpenFX Properties
  // =========================================================================
  // These work exactly as before - no changes needed to existing code

  props.set<PropId::OfxPropLabel>("MyHost-Aware Effect")
       .set<PropId::OfxPropVersionLabel>("1.0")
       .setAll<PropId::OfxPropVersion>({1, 0, 0});

  // =========================================================================
  // Host-Defined Properties
  // =========================================================================
  // Now we can also access MyHost-specific properties with the same type safety!

  // Try to get MyHost-specific properties
  // Use error_if_missing=false since these properties only exist in MyHost
  try {
    // Get the viewer process - this tells us the color management display transform
    auto viewerProcess = props.get<myhost::PropId::MyHostViewerProcess>(0, false);
    if (viewerProcess) {
      Logger::info("Running in MyHost with viewer process: {}", viewerProcess);
    }

    // Get the project path
    auto projectPath = props.get<myhost::PropId::MyHostProjectPath>(0, false);
    if (projectPath) {
      Logger::info("MyHost project path: {}", projectPath);
    }

    // Get the node name
    auto nodeName = props.get<myhost::PropId::MyHostNodeName>(0, false);
    if (nodeName) {
      Logger::info("This effect is in MyHost node: {}", nodeName);
    }

    // Set a custom node color (RGB 0-255)
    // This demonstrates setting host properties
    props.setAll<myhost::PropId::MyHostNodeColor>({128, 200, 255});
    Logger::info("Set MyHost node color to light blue");

  } catch (const PropertyNotFoundException& e) {
    // Not running in MyHost, or MyHost doesn't support these properties
    Logger::info("MyHost properties not available - probably not running in MyHost");
  }
}

// Example showing compile-time type safety
void demonstrateTypeSafety(OfxImageEffectHandle effect, const SuiteContainer& suites) {
  PropertyAccessor props(effect, suites);

  // ✅ This compiles - MyHostViewerProcess is a string property
  auto viewerProcess = props.get<myhost::PropId::MyHostViewerProcess>();

  // ✅ This compiles - MyHostNodeColor is an int property with dimension 3
  auto colors = props.getAll<myhost::PropId::MyHostNodeColor>();
  // colors is std::array<int, 3> because dimension is known at compile time

  // ✅ This compiles - setting an int array property
  props.setAll<myhost::PropId::MyHostNodeColor>({255, 128, 0});

  // ❌ This would NOT compile - type mismatch!
  // auto wrong = props.get<myhost::PropId::MyHostNodeColor, const char*>();
  //              ^^^^^^^^^ Compile error: MyHostNodeColor is int, not string

  // ✅ Standard OpenFX properties still work exactly as before
  props.set<PropId::OfxPropLabel>("My Effect");
  auto label = props.get<PropId::OfxPropLabel>();
}

// Example showing namespace flexibility
void demonstrateNamespaces(OfxImageEffectHandle effect, const SuiteContainer& suites) {
  PropertyAccessor props(effect, suites);

  // Method 1: Fully qualified names (explicit and clear)
  auto viewer1 = props.get<myhost::PropId::MyHostViewerProcess>(0, false);
  auto label1 = props.get<openfx::PropId::OfxPropLabel>();

  // Method 2: Using namespace for convenience
  {
    using namespace myhost;
    auto viewer2 = props.get<PropId::MyHostViewerProcess>(0, false);
    // Note: This would be ambiguous if both namespaces are in scope!
    // So we still need openfx:: for standard properties
    auto label2 = props.get<openfx::PropId::OfxPropLabel>();
  }

  // Method 3: Mix and match as needed
  props.set<openfx::PropId::OfxPropLabel>("Mixed Namespaces")
       .setAll<myhost::PropId::MyHostNodeColor>({200, 100, 50});
}

// Example showing the "escape hatch" still works
void demonstrateDynamicAccess(OfxPropertySetHandle propSet, const OfxPropertySuiteV1* propSuite) {
  PropertyAccessor props(propSet, propSuite);

  // For truly dynamic properties, the getRaw/setRaw methods still work
  auto unknownProp = props.getRaw<const char*>("com.somehost.unknownProperty", 0, false);

  // But when possible, use the type-safe methods with PropId enums
  auto typeSafe = props.get<myhost::PropId::MyHostViewerProcess>(0, false);
}

/*
 * KEY BENEFITS of this approach:
 *
 * 1. **Zero Coupling**: MyHost (the host) doesn't modify OpenFX core code
 *    - myhostPropsMetadata.h is distributed by MyHost, not OpenFX
 *    - Plugins include it only if they want MyHost-specific features
 *
 * 2. **Type Safety**: Compile-time checking for host properties
 *    - Wrong type? Compile error.
 *    - Wrong property ID? Compile error.
 *    - IDE autocomplete shows available properties
 *
 * 3. **Clean Syntax**: Same API for standard and host properties
 *    - props.get<openfx::PropId::OfxPropLabel>()
 *    - props.get<myhost::PropId::MyHostViewerProcess>()
 *
 * 4. **Backward Compatible**: Existing code continues to work
 *    - No changes needed to existing plugins
 *    - New features are opt-in
 *
 * 5. **Discoverability**: IDE autocomplete works
 *    - Type "myhost::PropId::" and see all available properties
 *    - Hover over a property to see its documentation
 *
 * 6. **Optional**: Plugins can ignore host extensions
 *    - Don't include myhostPropsMetadata.h? No problem.
 *    - Use getRaw() for dynamic access? Still works.
 */
