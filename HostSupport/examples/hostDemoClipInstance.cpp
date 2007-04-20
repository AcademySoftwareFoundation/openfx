#include <iostream>
#include <fstream>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhImageEffect.h"
#include "ofxhPluginAPICache.h"
#include "ofxhPluginCache.h"
#include "ofxhHost.h"
#include "ofxhImageEffectAPI.h"

// my host
#include "hostDemoHostDescriptor.h"
#include "hostDemoEffectInstance.h"
#include "hostDemoClipInstance.h"

namespace MyHost {

  MyClipInstance::MyClipInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Clip::Descriptor& desc) 
    : _effect(effect), _name(name), OFX::Host::Clip::Instance(desc)
  {}

  // live properties
  OfxStatus MyClipInstance::getPixelDepth(std::string &pixelDepth)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getComponents(std::string &components)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedBitDepth(std::string &unmappedBitDepth)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedComponents(std::string &unmappedComponents)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getPremult(std::string &premult)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getAspectRatio(double &aspectRatio)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getFrameRate(double &frameRate)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getFrameRange(double &startFrame, double &endFrame)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getField(std::string &field)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getConnected(int &connected)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedFrameRate(double &unmappedFrameRate)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame)
  {
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getContinuousSamples(int &continuousSamples)
  {
    return kOfxStatOK;
  }

  // get image
  OfxStatus MyClipInstance::getImage(OfxTime time, OfxRectD *h2, OFX::Host::Clip::Image*& image)
  {
    return kOfxStatOK;
  }

  // get clip rod
  OfxStatus MyClipInstance::getRegionOfDefinition(OfxRectD& rod)
  {
    return kOfxStatOK;
  }

} // MyHost