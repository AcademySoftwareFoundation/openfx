// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <memory>  // For std::unique_ptr

#include "openfx/ofxExceptions.h"
#include "openfx/ofxImage.h"
#include "openfx/ofxPropsAccess.h"

namespace openfx {

class Clip {
 private:
  const OfxImageEffectSuiteV1* mEffectSuite;
  const OfxPropertySuiteV1* mPropertySuite;
  OfxImageClipHandle mClip{};
  OfxPropertySetHandle mClipPropSet{};           // The clip property set handle
  std::unique_ptr<PropertyAccessor> mClipProps;  // Accessor: use a pointer to defer construction

 public:
  // Construct a clip given the raw clip handle.
  // Gets the property set and sets up accessor for it.
  Clip(const OfxImageEffectSuiteV1* effect_suite, const OfxPropertySuiteV1* prop_suite,
       OfxImageClipHandle clip)
      : mClip(clip), mEffectSuite(effect_suite), mPropertySuite(prop_suite), mClipProps(nullptr) {
    if (clip != nullptr) {
      OfxStatus status = mEffectSuite->clipGetPropertySet(clip, &mClipPropSet);
      if (status != kOfxStatOK)
        throw ClipNotFoundException(status);
      if (mClipPropSet) {
        mClipProps = std::make_unique<PropertyAccessor>(mClipPropSet, prop_suite);
      }
    }
  }

  // Construct a clip given effect and clip name.
  // Gets the clip with its property set, and sets up accessor for it.
  Clip(const OfxImageEffectSuiteV1* effect_suite, const OfxPropertySuiteV1* prop_suite,
       OfxImageEffectHandle effect, std::string_view clip_name)
      : mEffectSuite(effect_suite), mPropertySuite(prop_suite), mClipProps(nullptr) {
    OfxStatus status = effect_suite->clipGetHandle(effect, clip_name.data(), &mClip, &mClipPropSet);
    if (status != kOfxStatOK || !mClip || !mClipPropSet)
      throw ClipNotFoundException(status);
    mClipProps = std::make_unique<PropertyAccessor>(mClipPropSet, prop_suite);
  }

  // Default constructor: empty clip
  Clip() : mEffectSuite(nullptr), mClipProps(nullptr) {}

  // Destructor releases the resource
  ~Clip() {
    if (mClipPropSet) {
      mClipPropSet = nullptr;
    }
  }

  // Disable copying
  Clip(const Clip&) = delete;
  Clip& operator=(const Clip&) = delete;

  // Enable moving
  Clip(Clip&& other) noexcept
      : mEffectSuite(other.mEffectSuite),
        mPropertySuite(other.mPropertySuite),
        mClip(other.mClip),
        mClipPropSet(other.mClipPropSet),
        mClipProps(std::move(other.mClipProps)) {
    other.mClipPropSet = nullptr;
  }

  Clip& operator=(Clip&& other) noexcept {
    if (this != &other) {
      // Release any existing resource

      // Acquire the other's resource
      mEffectSuite = other.mEffectSuite;
      mPropertySuite = other.mPropertySuite;
      mClip = other.mClip;
      mClipPropSet = other.mClipPropSet;
      mClipProps = std::move(other.mClipProps);
      other.mClipPropSet = nullptr;
    }
    return *this;
  }

  // Get an image from the clip at this time
  openfx::Image get_image(OfxTime time, const OfxRectD* rect = nullptr) {
    return openfx::Image(mEffectSuite, mPropertySuite, mClip, time, rect);
  }

  // get the clip handle
  OfxImageClipHandle clip() const { return mClip; }

  // get the clip's prop set
  OfxPropertySetHandle get_propset() const { return mClipPropSet; }

  // Accessor for PropertyAccessor
  PropertyAccessor* props() { return mClipProps.get(); }
  const PropertyAccessor* props() const { return mClipProps.get(); }

  // Implicit conversions to the handle types
  explicit operator OfxPropertySetHandle() const { return mClipPropSet; }
  explicit operator OfxImageClipHandle() const { return mClip; }

  bool empty() const { return mClip == nullptr; }
};

}  // namespace openfx
