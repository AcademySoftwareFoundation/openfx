// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
//
// TEMPLATE: Host-Defined Properties Header
//
// This file serves as the canonical template for hosts that want to define
// custom properties with full compile-time type safety.
//
// HOW TO USE THIS TEMPLATE:
// 1. Copy this file to your host's SDK/include directory
// 2. Rename the namespace from 'myhost' to match your host (e.g., 'nuke', 'resolve', etc.)
// 3. Define your host's custom properties in the PropId enum
// 4. Update the prop_defs array with your property definitions
// 5. Add PropTraits specializations for each property
// 6. Distribute this header with your host's plugin SDK
//
// GENERATION: While this template can be manually edited, we recommend
// generating it from a YAML file using gen-props.py (see Support/Scripts/).
// This ensures consistency and reduces the chance of errors.
//
// This example uses "MyHost" as a fictitious host name to demonstrate the
// pattern without implying endorsement by any actual product.

#pragma once

#include <array>
#include <string_view>
#include <openfx/ofxPropsMetadata.h>
#include <openfx/ofxSpan.h>

namespace myhost {

// ============================================================================
// STEP 1: Define your host's property IDs
// ============================================================================
// Property ID enum for compile-time lookup and type safety.
//
// IMPORTANT: Enum values are ONLY used as array indices into prop_defs below.
// They don't need to avoid collisions with other hosts because:
//   - This is a separate C++ type (myhost::PropId vs other::PropId)
//   - Runtime property access uses STRING NAMES, not these numeric values
//   - No coordination needed between hosts - each namespace is independent
//
// Best practices:
//   - Start at 0 and increment sequentially for simple indexing
//   - Use descriptive names matching your property naming convention
//   - Order doesn't matter, but sequential is easiest to maintain
//
enum class PropId {
  MyHostViewerProcess = 0,  // Example: string property
  MyHostColorConfig = 1,    // Example: string property
  MyHostProjectPath = 2,    // Example: string property
  MyHostNodeName = 3,       // Example: string property
  MyHostNodeColor = 4,      // Example: int[3] property (RGB color)
};

namespace properties {

// ============================================================================
// STEP 2: Define type arrays for each property
// ============================================================================
// Each property needs a constexpr array specifying its supported types.
// Most properties have a single type, but multi-type properties are possible.
//
static constexpr openfx::PropType MyHostViewerProcess_types[] = {openfx::PropType::String};
static constexpr openfx::PropType MyHostColorConfig_types[] = {openfx::PropType::String};
static constexpr openfx::PropType MyHostProjectPath_types[] = {openfx::PropType::String};
static constexpr openfx::PropType MyHostNodeName_types[] = {openfx::PropType::String};
static constexpr openfx::PropType MyHostNodeColor_types[] = {openfx::PropType::Int};

// ============================================================================
// STEP 3: Define property metadata
// ============================================================================
// Property definitions array using openfx::PropDef structure.
//
// Each PropDef entry contains:
//   1. name: Full property name string (use reverse-DNS: "com.yourcompany.yourhost.PropertyName")
//   2. supportedTypes: span of the type array defined above
//   3. dimension: Number of values (1 for scalars, >1 for arrays)
//   4. enumValues: span of valid enum strings (empty for non-enum properties)
//
constexpr openfx::PropDef prop_defs[] = {
  // MyHostViewerProcess - "MyHost viewer process name (color management display transform)"
  { "com.example.myhost.ViewerProcess",
    openfx::span(MyHostViewerProcess_types, 1), 1, openfx::span<const char* const>() },
  // MyHostColorConfig - "Path to MyHost's color management config file"
  { "com.example.myhost.ColorConfig",
    openfx::span(MyHostColorConfig_types, 1), 1, openfx::span<const char* const>() },
  // MyHostProjectPath - "Path to the current MyHost project file"
  { "com.example.myhost.ProjectPath",
    openfx::span(MyHostProjectPath_types, 1), 1, openfx::span<const char* const>() },
  // MyHostNodeName - "Name of the MyHost node containing this effect"
  { "com.example.myhost.NodeName",
    openfx::span(MyHostNodeName_types, 1), 1, openfx::span<const char* const>() },
  // MyHostNodeColor - "RGB color of the node in MyHost's node graph (0-255)"
  { "com.example.myhost.NodeColor",
    openfx::span(MyHostNodeColor_types, 1), 3, openfx::span<const char* const>() },
};

// ============================================================================
// STEP 4: Define PropTraits specializations
// ============================================================================
// PropTraits maps each PropId enum value to its type metadata.
// This enables compile-time type checking in PropertyAccessor.
//
// Each specialization must define:
//   - type: The C++ type (const char* for strings, int for ints, etc.)
//   - is_multitype: false for single-type properties, true for multi-type
//   - dimension: Number of values (must match prop_defs entry)
//   - def: Reference to the corresponding prop_defs entry
//

// Base template (leave undefined - specializations required)
template<PropId id>
struct PropTraits;

// PropTraits specializations for each property
template<>
struct PropTraits<PropId::MyHostViewerProcess> {
  using type = const char*;
  static constexpr bool is_multitype = false;
  static constexpr int dimension = 1;
  static constexpr const openfx::PropDef& def = prop_defs[static_cast<int>(PropId::MyHostViewerProcess)];
};

template<>
struct PropTraits<PropId::MyHostColorConfig> {
  using type = const char*;
  static constexpr bool is_multitype = false;
  static constexpr int dimension = 1;
  static constexpr const openfx::PropDef& def = prop_defs[static_cast<int>(PropId::MyHostColorConfig)];
};

template<>
struct PropTraits<PropId::MyHostProjectPath> {
  using type = const char*;
  static constexpr bool is_multitype = false;
  static constexpr int dimension = 1;
  static constexpr const openfx::PropDef& def = prop_defs[static_cast<int>(PropId::MyHostProjectPath)];
};

template<>
struct PropTraits<PropId::MyHostNodeName> {
  using type = const char*;
  static constexpr bool is_multitype = false;
  static constexpr int dimension = 1;
  static constexpr const openfx::PropDef& def = prop_defs[static_cast<int>(PropId::MyHostNodeName)];
};

template<>
struct PropTraits<PropId::MyHostNodeColor> {
  using type = int;
  static constexpr bool is_multitype = false;
  static constexpr int dimension = 3;
  static constexpr const openfx::PropDef& def = prop_defs[static_cast<int>(PropId::MyHostNodeColor)];
};

}  // namespace properties

// ============================================================================
// STEP 5: Define ADL helper (DO NOT MODIFY)
// ============================================================================
// This function declaration enables Argument-Dependent Lookup (ADL) so that
// openfx::PropertyAccessor can automatically find your PropTraits.
//
// IMPORTANT: This must be declared in the same namespace as your PropId enum
// (i.e., the myhost namespace, NOT the properties sub-namespace).
//
// You should not need to modify this - just copy it as-is.
//
template<PropId id>
properties::PropTraits<id> prop_traits_helper(std::integral_constant<PropId, id>);

}  // namespace myhost
