// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

// Simple Brightness adjustment plugin - your first OpenFX plugin
// Based on the Support library for easier development

#include <algorithm>
#include <cmath>
#include <memory>

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsProcessing.h"

////////////////////////////////////////////////////////////////////////////////
// Base class for the brightness processor
class BrightnessBase : public OFX::ImageProcessor {
protected:
  OFX::Image *_srcImg;
  double _brightness;
  
public:
  BrightnessBase(OFX::ImageEffect &instance)
    : OFX::ImageProcessor(instance)
    , _srcImg(0)
    , _brightness(0)
  {        
  }

  void setSrcImg(OFX::Image *v) { _srcImg = v; }
  void setBrightness(double v) { _brightness = v; }
};

// Template to do the brightness processing
template <class PIX, int nComponents, int maxValue>
class ImageBrightness : public BrightnessBase {
public:
  ImageBrightness(OFX::ImageEffect &instance) 
    : BrightnessBase(instance)
  {}

  void multiThreadProcessImages(OfxRectI procWindow)
  {
    for (int y = procWindow.y1; y < procWindow.y2; y++) {
      if (_effect.abort()) break;

      PIX *dstPix = (PIX *)_dstImg->getPixelAddress(procWindow.x1, y);

      for (int x = procWindow.x1; x < procWindow.x2; x++) {
        PIX *srcPix = (PIX *)(_srcImg ? _srcImg->getPixelAddress(x, y) : 0);

        if (srcPix) {
          for (int c = 0; c < nComponents; ++c) {
            if (nComponents != 4 || c != 3) { // Don't modify alpha channel
              if (maxValue == 1) { // float
                dstPix[c] = srcPix[c] + _brightness;
              } else { // integer types
                float val = srcPix[c] / (float)maxValue;
                val = val + _brightness;
                val = std::max(0.0f, std::min(1.0f, val));
                dstPix[c] = (PIX)(val * maxValue);
              }
            } else {
              dstPix[c] = srcPix[c]; // copy alpha
            }
          }
        } else {
          // no source, make output black
          for (int c = 0; c < nComponents; ++c) {
            dstPix[c] = 0;
          }
        }
        dstPix += nComponents;
      }
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
// the plugin
class BrightnessPlugin : public OFX::ImageEffect {
protected:
  OFX::Clip *_srcClip;
  OFX::Clip *_dstClip;
  OFX::DoubleParam *_brightness;

public:
  BrightnessPlugin(OfxImageEffectHandle handle)
    : ImageEffect(handle)
    , _srcClip(0)
    , _dstClip(0)
    , _brightness(0)
  {
    _srcClip = fetchClip(kOfxImageEffectSimpleSourceClipName);
    _dstClip = fetchClip(kOfxImageEffectOutputClipName);
    _brightness = fetchDoubleParam("brightness");
  }

  virtual void render(const OFX::RenderArguments &args);
  virtual bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod);
  virtual void getRegionsOfInterest(const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois);
  virtual void getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences);

  // do the work in the multithread suite
  template <int nComponents>
  void renderForComponents(const OFX::RenderArguments &args);

  template <class PIX, int nComponents, int maxValue>
  void renderForBitDepth(const OFX::RenderArguments &args);
};

template <int nComponents>
void BrightnessPlugin::renderForComponents(const OFX::RenderArguments &args)
{
  OFX::BitDepthEnum srcBitDepth = _srcClip->getPixelDepth();
  
  switch (srcBitDepth) {
  case OFX::eBitDepthUByte:
    renderForBitDepth<unsigned char, nComponents, 255>(args);
    break;
  case OFX::eBitDepthUShort:
    renderForBitDepth<unsigned short, nComponents, 65535>(args);
    break;
  case OFX::eBitDepthFloat:
    renderForBitDepth<float, nComponents, 1>(args);
    break;
  default:
    OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
  }
}

template <class PIX, int nComponents, int maxValue>
void BrightnessPlugin::renderForBitDepth(const OFX::RenderArguments &args)
{
  std::unique_ptr<OFX::Image> src(_srcClip->fetchImage(args.time));
  std::unique_ptr<OFX::Image> dst(_dstClip->fetchImage(args.time));
  
  if (!src.get() || !dst.get()) {
    OFX::throwSuiteStatusException(kOfxStatFailed);
  }

  // get brightness value
  double brightness = _brightness->getValueAtTime(args.time);
  
  // set up the processor
  ImageBrightness<PIX, nComponents, maxValue> processor(*this);
  processor.setDstImg(dst.get());
  processor.setSrcImg(src.get());
  processor.setBrightness(brightness);
  processor.setRenderWindow(args.renderWindow);
  processor.process();
}

void BrightnessPlugin::render(const OFX::RenderArguments &args)
{
  OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();
  
  switch (dstComponents) {
  case OFX::ePixelComponentRGBA:
    renderForComponents<4>(args);
    break;
  case OFX::ePixelComponentRGB:
    renderForComponents<3>(args);
    break;
  case OFX::ePixelComponentAlpha:
    renderForComponents<1>(args);
    break;
  default:
    OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
    break;
  }
}

bool BrightnessPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod)
{
  if (_srcClip && _srcClip->isConnected()) {
    rod = _srcClip->getRegionOfDefinition(args.time);
    return true;
  }
  return false;
}

void BrightnessPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois)
{
  rois.setRegionOfInterest(*_srcClip, args.regionOfInterest);
}

void BrightnessPlugin::getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences)
{
  // don't change anything from the defaults
}

////////////////////////////////////////////////////////////////////////////////
// Plugin factory using the macro

mDeclarePluginFactory(BrightnessExamplePluginFactory, {}, {});

void BrightnessExamplePluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
  desc.setLabels("Brightness", "Brightness", "Brightness");
  desc.setPluginGrouping("Color");
  desc.addSupportedContext(OFX::eContextFilter);
  desc.addSupportedBitDepth(OFX::eBitDepthUByte);
  desc.addSupportedBitDepth(OFX::eBitDepthUShort);
  desc.addSupportedBitDepth(OFX::eBitDepthFloat);
  desc.setSingleInstance(false);
  desc.setHostFrameThreading(false);
  desc.setSupportsMultiResolution(true);
  desc.setSupportsTiles(true);
  desc.setTemporalClipAccess(false);
  desc.setRenderTwiceAlways(false);
  desc.setSupportsMultipleClipPARs(false);
}

void BrightnessExamplePluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context)
{
  OFX::ClipDescriptor *srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
  srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
  srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
  srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
  srcClip->setTemporalClipAccess(false);
  srcClip->setSupportsTiles(true);
  srcClip->setIsMask(false);

  OFX::ClipDescriptor *dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
  dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
  dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
  dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
  dstClip->setSupportsTiles(true);

  OFX::DoubleParamDescriptor *brightness = desc.defineDoubleParam("brightness");
  brightness->setLabels("Brightness", "Brightness", "Brightness adjustment");
  brightness->setScriptName("brightness");
  brightness->setHint("Amount to adjust brightness (-1.0 to 1.0)");
  brightness->setDefault(0.0);
  brightness->setRange(-1.0, 1.0);
  brightness->setDisplayRange(-1.0, 1.0);
  brightness->setIncrement(0.01);
  brightness->setDigits(3);
}

OFX::ImageEffect* BrightnessExamplePluginFactory::createInstance(OfxImageEffectHandle handle, OFX::ContextEnum context)
{
  return new BrightnessPlugin(handle);
}

namespace OFX
{
  namespace Plugin
  {
    void getPluginIDs(PluginFactoryArray &ids)
    {
      static BrightnessExamplePluginFactory p("com.reepost.brightness", 1, 0);
      ids.push_back(&p);
    }
  }
}