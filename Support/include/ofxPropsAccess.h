// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "ofxCore.h"
#include "ofxPropsMetadata.h"
#include "ofxStatusStrings.h"

#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

/**
 * OpenFX Property Accessor System - Usage Examples

// Basic property access with type safety
void basicPropertyAccess(OfxPropertySetHandle handle, OfxPropertySuiteV1*
propHost) {
    // Create a property accessor
    OpenFX::PropertyAccessor props(handle, propHost);

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
propHost) { OpenFX::PropertyAccessor props(handle, propHost);

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
propHost) { OpenFX::PropertyAccessor props(handle, propHost);

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
propHost) { OpenFX::PropertyAccessor props(handle, propHost);

    // Get an enum property value
    const char* fieldExtraction =
props.get<PropId::ImageClipPropFieldExtraction>();

    // Set an enum property using a valid value
    props.set<PropId::ImageClipPropFieldExtraction>("OfxImageFieldLower");

    // Access enum values directly using the EnumValue helper
    const char* noneField =
OpenFX::EnumValue<PropId::ImageClipPropFieldExtraction>::get(0); const char*
lowerField = OpenFX::EnumValue<PropId::ImageClipPropFieldExtraction>::get(1);

    // Check if a value is valid for an enum
    bool isValid =
OpenFX::EnumValue<PropId::ImageClipPropFieldExtraction>::isValid("OfxImageFieldUpper");

    // Get total number of enum values
    size_t enumCount =
OpenFX::EnumValue<PropId::ImageClipPropFieldExtraction>::size();
}

// End of examples
*/

namespace OpenFX {

// Exception class -- move this elsewhere
class PluginException : public std::runtime_error {
public:
  PluginException(const std::string &context_msg, OfxStatus status)
      : std::runtime_error(createMessage(context_msg, status)),
        status_(status) {}

  OfxStatus getStatus() const { return status_; }

private:
  static std::string createMessage(const std::string &context_msg,
                                   OfxStatus status) {
    std::string msg = "OpenFX error: " + context_msg + ": ";
    msg += ofxStatusToString(status);
    return msg;
  }

  OfxStatus status_;
};

static void handleOfxError(OfxStatus status, std::string file, int line,
                           std::string msg, std::string expr) {
  std::cerr << "ERR: OFX status " << ofxStatusToString(status) << " in " << file
            << ":" << line << "\n"
            << "  on: " << msg << "\n"
            << "  Expression: " << expr << "\n";
}

#define CHECK_OFX_STATUS(msg, expr)                                            \
  do {                                                                         \
    auto &&_status = (expr);                                                   \
    if (_status != kOfxStatOK) {                                               \
      handleOfxError(_status, __FILE__, __LINE__, msg, #expr);                 \
    }                                                                          \
  } while (0)

// Type-mapping helper to infer C++ type from PropType
template <PropType propType> struct PropTypeToNative {
  using type = void; // Default case, should never be used directly
};

// Specializations for each property type
template <> struct PropTypeToNative<PropType::Int> {
  using type = int;
};
template <> struct PropTypeToNative<PropType::Double> {
  using type = double;
};
template <> struct PropTypeToNative<PropType::Enum> {
  using type = const char *;
};
template <> struct PropTypeToNative<PropType::Bool> {
  using type = int;
};
template <> struct PropTypeToNative<PropType::String> {
  using type = const char *;
};
template <> struct PropTypeToNative<PropType::Pointer> {
  using type = void *;
};

// Helper to check if a type is compatible with a PropType
template <typename T, PropType propType> struct IsTypeCompatible {
  static constexpr bool value = false;
};

template <> struct IsTypeCompatible<int, PropType::Int> {
  static constexpr bool value = true;
};
template <> struct IsTypeCompatible<int, PropType::Bool> {
  static constexpr bool value = true;
};
template <> struct IsTypeCompatible<int, PropType::Enum> {
  static constexpr bool value = true;
};
template <> struct IsTypeCompatible<double, PropType::Double> {
  static constexpr bool value = true;
};
template <> struct IsTypeCompatible<const char *, PropType::String> {
  static constexpr bool value = true;
};
template <> struct IsTypeCompatible<const char *, PropType::Enum> {
  static constexpr bool value = true;
};
template <> struct IsTypeCompatible<void *, PropType::Pointer> {
  static constexpr bool value = true;
};

// Helper to create property enum values with strong typing
template <PropId id> struct EnumValue {
  static constexpr const char *get(size_t index) {
    static_assert(index < properties::PropTraits<id>::def.enumValuesCount,
                  "Property enum index out of range");
    return properties::PropTraits<id>::enumValues[index];
  }

  static constexpr size_t size() {
    return properties::PropTraits<id>::enumValues.size();
  }

  static constexpr bool isValid(const char *value) {
    for (int i = 0; i < properties::PropTraits<id>::def.enumValuesCount; i++) {
      auto val = properties::PropTraits<id>::def.enumValues[i];
      if (std::strcmp(val, value) == 0)
        return true;
    }
    return false;
  }
};

// Type-safe property accessor for any props of a given prop set
class PropertyAccessor {
public:
  explicit PropertyAccessor(OfxPropertySetHandle handle,
                            OfxPropertySuiteV1 *propHost)
      : handle_(handle), propHost_(propHost) {}

  // Get property value using PropId (compile-time type checking)
  template <PropId id>
  typename properties::PropTraits<id>::type get(int index = 0) const {
    using Traits = properties::PropTraits<id>;

    static_assert(
        !Traits::is_multitype,
        "This property supports multiple types. Use get<PropId, T>() instead.");

    using T = typename Traits::type;

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
      int value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetInt(handle_, Traits::def.name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, double> ||
                         std::is_same_v<T, float>) {
      double value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetDouble(handle_, Traits::def.name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, const char *>) {
      char *value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetString(handle_, Traits::def.name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, void *>) {
      void *value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetPointer(handle_, Traits::def.name, index, &value));
      return value;
    } else {
      static_assert(always_false<T>::value, "Unsupported property value type");
    }
  }

  // Get multi-type property value (requires explicit type)
  template <PropId id, typename T> T get(int index = 0) const {
    using Traits = properties::PropTraits<id>;

    // Check if T is compatible with any of the supported PropTypes
    constexpr bool isValidType = [&]() {
      constexpr auto types = std::array(Traits::def.supportedTypes,
                                        Traits::def.supportedTypesCount);
      for (const auto &type : types) {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
          if (type == PropType::Int || type == PropType::Bool ||
              type == PropType::Enum)
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

    static_assert(isValidType,
                  "Requested type is not compatible with this property");

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
      int value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetInt(handle_, Traits::def.name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, double>) {
      double value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetDouble(handle_, Traits::def.name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, const char *>) {
      char *value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetString(handle_, Traits::def.name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, void *>) {
      void *value;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetPointer(handle_, Traits::def.name, index, &value));
      return value;
    } else {
      static_assert(always_false<T>::value, "Unsupported property value type");
    }
  }

  // Set property value using PropId (compile-time type checking)
  template <PropId id>
  PropertyAccessor &set(typename properties::PropTraits<id>::type value,
                        int index = 0) {
    using Traits = properties::PropTraits<id>;

    static_assert(
        !Traits::is_multitype,
        "This property supports multiple types. Use set<PropId, T>() instead.");

    if constexpr (Traits::def.supportedTypes[0] == PropType::Enum) {
      bool isValidEnumValue = OpenFX::EnumValue<id>::isValid(value);
      assert(isValidEnumValue);
    }

    using T = typename Traits::type;

    if constexpr (std::is_same_v<T, bool>) { // allow bool -> int
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetInt(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, int>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetInt(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, float>) { // allow float -> double
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetDouble(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, double>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetDouble(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, const char *>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetString(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, void *>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetPointer(handle_, Traits::def.name, index, value));
    } else {
      static_assert(always_false<T>::value,
                    "Invalid value type when setting property");
    }
    return *this;
  }

  // Set multi-type property value (requires explicit type)
  // Should only be used for multitype props (SFINAE)
  template <
      PropId id, typename T,
      typename = std::enable_if_t<properties::PropTraits<id>::is_multitype>>
  PropertyAccessor &set(T value, int index = 0) {
    using Traits = properties::PropTraits<id>;

    // Check if T is compatible with any of the supported PropTypes
    constexpr bool isValidType = [&]() {
      for (int i = 0; i < Traits::def.supportedTypesCount; i++) {
        auto type = Traits::def.supportedTypes[i];
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
          if (type == PropType::Int || type == PropType::Bool)
            return true;
          else if (type == PropType::Enum)
            static_assert(always_false<T>::value,
                          "Integer values cannot be used for enum properties");
        } else if constexpr (std::is_same_v<T, double> ||
                             std::is_same_v<T, float>) {
          if (type == PropType::Double)
            return true;
        } else if constexpr (std::is_same_v<T, const char *>) {
          if (type == PropType::String) // no Enums here -- there shouldn't be
                                        // any multi-type enums
            return true;
        } else if constexpr (std::is_same_v<T, void *>) {
          if (type == PropType::Pointer)
            return true;
        }
      }
      return false;
    }();

    static_assert(isValidType,
                  "Requested type is not compatible with this property");

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetInt(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, double> ||
                         std::is_same_v<T, float>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetDouble(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, const char *>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetString(handle_, Traits::def.name, index, value));
    } else if constexpr (std::is_same_v<T, void *>) {
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propSetPointer(handle_, Traits::def.name, index, value));
    } else {
      static_assert(always_false<T>::value,
                    "Invalid value type when setting property");
    }
    return *this;
  }

  // Set all values of a prop

  // For single-type properties with any container
  template <PropId id,
            typename Container> // Container must have size() and operator[]
  PropertyAccessor &setAll(const Container &values) {
    static_assert(!properties::PropTraits<id>::is_multitype,
                  "This property supports multiple types. Use setAll<PropId, "
                  "ElementType>(container) instead.");

    for (size_t i = 0; i < values.size(); ++i) {
      this->template set<id>(values[i], static_cast<int>(i));
    }

    return *this;
  }

  // For single-type properties with initializer lists
  template <PropId id>
  PropertyAccessor &setAll(
      std::initializer_list<typename properties::PropTraits<id>::type> values) {
    static_assert(!properties::PropTraits<id>::is_multitype,
                  "This property supports multiple types. Use "
                  "setAllTyped<PropId, ElementType>() instead.");

    int index = 0;
    for (const auto &value : values) {
      this->template set<id>(value, index++);
    }

    return *this;
  }

  // For multi-type properties - require explicit ElementType
  template <PropId id, typename ElementType>
  PropertyAccessor &
  setAllTyped(const std::initializer_list<ElementType> &values) {
    static_assert(properties::PropTraits<id>::is_multitype,
                  "This property does not support multiple types. Use "
                  "setAll<PropId>() instead.");

    for (size_t i = 0; i < values.size(); ++i) {
      this->template set<id, ElementType>(values[i], static_cast<int>(i));
    }

    return *this;
  }

  // Overload for any container with multi-type properties
  template <PropId id, typename ElementType, typename Container>
  PropertyAccessor &setAllTyped(const Container &values) {
    static_assert(properties::PropTraits<id>::is_multitype,
                  "This property does not support multiple types. Use "
                  "setAll<PropId>() instead.");

    for (size_t i = 0; i < values.size(); ++i) {
      this->template set<id, ElementType>(values[i], static_cast<int>(i));
    }

    return *this;
  }

  // Get dimension of a property
  template <PropId id> int getDimension() const {
    using Traits = properties::PropTraits<id>;

    // If dimension is known at compile time, we can just return it
    if constexpr (Traits::dimension > 0) {
      return Traits::dimension;
    } else {
      // Otherwise query at runtime
      int dimension = 0;
      CHECK_OFX_STATUS(
          Traits::def.name,
          propHost_->propGetDimension(handle_, Traits::def.name, &dimension));
      return dimension;
    }
  }

  // "Escape hatch" for unchecked property access - get any property by name
  // with explicit type
  template <typename T> T getRaw(const char *name, int index = 0) const {
    if constexpr (std::is_same_v<T, int>) {
      int value;
      CHECK_OFX_STATUS(name,
                       propHost_->propGetInt(handle_, name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, double>) {
      double value;
      CHECK_OFX_STATUS(name,
                       propHost_->propGetDouble(handle_, name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, const char *>) {
      char *value;
      CHECK_OFX_STATUS(name,
                       propHost_->propGetString(handle_, name, index, &value));
      return value;
    } else if constexpr (std::is_same_v<T, void *>) {
      void *value;
      CHECK_OFX_STATUS(name,
                       propHost_->propGetPointer(handle_, name, index, &value));
      return value;
    } else {
      static_assert(always_false<T>::value, "Unsupported property type");
    }
  }

  // "Escape hatch" for unchecked property access - set any property by name
  // with explicit type
  template <typename T>
  PropertyAccessor &setRaw(const char *name, T value, int index = 0) {
    if constexpr (std::is_same_v<T, int>) {
      CHECK_OFX_STATUS(name,
                       propHost_->propSetInt(handle_, name, index, value));
    } else if constexpr (std::is_same_v<T, double>) {
      CHECK_OFX_STATUS(name,
                       propHost_->propSetDouble(handle_, name, index, value));
    } else if constexpr (std::is_same_v<T, const char *>) {
      CHECK_OFX_STATUS(name,
                       propHost_->propSetString(handle_, name, index, value));
    } else if constexpr (std::is_same_v<T, void *>) {
      CHECK_OFX_STATUS(name,
                       propHost_->propSetPointer(handle_, name, index, value));
    } else {
      static_assert(always_false<T>::value,
                    "Unsupported property type for setting");
    }
    return *this;
  }

  // Get raw dimension of a property
  int getDimensionRaw(const char *name) const {
    int dimension = 0;
    CHECK_OFX_STATUS(name,
                     propHost_->propGetDimension(handle_, name, &dimension));
    return dimension;
  }

private:
  OfxPropertySetHandle handle_;
  OfxPropertySuiteV1 *propHost_;

  // Helper for static_assert to fail compilation for unsupported types
  template <typename> struct always_false : std::false_type {};
};

// Namespace for property access constants and helpers
namespace prop {
// We'll use the existing defined constants like kOfxImageClipPropColourspace

// Helper to validate property existence at compile time
template <PropId id> constexpr bool exists() {
  return true; // All PropId values are valid by definition
}

// Helper to check if a property supports a specific C++ type
template <PropId id, typename T> constexpr bool supportsType() {
  constexpr auto supportedTypes =
      properties::PropTraits<id>::def.supportedTypes;

  for (const auto &type : supportedTypes) {
    if constexpr (std::is_same_v<T, int>) {
      if (type == PropType::Int || type == PropType::Bool ||
          type == PropType::Enum)
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
} // namespace prop

// Find a property definition by name

} // namespace OpenFX
