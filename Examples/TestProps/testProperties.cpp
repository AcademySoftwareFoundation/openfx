// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

/** @file testProperties.cpp OFX test plugin which logs all properties
    in various actions.

    Uses openfx::Logger for logging.
*/
#include "ofxImageEffect.h"
#include "ofxMemory.h"
#include "ofxMessage.h"
#include "ofxMultiThread.h"
#include "openfx/ofxPropsAccess.h"
#include "openfx/ofxPropsBySet.h"
#include "openfx/ofxPropsMetadata.h"
#include "openfx/ofxPropSetAccessors.h"  // Type-safe property set accessor classes
#include "openfx/ofxLog.h"                 // OpenFX logging
#include "../openfx-cpp/examples/host-specific-props/myhost/myhostPropsMetadata.h"  // Ensure example compiles
#include <map>    // stl maps
#include <string> // stl strings
#include <variant>
#include <vector>

#if defined __APPLE__ || defined __linux__ || defined __FreeBSD__
#define EXPORT __attribute__((visibility("default")))
#elif defined _WIN32
#define EXPORT OfxExport
#else
#error Not building on your operating system quite yet
#endif

using namespace openfx; // for props access

// Plugin identification
static constexpr const char* kPluginName = "PropertyTester";

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
      Logger::warn("Could not fetch the optional suite '{}' version {}", suiteName, suiteVersion);
  } else {
    if (suite == 0)
      Logger::error("Could not fetch the mandatory suite '{}' version {}", suiteName, suiteVersion);
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

// ========================================================================
// Read all props in a prop set
// ========================================================================

using PropValue = std::variant<int, double, const char *, void *>;

struct PropRecord {
  const PropDef &def;
  std::vector<PropValue> values;
};

// Fills in values and returns true if property exists, false (and no values) otherwise
bool readProperty(PropertyAccessor &accessor, const PropDef &def,
                  std::vector<PropValue> &values) {
  if (!accessor.exists(def.name)) {
    values.clear();
    return false;
  }
  int dimension = accessor.getDimensionRaw(def.name);

  values.clear();
  values.reserve(dimension);

  // Read all values based on the primary type
  PropType primaryType = def.supportedTypes[0];

  for (int i = 0; i < dimension; ++i) {
    if (primaryType == PropType::Int || primaryType == PropType::Bool) {
      values.push_back(accessor.getRaw<int>(def.name, i));
    } else if (primaryType == PropType::Double) {
      values.push_back(accessor.getRaw<double>(def.name, i));
    } else if (primaryType == PropType::String ||
               primaryType == PropType::Enum) {
      const char *strValue = accessor.getRaw<const char *>(def.name, i);
      values.push_back(strValue);

      // Validate enum values against spec
      if (primaryType == PropType::Enum && !def.enumValues.empty()) {
        bool valid = false;
        for (auto validValue : def.enumValues) {
          if (std::strcmp(strValue, validValue) == 0) {
            valid = true;
            break;
          }
        }
        if (!valid) {
          Logger::warn("Property '{}' has invalid enum value '{}' (not in spec)",
                      def.name, strValue);
          // Log valid values for debugging
          std::string validValues;
          for (size_t j = 0; j < def.enumValues.size(); ++j) {
            if (j > 0) validValues += ", ";
            validValues += def.enumValues[j];
          }
          Logger::warn("  Valid values are: {}", validValues);
        }
      }
    } else if (primaryType == PropType::Pointer) {
      values.push_back(accessor.getRaw<void *>(def.name, i));
    }
  }
  return true;
}

std::vector<PropRecord> getAllPropertiesOfSet(PropertyAccessor &accessor,
                                              const char *propertySetName) {
  std::vector<PropRecord> result;

  // Find the property set in the map
  auto setIt = prop_sets.find(propertySetName);
  if (setIt == prop_sets.end()) {
    Logger::error("Property set not found: {}", propertySetName);
    return {};
  }

  // Read each property and push onto result
  for (const auto &prop : setIt->second) {
    // Use template dispatch to get property with correct type
    std::vector<PropValue> values;
    bool status = readProperty(accessor, prop.def, values);
    if (status)
      result.push_back({prop.def, std::move(values)});
    else
      Logger::warn("Property not found: {}.{}", propertySetName, prop.def.name);
  }
  return result;
}

/**
 * Log all property values gotten from getAllPropertiesOfSet()
 */
void logPropValues(const std::string_view setName,
                   const std::vector<PropRecord> &props) {
  Logger::info("Properties for {}", setName);
  for (const auto &[propDef, values] : props) {
    // Build up the log string with property values
    std::ostringstream buf;
    buf << "  " << propDef.name << " (" << values.size() << "d) = [";
    for (size_t i = 0; i < values.size(); ++i) {
      std::visit(
          [&buf](auto &&value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
              buf << value;
            } else if constexpr (std::is_same_v<T, double>) {
              buf << value;
            } else if constexpr (std::is_same_v<T, const char *>) {
              buf << value;
            } else if constexpr (std::is_same_v<T, void *>) {
              buf << value;
            }
          },
          values[i]);
      if (i < values.size() - 1)
        buf << ",";
    }
    buf << "]";
    // log it
    Logger::info("{}", buf.str());
  }
}

/**
 * Test property set compliance - verify all properties in a property set are accessible
 * Returns number of failures
 *
 * Can test either:
 * 1. Regular property sets: testPropertySetCompliance(accessor, "EffectDescriptor")
 * 2. Action arguments: testPropertySetCompliance(accessor, kOfxActionRender, "inArgs")
 */
int testPropertySetCompliance(PropertyAccessor &accessor, const char *setName, const char *argType = nullptr) {
  std::string testName;
  std::vector<const char*> propNames;
  std::vector<const PropDef*> propDefs;
  std::vector<bool> propOptional;

  if (argType == nullptr) {
    // Testing a regular property set
    testName = std::string(setName);
    Logger::info("========================================");
    Logger::info("Testing property set compliance: {}", testName);
    Logger::info("========================================");

    // Find the property set
    auto setIt = prop_sets.find(setName);
    if (setIt == prop_sets.end()) {
      Logger::error("Property set '{}' not found in prop_sets", setName);
      return 1;
    }

    // Build lists from prop_sets map
    for (const auto &prop : setIt->second) {
      propNames.push_back(prop.def.name);
      propDefs.push_back(&prop.def);
      propOptional.push_back(prop.host_optional);
    }
  } else {
    // Testing action arguments
    testName = std::string(setName) + "." + argType;
    Logger::info("========================================");
    Logger::info("Testing action argument compliance: {}", testName);
    Logger::info("========================================");

    // Find the action's property list
    std::array<std::string_view, 2> key = {setName, argType};
    auto setIt = action_props.find(key);
    if (setIt == action_props.end()) {
      Logger::error("Action argument '{}' not found in action_props", testName);
      return 1;
    }

    // Build lists from action_props map
    for (const char* propName : setIt->second) {
      propNames.push_back(propName);

      // Find property definition
      auto propIt = std::find_if(prop_defs.data.begin(), prop_defs.data.end(),
                                   [propName](const PropDef& def) {
                                     return std::string_view(def.name) == std::string_view(propName);
                                   });

      if (propIt == prop_defs.data.end()) {
        Logger::error("  ✗ {} - property definition not found", propName);
        propDefs.push_back(nullptr);
        propOptional.push_back(false);
        continue;
      }

      propDefs.push_back(&(*propIt));
      propOptional.push_back(false);  // Action args are typically not marked as optional
    }
  }

  // Test each property
  int totalProps = 0;
  int accessibleProps = 0;
  int requiredMissing = 0;
  int optionalMissing = 0;
  int typeErrors = 0;

  for (size_t i = 0; i < propNames.size(); ++i) {
    totalProps++;
    const char* propName = propNames[i];
    const PropDef* propDef = propDefs[i];
    bool isOptional = propOptional[i];

    if (propDef == nullptr) {
      typeErrors++;
      continue;
    }

    try {
      bool exists = accessor.exists(propName);
      if (!exists)
        throw openfx::PropertyNotFoundException(-1);

      // Try to get dimension (this will fail if property doesn't exist)
      int dimension = accessor.getDimensionRaw(propName);

      // Verify dimension matches spec (0 means variable dimension)
      if (propDef->dimension != 0 && dimension != propDef->dimension) {
        Logger::warn("  ✗ {} - dimension mismatch: expected {}, got {}",
                    propName, propDef->dimension, dimension);
      }

      // Try to read the property based on its type
      std::vector<PropValue> values;
      try {
        readProperty(accessor, *propDef, values);
        accessibleProps++;

        // For optional properties that are present, log them
        if (isOptional) {
          Logger::info("  ✓ {} - accessible (optional, dimension={})", propName, dimension);
        }
      } catch (const std::exception& e) {
        typeErrors++;
        Logger::error("  ✗ {} - type error: {}", propName, e.what());
      }

    } catch (...) {
      // Property not accessible
      if (isOptional) {
        optionalMissing++;
        // Silently skip optional missing properties
      } else {
        requiredMissing++;
        Logger::warn("  ✗ {} - NOT ACCESSIBLE{}", propName, argType ? "" : " (required!)");
      }
    }
  }

  // Summary
  Logger::info("----------------------------------------");
  Logger::info("'{}' compliance results:", testName);
  Logger::info("  Total properties defined: {}", totalProps);
  Logger::info("  Accessible: {}", accessibleProps);
  if (argType == nullptr) {
    Logger::info("  Required missing: {}", requiredMissing);
    Logger::info("  Optional missing: {}", optionalMissing);
  } else {
    Logger::info("  Missing: {}", requiredMissing + optionalMissing);
  }
  Logger::info("  Type errors: {}", typeErrors);

  int failures = requiredMissing + typeErrors;
  if (failures == 0) {
    Logger::info("  ✓ COMPLIANCE TEST PASSED");
  } else {
    Logger::error("  ✗ COMPLIANCE TEST FAILED ({} issues)", failures);
  }
  Logger::info("========================================");

  return failures;
}

// ========================================================================

////////////////////////////////////////////////////////////////////////////////
// how many times has actionLoad been called
static int gLoadCount = 0;

/** @brief Called at load */
static OfxStatus actionLoad(void) {
  Logger::info("loadAction()");
  if (gLoadCount != 0)
    Logger::error("Load action called more than once without unload being called");
  gLoadCount++;

  OfxStatus status = kOfxStatOK;

  try {
    // fetch the suites
    if (gHost == 0)
      Logger::error("Host pointer has not been set");
    if (!gHost)
      throw kOfxStatErrBadHandle;

    if (gLoadCount == 1) {
      Logger::info("loadAction - loading suites");
      gEffectSuite =
          (OfxImageEffectSuiteV1 *)fetchSuite(kOfxImageEffectSuite, 1);
      gPropSuite = (OfxPropertySuiteV1 *)fetchSuite(kOfxPropertySuite, 1);
      gParamSuite = (OfxParameterSuiteV1 *)fetchSuite(kOfxParameterSuite, 1);
      gMemorySuite = (OfxMemorySuiteV1 *)fetchSuite(kOfxMemorySuite, 1);
      gThreadSuite =
          (OfxMultiThreadSuiteV1 *)fetchSuite(kOfxMultiThreadSuite, 1);
      gMessageSuite = (OfxMessageSuiteV1 *)fetchSuite(kOfxMessageSuite, 1);

      Logger::info("loadAction - getting all props...");
      // Get all host props, property set name "ImageEffectHost"
      PropertyAccessor accessor = PropertyAccessor(gHost->host, gPropSuite);
      const auto prop_values =
          getAllPropertiesOfSet(accessor, "ImageEffectHost");
      Logger::info("loadAction - got {} props", prop_values.size());
      logPropValues("ImageEffectHost", prop_values);

      // Test host property set compliance
      Logger::info("loadAction - testing compliance...");
      testPropertySetCompliance(accessor, "ImageEffectHost");
    }
  }

  catch (int err) {
    Logger::error("loadAction - caught err {}", err);
    status = err;
  }
  catch (...) {
    Logger::error("loadAction - caught unknown err");
    status = kOfxStatErrFatal;
  }

  Logger::info("loadAction returning {}", status);
  return status;
}

/** @brief Called before unload */
static OfxStatus unLoadAction(void) {
  if (gLoadCount <= 0)
    Logger::error("UnLoad action called without a corresponding load action having been called");
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
static OfxStatus instanceChanged(OfxImageEffectHandle instance,
                                 OfxPropertySetHandle inArgs,
                                 OfxPropertySetHandle outArgs) {
  // Test property set compliance
  PropertyAccessor in_accessor = PropertyAccessor(inArgs, gPropSuite);
  testPropertySetCompliance(in_accessor, kOfxActionInstanceChanged, "inArgs");
  PropertyAccessor out_accessor = PropertyAccessor(outArgs, gPropSuite);
  testPropertySetCompliance(out_accessor, kOfxActionInstanceChanged, "outArgs");

  auto accessor = PropertyAccessor(instance, gEffectSuite, gPropSuite);
  testPropertySetCompliance(accessor, "EffectInstance");

  // don't trap any others
  return kOfxStatReplyDefault;
}

// the process code  that the host sees
static OfxStatus render(OfxImageEffectHandle instance,
                        OfxPropertySetHandle inArgs,
                        OfxPropertySetHandle outArgs) {
  // Test property set compliance
  PropertyAccessor in_accessor = PropertyAccessor(inArgs, gPropSuite);
  testPropertySetCompliance(in_accessor, kOfxImageEffectActionRender, "inArgs");
  PropertyAccessor out_accessor = PropertyAccessor(outArgs, gPropSuite);
  testPropertySetCompliance(out_accessor, kOfxImageEffectActionRender, "outArgs");

  auto accessor = PropertyAccessor(instance, gEffectSuite, gPropSuite);
  testPropertySetCompliance(accessor, "EffectInstance");

  return kOfxStatOK;
}


static OfxStatus getRegionOfDefinition(OfxImageEffectHandle instance,
                        OfxPropertySetHandle inArgs,
                        OfxPropertySetHandle outArgs) {
  // Test property set compliance
  PropertyAccessor in_accessor = PropertyAccessor(inArgs, gPropSuite);
  testPropertySetCompliance(in_accessor, kOfxImageEffectActionGetRegionOfDefinition, "inArgs");
  PropertyAccessor out_accessor = PropertyAccessor(outArgs, gPropSuite);
  testPropertySetCompliance(out_accessor, kOfxImageEffectActionGetRegionOfDefinition, "outArgs");

  auto accessor = PropertyAccessor(instance, gEffectSuite, gPropSuite);
  testPropertySetCompliance(accessor, "EffectInstance");

  return kOfxStatReplyDefault;
}

static OfxStatus getRegionsOfInterest(OfxImageEffectHandle instance,
                        OfxPropertySetHandle inArgs,
                        OfxPropertySetHandle outArgs) {
  // Test property set compliance
  PropertyAccessor in_accessor = PropertyAccessor(inArgs, gPropSuite);
  testPropertySetCompliance(in_accessor, kOfxImageEffectActionGetRegionsOfInterest, "inArgs");
  // No outArgs here
  // PropertyAccessor out_accessor = PropertyAccessor(outArgs, gPropSuite);
  // testPropertySetCompliance(out_accessor, kOfxImageEffectActionGetRegionsOfInterest, "outArgs");

  auto accessor = PropertyAccessor(instance, gEffectSuite, gPropSuite);
  testPropertySetCompliance(accessor, "EffectInstance");

  return kOfxStatReplyDefault;
}

//  describe the plugin in context
static OfxStatus describeInContext(OfxImageEffectHandle effect,
                                   OfxPropertySetHandle inArgs) {
  // Test property set compliance
  PropertyAccessor accessor = PropertyAccessor(inArgs, gPropSuite);
  testPropertySetCompliance(accessor, kOfxImageEffectActionDescribeInContext, "inArgs");
  accessor = PropertyAccessor(effect, gEffectSuite, gPropSuite);
  testPropertySetCompliance(accessor, "EffectDescriptor");

  OfxPropertySetHandle props;
  // define the output clip
  gEffectSuite->clipDefine(effect, kOfxImageEffectOutputClipName, &props);
  accessor = PropertyAccessor(props, gPropSuite);
  accessor.setAll<PropId::OfxImageEffectPropSupportedComponents>(
      {kOfxImageComponentRGBA, kOfxImageComponentAlpha});

  // define the single source clip
  gEffectSuite->clipDefine(effect, kOfxImageEffectSimpleSourceClipName, &props);
  accessor = PropertyAccessor(props, gPropSuite);
  accessor.setAll<PropId::OfxImageEffectPropSupportedComponents>(
      {kOfxImageComponentRGBA, kOfxImageComponentAlpha});

  // Params
  OfxParamSetHandle paramSet;
  gEffectSuite->getParamSet(effect, &paramSet);

  // simple param test
  gParamSuite->paramDefine(paramSet, kOfxParamTypeDouble, "scale", &props);
  accessor = PropertyAccessor(props, gPropSuite);
  // Traditional API - note explicit type for multi-type property
  accessor.set<PropId::OfxParamPropDefault, double>(0)
      .set<PropId::OfxParamPropHint>("Enables scales on individual components")
      .set<PropId::OfxParamPropScriptName>("scale")
      .set<PropId::OfxPropLabel>("Scale Param");

  // NEW: Can also use type-safe property set accessor for multi-type properties
  // The accessor class provides templated methods for multi-type properties
  // Note: dimension=0 properties (like Min/Max/Default) still need index parameter
  openfx::ParamDouble1D paramDesc(accessor);
  paramDesc.setDefault<double>(1.0);  // dimension=0, so default index=0
  paramDesc.setMin<double>(0.0, 0);   // explicit index for dimension=0
  paramDesc.setMax<double>(10.0, 0);  // explicit index for dimension=0
  Logger::info("  Using ParamDouble1D accessor with multi-type properties!");

  // Log all the effect descriptor's props
  OfxPropertySetHandle effectProps;
  gEffectSuite->getPropertySet(effect, &effectProps);
  PropertyAccessor effect_accessor = PropertyAccessor(effectProps, gPropSuite);
  const auto prop_values =
      getAllPropertiesOfSet(effect_accessor, "EffectDescriptor");
  logPropValues("EffectDescriptor", prop_values);

  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// code for the plugin's description routine

// contexts we support
static std::vector<const char *> supportedContexts{
    kOfxImageEffectContextGenerator,  kOfxImageEffectContextFilter,
    kOfxImageEffectContextTransition, kOfxImageEffectContextPaint,
    kOfxImageEffectContextGeneral,    kOfxImageEffectContextRetimer};

// pixel depths we support
static std::vector<const char *> supportedPixelDepths{
    kOfxBitDepthByte, kOfxBitDepthShort, kOfxBitDepthFloat};

static OfxStatus actionDescribe(OfxImageEffectHandle effect) {
  // get the property handle for the plugin
  OfxPropertySetHandle effectProps;
  gEffectSuite->getPropertySet(effect, &effectProps);


  PropertyAccessor accessor = PropertyAccessor(effectProps, gPropSuite);
  // Test property set compliance
  testPropertySetCompliance(accessor, "EffectDescriptor");

  // Low-level PropertyAccessor API
  accessor.set<PropId::OfxPropLabel>("Property Tester v2")
    .set<PropId::OfxPropVersionLabel>("1.0", 0, false)
    .setAll<PropId::OfxPropVersion>({1, 0, 0}, false)
    .set<PropId::OfxPropPluginDescription>(
                                           "Sample plugin which logs all actions and properties", 0, false)
    .set<PropId::OfxImageEffectPluginPropGrouping>("OFX Examples")
    .set<PropId::OfxImageEffectPropMultipleClipDepths>(false)
    .setAll<PropId::OfxImageEffectPropSupportedContexts>(supportedContexts)
    .setAll<PropId::OfxImageEffectPropSupportedPixelDepths>(
                                                            supportedPixelDepths);

  // OR: high-level, simpler (still type-safe) property set accessor API
  Logger::info("Testing property set accessor classes...");
  openfx::EffectDescriptor effectDesc(accessor);

  // Simplified setters via property set class (same as above, but more convenient)
  // Also chainable for fluent interface!
  effectDesc.setLabel("Property Tester V2")
      .setVersionLabel("1.0")
      .setVersion({1,0,0})
      .setPluginDescription("Sample plugin, logging all actions & properties")
      .setImageEffectPluginPropGrouping("OFX Examples")
      .setMultipleClipDepths(false)
    .setSupportedContexts(supportedContexts)
    .setSupportedPixelDepths(supportedPixelDepths);

  // Test host-specific property extensibility (will fail at runtime but compiles!)
  Logger::info("Testing host-specific property extensibility...");
  try {
    // Test myhost properties (from examples/host-specific-props)
    auto viewerProcess = accessor.get<myhost::PropId::MyHostViewerProcess>(0, false);
    Logger::info("  MyHost viewer process: {}", viewerProcess ? viewerProcess : "(not available)");

    auto projectPath = accessor.get<myhost::PropId::MyHostProjectPath>(0, false);
    Logger::info("  MyHost project path: {}", projectPath ? projectPath : "(not available)");

    auto nodeColor = accessor.getAll<myhost::PropId::MyHostNodeColor>();
    Logger::info("  MyHost node color dimension: {}", nodeColor.size());

    // Test setting host properties
    accessor.setAll<myhost::PropId::MyHostNodeColor>({255, 128, 64});

  } catch (const PropertyNotFoundException& e) {
    Logger::info("  Host properties not available (expected) - but compilation succeeded!");
  } catch (...) {
    Logger::info("  Host properties failed (expected) - but compilation succeeded!");
  }

  // After setting up, log all known props
  const auto prop_values = getAllPropertiesOfSet(accessor, "EffectDescriptor");
  logPropValues("EffectDescriptor", prop_values);

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
      Logger::warn("Handle passed to '{}' is not null", action);
    } else if (handle == 0) {
      Logger::error("'Handle passed to '{}' is null", action);
    }
  }

  if (inArgsCanBeNull) {
    if (inArgsHandle != 0) {
      Logger::warn("'inArgs' Handle passed to '{}' is not null", action);
    } else if (inArgsHandle == 0) {
      Logger::error("'inArgs' handle passed to '{}' is null", action);
    }
  }

  if (outArgsCanBeNull) {
    if (outArgsHandle != 0) {
      Logger::warn("'outArgs' Handle passed to '{}' is not null", action);
    } else if (outArgsHandle == 0) {
      Logger::error("'outArgs' handle passed to '{}' is null", action);
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
  Logger::info(">>> {}", action);
  OfxStatus stat = kOfxStatReplyDefault;

  try {
    // cast to handle appropriate type
    OfxImageEffectHandle effectHandle = (OfxImageEffectHandle)handle;

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
      stat = instanceChanged(effectHandle, inArgsHandle, outArgsHandle);
    } else if (!strcmp(action, kOfxActionBeginInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
    } else if (!strcmp(action, kOfxActionEndInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, true);
    } else if (!strcmp(action, kOfxActionBeginInstanceEdit)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
    } else if (!strcmp(action, kOfxActionEndInstanceEdit)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true,
                       true);
    } else if (!strcmp(action, kOfxImageEffectActionGetRegionOfDefinition)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
      stat = getRegionOfDefinition(effectHandle, inArgsHandle, outArgsHandle);
    } else if (!strcmp(action, kOfxImageEffectActionGetRegionsOfInterest)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false,
                       false, false);
      stat = getRegionsOfInterest(effectHandle, inArgsHandle, outArgsHandle);
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
      Logger::error("Unknown action '{}'", action);
    }
  } catch (std::bad_alloc) {
    // catch memory
    Logger::error("OFX Plugin Memory error");
    stat = kOfxStatErrMemory;
  } catch (const std::exception &e) {
    // standard exceptions
    Logger::error("Plugin exception: '{}'", e.what());
    stat = kOfxStatErrUnknown;
  } catch (int err) {
    // ho hum, gone wrong somehow
    Logger::error("Misc int plugin exception: '{}'", mapStatus(err));
    stat = err;
  } catch (...) {
    // everything else
    Logger::error("Uncaught misc plugin exception");
    stat = kOfxStatErrUnknown;
  }

  Logger::info("<<< {} = {}", action, stat);

  // other actions to take the default value
  return stat;
}

// function to set the host structure
static void setHostFunc(OfxHost *hostStruct) {
  // Set the plugin name context for all logging
  Logger::setContext(kPluginName);

  Logger::info("setHostFunc()");
  if (hostStruct == 0)
    Logger::error("host is a null pointer");
  gHost = hostStruct;
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
  Logger::info("OfxGetPlugin - start()");
  Logger::info("  asking for {}th plugin", nth);
  if (nth != 0)
    Logger::error("requested plugin {} is more than the number of plugins in the file", nth);
  Logger::info("OfxGetPlugin - stop");

  if (nth == 0)
    return &basicPlugin;
  return 0;
}

EXPORT int OfxGetNumberOfPlugins(void) {
  Logger::info("OfxGetNumberOfPlugins - start()");
  Logger::info("OfxGetNumberOfPlugins - stop");
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// global destructor, the destructor is called when the plugin is unloaded
class GlobalDestructor {
public:
  ~GlobalDestructor();
};

GlobalDestructor::~GlobalDestructor() {}

static GlobalDestructor globalDestructor;
