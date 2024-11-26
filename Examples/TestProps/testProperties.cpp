// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

/** @file testProperties.cpp OFX test plugin which logs all properties
    in various actions.

    Uses spdlog and fmt to log all the info.
*/
#include "ofxImageEffect.h"
#include "ofxMemory.h"
#include "ofxMessage.h"
#include "ofxMultiThread.h"
#include "ofxPropsBySet.h" // in Support/include
#include "ofxPropsMetadata.h"
#include "spdlog/spdlog.h"
#include <map>    // stl maps
#include <string> // stl strings
#include <variant>

#if defined __APPLE__ || defined __linux__ || defined __FreeBSD__
#define EXPORT __attribute__((visibility("default")))
#elif defined _WIN32
#define EXPORT OfxExport
#else
#error Not building on your operating system quite yet
#endif

static OfxHost *gHost;
static OfxImageEffectSuiteV1 *gEffectSuite;
static OfxPropertySuiteV1 *gPropSuite;
static OfxInteractSuiteV1 *gInteractSuite;
static OfxParameterSuiteV1 *gParamSuite;
static OfxMemorySuiteV1 *gMemorySuite;
static OfxMultiThreadSuiteV1 *gThreadSuite;
static OfxMessageSuiteV1 *gMessageSuite;

////////////////////////////////////////////////////////////////////////////////
// fetch a suite
static const void *fetchSuite(const char *suiteName, int suiteVersion,
                              bool optional = false) {
  const void *suite = gHost->fetchSuite(gHost->host, suiteName, suiteVersion);
  if (optional) {
    if (suite == 0)
      spdlog::warn("Could not fetch the optional suite '%s' version %d;",
                   suiteName, suiteVersion);
  } else {
    if (suite == 0)
      spdlog::error("Could not fetch the mandatory suite '%s' version %d;",
                    suiteName, suiteVersion);
  }
  if (!optional && suite == 0)
    throw kOfxStatErrMissingHostFeature;
  return suite;
}

////////////////////////////////////////////////////////////////////////////////
// maps status to a string
static const char *mapStatus(OfxStatus stat) {
  switch (stat) {
  case kOfxStatOK:
    return "kOfxStatOK";
  case kOfxStatFailed:
    return "kOfxStatFailed";
  case kOfxStatErrFatal:
    return "kOfxStatErrFatal";
  case kOfxStatErrUnknown:
    return "kOfxStatErrUnknown";
  case kOfxStatErrMissingHostFeature:
    return "kOfxStatErrMissingHostFeature";
  case kOfxStatErrUnsupported:
    return "kOfxStatErrUnsupported";
  case kOfxStatErrExists:
    return "kOfxStatErrExists";
  case kOfxStatErrFormat:
    return "kOfxStatErrFormat";
  case kOfxStatErrMemory:
    return "kOfxStatErrMemory";
  case kOfxStatErrBadHandle:
    return "kOfxStatErrBadHandle";
  case kOfxStatErrBadIndex:
    return "kOfxStatErrBadIndex";
  case kOfxStatErrValue:
    return "kOfxStatErrValue";
  case kOfxStatReplyYes:
    return "kOfxStatReplyYes";
  case kOfxStatReplyNo:
    return "kOfxStatReplyNo";
  case kOfxStatReplyDefault:
    return "kOfxStatReplyDefault";
  case kOfxStatErrImageFormat:
    return "kOfxStatErrImageFormat";
  }
  return "UNKNOWN STATUS CODE";
}


/*************************************************************************/

/* PropertyValue usage (using enum because it's most complex):
    std::vector<std::string> colors = {"red", "green", "blue"};
    PropertyValue enumProp(3, "red", colors);
    enumProp.set<std::string>(1, "green"); // throws exception if not allowed
    auto values = enumProp.get<std::string>();
    std::cout << enumProp.toString() << std::endl;

    PropertyValue intProp(5, 0);
    intProp.set<int>({1, 2, 3, 4, 5});
    intProp.set<int>(2, 10);  // Changes the third element to 10
    std::vector<int> values = intProp.get<int>();
    int value = intProp.get<int>(2);  // Retrieves the value 10
    intProp.size(); // returns 5
*/

class PropertyValue {
public:
  enum class Type { Int, Double, Bool, String, Pointer, Enum };

  using value_type = std::variant<int, double, bool, std::string, void *>;

private:
  std::vector<value_type> data;
  Type type_;
  std::vector<std::string> enum_values; // Only used if type_ == Type::Enum

  template <typename T>
  static constexpr bool is_supported_type =
      std::is_same_v<T, int> || std::is_same_v<T, double> ||
      std::is_same_v<T, bool> || std::is_same_v<T, std::string> ||
      std::is_same_v<T, void *>;

  template <typename T> static std::string type_name() {
    if constexpr (std::is_same_v<T, int>)
      return "int";
    else if constexpr (std::is_same_v<T, double>)
      return "double";
    else if constexpr (std::is_same_v<T, bool>)
      return "bool";
    else if constexpr (std::is_same_v<T, std::string>)
      return "string";
    else if constexpr (std::is_same_v<T, void *>)
      return "pointer";
    else
      return "unknown";
  }

  static std::string type_to_string(Type t) {
    switch (t) {
    case Type::Int:
      return "int";
    case Type::Double:
      return "double";
    case Type::Bool:
      return "bool";
    case Type::String:
      return "string";
    case Type::Pointer:
      return "pointer";
    case Type::Enum:
      return "enum";
    default:
      return "unknown";
    }
  }

  static std::string format_value(const value_type &v) {
    return std::visit(
        [](const auto &x) {
          using T = std::decay_t<decltype(x)>;
          if constexpr (std::is_same_v<T, bool>) {
            return std::string(x ? "true" : "false");
          } else if constexpr (std::is_same_v<T, std::string>) {
            return fmt::format("\"{}\"", x);
          } else if constexpr (std::is_same_v<T, void *>) {
            return fmt::format("{:p}", x);
          } else {
            return fmt::format("{}", x);
          }
        },
        v);
  }

  void check_enum_value(const std::string &value) const {
    if (std::find(enum_values.begin(), enum_values.end(), value) ==
        enum_values.end()) {
      throw std::invalid_argument(
          fmt::format("Invalid enum value '{}'. Allowed values are: {}", value,
                      enum_values_list()));
    }
  }

  std::string enum_values_list() const {
    std::string list;
    for (size_t i = 0; i < enum_values.size(); ++i) {
      if (i > 0)
        list += ", ";
      list += enum_values[i];
    }
    return list;
  }

public:
  // Constructor for basic types
  template <typename T>
  PropertyValue(std::size_t size, const T &initial_value = T{})
      : data(size, initial_value) {
    static_assert(is_supported_type<T>, "Unsupported type");

    if constexpr (std::is_same_v<T, int>)
      type_ = Type::Int;
    else if constexpr (std::is_same_v<T, double>)
      type_ = Type::Double;
    else if constexpr (std::is_same_v<T, bool>)
      type_ = Type::Bool;
    else if constexpr (std::is_same_v<T, std::string>)
      type_ = Type::String;
    else if constexpr (std::is_same_v<T, void *>)
      type_ = Type::Pointer;
  }

  // Constructor for enum type
  PropertyValue(std::size_t size, const std::string &initial_value,
                const std::vector<std::string> &allowed_values)
      : data(size, initial_value), type_(Type::Enum),
        enum_values(allowed_values) {
    check_enum_value(initial_value); // Validate initial value
  }

  // Set values from a vector
  template <typename T> void set(const std::vector<T> &values) {
    static_assert(is_supported_type<T>, "Unsupported type");
    if (values.size() != data.size()) {
      throw std::length_error(
          fmt::format("Size mismatch: container size is {}, input size is {}",
                      data.size(), values.size()));
    }
    if (type_ != get_type<T>()) {
      throw std::runtime_error(
          fmt::format("Type mismatch: container type is {}, input type is {}",
                      type_to_string(type_), type_name<T>()));
    }
    if (type_ == Type::Enum) {
      for (const auto &val : values) {
        check_enum_value(val);
      }
    }
    for (size_t i = 0; i < values.size(); ++i) {
      data[i] = values[i];
    }
  }

  // Set value at a specific index
  template <typename T> void set(std::size_t index, const T &value) {
    static_assert(is_supported_type<T>, "Unsupported type");
    if (index >= data.size()) {
      throw std::out_of_range(
          fmt::format("Index {} out of range. Size is {}", index, data.size()));
    }
    if (type_ != get_type<T>()) {
      throw std::runtime_error(
          fmt::format("Type mismatch: container type is {}, input type is {}",
                      type_to_string(type_), type_name<T>()));
    }
    if (type_ == Type::Enum) {
      check_enum_value(value);
    }
    data[index] = value;
  }

  // Get all values as a vector
  template <typename T> std::vector<T> get() const {
    static_assert(is_supported_type<T>, "Unsupported type");
    if (type_ != get_type<T>()) {
      throw std::runtime_error(fmt::format(
          "Type mismatch: container type is {}, requested type is {}",
          type_to_string(type_), type_name<T>()));
    }
    std::vector<T> result;
    result.reserve(data.size());
    for (const auto &v : data) {
      result.push_back(std::get<T>(v));
    }
    return result;
  }

  // Get value at a specific index
  template <typename T> T get(std::size_t index) const {
    static_assert(is_supported_type<T>, "Unsupported type");
    if (index >= data.size()) {
      throw std::out_of_range(
          fmt::format("Index {} out of range. Size is {}", index, data.size()));
    }
    if (type_ != get_type<T>()) {
      throw std::runtime_error(fmt::format(
          "Type mismatch: container type is {}, requested type is {}",
          type_to_string(type_), type_name<T>()));
    }
    return std::get<T>(data[index]);
  }

  std::string toString() const {
    if (data.empty())
      return "<empty>";
    std::string values;
    for (size_t i = 0; i < data.size(); ++i) {
      if (i > 0)
        values += ", ";
      values += format_value(data[i]);
    }
    std::string type_str = type_to_string(type_);
    if (type_ == Type::Enum) {
      type_str += "{" + enum_values_list() + "}";
    }
    return fmt::format("<{}>[{}]", type_str, values);
  }

  std::size_t size() const { return data.size(); }
  Type type() const { return type_; }
  void resize(std::size_t new_size) { data.resize(new_size); }
  void reserve(std::size_t new_capacity) { data.reserve(new_capacity); }
  void clear() { data.clear(); }
  bool empty() const { return data.empty(); }

private:
  // Helper to get Type enum from a template type
  template <typename T> static constexpr Type get_type() {
    if constexpr (std::is_same_v<T, int>)
      return Type::Int;
    else if constexpr (std::is_same_v<T, double>)
      return Type::Double;
    else if constexpr (std::is_same_v<T, bool>)
      return Type::Bool;
    else if constexpr (std::is_same_v<T, std::string>)
      return Type::String;
    else if constexpr (std::is_same_v<T, void *>)
      return Type::Pointer;
    else
      return Type::Enum; // For enum type
  }
};

// Use this as comparator to simplify maps on strings
struct StringCompare {
    using is_transparent = void;  // Enables heterogeneous lookup
    bool operator()(std::string_view a, std::string_view b) const {
        return a < b;
    }
};

/** @brief wraps up a set of properties
   auto ps = PropertySet::create(name, handle); // returns std::optional (false if no such name)
   ps->add(propname); // adds from props_metadata
   auto data = ps->get<T>(propname);
   ps->set(propname, {...values...});
 */
class PropertySet {
protected:
  std::string name;
  std::map<const std::string, PropertyValue, StringCompare> properties;
  int _propLogMessages; // if more than 0, don't log ordinary messages
  OfxPropertySetHandle _propHandle;

  std::vector<OpenFX::Prop> prop_defs;

private:
  PropertySet();

public:
  static std::optional<PropertySet> create(const std::string &name,
                                           OfxPropertySetHandle h = 0) {
    PropertySet ps;
    ps.name = name;
    ps._propHandle = h;

    auto it = OpenFX::prop_sets.find(name.c_str());
    if (it == OpenFX::prop_sets.end())
      return std::nullopt;
    ps.prop_defs = it->second;
    return ps;
  }

  virtual ~PropertySet();
  void propSetHandle(OfxPropertySetHandle h) { _propHandle = h; }

  std::optional<const OpenFX::PropsMetadata *>
  find_prop(std::string_view propname) {
    auto it = std::find_if(
        OpenFX::props_metadata.begin(), OpenFX::props_metadata.end(),
        [&](const OpenFX::PropsMetadata &pm) { return pm.name == propname; });
    if (it == OpenFX::props_metadata.end())
      return std::nullopt;
    return &(*it);
  }

  void add(const std::string_view& propname) {
    if (properties.find(propname) != properties.end()) {
      // already in the map; do nothing
      return;
    }

    if (auto meta = find_prop(propname)) {
      auto &metadata = *(*meta);
      int n = metadata.dimension > 0 ? metadata.dimension : 4; // XXX
      PropertyValue v(0, 0);
      switch (metadata.types[0]) {
      case OpenFX::PropType::Int:
        v = PropertyValue(n, 0);
        break;
      case OpenFX::PropType::Double:
        v = PropertyValue(n, 0.0);
        break;
      case OpenFX::PropType::Bool:
        v = PropertyValue(n, true);
        break;
      case OpenFX::PropType::String:
        v = PropertyValue(n, "");
        break;
      case OpenFX::PropType::Bytes:
        v = PropertyValue(n, "");
        break;
      case OpenFX::PropType::Pointer:
        v = PropertyValue(n, (void *)0);
        break;
      default:
        throw std::runtime_error(fmt::format("{}: Invalid type for {} in props_metadata", name, propname));
      }
      properties[std::string(propname)] = v;
      spdlog::info("Propset {}: added property {}, value {}", name, propname, v);
    }
    else {
      throw std::runtime_error(fmt::format("{}: No such propname {} found in props_metadata", name, propname));
    }
  }

  template <typename T>
  const std::vector<T> &get_current(std::string_view propname) {
    return properties.find(propname)->second.get<T>();
  };
  template <typename T> const T get_current(std::string_view propname, int index) {
    return properties.find(propname)->second.get<T>(index);
  }
  template <typename T>
  void set_current(std::string_view propname, int n, const T *values) {
     properties.find(propname)->second.set<T>(values);
  };

  // Get values from the host, saving locally
  void get(std::string_view propname) {
    const int max_n = 128;
    auto& prop = properties.find(propname)->second;
    switch (prop.type()) {
    case PropertyValue::Type::Bool:
      [[fallthrough]];
    case PropertyValue::Type::Int: {
      int values[max_n];
      int dim;
      OfxStatus stat;
      stat = gPropSuite->propGetDimension(_propHandle, propname.data(), &dim);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting dims for prop {} in suite {}: {}",
                                                 propname, name, mapStatus(stat)));
      stat =
        gPropSuite->propGetIntN(_propHandle, propname.data(), dim, values);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting values for {}-d prop {} in suite {}: {}",
                                                 dim, propname, name, mapStatus(stat)));
      // Save locally, checking dim & type
      prop.set(std::vector(dim, values));
      break;
    }
    case PropertyValue::Type::Double: {
      double values[max_n];
      int dim;
      OfxStatus stat;
      stat = gPropSuite->propGetDimension(_propHandle, propname.data(), &dim);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting dims for prop {} in suite {}: {}",
                                                 propname, name, mapStatus(stat)));
      stat =
        gPropSuite->propGetDoubleN(_propHandle, propname.data(), dim, values);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting values for {}-d prop {} in suite {}: {}",
                                                 dim, propname, name, mapStatus(stat)));
      // Save locally, checking dim & type
      prop.set(std::vector(dim, values));
      break;
    }
    case PropertyValue::Type::String:
      [[fallthrough]];
    case PropertyValue::Type::Enum: {
      char *values[max_n];
      int dim;
      OfxStatus stat;
      stat = gPropSuite->propGetDimension(_propHandle, propname.data(), &dim);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting dims for prop {} in suite {}: {}",
                                                 propname, name, mapStatus(stat)));
      stat =
        gPropSuite->propGetStringN(_propHandle, propname.data(), dim, values);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting values for {}-d prop {} in suite {}: {}",
                                                 dim, propname, name, mapStatus(stat)));
      // Save locally, checking dim & type
      prop.set(std::vector(dim, values));
      break;
    }
    case PropertyValue::Type::Pointer: {
      void *values[max_n];
      int dim;
      OfxStatus stat;
      stat = gPropSuite->propGetDimension(_propHandle, propname.data(), &dim);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting dims for prop {} in suite {}: {}",
                                                 propname, name, mapStatus(stat)));
      stat =
        gPropSuite->propGetPointerN(_propHandle, propname.data(), dim, values);
      if (stat != kOfxStatOK)
        throw new std::runtime_error(
                                     fmt::format("Error getting values for {}-d prop {} in suite {}: {}",
                                                 dim, propname, name, mapStatus(stat)));
      // Save locally, checking dim & type
      prop.set(std::vector(dim, values));
      break;
    }
    }
  }

  // inc/dec the log flag to enable/disable ordinary message logging
  void propEnableLog(void) { --_propLogMessages; }
  void propDisableLog(void) { ++_propLogMessages; }
};


////////////////////////////////////////////////////////////////////////////////
// Describes a set of properties
class PropertySetDescription : PropertySet {
protected:
  const char *_setName;
  PropertyDescription *_descriptions;
  int _nDescriptions;

  std::map<std::string, PropertyDescription *> _descriptionsByName;

public:
  PropertySetDescription(const char *setName, OfxPropertySetHandle handle,
                         PropertyDescription *v, int nV);
  void checkProperties(bool logOrdinaryMessages =
                           false); // see if they are there in the first place
  void checkDefaults(bool logOrdinaryMessages = false); // check default values
  void retrieveValues(
      bool logOrdinaryMessages = false); // get current values on the host
  void setValues(
      bool logOrdinaryMessages = false); // set values to the requested ones
  PropertyDescription *findDescription(
      const std::string &name); // find a property with the given name

  int intPropValue(const std::string &name, int idx = 0);
  double doublePropValue(const std::string &name, int idx = 0);
  void *pointerPropValue(const std::string &name, int idx = 0);
  const std::string &stringPropValue(const std::string &name, int idx = 0);
};

////////////////////////////////////////////////////////////////////////////////
// property set code
PropertySet::~PropertySet() {}

OfxStatus PropertySet::propSet(const char *property, void *value, int idx) {
  OfxStatus stat =
      gPropSuite->propSetPointer(_propHandle, property, idx, value);
  if (stat != kOfxStatOK)
    spdlog::error("Failed on setting pointer property %s[%d] to %p, host "
                  "returned status %s;",
                  property, idx, value, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Set pointer property %s[%d] = %p;", property, idx, value);
  return stat;
}

OfxStatus PropertySet::propSet(const char *property, const std::string &value,
                               int idx) {
  OfxStatus stat =
      gPropSuite->propSetString(_propHandle, property, idx, value.c_str());
  if (stat != kOfxStatOK)
    spdlog::error("Failed on setting string property %s[%d] to '%s', host "
                  "returned status %s;",
                  property, idx, value.c_str(), mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Set string property %s[%d] = '%s';", property, idx,
                 value.c_str());
  return stat;
}

OfxStatus PropertySet::propSet(const char *property, double value, int idx) {
  OfxStatus stat = gPropSuite->propSetDouble(_propHandle, property, idx, value);
  if (stat != kOfxStatOK)
    spdlog::error("Failed on setting double property %s[%d] to %g, host "
                  "returned status %s;",
                  property, idx, value, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Set double property %s[%d] = %g;", property, idx, value);
  return stat;
}

OfxStatus PropertySet::propSet(const char *property, int value, int idx) {
  OfxStatus stat = gPropSuite->propSetInt(_propHandle, property, idx, value);
  if (stat != kOfxStatOK)
    spdlog::error("Failed on setting int property %s[%d] to %d, host returned "
                  "status '%s';",
                  property, idx, value, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Set int property %s[%d] = %d;", property, idx, value);
  return stat;
}

OfxStatus PropertySet::propGet(const char *property, void *&value,
                               int idx) const {
  OfxStatus stat =
      gPropSuite->propGetPointer(_propHandle, property, idx, &value);
  if (stat != kOfxStatOK)
    spdlog::error(
        "Failed on fetching pointer property %s[%d], host returned status %s;",
        property, idx, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Fetched pointer property %s[%d] = %p;", property, idx, value);
  return stat;
}

OfxStatus PropertySet::propGet(const char *property, double &value,
                               int idx) const {
  OfxStatus stat =
      gPropSuite->propGetDouble(_propHandle, property, idx, &value);
  if (stat != kOfxStatOK)
    spdlog::error(
        "Failed on fetching double property %s[%d], host returned status %s;",
        property, idx, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Fetched double property %s[%d] = %g;", property, idx, value);
  return stat;
}

OfxStatus PropertySet::propGetN(const char *property, double *values,
                                int N) const {
  OfxStatus stat = gPropSuite->propGetDoubleN(_propHandle, property, N, values);
  if (stat != kOfxStatOK)
    spdlog::error("Failed on fetching multiple double property %s X %d, host "
                  "returned status %s;",
                  property, N, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0) {
    spdlog::info("Fetched multiple double property %s X %d;", property, N);
    for (int i = 0; i < N; i++) {
      spdlog::info("  %s[%d] = %g;", property, i, values[i]);
    }
  }
  return stat;
}

OfxStatus PropertySet::propGetN(const char *property, int *values,
                                int N) const {
  OfxStatus stat = gPropSuite->propGetIntN(_propHandle, property, N, values);
  if (stat != kOfxStatOK)
    spdlog::error("Failed on fetching multiple int property %s X %d, host "
                  "returned status %s;",
                  property, N, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0) {
    spdlog::info("Fetched multiple int property %s X %d;", property, N);
    for (int i = 0; i < N; i++) {
      spdlog::info("  %s[%d] = %d;", property, i, values[i]);
    }
  }
  return stat;
}

OfxStatus PropertySet::propGet(const char *property, int &value,
                               int idx) const {
  OfxStatus stat = gPropSuite->propGetInt(_propHandle, property, idx, &value);
  if (stat != kOfxStatOK)
    spdlog::error(
        "Failed on fetching int property %s[%d], host returned status %s;",
        property, idx, mapStatus(stat));
  if (stat == kOfxStatOK && _propLogMessages <= 0)
    spdlog::info("Fetched int property %s[%d] = %d;", property, idx, value);
  return stat;
}

OfxStatus PropertySet::propGet(const char *property, std::string &value,
                               int idx) const {
  char *str;
  OfxStatus stat = gPropSuite->propGetString(_propHandle, property, idx, &str);
  if (stat != kOfxStatOK)
    spdlog::error(
        "Failed on fetching string property %s[%d], host returned status %s;",
        property, idx, mapStatus(stat));
  if (kOfxStatOK == stat) {
    value = str;
    if (_propLogMessages <= 0)
      spdlog::info("Fetched string property %s[%d] = '%s';", property, idx,
                   value.c_str());
  } else {
    value = "";
  }
  return stat;
}

OfxStatus PropertySet::propGetN(const char *property, std::string *values,
                                int N) const {
  char **strs = new char *[N];

  OfxStatus stat = gPropSuite->propGetStringN(_propHandle, property, N, strs);

  if (stat != kOfxStatOK)
    spdlog::error("Failed on fetching multiple string property %s X %d, host "
                  "returned status %s;",
                  property, N, mapStatus(stat));

  if (kOfxStatOK == stat) {
    if (_propLogMessages <= 0)
      spdlog::info("Fetched multiple string property %s X %d;", property, N);
    for (int i = 0; i < N; i++) {
      values[i] = strs[i];
      if (_propLogMessages <= 0)
        spdlog::info("  %s[%d] = '%s';", property, i, strs[i]);
    }
  } else {
    for (int i = 0; i < N; i++) {
      values[i] = "";
    }
  }

  delete[] strs;

  return stat;
}

OfxStatus PropertySet::propGetDimension(const char *property, int &size) const {
  OfxStatus stat = gPropSuite->propGetDimension(_propHandle, property, &size);
  if (stat != kOfxStatOK)
    spdlog::error("Failed on fetching dimension for property %s, host returned "
                  "status %s;",
                  property, mapStatus(stat));
  return stat;
}

////////////////////////////////////////////////////////////////////////////////
// PropertyDescription code

// check to see if this property exists on the host
void PropertyDescription::checkProperty(PropertySet &propSet) {
  // see if it exists by fetching the dimension,
  int dimension;
  OfxStatus stat = propSet.propGetDimension(_name, dimension);
  if (stat == kOfxStatOK) {
    if (_dimension != -1)
      if (dimension != _dimension)
        spdlog::error(
            "Host reports property '%s' has dimension %d, it should be %d;",
            _name, dimension, _dimension);

    // check type by getting the first element, the property getting will print
    // failure messages to the log
    if (dimension > 0) {
      void *vP;
      int vI;
      double vD;
      std::string vS;

      switch (_ilk) {
      case PropertyDescription::ePointer:
        propSet.propGet(_name, vP);
        break;
      case PropertyDescription::eInt:
        propSet.propGet(_name, vI);
        break;
      case PropertyDescription::eString:
        propSet.propGet(_name, vS);
        break;
      case PropertyDescription::eDouble:
        propSet.propGet(_name, vD);
        break;
      }
    }
  }
}

// see if the default values on the property set agree
void PropertyDescription::checkDefault(PropertySet &propSet) {
  if (_nDefs > 0) {
    // fetch the dimension on the host
    int hostDimension;
    OfxStatus stat = propSet.propGetDimension(_name, hostDimension);
    (void)stat;

    if (hostDimension != _nDefs)
      spdlog::error("Host reports default dimension of '%s' is %d, which is "
                    "different to the default value %d;",
                    _name, hostDimension, _nDefs);

    int N = hostDimension < _nDefs ? hostDimension : _nDefs;

    for (int i = 0; i < N; i++) {
      void *vP;
      int vI;
      double vD;
      std::string vS;

      switch (_ilk) {
      case PropertyDescription::ePointer:
        propSet.propGet(_name, vP, i);
        if (vP != (void *)_defs[i])
          spdlog::error("Default value of %s[%d] = %p, it should be %p;", _name,
                        i, vP, (void *)_defs[i]);
        break;
      case PropertyDescription::eInt:
        propSet.propGet(_name, vI, i);
        if (vI != (int)_defs[i])
          spdlog::error("Default value of %s[%d] = %d, it should be %d;", _name,
                        i, vI, (int)_defs[i]);
        break;
      case PropertyDescription::eString:
        propSet.propGet(_name, vS, i);
        if (vS != _defs[i].vString)
          spdlog::error("Default value of %s[%d] = '%s', it should be '%s';",
                        _name, i, vS.c_str(), _defs[i].vString.c_str());
        break;
      case PropertyDescription::eDouble:
        propSet.propGet(_name, vD, i);
        if (vD != (double)_defs[i])
          spdlog::error("Default value of %s[%d] = %g, it should be %g;", _name,
                        i, vD, (double)_defs[i]);
        break;
      }
    }
  }
}

// get the current value from the property set into me
void PropertyDescription::retrieveValue(PropertySet &propSet) {
  if (_currentVals)
    delete[] _currentVals;
  _currentVals = 0;
  _nCurrentVals = 0;

  // fetch the dimension on the host
  int hostDimension;
  OfxStatus stat = propSet.propGetDimension(_name, hostDimension);
  if (stat == kOfxStatOK && hostDimension > 0) {
    _nCurrentVals = hostDimension;
    _currentVals = new PropertyValueOnion[hostDimension];

    for (int i = 0; i < hostDimension; i++) {
      void *vP;
      int vI;
      double vD;
      std::string vS;

      switch (_ilk) {
      case PropertyDescription::ePointer:
        stat = propSet.propGet(_name, vP, i);
        _currentVals[i] = (stat == kOfxStatOK) ? vP : (void *)(0);
        break;
      case PropertyDescription::eInt:
        stat = propSet.propGet(_name, vI, i);
        _currentVals[i] = (stat == kOfxStatOK) ? vI : 0;
        break;
      case PropertyDescription::eString:
        stat = propSet.propGet(_name, vS, i);
        if (stat == kOfxStatOK)
          _currentVals[i] = vS;
        else
          _currentVals[i] = std::string("");
        break;
      case PropertyDescription::eDouble:
        stat = propSet.propGet(_name, vD, i);
        _currentVals[i] = (stat == kOfxStatOK) ? vD : 0.0;
        break;
      }
    }
  }
}

// set the property from my 'set' property
void PropertyDescription::setValue(PropertySet &propSet) {
  // fetch the dimension on the host
  if (_nWantedVals > 0) {
    int i;
    for (i = 0; i < _nWantedVals; i++) {
      switch (_ilk) {
      case PropertyDescription::ePointer:
        propSet.propSet(_name, _wantedVals[i].vPointer, i);
        break;
      case PropertyDescription::eInt:
        propSet.propSet(_name, _wantedVals[i].vInt, i);
        break;
      case PropertyDescription::eString:
        propSet.propSet(_name, _wantedVals[i].vString, i);
        break;
      case PropertyDescription::eDouble:
        propSet.propSet(_name, _wantedVals[i].vDouble, i);
        break;
      }
    }

    // Now fetch the current values back into current. Don't be verbose about
    // it.
    propSet.propDisableLog();
    retrieveValue(propSet);
    propSet.propEnableLog();

    // and see if they are the same
    if (_nWantedVals != _nCurrentVals)
      spdlog::error("After setting property %s, the dimension %d is not the "
                    "same as what was set %d",
                    _name, _nCurrentVals, _nWantedVals);
    int N = _nWantedVals < _nCurrentVals ? _nWantedVals : _nCurrentVals;
    for (i = 0; i < N; i++) {
      switch (_ilk) {
      case PropertyDescription::ePointer:
        if (_wantedVals[i].vPointer != _currentVals[i].vPointer)
          spdlog::error("After setting pointer value %s[%d] value fetched back "
                        "%p not same as value set %p",
                        _name, i, _wantedVals[i].vPointer,
                        _currentVals[i].vPointer);
        break;
      case PropertyDescription::eInt:
        if (_wantedVals[i].vInt != _currentVals[i].vInt)
          spdlog::error("After setting int value %s[%d] value fetched back %d "
                        "not same as value set %d",
                        _name, i, _wantedVals[i].vInt, _currentVals[i].vInt);
        break;
      case PropertyDescription::eString:
        if (_wantedVals[i].vString != _currentVals[i].vString)
          spdlog::error("After setting string value %s[%d] value fetched back "
                        "'%s' not same as value set '%s'",
                        _name, i, _wantedVals[i].vString.c_str(),
                        _currentVals[i].vString.c_str());
        break;
      case PropertyDescription::eDouble:
        if (_wantedVals[i].vDouble != _currentVals[i].vDouble)
          spdlog::error("After setting double value %s[%d] value fetched back "
                        "%g not same as value set %g",
                        _name, i, _wantedVals[i].vDouble,
                        _currentVals[i].vDouble);
        break;
      }
    }
  }
}

void PropertySetDescription::checkProperties(bool logOrdinaryMessages) {
  spdlog::info("PropertySetDescription::checkProperties - start(checking "
               "properties on %s);\n{",
               _setName);

  // don't print ordinary messages whilst we are checking them
  if (!logOrdinaryMessages)
    propDisableLog();

  // check each property in the description
  for (int i = 0; i < _nDescriptions; i++) {
    _descriptions[i].checkProperty(*this);
  }
  if (!logOrdinaryMessages)
    propEnableLog();

  spdlog::info("}PropertySetDescription::checkProperties - stop;");
}

void PropertySetDescription::checkDefaults(bool logOrdinaryMessages) {
  spdlog::info("PropertySetDescription::checkDefaults - start(checking default "
               "value of properties on %s);\n{",
               _setName);

  // don't print ordinary messages whilst we are checking them
  if (!logOrdinaryMessages)
    propDisableLog();

  // check each property in the description
  for (int i = 0; i < _nDescriptions; i++) {
    _descriptions[i].checkDefault(*this);
  }
  if (!logOrdinaryMessages)
    propEnableLog();

  spdlog::info("}PropertySetDescription::checkDefaults - stop;");
}

void PropertySetDescription::retrieveValues(bool logOrdinaryMessages) {
  spdlog::info("PropertySetDescription::retrieveValues - start(retrieving "
               "values of properties on %s);\n{",
               _setName);

  if (!logOrdinaryMessages)
    propDisableLog();

  // check each property in the description
  for (int i = 0; i < _nDescriptions; i++) {
    _descriptions[i].retrieveValue(*this);
  }

  if (!logOrdinaryMessages)
    propEnableLog();

  spdlog::info("}PropertySetDescription::retrieveValues - stop;");
}

void PropertySetDescription::setValues(bool logOrdinaryMessages) {
  spdlog::info("PropertySetDescription::setValues - start(retrieving values of "
               "properties on %s);\n{",
               _setName);

  if (!logOrdinaryMessages)
    propDisableLog();

  // check each property in the description
  for (int i = 0; i < _nDescriptions; i++) {
    _descriptions[i].setValue(*this);
  }

  if (!logOrdinaryMessages)
    propEnableLog();

  spdlog::info("}PropertySetDescription::setValues - stop;");
}

// find a property with the given name out of our set of properties
PropertyDescription *
PropertySetDescription::findDescription(const std::string &name) {
  std::map<std::string, PropertyDescription *>::iterator iter;
  iter = _descriptionsByName.find(name);
  if (iter != _descriptionsByName.end()) {
    return iter->second;
  }
  return 0;
}

// find value of the named property from the _currentVals array
int PropertySetDescription::intPropValue(const std::string &name, int idx) {
  PropertyDescription *desc = 0;
  desc = findDescription(name);
  if (desc) {
    if (idx < desc->_nCurrentVals) {
      return int(desc->_currentVals[idx]);
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// host description stuff

// list of the properties on the host. We can't set any of these, and most don't
// have defaults
static PropertyDescription gHostPropDescription[] = {
    PropertyDescription(kOfxPropType, 1, "", false, kOfxTypeImageEffectHost,
                        true),
    PropertyDescription(kOfxPropName, 1, "", false, "", false),
    PropertyDescription(kOfxPropLabel, 1, "", false, "", false),
    PropertyDescription(kOfxImageEffectHostPropIsBackground, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxImageEffectPropSupportsOverlays, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxImageEffectPropSupportsMultiResolution, 1, 0, false,
                        0, false),
    PropertyDescription(kOfxImageEffectPropSupportsTiles, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxImageEffectPropTemporalClipAccess, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxImageEffectPropSupportsMultipleClipDepths, 1, 0,
                        false, 0, false),
    PropertyDescription(kOfxImageEffectPropSupportsMultipleClipPARs, 1, 0,
                        false, 0, false),
    PropertyDescription(kOfxImageEffectPropSetableFrameRate, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxImageEffectPropSetableFielding, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxImageEffectPropSupportedComponents, -1, "", false,
                        "", false),
    PropertyDescription(kOfxImageEffectPropSupportedContexts, -1, "", false, "",
                        false),
    PropertyDescription(kOfxParamHostPropSupportsStringAnimation, 1, 0, false,
                        0, false),
    PropertyDescription(kOfxParamHostPropSupportsCustomInteract, 1, 0, false, 0,
                        false),
    PropertyDescription(kOfxParamHostPropSupportsChoiceAnimation, 1, 0, false,
                        0, false),
    PropertyDescription(kOfxParamHostPropSupportsBooleanAnimation, 1, 0, false,
                        0, false),
    PropertyDescription(kOfxParamHostPropSupportsCustomAnimation, 1, 0, false,
                        0, false),
    PropertyDescription(kOfxParamHostPropMaxParameters, 1, 0, false, 0, false),
    PropertyDescription(kOfxParamHostPropMaxPages, 1, 0, false, 0, false),
    PropertyDescription(kOfxParamHostPropPageRowColumnCount, 2, 0, false, 0,
                        false)};

// some host property descriptions we may be interested int
class HostDescription : public PropertySet {
public:
  int hostIsBackground;
  int supportsOverlays;
  int supportsMultiResolution;
  int supportsTiles;
  int temporalClipAccess;
  int supportsMultipleClipDepths;
  int supportsMultipleClipPARs;
  int supportsSetableFrameRate;
  int supportsSetableFielding;
  int supportsCustomAnimation;
  int supportsStringAnimation;
  int supportsCustomInteract;
  int supportsChoiceAnimation;
  int supportsBooleanAnimation;
  int maxParameters;
  int maxPages;
  int pageRowCount;
  int pageColumnCount;

  HostDescription(OfxPropertySetHandle handle);
};
HostDescription *gHostDescription = 0;

// create a host description, checking properties on the way
HostDescription::HostDescription(OfxPropertySetHandle handle)
    : PropertySet(handle), hostIsBackground(false), supportsOverlays(false),
      supportsMultiResolution(false), supportsTiles(false),
      temporalClipAccess(false), supportsMultipleClipDepths(false),
      supportsMultipleClipPARs(false), supportsSetableFrameRate(false),
      supportsSetableFielding(false), supportsCustomAnimation(false),
      supportsStringAnimation(false), supportsCustomInteract(false),
      supportsChoiceAnimation(false), supportsBooleanAnimation(false),
      maxParameters(-1), maxPages(-1), pageRowCount(-1), pageColumnCount(-1) {
  spdlog::info("HostDescription::HostDescription - start ( fetching host "
               "description);\n{");

  // do basic existence checking with a PropertySetDescription
  PropertySetDescription hostPropSet("Host", handle, gHostPropDescription,
                                     sizeof(gHostPropDescription) /
                                         sizeof(PropertyDescription));
  hostPropSet.checkProperties();
  hostPropSet.checkDefaults();
  hostPropSet.retrieveValues(true);

  // now go through and fill in the host description
  hostIsBackground =
      hostPropSet.intPropValue(kOfxImageEffectHostPropIsBackground);
  supportsOverlays =
      hostPropSet.intPropValue(kOfxImageEffectPropSupportsOverlays);
  supportsMultiResolution =
      hostPropSet.intPropValue(kOfxImageEffectPropSupportsMultiResolution);
  supportsTiles = hostPropSet.intPropValue(kOfxImageEffectPropSupportsTiles);
  temporalClipAccess =
      hostPropSet.intPropValue(kOfxImageEffectPropTemporalClipAccess);
  supportsMultipleClipDepths =
      hostPropSet.intPropValue(kOfxImageEffectPropSupportsMultipleClipDepths);
  supportsMultipleClipPARs =
      hostPropSet.intPropValue(kOfxImageEffectPropSupportsMultipleClipPARs);
  supportsSetableFrameRate =
      hostPropSet.intPropValue(kOfxImageEffectPropSetableFrameRate);
  supportsSetableFielding =
      hostPropSet.intPropValue(kOfxImageEffectPropSetableFielding);

  supportsStringAnimation =
      hostPropSet.intPropValue(kOfxParamHostPropSupportsStringAnimation);
  supportsCustomInteract =
      hostPropSet.intPropValue(kOfxParamHostPropSupportsCustomInteract);
  supportsChoiceAnimation =
      hostPropSet.intPropValue(kOfxParamHostPropSupportsChoiceAnimation);
  supportsBooleanAnimation =
      hostPropSet.intPropValue(kOfxParamHostPropSupportsBooleanAnimation);
  supportsCustomAnimation =
      hostPropSet.intPropValue(kOfxParamHostPropSupportsCustomAnimation);
  maxParameters = hostPropSet.intPropValue(kOfxParamHostPropMaxParameters);
  maxPages = hostPropSet.intPropValue(kOfxParamHostPropMaxPages);
  pageRowCount =
      hostPropSet.intPropValue(kOfxParamHostPropPageRowColumnCount, 0);
  pageColumnCount =
      hostPropSet.intPropValue(kOfxParamHostPropPageRowColumnCount, 1);

  spdlog::info("}HostDescription::HostDescription - stop;");
}

////////////////////////////////////////////////////////////////////////////////
// test the memory suite
static void testMemorySuite(void) {
  spdlog::info("testMemorySuite - start();\n{");
  void *oneMeg;

  OfxStatus stat = gMemorySuite->memoryAlloc(NULL, 1024 * 1024, &oneMeg);
  if (stat != kOfxStatOK)
    spdlog::error(
        "OfxMemorySuiteV1::memoryAlloc failed to alloc 1MB, returned %s",
        mapStatus(stat));

  if (stat == kOfxStatOK) {
    // touch 'em all to see if it crashes
    char *lotsOfChars = (char *)oneMeg;
    for (int i = 0; i < 1024 * 1024; i++) {
      *lotsOfChars++ = 0;
    }

    stat = gMemorySuite->memoryFree(oneMeg);
    if (stat != kOfxStatOK)
      spdlog::error(
          "OfxMemorySuiteV1::memoryFree failed to free 1MB, returned %s",
          mapStatus(stat));
  }

  spdlog::info("}HostDescription::HostDescription - stop;");
}

////////////////////////////////////////////////////////////////////////////////
// how many times has actionLoad been called
static int gLoadCount = 0;

/** @brief Called at load */
static OfxStatus actionLoad(void) {
  spdlog::info("loadAction - start();\n{");
  if (gLoadCount != 0)
    spdlog::error(
        "Load action called more than once without unload being called;");
  gLoadCount++;

  OfxStatus status = kOfxStatOK;

  try {
    // fetch the suites
    if (gHost == 0)
      spdlog::error("Host pointer has not been set;");
    if (!gHost)
      throw kOfxStatErrBadHandle;

    if (gLoadCount == 1) {
      gEffectSuite =
          (OfxImageEffectSuiteV1 *)fetchSuite(kOfxImageEffectSuite, 1);
      gPropSuite = (OfxPropertySuiteV1 *)fetchSuite(kOfxPropertySuite, 1);
      gParamSuite = (OfxParameterSuiteV1 *)fetchSuite(kOfxParameterSuite, 1);
      gMemorySuite = (OfxMemorySuiteV1 *)fetchSuite(kOfxMemorySuite, 1);
      gThreadSuite =
          (OfxMultiThreadSuiteV1 *)fetchSuite(kOfxMultiThreadSuite, 1);
      gMessageSuite = (OfxMessageSuiteV1 *)fetchSuite(kOfxMessageSuite, 1);

      // OK check and fetch host information
      gHostDescription = new HostDescription(gHost->host);

      // fetch the interact suite if the host supports interaction
      if (gHostDescription->supportsOverlays ||
          gHostDescription->supportsCustomInteract)
        gInteractSuite = (OfxInteractSuiteV1 *)fetchSuite(kOfxInteractSuite, 1);

      // test the memory suite
      testMemorySuite();
    }
  }

  catch (int err) {
    status = err;
  }

  spdlog::info("}loadAction - stop;");
  return status;
}

/** @brief Called before unload */
static OfxStatus unLoadAction(void) {
  if (gLoadCount <= 0)
    spdlog::error("UnLoad action called without a corresponding load action "
                  "having been called;");
  gLoadCount--;

  // force these to null
  gEffectSuite = 0;
  gPropSuite = 0;
  gParamSuite = 0;
  gMemorySuite = 0;
  gThreadSuite = 0;
  gMessageSuite = 0;
  gInteractSuite = 0;
  return kOfxStatOK;
}

//  instance construction
static OfxStatus createInstance(OfxImageEffectHandle /*effect*/) {
  return kOfxStatOK;
}

// instance destruction
static OfxStatus destroyInstance(OfxImageEffectHandle /*effect*/) {
  return kOfxStatOK;
}

// tells the host what region we are capable of filling
OfxStatus getSpatialRoD(OfxImageEffectHandle /*effect*/,
                        OfxPropertySetHandle /*inArgs*/,
                        OfxPropertySetHandle /*outArgs*/) {
  return kOfxStatOK;
}

// tells the host how much of the input we need to fill the given window
OfxStatus getSpatialRoI(OfxImageEffectHandle /*effect*/,
                        OfxPropertySetHandle /*inArgs*/,
                        OfxPropertySetHandle /*outArgs*/) {
  return kOfxStatOK;
}

// Tells the host how many frames we can fill, only called in the general
// context. This is actually redundant as this is the default behaviour, but for
// illustrative purposes.
OfxStatus getTemporalDomain(OfxImageEffectHandle /*effect*/,
                            OfxPropertySetHandle /*inArgs*/,
                            OfxPropertySetHandle /*outArgs*/) {
  return kOfxStatOK;
}

// Set our clip preferences
static OfxStatus getClipPreferences(OfxImageEffectHandle /*effect*/,
                                    OfxPropertySetHandle /*inArgs*/,
                                    OfxPropertySetHandle /*outArgs*/) {
  return kOfxStatOK;
}

// are the settings of the effect performing an identity operation
static OfxStatus isIdentity(OfxImageEffectHandle /*effect*/,
                            OfxPropertySetHandle /*inArgs*/,
                            OfxPropertySetHandle /*outArgs*/) {
  // In this case do the default, which in this case is to render
  return kOfxStatReplyDefault;
}

////////////////////////////////////////////////////////////////////////////////
// function called when the instance has been changed by anything
static OfxStatus instanceChanged(OfxImageEffectHandle /*effect*/,
                                 OfxPropertySetHandle /*inArgs*/,
                                 OfxPropertySetHandle /*outArgs*/) {
  // don't trap any others
  return kOfxStatReplyDefault;
}

// the process code  that the host sees
static OfxStatus render(OfxImageEffectHandle /*instance*/,
                        OfxPropertySetHandle /*inArgs*/,
                        OfxPropertySetHandle /*outArgs*/) {
  return kOfxStatOK;
}

//  describe the plugin in context
static OfxStatus describeInContext(OfxImageEffectHandle /*effect*/,
                                   OfxPropertySetHandle /*inArgs*/) {
  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// code for the plugin's description routine

// contexts we can be
static const char *gSupportedContexts[] = {
    kOfxImageEffectContextGenerator,  kOfxImageEffectContextFilter,
    kOfxImageEffectContextTransition, kOfxImageEffectContextPaint,
    kOfxImageEffectContextGeneral,    kOfxImageEffectContextRetimer};

// pixel depths we can be
static const char *gSupportedPixelDepths[] = {
    kOfxBitDepthByte, kOfxBitDepthShort, kOfxBitDepthFloat};

// the values to set and the defaults to check on the various properties
static PropertyDescription gPluginPropertyDescriptions[] = {
    PropertyDescription(kOfxPropType, 1, "", false, kOfxTypeImageEffect, true),
    PropertyDescription(kOfxPropLabel, 1, "OFX Test Properties", true, "",
                        false),
    PropertyDescription(kOfxPropShortLabel, 1, "OFX Test Props", true, "",
                        false),
    PropertyDescription(kOfxPropLongLabel, 1, "OFX Test Properties", true, "",
                        false),
    PropertyDescription(kOfxPluginPropFilePath, 1, "", false, "", false),
    PropertyDescription(kOfxImageEffectPluginPropGrouping, 1, "OFX Example",
                        true, "", false),
    PropertyDescription(kOfxImageEffectPluginPropSingleInstance, 1, 0, true, 0,
                        true),
    PropertyDescription(kOfxImageEffectPluginRenderThreadSafety, 1,
                        kOfxImageEffectRenderFullySafe, true,
                        kOfxImageEffectRenderFullySafe, true),
    PropertyDescription(kOfxImageEffectPluginPropHostFrameThreading, 1, 0, true,
                        0, true),
    PropertyDescription(kOfxImageEffectPluginPropOverlayInteractV1, 1,
                        (void *)(0), true, (void *)(0), true),
    PropertyDescription(kOfxImageEffectPropSupportsMultiResolution, 1, 1, true,
                        1, true),
    PropertyDescription(kOfxImageEffectPropSupportsTiles, 1, 1, true, 1, true),
    PropertyDescription(kOfxImageEffectPropTemporalClipAccess, 1, 0, true, 0,
                        true),
    PropertyDescription(kOfxImageEffectPluginPropFieldRenderTwiceAlways, 1, 1,
                        true, 1, true),
    PropertyDescription(kOfxImageEffectPropSupportsMultipleClipDepths, 1, 0,
                        true, 0, true),
    PropertyDescription(kOfxImageEffectPropSupportsMultipleClipPARs, 1, 0, true,
                        0, true),
    PropertyDescription(
        kOfxImageEffectPropSupportedContexts, -1, gSupportedContexts,
        sizeof(gSupportedContexts) / sizeof(char *), gSupportedContexts, 0),
    PropertyDescription(kOfxImageEffectPropSupportedPixelDepths, -1,
                        gSupportedPixelDepths,
                        sizeof(gSupportedPixelDepths) / sizeof(char *),
                        gSupportedPixelDepths, 0)};

static OfxStatus actionDescribe(OfxImageEffectHandle effect) {
  // get the property handle for the plugin
  OfxPropertySetHandle effectProps;
  gEffectSuite->getPropertySet(effect, &effectProps);

  // check the defaults
  PropertySetDescription pluginPropSet(
      "Plugin", effectProps, gPluginPropertyDescriptions,
      sizeof(gPluginPropertyDescriptions) / sizeof(PropertyDescription));
  pluginPropSet.checkProperties();
  pluginPropSet.checkDefaults();
  pluginPropSet.retrieveValues(true);
  pluginPropSet.setValues();

  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////.
// check handles to the main function
static void checkMainHandles(const char *action, const void *handle,
                             OfxPropertySetHandle inArgsHandle,
                             OfxPropertySetHandle outArgsHandle,
                             bool handleCanBeNull, bool inArgsCanBeNull,
                             bool outArgsCanBeNull) {
  if (handleCanBeNull) {
    if (handle != 0) {
      spdlog::warn("Handle passed to '%s' is not null;", action);
    } else if (handle == 0) {
      spdlog::error("'Handle passed to '%s' is null;", action);
    }
  }

  if (inArgsCanBeNull) {
    if (inArgsHandle != 0) {
      spdlog::warn("'inArgs' Handle passed to '%s' is not null;", action);
    } else if (inArgsHandle == 0) {
      spdlog::error("'inArgs' handle passed to '%s' is null;", action);
    }
  }

  if (outArgsCanBeNull) {
    if (outArgsHandle != 0) {
      spdlog::warn("'outArgs' Handle passed to '%s' is not null;", action);
    } else if (outArgsHandle == 0) {
      spdlog::error("'outArgs' handle passed to '%s' is null;", action);
    }
  }

  if (!handleCanBeNull && !handle)
    throw kOfxStatErrBadHandle;
  if (!inArgsCanBeNull && !inArgsHandle)
    throw kOfxStatErrBadHandle;
  if (!outArgsCanBeNull && !outArgsHandle)
    throw kOfxStatErrBadHandle;
}

////////////////////////////////////////////////////////////////////////////////
// The main function
static OfxStatus pluginMain(const char *action, const void *handle,
                            OfxPropertySetHandle inArgsHandle,
                            OfxPropertySetHandle outArgsHandle) {
  spdlog::info("pluginMain - start();\n{");
  spdlog::info("  action is '%s';", action);
  OfxStatus stat = kOfxStatReplyDefault;

  try {
    // cast to handle appropriate type
    OfxImageEffectHandle effectHandle = (OfxImageEffectHandle)handle;

    // construct two property set wrappers
    PropertySet inArgs(inArgsHandle);
    PropertySet outArgs(outArgsHandle);

    if (!strcmp(action, kOfxActionLoad)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, true, true,
                       true);
      stat = actionLoad();
    } else if (!strcmp(action, kOfxActionUnload)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, true, true,
                       true);
      stat = unLoadAction();
    } else if (!strcmp(action, kOfxActionDescribe)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
      stat = actionDescribe(effectHandle);
    } else if (!strcmp(action, kOfxActionPurgeCaches)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
    } else if (!strcmp(action, kOfxActionSyncPrivateData)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
    } else if (!strcmp(action, kOfxActionCreateInstance)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
      stat = createInstance(effectHandle);
    } else if (!strcmp(action, kOfxActionDestroyInstance)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
      stat = destroyInstance(effectHandle);
    } else if (!strcmp(action, kOfxActionInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
    } else if (!strcmp(action, kOfxActionBeginInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
    } else if (!strcmp(action, kOfxActionEndInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
      stat = instanceChanged(effectHandle, inArgsHandle, outArgsHandle);
    } else if (!strcmp(action, kOfxActionBeginInstanceEdit)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
    } else if (!strcmp(action, kOfxActionEndInstanceEdit)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
    } else if (!strcmp(action, kOfxImageEffectActionGetRegionOfDefinition)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
    } else if (!strcmp(action, kOfxImageEffectActionGetRegionsOfInterest)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
    } else if (!strcmp(action, kOfxImageEffectActionGetTimeDomain)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       false);
    } else if (!strcmp(action, kOfxImageEffectActionGetFramesNeeded)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
    } else if (!strcmp(action, kOfxImageEffectActionGetClipPreferences)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
      stat = getClipPreferences(effectHandle, inArgsHandle, outArgsHandle);
    } else if (!strcmp(action, kOfxImageEffectActionIsIdentity)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
      stat = isIdentity(effectHandle, inArgsHandle, outArgsHandle);
    } else if (!strcmp(action, kOfxImageEffectActionRender)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
      stat = render(effectHandle, inArgsHandle, outArgsHandle);
    } else if (!strcmp(action, kOfxImageEffectActionBeginSequenceRender)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
    } else if (!strcmp(action, kOfxImageEffectActionEndSequenceRender)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
    } else if (!strcmp(action, kOfxImageEffectActionDescribeInContext)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
      stat = describeInContext(effectHandle, inArgsHandle);
    } else {
      spdlog::error("Unknown action '%s';", action);
    }
  } catch (std::bad_alloc) {
    // catch memory
    spdlog::error("OFX Plugin Memory error;");
    stat = kOfxStatErrMemory;
  } catch (const std::exception &e) {
    // standard exceptions
    spdlog::error("Plugin exception: '%s';", e.what());
    stat = kOfxStatErrUnknown;
  } catch (int err) {
    // ho hum, gone wrong somehow
    spdlog::error("Misc int plugin exception: '%s';", mapStatus(err));
    stat = err;
  } catch (...) {
    // everything else
    spdlog::error("Uncaught misc plugin exception;");
    stat = kOfxStatErrUnknown;
  }

  spdlog::info("}pluginMain - stop;");

  // other actions to take the default value
  return stat;
}

// function to set the host structure
static void setHostFunc(OfxHost *hostStruct) {
  spdlog::info("setHostFunc - start();\n{");
  if (hostStruct == 0)
    spdlog::error("host is a null pointer;");
  gHost = hostStruct;
  spdlog::info("}setHostFunc - stop;");
}

////////////////////////////////////////////////////////////////////////////////
// the plugin struct
static OfxPlugin basicPlugin = {kOfxImageEffectPluginApi,
                                1,
                                "net.sf.openfx:PropertyTestPlugin",
                                1,
                                0,
                                setHostFunc,
                                pluginMain};

// the two mandated functions
EXPORT OfxPlugin *OfxGetPlugin(int nth) {
  spdlog::info("OfxGetPlugin - start();\n{");
  spdlog::info("  asking for %dth plugin;", nth);
  if (nth != 0)
    spdlog::error(
        "requested plugin %d is more than the number of plugins in the file;",
        nth);
  spdlog::info("}OfxGetPlugin - stop;");

  if (nth == 0)
    return &basicPlugin;
  return 0;
}

EXPORT int OfxGetNumberOfPlugins(void) {
  spdlog::info("OfxGetNumberOfPlugins - start();\n{");
  spdlog::info("}OfxGetNumberOfPlugins - stop;");
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// globals destructor, the destructor is called when the plugin is unloaded
class GlobalDestructor {
public:
  ~GlobalDestructor();
};

GlobalDestructor::~GlobalDestructor() {
  if (gHostDescription)
    delete gHostDescription;
}

static GlobalDestructor globalDestructor;
