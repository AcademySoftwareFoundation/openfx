// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <math.h>
#include <ofxCore.h>

#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

#include "ofxExceptions.h"
#include "ofxLog.h"
#include "ofxPropsMetadata.h"
#include "ofxSuites.h"

/**
 * OpenFX Property Accessor System - Usage Examples

// Basic property access with type safety
void basicPropertyAccess(OfxPropertySetHandle handle, OfxPropertySuiteV1*
propHost) {
    // Create a property accessor
    openfx::PropertyAccessor props(handle, propHost);

    // Get a string property - type is deduced from PropTraits
    const char* colorspace = props.get<PropId::ImageClipPropColourspace>();

    // Get a boolean property - returned as int (0 or 1)
    int isConnected = props.get<PropId::ImageClipPropConnected>();

    // Set a property value (type checked at compile time)
    props.set<PropId::ImageClipPropConnected>(1);

    // Set all of a property's values (type checked at compile time)
    // Supports any container with size() and operator[], or initializer-list
    props.setAll<PropId::OfxPropVersion>({1, 0, 0});

    // Get and set properties by index (for multi-dimensional properties)
    const char* fieldMode = props.get<PropId::ImageClipPropFieldExtraction>(0);
    props.set<PropId::ImageClipPropFieldOrder>("OfxImageFieldLower", 0);

    // Chaining set operations
    props.set<PropId::PropertyA>(valueA)
         .set<PropId::PropertyB>(valueB)
         .set<PropId::PropertyC>(valueC);

    // Get dimension of a property
    int dimension = props.getDimension<PropId::ImageClipPropFieldExtraction>();
}

// Working with multi-type properties
void multiTypePropertyAccess(OfxPropertySetHandle handle, OfxPropertySuiteV1*
propHost) { openfx::PropertyAccessor props(handle, propHost);

    // For multi-type properties, explicitly specify the type
    double maxValueDouble = props.get<PropId::ParamPropDisplayMax, double>();

    // You can also get the same property as a different type
    int maxValueInt = props.get<PropId::ParamPropDisplayMax, int>();

    // Setting values with explicit types
    props.set<PropId::ParamPropDisplayMax, double>(10.5);
    props.set<PropId::ParamPropDisplayMax, int>(10);

    // Attempting to use an incompatible type would cause a compile error:
    // const char* str = props.get<PropId::ParamPropDisplayMax, const char*>();
// Error!
}


// Using the "escape hatch" for dynamic property access
void dynamicPropertyAccess(OfxPropertySetHandle handle, OfxPropertySuiteV1*
propHost) { openfx::PropertyAccessor props(handle, propHost);

    // Get and set properties by name without compile-time checking
    const char* pluginDefined = props.getRaw<const
char*>("PluginDefinedProperty"); props.setRaw<int>("DynamicIntProperty", 42);

    // Get dimension of a dynamic property
    int dim = props.getDimensionRaw("DynamicArrayProperty");

    // When property names come from external sources
    const char* propName = getPropertyNameFromPlugin();
    double value = props.getRaw<double>(propName);
}

// Working with enum properties
void enumPropertyHandling(OfxPropertySetHandle handle, OfxPropertySuiteV1*
propHost) { openfx::PropertyAccessor props(handle, propHost);

    // Get an enum property value
    const char* fieldExtraction =
props.get<PropId::ImageClipPropFieldExtraction>();

    // Set an enum property using a valid value
    props.set<PropId::ImageClipPropFieldExtraction>("OfxImageFieldLower");

    // Access enum values directly using the EnumValue helper
    const char* noneField =
openfx::EnumValue<PropId::ImageClipPropFieldExtraction>::get(0); const char*
lowerField = openfx::EnumValue<PropId::ImageClipPropFieldExtraction>::get(1);

    // Check if a value is valid for an enum
    bool isValid =
openfx::EnumValue<PropId::ImageClipPropFieldExtraction>::isValid("OfxImageFieldUpper");

    // Get total number of enum values
    size_t enumCount =
openfx::EnumValue<PropId::ImageClipPropFieldExtraction>::size();
}

// End of examples
*/

// Status checking private macros

#define _OPENFX_CHECK_THROW(expr, msg)                                                         \
  do {                                                                                         \
    auto &&_status = (expr);                                                                   \
    if (_status != kOfxStatOK) {                                                               \
      openfx::Logger::error("{} in {}:{}: {}", ofxStatusToString(_status), __FILE__, __LINE__, \
                            msg);                                                              \
      throw openfx::PropertyNotFoundException(_status);                                        \
    }                                                                                          \
  } while (0)

#define _OPENFX_CHECK_WARN(expr, msg)                                                         \
  do {                                                                                        \
    auto &&_status = (expr);                                                                  \
    if (_status != kOfxStatOK) {                                                              \
      openfx::Logger::warn("{} in {}:{}: {}", ofxStatusToString(_status), __FILE__, __LINE__, \
                           msg);                                                              \
    }                                                                                         \
  } while (0)

#define _OPENFX_CHECK(expr, msg, error_if_fail) \
  do {                                          \
    if (error_if_fail) {                        \
      _OPENFX_CHECK_THROW(expr, msg);           \
    } else {                                    \
      _OPENFX_CHECK_WARN(expr, msg);            \
    }                                           \
  } while (0)

namespace openfx {

// ============================================================================
// Host-Extensible Property System Support
// ============================================================================
//
// This section enables hosts to define their own custom properties in their
// own namespaces while maintaining the same type safety as standard OpenFX
// properties. This is achieved through C++17 auto template parameters and
// argument-dependent lookup (ADL).
//
// How it works:
//   1. Hosts define their own PropId enum in their namespace (e.g., nuke::PropId)
//   2. Hosts define PropTraits specializations in their namespace
//   3. Hosts define a prop_traits_helper function for ADL lookup
//   4. PropertyAccessor uses template<auto id> to accept any enum type
//   5. PropTraits_t<id> uses ADL to find the correct PropTraits in the right namespace
//
// Example host usage (in host's header file):
//   namespace myhost {
//     enum class PropId { CustomProperty, ... };
//     namespace properties {
//       template<PropId id> struct PropTraits;
//       template<> struct PropTraits<PropId::CustomProperty> { ... };
//     }
//     // Enable ADL lookup
//     template<PropId id>
//     properties::PropTraits<id> prop_traits_helper(std::integral_constant<PropId, id>);
//   }
//
// Plugin usage:
//   props.get<openfx::PropId::OfxPropLabel>();  // Standard property
//   props.get<myhost::PropId::CustomProperty>();  // Host property
//
// ============================================================================

// Forward declare the ADL helper for standard OpenFX properties.
// This function is never actually called - it's only used for type deduction via decltype.
// It must be in the same namespace as PropId (openfx) for ADL to work.
template<PropId id>
properties::PropTraits<id> prop_traits_helper(std::integral_constant<PropId, id>);

// PropTraits_t: Type alias that uses ADL to find the correct PropTraits
// for any PropId enum (openfx::PropId or host-defined).
// The decltype+ADL pattern allows each namespace to provide its own prop_traits_helper.
template<auto id>
using PropTraits_t = decltype(prop_traits_helper(std::integral_constant<decltype(id), id>{}));

// Type-mapping helper to infer C++ type from PropType
template <PropType propType>
struct PropTypeToNative {
  using type = void;  // Default case, should never be used directly
};

// Specializations for each property type
template <>
struct PropTypeToNative<PropType::Int> {
  using type = int;
};
template <>
struct PropTypeToNative<PropType::Double> {
  using type = double;
};
template <>
struct PropTypeToNative<PropType::Enum> {
  using type = const char *;
};
template <>
struct PropTypeToNative<PropType::Bool> {
  using type = int;
};
template <>
struct PropTypeToNative<PropType::String> {
  using type = const char *;
};
template <>
struct PropTypeToNative<PropType::Pointer> {
  using type = void *;
};

// Helper to access enum property values with strong typing.
// Works with any PropId enum (standard OpenFX or host-defined).
template <auto id>
struct EnumValue {
  using Traits = PropTraits_t<id>;

  static constexpr const char *get(size_t index) {
    static_assert(index < Traits::def.enumValues.size(),
                  "Property enum index out of range");
    return Traits::def.enumValues[index];
  }

  static constexpr size_t size() {
    return Traits::def.enumValues.size();
  }

  static constexpr bool isValid(const char *value) {
    for (auto val : Traits::def.enumValues) {
      if (std::strcmp(val, value) == 0)
        return true;
    }
    return false;
  }
};

// Type-safe property accessor for any props of a given prop set
class PropertyAccessor {
 public:
  // Basic constructor
  explicit PropertyAccessor(OfxPropertySetHandle propset, const OfxPropertySuiteV1 *prop_suite)
      : propset_(propset), propSuite_(prop_suite) {
    if (!propSuite_) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing property suite");
    }
  }

  // Constructor taking a prop set and a suites container, for simplicity
  explicit PropertyAccessor(OfxPropertySetHandle propset, const SuiteContainer &suites)
      : propset_(propset) {
    propSuite_ = suites.get<OfxPropertySuiteV1>();
    if (!propSuite_) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing property suite");
    }
  }

  // Convenience constructors for ImageEffect -- get effect property set & construct accessor
  explicit PropertyAccessor(OfxImageEffectHandle effect, const OfxImageEffectSuiteV1 *effects_suite,
                            const OfxPropertySuiteV1 *prop_suite)
      : propset_(nullptr), propSuite_(prop_suite) {
    if (!propSuite_) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing property suite");
    }
    if (!effects_suite) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing effects suite");
    }
    effects_suite->getPropertySet(effect, &propset_);
    assert(propset_);
  }

  explicit PropertyAccessor(OfxImageEffectHandle effect, const SuiteContainer &suites)
      : propset_(nullptr) {
    propSuite_ = suites.get<OfxPropertySuiteV1>();
    if (!propSuite_) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing property suite");
    }
    auto effects_suite = suites.get<OfxImageEffectSuiteV1>();
    if (!effects_suite) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing effects suite");
    }
    effects_suite->getPropertySet(effect, &propset_);
    assert(propset_);
  }

  // Convenience constructors for Interact -- get effect property set & construct accessor
  explicit PropertyAccessor(OfxInteractHandle interact, const OfxInteractSuiteV1 *interact_suite,
                            const OfxPropertySuiteV1 *prop_suite)
      : propset_(nullptr), propSuite_(prop_suite) {
    if (!interact_suite) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing interact suite");
    }
    interact_suite->interactGetPropertySet(interact, &propset_);
    assert(propset_);
  }
  explicit PropertyAccessor(OfxInteractHandle interact, const SuiteContainer &suites)
      : propset_(nullptr) {
    propSuite_ = suites.get<OfxPropertySuiteV1>();
    auto interact_suite = suites.get<OfxInteractSuiteV1>();
    if (!interact_suite) {
      throw SuiteNotFoundException(kOfxStatErrMissingHostFeature,
                                   "PropertyAccessor: missing interact suite");
    }
    interact_suite->interactGetPropertySet(interact, &propset_);
    assert(propset_);
  }

  // Get property value using PropId (compile-time type checking).
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id, typename = std::enable_if_t<!PropTraits_t<id>::is_multitype>>
  typename PropTraits_t<id>::type get(int index = 0, bool error_if_missing = true) const {
    using Traits = PropTraits_t<id>;

    static_assert(!Traits::is_multitype,
                  "This property supports multiple types. Use get<PropId, T>() instead.");

    using T = typename Traits::type;

    assert(propset_ != nullptr);
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
      int value = 0;
      _OPENFX_CHECK(propSuite_->propGetInt(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
      double value = 0;
      _OPENFX_CHECK(propSuite_->propGetDouble(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, const char *>) {
      char *value = nullptr;
      _OPENFX_CHECK(propSuite_->propGetString(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, void *>) {
      void *value = nullptr;
      _OPENFX_CHECK(propSuite_->propGetPointer(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else {
      static_assert(always_false<T>::value, "Unsupported property value type");
    }
  }

  // Get multi-type property value (requires explicit type).
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id, typename T,
            typename = std::enable_if_t<PropTraits_t<id>::is_multitype>>
  T get(int index = 0, bool error_if_missing = true) const {
    using Traits = PropTraits_t<id>;

    // Check if T is compatible with any of the supported PropTypes
    constexpr bool isValidType = [&]() {
      for (const auto &type : Traits::def.supportedTypes) {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
          if (type == PropType::Int || type == PropType::Bool || type == PropType::Enum)
            return true;
        } else if constexpr (std::is_same_v<T, double>) {
          if (type == PropType::Double)
            return true;
        } else if constexpr (std::is_same_v<T, const char *>) {
          if (type == PropType::String || type == PropType::Enum)
            return true;
        } else if constexpr (std::is_same_v<T, void *>) {
          if (type == PropType::Pointer)
            return true;
        }
      }
      return false;
    }();

    static_assert(isValidType, "Requested type is not compatible with this property");
    assert(propset_ != nullptr);

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
      int value = 0;
      _OPENFX_CHECK(propSuite_->propGetInt(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, double>) {
      double value = NAN;
      _OPENFX_CHECK(propSuite_->propGetDouble(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, const char *>) {
      char *value = nullptr;
      _OPENFX_CHECK(propSuite_->propGetString(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, void *>) {
      void *value = nullptr;
      _OPENFX_CHECK(propSuite_->propGetPointer(propset_, Traits::def.name, index, &value),
                    Traits::def.name, error_if_missing);
      return value;
    } else {
      static_assert(always_false<T>::value, "Unsupported property value type");
    }
  }

  // Set property value using PropId (compile-time type checking).
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id>
  PropertyAccessor &set(typename PropTraits_t<id>::type value, int index = 0,
                        bool error_if_missing = true) {
    using Traits = PropTraits_t<id>;

    static_assert(!Traits::is_multitype,
                  "This property supports multiple types. Use set<PropId, T>() instead.");

    if constexpr (Traits::def.supportedTypes[0] == PropType::Enum) {
      bool isValidEnumValue = openfx::EnumValue<id>::isValid(value);
      assert(isValidEnumValue);
    }
    assert(propset_ != nullptr);

    using T = typename Traits::type;

    if constexpr (std::is_same_v<T, bool>) {  // allow bool -> int
      _OPENFX_CHECK(propSuite_->propSetInt(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, int>) {
      _OPENFX_CHECK(propSuite_->propSetInt(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, float>) {  // allow float -> double
      _OPENFX_CHECK(propSuite_->propSetDouble(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, double>) {
      _OPENFX_CHECK(propSuite_->propSetDouble(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, const char *>) {
      _OPENFX_CHECK(propSuite_->propSetString(propset_, Traits::def.name, index, value),
                    openfx::format("{}={}", Traits::def.name, value), error_if_missing);
    } else if constexpr (std::is_same_v<T, void *>) {
      _OPENFX_CHECK(propSuite_->propSetPointer(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else {
      static_assert(always_false<T>::value, "Invalid value type when setting property");
    }
    return *this;
  }

  // Set multi-type property value (requires explicit type).
  // Should only be used for multitype props (SFINAE).
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id, typename T,
            typename = std::enable_if_t<PropTraits_t<id>::is_multitype>>
  PropertyAccessor &set(T value, int index = 0, bool error_if_missing = true) {
    using Traits = PropTraits_t<id>;

    // Check if T is compatible with any of the supported PropTypes
    constexpr bool isValidType = [&]() {
      for (const auto &type : Traits::def.supportedTypes) {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
          if (type == PropType::Int || type == PropType::Bool)
            return true;
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
          if (type == PropType::Double)
            return true;
        } else if constexpr (std::is_same_v<T, const char *>) {
          if (type == PropType::String)  // no Enums here -- there shouldn't be
                                         // any multi-type enums
            return true;
        } else if constexpr (std::is_same_v<T, void *>) {
          if (type == PropType::Pointer)
            return true;
        }
      }
      return false;
    }();

    static_assert(isValidType, "Requested type is not compatible with this property");
    assert(propset_ != nullptr);

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
      _OPENFX_CHECK(propSuite_->propSetInt(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
      _OPENFX_CHECK(propSuite_->propSetDouble(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, const char *>) {
      _OPENFX_CHECK(propSuite_->propSetString(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else if constexpr (std::is_same_v<T, void *>) {
      _OPENFX_CHECK(propSuite_->propSetPointer(propset_, Traits::def.name, index, value),
                    Traits::def.name, error_if_missing);
    } else {
      static_assert(always_false<T>::value, "Invalid value type when setting property");
    }
    return *this;
  }

  // Get all values of a property (for single-type properties).
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id>
  auto getAll(bool error_if_missing = true) const {
    static_assert(
        !PropTraits_t<id>::is_multitype,
        "This property supports multiple types. Use getAllTyped<PropId, ElementType>() instead.");
    assert(propset_ != nullptr);

    using ValueType = typename PropTraits_t<id>::type;

    // If dimension is known at compile time, use std::array for stack allocation
    if constexpr (PropTraits_t<id>::def.dimension > 0) {
      constexpr int dim = PropTraits_t<id>::def.dimension;
      std::array<ValueType, dim> values;

      for (int i = 0; i < dim; ++i) {
        values[i] = get<id>(i, error_if_missing);
      }

      return values;
    } else {
      // Otherwise use std::vector for dynamic sizing
      int dimension = getDimension<id>();
      std::vector<ValueType> values;
      values.reserve(dimension);

      for (int i = 0; i < dimension; ++i) {
        values.push_back(get<id>(i, error_if_missing));
      }

      return values;
    }
  }

  // Get all values of a multi-type property - require explicit ElementType.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id, typename ElementType>
  auto getAllTyped(bool error_if_missing = true) const {
    static_assert(PropTraits_t<id>::is_multitype,
                  "This property does not support multiple types. Use getAll<PropId>() instead.");
    assert(propset_ != nullptr);

    // If dimension is known at compile time, use std::array for stack allocation
    if constexpr (PropTraits_t<id>::def.dimension > 0) {
      constexpr int dim = PropTraits_t<id>::def.dimension;
      std::array<ElementType, dim> values;

      for (int i = 0; i < dim; ++i) {
        values[i] = get<id, ElementType>(i, error_if_missing);
      }

      return values;
    } else {
      // Otherwise use std::vector for dynamic sizing
      int dimension = getDimension<id>();
      std::vector<ElementType> values;
      values.reserve(dimension);

      for (int i = 0; i < dimension; ++i) {
        values.push_back(get<id, ElementType>(i, error_if_missing));
      }

      return values;
    }
  }

  // Set all values of a prop

  // For single-type properties with any container.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id,
            typename Container>  // Container must have size() and operator[]
  PropertyAccessor &setAll(const Container &values, bool error_if_missing = true) {
    static_assert(!PropTraits_t<id>::is_multitype,
                  "This property supports multiple types. Use setAll<PropId, "
                  "ElementType>(container) instead.");
    assert(propset_ != nullptr);

    for (size_t i = 0; i < values.size(); ++i) {
      this->template set<id>(values[i], static_cast<int>(i), error_if_missing);
    }

    return *this;
  }

  // For single-type properties with initializer lists.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id>
  PropertyAccessor &setAll(std::initializer_list<typename PropTraits_t<id>::type> values,
                           bool error_if_missing = true) {
    static_assert(!PropTraits_t<id>::is_multitype,
                  "This property supports multiple types. Use "
                  "setAllTyped<PropId, ElementType>() instead.");
    assert(propset_ != nullptr);

    int index = 0;
    for (const auto &value : values) {
      this->template set<id>(value, index++, error_if_missing);
    }

    return *this;
  }

  // For 2-d (PointD) single-type properties.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id,
            std::enable_if_t<PropTraits_t<id>::def.dimension == 2 &&
                                 !PropTraits_t<id>::is_multitype &&
                                 std::is_same_v<typename PropTraits_t<id>::type, double>,
                             int> = 0>
  PropertyAccessor &set(OfxPointD values, bool error_if_missing = true) {
    assert(propset_ != nullptr);
    this->template set<id>(values.x, 0, error_if_missing);
    this->template set<id>(values.y, 1, error_if_missing);
    return *this;
  }

  // For 2-d (PointI) single-type properties.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id,
            std::enable_if_t<PropTraits_t<id>::def.dimension == 2 &&
                                 !PropTraits_t<id>::is_multitype &&
                                 std::is_same_v<typename PropTraits_t<id>::type, double>,
                             int> = 0>
  PropertyAccessor &set(OfxPointI values, bool error_if_missing = true) {
    assert(propset_ != nullptr);
    this->template set<id>(values.x, 0, error_if_missing);
    this->template set<id>(values.y, 1, error_if_missing);
    return *this;
  }

  // For 4-d (RectD) single-type properties.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id,
            std::enable_if_t<PropTraits_t<id>::def.dimension == 4 &&
                                 !PropTraits_t<id>::is_multitype &&
                                 std::is_same_v<typename PropTraits_t<id>::type, double>,
                             int> = 0>
  PropertyAccessor &set(OfxRectD values, bool error_if_missing = true) {
    assert(propset_ != nullptr);
    this->template set<id>(values.x1, 0, error_if_missing);
    this->template set<id>(values.y1, 1, error_if_missing);
    this->template set<id>(values.x2, 2, error_if_missing);
    this->template set<id>(values.y2, 3, error_if_missing);
    return *this;
  }

  // For 4-d (RectI) single-type properties.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id,
            std::enable_if_t<PropTraits_t<id>::def.dimension == 4 &&
                                 !PropTraits_t<id>::is_multitype &&
                                 std::is_same_v<typename PropTraits_t<id>::type, double>,
                             int> = 0>
  PropertyAccessor &set(OfxRectI values, bool error_if_missing = true) {
    assert(propset_ != nullptr);
    this->template set<id>(values.x1, 0, error_if_missing);
    this->template set<id>(values.y1, 1, error_if_missing);
    this->template set<id>(values.x2, 2, error_if_missing);
    this->template set<id>(values.y2, 3, error_if_missing);
    return *this;
  }

  // For multi-type properties - require explicit ElementType.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id, typename ElementType>
  PropertyAccessor &setAllTyped(const std::initializer_list<ElementType> &values,
                                bool error_if_missing = true) {
    static_assert(PropTraits_t<id>::is_multitype,
                  "This property does not support multiple types. Use "
                  "setAll<PropId>() instead.");
    assert(propset_ != nullptr);

    for (size_t i = 0; i < values.size(); ++i) {
      this->template set<id, ElementType>(values[i], static_cast<int>(i), error_if_missing);
    }

    return *this;
  }

  // Overload for any container with multi-type properties.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id, typename ElementType, typename Container>
  PropertyAccessor &setAllTyped(const Container &values, bool error_if_missing = true) {
    static_assert(PropTraits_t<id>::is_multitype,
                  "This property does not support multiple types. Use "
                  "setAll<PropId>() instead.");
    assert(propset_ != nullptr);

    for (size_t i = 0; i < values.size(); ++i) {
      this->template set<id, ElementType>(values[i], static_cast<int>(i), error_if_missing);
    }

    return *this;
  }

  // Get dimension of a property.
  // Works with any PropId enum (openfx::PropId or host-defined).
  template <auto id>
  int getDimension(bool error_if_missing = true) const {
    using Traits = PropTraits_t<id>;
    assert(propset_ != nullptr);

    // If dimension is known at compile time, we can just return it
    if constexpr (Traits::dimension > 0) {
      return Traits::dimension;
    } else {
      // Otherwise query at runtime
      int dimension = 0;
      _OPENFX_CHECK(propSuite_->propGetDimension(propset_, Traits::def.name, &dimension),
                    Traits::def.name, error_if_missing);
      return dimension;
    }
  }

  // "Escape hatch" for unchecked property access - get any property by name
  // with explicit type
  template <typename T>
  T getRaw(const char *name, int index = 0, bool error_if_missing = true) const {
    assert(propset_ != nullptr);
    if constexpr (std::is_same_v<T, int>) {
      int value = 0;
      _OPENFX_CHECK(propSuite_->propGetInt(propset_, name, index, &value), name, error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, double>) {
      double value = NAN;
      _OPENFX_CHECK(propSuite_->propGetDouble(propset_, name, index, &value), name,
                    error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, const char *>) {
      char *value = nullptr;
      _OPENFX_CHECK(propSuite_->propGetString(propset_, name, index, &value), name,
                    error_if_missing);
      return value;
    } else if constexpr (std::is_same_v<T, void *>) {
      void *value = nullptr;
      _OPENFX_CHECK(propSuite_->propGetPointer(propset_, name, index, &value), name,
                    error_if_missing);
      return value;
    } else {
      static_assert(always_false<T>::value, "Unsupported property type");
    }
  }

  // "Escape hatch" for unchecked property access - set any property by name
  // with explicit type
  template <typename T>
  PropertyAccessor &setRaw(const char *name, T value, int index = 0, bool error_if_missing = true) {
    assert(propset_ != nullptr);
    if constexpr (std::is_same_v<T, int>) {
      _OPENFX_CHECK(propSuite_->propSetInt(propset_, name, index, value), name, error_if_missing);
    } else if constexpr (std::is_same_v<T, double>) {
      _OPENFX_CHECK(propSuite_->propSetDouble(propset_, name, index, value), name,
                    error_if_missing);
    } else if constexpr (std::is_same_v<T, const char *>) {
      _OPENFX_CHECK(propSuite_->propSetString(propset_, name, index, value), name,
                    error_if_missing);
    } else if constexpr (std::is_same_v<T, void *>) {
      _OPENFX_CHECK(propSuite_->propSetPointer(propset_, name, index, value), name,
                    error_if_missing);
    } else {
      static_assert(always_false<T>::value, "Unsupported property type for setting");
    }
    return *this;
  }

  // Get raw dimension of a property
  int getDimensionRaw(const char *name, bool error_if_missing = true) const {
    assert(propset_ != nullptr);
    int dimension = 0;
    _OPENFX_CHECK(propSuite_->propGetDimension(propset_, name, &dimension), name, error_if_missing);
    return dimension;
  }

 private:
  OfxPropertySetHandle propset_;
  const OfxPropertySuiteV1 *propSuite_;

  // Helper for static_assert to fail compilation for unsupported types
  template <typename>
  struct always_false : std::false_type {};
};

// Namespace for property access constants and helpers
namespace prop {
// We'll use the existing defined constants like kOfxImageClipPropColourspace

// Helper to validate property existence at compile time.
// Works with any PropId enum (openfx::PropId or host-defined).
template <auto id>
constexpr bool exists() {
  return true;  // All PropId values are valid by definition
}

// Helper to check if a property supports a specific C++ type.
// Works with any PropId enum (openfx::PropId or host-defined).
template <auto id, typename T>
constexpr bool supportsType() {
  constexpr auto supportedTypes = PropTraits_t<id>::def.supportedTypes;

  for (const auto &type : supportedTypes) {
    if constexpr (std::is_same_v<T, int>) {
      if (type == PropType::Int || type == PropType::Bool || type == PropType::Enum)
        return true;
    } else if constexpr (std::is_same_v<T, double>) {
      if (type == PropType::Double)
        return true;
    } else if constexpr (std::is_same_v<T, const char *>) {
      if (type == PropType::String || type == PropType::Enum)
        return true;
    } else if constexpr (std::is_same_v<T, void *>) {
      if (type == PropType::Pointer)
        return true;
    }
  }
  return false;
}
}  // namespace prop

#undef _OPENFX_CHECK
#undef _OPENFX_CHECK_THROW
#undef _OPENFX_CHECK_WARN

}  // namespace openfx
