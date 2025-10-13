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
#include "openfx/ofxPropsAccess.h"
#include "openfx/ofxPropsBySet.h"
#include "openfx/ofxPropsMetadata.h"
#include "spdlog/spdlog.h"
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
      spdlog::warn("Could not fetch the optional suite '{}' version {};",
                   suiteName, suiteVersion);
  } else {
    if (suite == 0)
      spdlog::error("Could not fetch the mandatory suite '{}' version {};",
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

// ========================================================================
// Read all props in a prop set
// ========================================================================

using PropValue = std::variant<int, double, const char *, void *>;

struct PropRecord {
  const PropDef &def;
  std::vector<PropValue> values;
};

void readProperty(PropertyAccessor &accessor, const PropDef &def,
                  std::vector<PropValue> &values) {
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
          spdlog::warn("Property '{}' has invalid enum value '{}' (not in spec)",
                      def.name, strValue);
          // Log valid values for debugging
          std::string validValues;
          for (size_t j = 0; j < def.enumValues.size(); ++j) {
            if (j > 0) validValues += ", ";
            validValues += def.enumValues[j];
          }
          spdlog::warn("  Valid values are: {}", validValues);
        }
      }
    } else if (primaryType == PropType::Pointer) {
      values.push_back(accessor.getRaw<void *>(def.name, i));
    }
  }
}

std::vector<PropRecord> getAllPropertiesOfSet(PropertyAccessor &accessor,
                                              const char *propertySetName) {
  std::vector<PropRecord> result;

  // Find the property set in the map
  auto setIt = prop_sets.find(propertySetName);
  if (setIt == prop_sets.end()) {
    spdlog::error("Property set not found: {}", propertySetName);
    return {};
  }

  // Read each property and push onto result
  for (const auto &prop : setIt->second) {
    // Use template dispatch to get property with correct type
    std::vector<PropValue> values;
    readProperty(accessor, prop.def, values);
    result.push_back({prop.def, std::move(values)});
  }
  return result;
}

/**
 * Log all property values gotten from getAllPropertiesOfSet()
 */
void logPropValues(const std::string_view setName,
                   const std::vector<PropRecord> &props) {
  spdlog::info("Properties for {}:", setName);
  for (const auto &[propDef, values] : props) {
    // Build up the log string piecemeal
    std::string buf;
    fmt::format_to(std::back_inserter(buf), "  {} ({}d) = [", propDef.name,
                   values.size());
    for (size_t i = 0; i < values.size(); ++i) {
      std::visit(
          [&buf](auto &&value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
              fmt::format_to(std::back_inserter(buf), "{}", value);
            } else if constexpr (std::is_same_v<T, double>) {
              fmt::format_to(std::back_inserter(buf), "{}", value);
            } else if constexpr (std::is_same_v<T, const char *>) {
              fmt::format_to(std::back_inserter(buf), "{}", value);
            } else if constexpr (std::is_same_v<T, void *>) {
              fmt::format_to(std::back_inserter(buf), "{:p}", value);
            }
          },
          values[i]);
      if (i < values.size() - 1)
        fmt::format_to(std::back_inserter(buf), ",");
    }
    fmt::format_to(std::back_inserter(buf), "]");
    // log it
    spdlog::info("{}", buf);
  }
}

// ========================================================================

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

      // Get all host props, propset name "ImageEffectHost"
      // (too bad prop sets don't know their own name)
      PropertyAccessor accessor = PropertyAccessor(gHost->host, gPropSuite);
      const auto prop_values =
          getAllPropertiesOfSet(accessor, "ImageEffectHost");
      logPropValues("ImageEffectHost", prop_values);
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
static OfxStatus describeInContext(OfxImageEffectHandle effect,
                                   OfxPropertySetHandle inArgs) {
  PropertyAccessor accessor = PropertyAccessor(inArgs, gPropSuite);
  spdlog::info("describeInContext: inArgs->context = {}",
               accessor.get<PropId::OfxImageEffectPropContext>());

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
  accessor.set<PropId::OfxParamPropDefault, double>(0)
      .set<PropId::OfxParamPropHint>("Enables scales on individual components")
      .set<PropId::OfxParamPropScriptName>("scale")
      .set<PropId::OfxPropLabel>("Scale Param");

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

  accessor.set<PropId::OfxPropLabel>("Property Tester")
      .set<PropId::OfxPropVersionLabel>("1.0")
      .setAll<PropId::OfxPropVersion>({1, 0, 0})
      .set<PropId::OfxPropPluginDescription>(
          "Sample plugin which logs all actions and properties")
      .set<PropId::OfxImageEffectPluginPropGrouping>("OFX Examples")
      .set<PropId::OfxImageEffectPropMultipleClipDepths>(false)
      .setAll<PropId::OfxImageEffectPropSupportedComponents>(
          {kOfxImageComponentRGBA, kOfxImageComponentAlpha})
      .setAll<PropId::OfxImageEffectPropSupportedContexts>(supportedContexts)
      .setAll<PropId::OfxImageEffectPropSupportedPixelDepths>(
          supportedPixelDepths);

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
      spdlog::warn("Handle passed to '{}' is not null;", action);
    } else if (handle == 0) {
      spdlog::error("'Handle passed to '{}' is null;", action);
    }
  }

  if (inArgsCanBeNull) {
    if (inArgsHandle != 0) {
      spdlog::warn("'inArgs' Handle passed to '{}' is not null;", action);
    } else if (inArgsHandle == 0) {
      spdlog::error("'inArgs' handle passed to '{}' is null;", action);
    }
  }

  if (outArgsCanBeNull) {
    if (outArgsHandle != 0) {
      spdlog::warn("'outArgs' Handle passed to '{}' is not null;", action);
    } else if (outArgsHandle == 0) {
      spdlog::error("'outArgs' handle passed to '{}' is null;", action);
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
  spdlog::info("  action is '{}';", action);
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
      spdlog::error("Unknown action '{}';", action);
    }
  } catch (std::bad_alloc) {
    // catch memory
    spdlog::error("OFX Plugin Memory error;");
    stat = kOfxStatErrMemory;
  } catch (const std::exception &e) {
    // standard exceptions
    spdlog::error("Plugin exception: '{}';", e.what());
    stat = kOfxStatErrUnknown;
  } catch (int err) {
    // ho hum, gone wrong somehow
    spdlog::error("Misc int plugin exception: '{}';", mapStatus(err));
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
  spdlog::info("  asking for {}th plugin;", nth);
  if (nth != 0)
    spdlog::error(
        "requested plugin {} is more than the number of plugins in the file;",
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
// global destructor, the destructor is called when the plugin is unloaded
class GlobalDestructor {
public:
  ~GlobalDestructor();
};

GlobalDestructor::~GlobalDestructor() {}

static GlobalDestructor globalDestructor;
