// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <ofxCore.h>
#include <ofxImageEffect.h>
#include <ofxPropsAccess.h>

#include <memory>  // For std::unique_ptr

#include "ofxExceptions.h"

namespace openfx {

class Image {
 private:
  // The image property set handle
  const OfxImageEffectSuiteV1* mEffectSuite;
  OfxPropertySetHandle mImg{};
  std::unique_ptr<PropertyAccessor> mImgProps;  // Use a pointer to defer construction

 public:
  // Constructor acquires the resource
  Image(const OfxImageEffectSuiteV1* effect_suite, const OfxPropertySuiteV1* prop_suite,
        OfxImageClipHandle clip, OfxTime time, OfxRectD* rect = nullptr)
      : mEffectSuite(effect_suite), mImgProps(nullptr) {
    if (clip != nullptr) {
      OfxStatus status = mEffectSuite->clipGetImage(clip, time, rect, &mImg);
      if (status != kOfxStatOK)
        throw ImageNotFoundException(status);
      if (mImg) {
        mImgProps = std::make_unique<PropertyAccessor>(mImg, prop_suite);
      }
    }
  }

  // Default constructor: empty image
  Image() : mEffectSuite(nullptr), mImgProps(nullptr) {}

  // Destructor releases the resource
  ~Image() {
    if (mImg) {
      mEffectSuite->clipReleaseImage(mImg);
      mImg = nullptr;
    }
  }

  // Disable copying
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  // Enable moving
  Image(Image&& other) noexcept
      : mEffectSuite(other.mEffectSuite), mImg(other.mImg), mImgProps(std::move(other.mImgProps)) {
    other.mImg = nullptr;
  }

  Image& operator=(Image&& other) noexcept {
    if (this != &other) {
      // Release any existing resource
      if (mImg) {
        mEffectSuite->clipReleaseImage(mImg);
      }

      // Acquire the other's resource
      mEffectSuite = other.mEffectSuite;
      mImg = other.mImg;
      mImgProps = std::move(other.mImgProps);
      other.mImg = nullptr;
    }
    return *this;
  }

  // Accessor to get the underlying handle
  OfxPropertySetHandle get() const { return mImg; }

  // Accessor for PropertyAccessor
  PropertyAccessor* props() { return mImgProps.get(); }
  const PropertyAccessor* props() const { return mImgProps.get(); }

  // Implicit conversion to the handle type
  explicit operator OfxPropertySetHandle() const { return mImg; }

  bool empty() const { return mImg == nullptr; }
};

}  // namespace openfx
