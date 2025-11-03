// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
//
// Example: Host-Defined Properties for MyHost
//
// This file demonstrates how a host (MyHost) would define custom properties
// that plugins can access with the same type safety as standard OpenFX properties.
//
// In practice, this file would be generated from myhost-props.yml using gen-props.py
// (or a similar tool provided by the host).
//
// NOTE: This is for demonstration purposes only. MyHost is a fictitious host
// used to show the extensibility pattern without implying endorsement by any
// actual product.

#pragma once

#include <array>
#include <string_view>
#include <openfx/ofxPropsMetadata.h>

namespace myhost {

// Property ID enum for compile-time lookup and type safety.
// NOTE: Enum values are used only for indexing into prop_defs array below.
// They don't need to avoid collisions with other namespaces since this is
// a separate C++ type. Starting at 10000 is arbitrary but helps distinguish
// from standard OpenFX properties when debugging.
enum class PropId {
  MyHostViewerProcess = 10000,
  MyHostColorConfig = 10001,
  MyHostProjectPath = 10002,
  MyHostNodeName = 10003,
  MyHostNodeColor = 10004,
};

namespace properties {

// Property definitions using the same structure as OpenFX properties
constexpr openfx::PropDef prop_defs[] = {
  // MyHostViewerProcess
  {
    .name = "com.example.myhost.ViewerProcess",
    .supportedTypes = std::array{openfx::PropType::String},
    .dimension = 1,
    .enumValues = openfx::ofxSpan<const char* const>{},
    .description = "MyHost viewer process name (color management display transform)",
  },
  // MyHostColorConfig
  {
    .name = "com.example.myhost.ColorConfig",
    .supportedTypes = std::array{openfx::PropType::String},
    .dimension = 1,
    .enumValues = openfx::ofxSpan<const char* const>{},
    .description = "Path to MyHost's color management config file",
  },
  // MyHostProjectPath
  {
    .name = "com.example.myhost.ProjectPath",
    .supportedTypes = std::array{openfx::PropType::String},
    .dimension = 1,
    .enumValues = openfx::ofxSpan<const char* const>{},
    .description = "Path to the current MyHost project file",
  },
  // MyHostNodeName
  {
    .name = "com.example.myhost.NodeName",
    .supportedTypes = std::array{openfx::PropType::String},
    .dimension = 1,
    .enumValues = openfx::ofxSpan<const char* const>{},
    .description = "Name of the MyHost node containing this effect",
  },
  // MyHostNodeColor
  {
    .name = "com.example.myhost.NodeColor",
    .supportedTypes = std::array{openfx::PropType::Int},
    .dimension = 3,
    .enumValues = openfx::ofxSpan<const char* const>{},
    .description = "RGB color of the node in MyHost's node graph (0-255)",
  },
};

// Base template struct for property traits
template<PropId id>
struct PropTraits;

// Helper macro to define PropTraits specializations
#define MYHOST_DEFINE_PROP_TRAITS(id, _type, _is_multitype) \
template<> \
struct PropTraits<PropId::id> { \
    using type = _type; \
    static constexpr bool is_multitype = _is_multitype; \
    static constexpr int dimension = prop_defs[static_cast<int>(PropId::id) - 10000].dimension; \
    static constexpr const openfx::PropDef& def = prop_defs[static_cast<int>(PropId::id) - 10000]; \
}

// Define traits for each property
MYHOST_DEFINE_PROP_TRAITS(MyHostViewerProcess, const char*, false);
MYHOST_DEFINE_PROP_TRAITS(MyHostColorConfig, const char*, false);
MYHOST_DEFINE_PROP_TRAITS(MyHostProjectPath, const char*, false);
MYHOST_DEFINE_PROP_TRAITS(MyHostNodeName, const char*, false);
MYHOST_DEFINE_PROP_TRAITS(MyHostNodeColor, int, false);

#undef MYHOST_DEFINE_PROP_TRAITS

}  // namespace properties

// ADL helper function for PropTraits lookup.
// This enables openfx::PropertyAccessor to find myhost::properties::PropTraits
// via argument-dependent lookup.
template<PropId id>
properties::PropTraits<id> prop_traits_helper(std::integral_constant<PropId, id>);

}  // namespace myhost
