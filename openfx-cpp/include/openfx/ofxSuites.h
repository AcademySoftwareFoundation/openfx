/**************************************************************/
/*                                                            */
/*  Copyright 2025 GoPro, Inc.                                */
/*  All Rights Reserved.                                      */
/*                                                            */
/**************************************************************/

#pragma once

#include <ofxCore.h>
#include <ofxDialog.h>
#include <ofxDrawSuite.h>
#include <ofxGPURender.h>
#include <ofxImageEffect.h>
#include <ofxMessage.h>
#include <ofxParam.h>
#include <ofxParametricParam.h>
#include <ofxProgress.h>
#include <ofxProperty.h>
#include <ofxTimeLine.h>

#include <cassert>
#include <string>
#include <unordered_map>

/***
  Usage:

  === Adding suites in onLoad:

    openfx::SuiteContainer gSuites;

    OfxStatus OnLoad() {
      OfxParameterSuiteV1* paramSuite =
        static_cast<OfxParameterSuiteV1*>(gHost->fetchSuite(gHost->host, kOfxParameterSuite, 1));
      if (paramSuite)
        gSuites.add(kOfxParameterSuite, 1, paramSuite);

      // or use the convenience macro:
      OPENFX_FETCH_SUITE(gSuites, kOfxParameterSuite, 1, OfxParameterSuiteV1);
      OPENFX_FETCH_SUITE(gSuites, kOfxPropertySuite, 1, OfxPropertySuiteV1);
      // ...
    }

    In both cases, missing suites will be null, Use gSuites.has<type>() to check.

  === Getting suites:
    auto paramSuite = gSuites.get<OfxParameterSuiteV1>();
    auto propSuite = gSuites.get<OfxPropertySuiteV1>();
    auto myCustomSuite = gSuites.get<customSuiteType>("customSuiteName");
    if (suites.has<OfxProgressSuiteV2>()) {
      // ...
    }
*/

// Use this in onLoad action to fetch suites and store them in the suites container
#define OPENFX_FETCH_SUITE(container, suiteName, suiteVersion, suiteType)                       \
  do {                                                                                          \
    const suiteType* suite =                                                                    \
        static_cast<const suiteType*>(gHost->fetchSuite(gHost->host, suiteName, suiteVersion)); \
    if (suite) {                                                                                \
      container.add(suiteName, suiteVersion, suite);                                            \
    }                                                                                           \
  } while (0)

namespace openfx {

namespace detail {
struct SuiteKey {
  std::string name;
  int version;

  SuiteKey(const std::string& n, int v) : name(n), version(v) {}

  bool operator==(const SuiteKey& other) const {
    return name == other.name && version == other.version;
  }
};

// Hash a SuiteKey
struct SuiteKeyHash {
  size_t operator()(const SuiteKey& key) const {
    return std::hash<std::string>()(key.name) ^ (std::hash<int>()(key.version) << 1);
  }
};

}  // namespace detail

struct SuiteContainer {
  std::unordered_map<detail::SuiteKey, const void*, detail::SuiteKeyHash> suites;

  template <typename T>
  const T* get(const std::string& name, int version) const {
    auto it = suites.find({name, version});
    return (it != suites.end()) ? static_cast<T*>(it->second) : nullptr;
  }

  template <typename T>
  const T* get() const {
    // Default impl, to be specialized for different suite types
    assert((false && "Calling generic SuiteContainer.get with no suite name"));
    return nullptr;  // Default implementation returns nullptr
  }

  // Check if suite is registered in the container
  bool has(const std::string& name, int version) const {
    return suites.find({name, version}) != suites.end();
  }

  template <typename T>
  bool has() const {
    // Will be specialized just like get()
    assert((false && "Calling generic SuiteContainer.has with no suite name"));
    return false;
  }

  template <typename T>
  void add(const std::string& name, int version, T* suite) {
    suites[{name, version}] = static_cast<const void*>(suite);
  }
};

// Macro to define `get` and `has` specializations for a suite
#define DEFINE_SUITE(suiteType, suiteName, suiteVersion)                          \
  template <>                                                                     \
  inline const suiteType* SuiteContainer::get<const suiteType>() const {          \
    try {                                                                         \
      return static_cast<const suiteType*>(suites.at({suiteName, suiteVersion})); \
    } catch (std::out_of_range & e) {                                             \
      return nullptr;                                                             \
    }                                                                             \
  }                                                                               \
  template <>                                                                     \
  inline const suiteType* SuiteContainer::get<suiteType>() const {                \
    try {                                                                         \
      return static_cast<const suiteType*>(suites.at({suiteName, suiteVersion})); \
    } catch (std::out_of_range & e) {                                             \
      return nullptr;                                                             \
    }                                                                             \
  }                                                                               \
  template <>                                                                     \
  inline bool SuiteContainer::has<suiteType>() const {                            \
    return suites.find({suiteName, suiteVersion}) != suites.end();                \
  }

// Define specializations for standard OFX suites
DEFINE_SUITE(OfxTimeLineSuiteV1, kOfxTimeLineSuite, 1);
DEFINE_SUITE(OfxParameterSuiteV1, kOfxParameterSuite, 1);
DEFINE_SUITE(OfxPropertySuiteV1, kOfxPropertySuite, 1);
DEFINE_SUITE(OfxDialogSuiteV1, kOfxDialogSuite, 1);
DEFINE_SUITE(OfxMessageSuiteV1, kOfxMessageSuite, 1);
DEFINE_SUITE(OfxMessageSuiteV2, kOfxMessageSuite, 2);
DEFINE_SUITE(OfxParametricParameterSuiteV1, kOfxParametricParameterSuite, 1);
DEFINE_SUITE(OfxMultiThreadSuiteV1, kOfxMultiThreadSuite, 1);
DEFINE_SUITE(OfxProgressSuiteV1, kOfxProgressSuite, 1);
DEFINE_SUITE(OfxProgressSuiteV2, kOfxProgressSuite, 2);
DEFINE_SUITE(OfxImageEffectOpenGLRenderSuiteV1, kOfxOpenGLRenderSuite, 1);
DEFINE_SUITE(OfxOpenCLProgramSuiteV1, kOfxOpenCLProgramSuite, 1);
DEFINE_SUITE(OfxMemorySuiteV1, kOfxMemorySuite, 1);
DEFINE_SUITE(OfxImageEffectSuiteV1, kOfxImageEffectSuite, 1);
DEFINE_SUITE(OfxDrawSuiteV1, kOfxDrawSuite, 1);
DEFINE_SUITE(OfxInteractSuiteV1, kOfxInteractSuite, 1);

#undef DEFINE_SUITE

}  // namespace openfx
