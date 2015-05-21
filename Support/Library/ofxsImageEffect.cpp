/*
OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
Copyright (C) 2004-2007 The Open Effects Association Ltd
Author Bruno Nicoletti bruno@thefoundry.co.uk

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
* Neither the name The Open Effects Association Ltd, nor the names of its 
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The Open Effects Association Ltd
1 Wardour St
London W1D 6PA
England


*/

/** @brief This file contains code that skins the ofx effect suite */

#include "ofxsSupportPrivate.h"
#include <algorithm> // for find
#include <cstring> // for strlen
#ifdef DEBUG
#include <iostream>
#endif
#include <stdexcept>
#ifdef OFX_EXTENSIONS_NUKE
#include "nuke/fnOfxExtensions.h"
#endif
#include "ofxNatron.h"
#ifdef OFX_SUPPORTS_OPENGLRENDER
#include "ofxOpenGLRender.h"
#endif
#include "ofxsCore.h"

#if defined __APPLE__ || defined linux || defined __FreeBSD__
# if __GNUC__ >= 4
#  define EXPORT __attribute__((visibility("default")))
#  define LOCAL  __attribute__((visibility("hidden")))
# else
#  define EXPORT
#  define LOCAL
# endif
#elif defined _WIN32
#  define EXPORT OfxExport
#  define LOCAL
#else
#  error Not building on your operating system quite yet
#endif

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries. */
namespace OFX {

  // globals to keep consistent data structures around.
  OFX::PluginFactoryArray plugIDs;
  //Put it all into a map, so we know when to delete what!
  struct OfxPlugInfo
  {
    OfxPlugInfo():_factory(0), _plug(0){}
    OfxPlugInfo(OFX::PluginFactory* f, OfxPlugin* p):_factory(f), _plug(p){}
    OFX::PluginFactory* _factory;
    OfxPlugin* _plug;
  };
  typedef std::map<std::string, OfxPlugInfo> OfxPlugInfoMap;
  OfxPlugInfoMap plugInfoMap;

  typedef std::vector<OfxPlugin*> OfxPluginArray;
  OfxPluginArray ofxPlugs;

  /** @brief the global host description */
  ImageEffectHostDescription gHostDescription;
  bool gHostDescriptionHasInit = false;

  bool ImageEffectHostDescription::supportsPixelComponent(const PixelComponentEnum component) const
  {
    return std::find(_supportedComponents.begin(), _supportedComponents.end(), component) != _supportedComponents.end();
  }
  bool ImageEffectHostDescription::supportsBitDepth( const BitDepthEnum bitDepth) const
  {
    return std::find(_supportedPixelDepths.begin(), _supportedPixelDepths.end(), bitDepth) != _supportedPixelDepths.end();
  }
  bool ImageEffectHostDescription::supportsContext(const ContextEnum context) const
  {
    return std::find(_supportedContexts.begin(), _supportedContexts.end(), context) != _supportedContexts.end();
  }
	
  /** @return default pixel depth supported by host application. */
  BitDepthEnum ImageEffectHostDescription::getDefaultPixelDepth() const
  {
    if(!_supportedPixelDepths.empty()) {
      return _supportedPixelDepths[0];
    } else {
      OFX::Log::warning(true, "The host doesn't define supported pixel depth. (size: %d)", (int)_supportedPixelDepths.size());
      return eBitDepthFloat;
    }
  }
	
  /** @return default pixel component supported by host application. */
  PixelComponentEnum ImageEffectHostDescription::getDefaultPixelComponent() const
  {
    if(! _supportedComponents.empty()) {
      return _supportedComponents[0];
    } else {
      OFX::Log::warning(true, "The host doesn't define supported pixel component. (size: %d)", _supportedComponents.size());
      return ePixelComponentRGBA;
    }
  }

  ImageEffectHostDescription* getImageEffectHostDescription()
  {
    if(gHostDescriptionHasInit)
      return &gHostDescription;
    return NULL;
  }

  namespace Private {        
    // Suite and host pointers
    OfxHost               *gHost = 0;
    OfxImageEffectSuiteV1 *gEffectSuite = 0;
    OfxPropertySuiteV1    *gPropSuite = 0;
    OfxInteractSuiteV1    *gInteractSuite = 0;
    OfxParameterSuiteV1   *gParamSuite = 0;
    OfxMemorySuiteV1      *gMemorySuite = 0;
    OfxMultiThreadSuiteV1 *gThreadSuite = 0;
    OfxMessageSuiteV1     *gMessageSuite = 0;
    OfxMessageSuiteV2     *gMessageSuiteV2 = 0;
    OfxProgressSuiteV1     *gProgressSuite = 0;
    OfxTimeLineSuiteV1     *gTimeLineSuite = 0;
    OfxParametricParameterSuiteV1 *gParametricParameterSuite = 0;
#ifdef OFX_SUPPORTS_OPENGLRENDER
    OfxImageEffectOpenGLRenderSuiteV1 *gOpenGLRenderSuite = 0;
#endif
#ifdef OFX_EXTENSIONS_NUKE
    NukeOfxCameraSuiteV1* gCameraParameterSuite = 0;
    FnOfxImageEffectPlaneSuiteV1* gImageEffectPlaneSuiteV1 = 0;
    FnOfxImageEffectPlaneSuiteV2* gImageEffectPlaneSuiteV2 = 0;
#endif
#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
    OfxHWNDInteractSuiteV1 *gHWNDInteractSuite = 0;
#endif // #if defined(WIN32) || defined(WIN64)
    OfxVegasProgressSuiteV1 *gVegasProgressSuite = 0;
    OfxVegasStereoscopicImageSuiteV1 *gVegasStereoscopicImageSuite = 0;
    OfxVegasKeyframeSuiteV1 *gVegasKeyframeSuite = 0;
#endif

    // @brief the set of descriptors, one per context used by kOfxActionDescribeInContext,
    //'eContextNone' is the one used by the kOfxActionDescribe
    EffectDescriptorMap gEffectDescriptors;
  };

  /** @brief map a std::string to a context */
  ContextEnum mapToContextEnum(const std::string &s) throw(std::invalid_argument)
  {
    if(s == kOfxImageEffectContextGenerator) return eContextGenerator;
    if(s == kOfxImageEffectContextFilter) return eContextFilter;
    if(s == kOfxImageEffectContextTransition) return eContextTransition;
    if(s == kOfxImageEffectContextPaint) return eContextPaint;
    if(s == kOfxImageEffectContextGeneral) return eContextGeneral;
    if(s == kOfxImageEffectContextRetimer) return eContextRetimer;
#ifdef OFX_EXTENSIONS_TUTTLE
    if(s == kOfxImageEffectContextReader) return eContextReader;
    if(s == kOfxImageEffectContextWriter) return eContextWriter;
#endif
#ifdef OFX_EXTENSIONS_NATRON
    if(s == kNatronOfxImageEffectContextTracker) return eContextTracker;
#endif
    OFX::Log::error(true, "Unknown image effect context '%s'", s.c_str());
    throw std::invalid_argument(s);
  }

  const char* mapMessageTypeEnumToStr(OFX::Message::MessageTypeEnum type)
  {
    if(type == OFX::Message::eMessageFatal)
      return kOfxMessageFatal;
    else if(type == OFX::Message::eMessageError)
      return kOfxMessageError;
    else if(type == OFX::Message::eMessageMessage)
      return kOfxMessageMessage;
    else if(type == OFX::Message::eMessageWarning)
      return kOfxMessageWarning;
    else if(type == OFX::Message::eMessageLog)
      return kOfxMessageLog;
    else if(type == OFX::Message::eMessageQuestion)
      return kOfxMessageQuestion;
    OFX::Log::error(true, "Unknown message type enum '%d'", type);
    return 0;
  }

  OFX::Message::MessageReplyEnum mapToMessageReplyEnum(OfxStatus stat)
  {
    if(stat == kOfxStatOK)
      return OFX::Message::eMessageReplyOK;
    else if(stat == kOfxStatReplyYes)
      return OFX::Message::eMessageReplyYes;
    else if(stat == kOfxStatReplyNo)
      return OFX::Message::eMessageReplyNo;
    else if(stat == kOfxStatFailed)
      return OFX::Message::eMessageReplyFailed;
    OFX::Log::error(true, "Unknown message reply status enum '%d'", stat);
    return OFX::Message::eMessageReplyFailed;
  }

  /** @brief map a std::string to a context */
  InstanceChangeReason mapToInstanceChangedReason(const std::string &s) throw(std::invalid_argument)
  {
    if(s == kOfxChangePluginEdited) return eChangePluginEdit;
    if(s == kOfxChangeUserEdited) return eChangeUserEdit;
    if(s == kOfxChangeTime) return eChangeTime;
    OFX::Log::error(true, "Unknown instance changed reason '%s'", s.c_str());
    throw std::invalid_argument(s);
  }

  /** @brief turns a bit depth string into and enum */
  BitDepthEnum mapStrToBitDepthEnum(const std::string &str) throw(std::invalid_argument)
  {
    if(str == kOfxBitDepthByte) {
      return eBitDepthUByte;
    }
    else if(str == kOfxBitDepthShort) {
      return eBitDepthUShort;
    }
    else if(str == kOfxBitDepthHalf) {
      return eBitDepthHalf;
    }
    else if(str == kOfxBitDepthFloat) {
      return eBitDepthFloat;
    }
#ifdef OFX_EXTENSIONS_VEGAS
    else if(str == kOfxBitDepthByteBGR) {
      return eBitDepthUByteBGRA;
    }
    else if(str == kOfxBitDepthShortBGR) {
      return eBitDepthUShortBGRA;
    }
    else if(str == kOfxBitDepthFloatBGR) {
      return eBitDepthFloatBGRA;
    }
#endif
    else if(str == kOfxBitDepthNone) {
      return eBitDepthNone;
    }
    else {
      return eBitDepthCustom;
    }
  }

#ifdef OFX_SUPPORTS_OPENGLRENDER
    /** @brief turns a bit depth string into and enum */
    static std::string mapBitDepthEnumToStr(BitDepthEnum bitDepth)
    {
      switch (bitDepth) {
      case eBitDepthUByte:
        return kOfxBitDepthByte;
      case eBitDepthUShort:
        return kOfxBitDepthShort;
      case eBitDepthHalf:
        return kOfxBitDepthHalf;
      case eBitDepthFloat:
        return kOfxBitDepthFloat;
#ifdef OFX_EXTENSIONS_VEGAS
      case eBitDepthUByteBGRA:
        return kOfxBitDepthByteBGR;
      case eBitDepthUShortBGRA:
        return kOfxBitDepthShortBGR;
      case eBitDepthFloatBGRA:
        return kOfxBitDepthFloatBGR;
#endif
      case eBitDepthNone:
        return kOfxBitDepthNone;
      case eBitDepthCustom:
        return std::string();
        }
      return std::string();
    }
#endif

  /** @brief turns a pixel component string into and enum */
  PixelComponentEnum mapStrToPixelComponentEnum(const std::string &str) throw(std::invalid_argument)
  {
    if(str == kOfxImageComponentRGBA) {
      return ePixelComponentRGBA;
    }
    else if(str == kOfxImageComponentRGB) {
      return ePixelComponentRGB;
    }
    else if(str == kOfxImageComponentAlpha) {
      return ePixelComponentAlpha;
    }
    else if(str == kOfxImageComponentNone) {
      return ePixelComponentNone;
    }
#ifdef OFX_EXTENSIONS_NUKE
    else if(str == kFnOfxImageComponentMotionVectors) {
        return ePixelComponentMotionVectors;
    }
    else if(str == kFnOfxImageComponentStereoDisparity) {
        return ePixelComponentStereoDisparity;
    }
#endif
#ifdef OFX_EXTENSIONS_NATRON
    else if (str == kNatronOfxImageComponentXY) {
        return ePixelComponentXY;
    }
#endif
    else {
      return ePixelComponentCustom;
    }
  }

  /** @brief turns a premultiplication string into and enum */
  static PreMultiplicationEnum mapStrToPreMultiplicationEnum(const std::string &str) throw(std::invalid_argument)
  {
    if(str == kOfxImageOpaque) {
      return eImageOpaque;
    }
    else if(str == kOfxImagePreMultiplied) {
      return eImagePreMultiplied;
    }
    else if(str == kOfxImageUnPreMultiplied) {
      return eImageUnPreMultiplied;
    }
    else {
      throw std::invalid_argument("");
    }
  }

  /** @brief turns a field string into and enum */
  FieldEnum mapStrToFieldEnum(const std::string &str)  throw(std::invalid_argument)
  {
    if(str == kOfxImageFieldNone) {
      return eFieldNone;
    }
    else if(str == kOfxImageFieldBoth) {
      return eFieldBoth;
    }
    else if(str == kOfxImageFieldLower) {
      return eFieldLower;
    }
    else if(str == kOfxImageFieldUpper) {
      return eFieldUpper;
    }
    else {
      throw std::invalid_argument("");
    }
  }

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief map a std::string to a RenderQuality */
  VegasRenderQualityEnum mapToVegasRenderQualityEnum(const std::string &s) throw(std::invalid_argument)
  {
    if(s == kOfxImageEffectPropRenderQualityDraft  )       return eVegasRenderQualityDraft;
    if(s == kOfxImageEffectPropRenderQualityPreview)       return eVegasRenderQualityPreview;
    if(s == kOfxImageEffectPropRenderQualityGood   )       return eVegasRenderQualityGood;
    if(s.empty() || s == kOfxImageEffectPropRenderQualityBest   )       return eVegasRenderQualityBest;
    OFX::Log::error(true, "Unknown Vegas RenderQuality '%s'", s.c_str());
    throw std::invalid_argument(s);
  }

  /** @brief map a std::string to a context */
  VegasContextEnum mapToVegasContextEnum(const std::string &s) throw(std::invalid_argument)
  {
    if(s.empty() || s == kOfxImageEffectPropVegasContextUnknown)       return eVegasContextUnknown;
    if(s == kOfxImageEffectPropVegasContextMedia)         return eVegasContextMedia;
    if(s == kOfxImageEffectPropVegasContextTrack)         return eVegasContextTrack;
    if(s == kOfxImageEffectPropVegasContextEvent)         return eVegasContextEvent;
    if(s == kOfxImageEffectPropVegasContextEventFadeIn)   return eVegasContextEventFadeIn;
    if(s == kOfxImageEffectPropVegasContextEventFadeOut)  return eVegasContextEventFadeOut;
    if(s == kOfxImageEffectPropVegasContextProject)       return eVegasContextProject;
    if(s == kOfxImageEffectPropVegasContextGenerator)     return eVegasContextGenerator;
    OFX::Log::error(true, "Unknown Vegas image effect context '%s'", s.c_str());
    throw std::invalid_argument(s);
  }
#endif

#if defined(OFX_EXTENSIONS_NATRON)
  /** @brief extract layer name (first element) and channel names (other elements) from the kOfxImageEffectPropComponents property value, @see getPixelComponentsProperty() */
  std::vector<std::string> mapPixelComponentCustomToLayerChannels(const std::string& comp)
  {
    std::vector<std::string> retval;

    const std::size_t foundPlaneLen = std::strlen(kNatronOfxImageComponentsPlane);
    std::size_t foundPlane = comp.find(kNatronOfxImageComponentsPlane);
    if (foundPlane == std::string::npos) {
      return retval;
    }

    std::size_t foundChannel = comp.find(kNatronOfxImageComponentsPlaneChannel, foundPlane + foundPlaneLen);
    if (foundChannel == std::string::npos) {
      return retval;
    }

    retval.push_back(comp.substr(foundPlane + foundPlaneLen, foundChannel - (foundPlane + foundPlaneLen)));

    const std::size_t foundChannelLen = std::strlen(kNatronOfxImageComponentsPlaneChannel);
    while (foundChannel != std::string::npos) {
      std::size_t nextChannel = comp.find(kNatronOfxImageComponentsPlaneChannel, foundChannel + foundChannelLen);
      std::string chan = comp.substr(foundChannel + foundChannelLen, nextChannel - (foundChannel + foundChannelLen));
      retval.push_back(chan);
      foundChannel = nextChannel;
    }

    return retval;
  }
#endif


  ////////////////////////////////////////////////////////////////////////////////
  // clip descriptor

  /** @brief hidden constructor */
  ClipDescriptor::ClipDescriptor(const std::string &name, OfxPropertySetHandle props)
    : _clipName(name)
    , _clipProps(props)
  {
    OFX::Validation::validateClipDescriptorProperties(props);
  }

  /** @brief set the label properties */
  void ClipDescriptor::setLabel(const std::string &label)
  {
    _clipProps.propSetString(kOfxPropLabel, label);
  }

  /** @brief set the label properties */
  void ClipDescriptor::setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel)
  {
    setLabel(label);
    _clipProps.propSetString(kOfxPropShortLabel, shortLabel, false);
    _clipProps.propSetString(kOfxPropLongLabel, longLabel, false);
  }

  /** @brief set how fielded images are extracted from the clip defaults to eFieldExtractDoubled */
  void ClipDescriptor::setFieldExtraction(FieldExtractionEnum v)
  {
    switch(v) 
    {
    case eFieldExtractBoth :
      _clipProps.propSetString(kOfxImageClipPropFieldExtraction, kOfxImageFieldBoth);
      break;

    case eFieldExtractSingle :
      _clipProps.propSetString(kOfxImageClipPropFieldExtraction, kOfxImageFieldSingle);
      break;

    case eFieldExtractDoubled :
      _clipProps.propSetString(kOfxImageClipPropFieldExtraction, kOfxImageFieldDoubled);
      break;
    }
  }

  /** @brief set which components are supported, defaults to none set, this must be called at least once! */
  void ClipDescriptor::addSupportedComponent(PixelComponentEnum v)
  {
    int n = _clipProps.propGetDimension(kOfxImageEffectPropSupportedComponents);
    switch(v) 
    {
    case ePixelComponentNone :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kOfxImageComponentNone, n);
      break;

    case ePixelComponentRGBA :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGBA, n);
      break;

    case ePixelComponentRGB :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGB, n);
      break;

    case ePixelComponentAlpha :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kOfxImageComponentAlpha, n);
      break;
#ifdef OFX_EXTENSIONS_NUKE
    case ePixelComponentMotionVectors :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kFnOfxImageComponentMotionVectors, n);
      break;

    case ePixelComponentStereoDisparity :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kFnOfxImageComponentStereoDisparity, n);
      break;
#endif
#ifdef OFX_EXTENSIONS_NATRON
    case ePixelComponentXY:
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kNatronOfxImageComponentXY, n);
      break;
#endif
    case ePixelComponentCustom :
      break;
    }
  }

  /** @brief set which components are supported, defaults to none set, this must be called at least once! */
  void ClipDescriptor::addSupportedComponent(const std::string &comp)
  {
    int n = _clipProps.propGetDimension(kOfxImageEffectPropSupportedComponents);
    _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, comp, n);

  }

  /** @brief say whether we are going to do random temporal access on this clip, defaults to false */
  void ClipDescriptor::setTemporalClipAccess(bool v)
  {
    _clipProps.propSetInt(kOfxImageEffectPropTemporalClipAccess, int(v));
  }

  /** @brief say whether if the clip is optional, defaults to false */
  void ClipDescriptor::setOptional(bool v)
  {
    _clipProps.propSetInt(kOfxImageClipPropOptional, int(v));
  }

  /** @brief say whether this clip supports tiling, defaults to true */
  void ClipDescriptor::setSupportsTiles(bool v)
  {
    _clipProps.propSetInt(kOfxImageEffectPropSupportsTiles, int(v));
  }

  /** @brief say whether this clip is a 'mask', so the host can know to replace with a roto or similar, defaults to false */
  void ClipDescriptor::setIsMask(bool v)
  {
    _clipProps.propSetInt(kOfxImageClipPropIsMask, int(v));
  }

#ifdef OFX_EXTENSIONS_NUKE
  /** @brief say whether this clip may contain images with a transform attached */
  void ClipDescriptor::setCanTransform(bool v)
  {
    _clipProps.propSetInt(kFnOfxImageEffectCanTransform, int(v), false);
  }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  // image effect descriptor

  /** @brief effect descriptor ctor */
  ImageEffectDescriptor::ImageEffectDescriptor(OfxImageEffectHandle handle)
    : _effectHandle(handle)
  {
    // fetch the property set handle of the effect
    OfxPropertySetHandle props;
    OfxStatus stat = OFX::Private::gEffectSuite->getPropertySet(handle, &props);
    throwSuiteStatusException(stat);
    _effectProps.propSetHandle(props);

    OFX::Validation::validatePluginDescriptorProperties(props);

    // fetch the param set handle and set it in our ParamSetDescriptor base
    OfxParamSetHandle paramSetHandle;
    stat = OFX::Private::gEffectSuite->getParamSet(handle, &paramSetHandle);
    throwSuiteStatusException(stat);
    setParamSetHandle(paramSetHandle);
  }


  /** @brief dtor */
  ImageEffectDescriptor::~ImageEffectDescriptor()
  {
    // delete any clip descriptors we may have constructed
    std::map<std::string, ClipDescriptor *>::iterator iter;
    for(iter = _definedClips.begin(); iter != _definedClips.end(); ++iter) {
      if(iter->second) {
        delete iter->second;
        iter->second = NULL;
      }
    }
  }

  /** @brief, set the label properties in a plugin */
  void ImageEffectDescriptor::setLabel(const std::string &label)
  {
    _effectProps.propSetString(kOfxPropLabel, label);
  }

  /** @brief, set the label properties in a plugin */
  void ImageEffectDescriptor::setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel)
  {
    setLabel(label);
    _effectProps.propSetString(kOfxPropShortLabel, shortLabel, false);
    _effectProps.propSetString(kOfxPropLongLabel, longLabel, false);
  }


  /** @brief, set the version properties in a plugin */
  void ImageEffectDescriptor::setVersion(int major, int minor, int micro, int build, const std::string &versionLabel)
  {
    _effectProps.propSetInt(kOfxPropVersion, major, 0, false); // introduced in OFX 1.2
    if (minor || micro || build) {
      _effectProps.propSetInt(kOfxPropVersion, minor, 1, false); // introduced in OFX 1.2
      if (micro || build) {
        _effectProps.propSetInt(kOfxPropVersion, micro, 2, false); // introduced in OFX 1.2
        if (build) {
          _effectProps.propSetInt(kOfxPropVersion, build, 3, false); // introduced in OFX 1.2
        }
      }
    }
    if (!versionLabel.empty()) {
      _effectProps.propSetString(kOfxPropVersionLabel, versionLabel, false);
    }
  }

  /** @brief Set the plugin grouping */
  void ImageEffectDescriptor::setPluginGrouping(const std::string &group)
  {
    _effectProps.propSetString(kOfxImageEffectPluginPropGrouping, group);
  }

  /** @brief Set the plugin description, defaults to "" */
  void ImageEffectDescriptor::setPluginDescription(const std::string &description)
  {
    _effectProps.propSetString(kOfxPropPluginDescription, description, false); // introduced in OFX 1.2
  }

  /** @brief Add a context to those supported */
  void ImageEffectDescriptor::addSupportedContext(ContextEnum v)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropSupportedContexts);
    switch (v) 
    {
    case eContextNone :
      break;
    case eContextGenerator :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGenerator, n);
      break;
    case eContextFilter :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextFilter, n);
      break;
    case eContextTransition :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextTransition, n);
      break;
    case eContextPaint :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextPaint, n);
      break;
    case eContextGeneral :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGeneral, n);
      break;
    case eContextRetimer :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextRetimer, n);
      break;
#ifdef OFX_EXTENSIONS_TUTTLE
    case eContextReader :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextReader, n);
      break;
    case eContextWriter :
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextWriter, n);
      break;
#endif
#ifdef OFX_EXTENSIONS_NATRON
    case eContextTracker:
      _effectProps.propSetString(kOfxImageEffectPropSupportedContexts, kNatronOfxImageEffectContextTracker, n);
      break;
#endif
    }
  }

  void ImageEffectDescriptor::setOverlayInteractDescriptor(EffectOverlayDescriptor* desc)
  {
    _overlayDescriptor.reset(desc);
    if(OFX::gHostDescription.supportsOverlays && desc->getMainEntry())
      _effectProps.propSetPointer(kOfxImageEffectPluginPropOverlayInteractV1, (void*)desc->getMainEntry());
  }

#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
  void ImageEffectDescriptor::setHWNDInteractDescriptor(HWNDInteractDescriptor* desc)
  {
    _hwndInteractDescriptor.reset(desc);
    if(desc->getMainEntry())
      _effectProps.propSetPointer(kOfxImageEffectPluginPropHWndInteractV1, (void*)desc->getMainEntry());
  }
#endif // #if defined(WIN32) || defined(WIN64)
#endif

  /** @brief Add a pixel depth to those supported */
  void ImageEffectDescriptor::addSupportedBitDepth(BitDepthEnum v)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropSupportedPixelDepths);
    switch(v) 
    {
    case eBitDepthNone :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthNone  , n);
      break;
    case eBitDepthUByte :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthByte  , n);
      break;
    case eBitDepthUShort :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthShort  , n);
      break;
    case eBitDepthHalf :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthHalf  , n);
      break;
    case eBitDepthFloat :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthFloat  , n);
      break;
#ifdef OFX_EXTENSIONS_VEGAS
    case eBitDepthUByteBGRA :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthByteBGR  , n);
      break;
    case eBitDepthUShortBGRA :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthShortBGR  , n);
      break;
    case eBitDepthFloatBGRA :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthFloatBGR  , n);
      break;
#endif
    case eBitDepthCustom :
      break;
    }
  }

#ifdef OFX_EXTENSIONS_TUTTLE
  /** @brief Add a file extension to those supported */
  void ImageEffectDescriptor::addSupportedExtension(const std::string& extension)
  {
    // only Tuttle support this property ( out of standard )
    //if( OFX::Private::gHostDescription.hostName == "TuttleOfx" ) {
    try {
      const int n = _effectProps.propGetDimension( kTuttleOfxImageEffectPropSupportedExtensions );
      _effectProps.propSetString(kTuttleOfxImageEffectPropSupportedExtensions, extension, n);
    } catch (OFX::Exception::PropertyUnknownToHost &e) {
      // ignore exception
    }
  }

  void ImageEffectDescriptor::addSupportedExtensions(const std::vector<std::string>& extensions)
  {
    // only Tuttle support this property ( out of standard )
    //if( OFX::Private::gHostDescription.hostName == "TuttleOfx" ) {
    try {
      int n = _effectProps.propGetDimension( kTuttleOfxImageEffectPropSupportedExtensions );
      
        for (std::vector<std::string>::const_iterator it = extensions.begin(); it != extensions.end(); ++it, ++n) {
        _effectProps.propSetString(kTuttleOfxImageEffectPropSupportedExtensions, *it, n);
      }
    } catch (OFX::Exception::PropertyUnknownToHost &e) {
      // ignore exception
    }
  }

  void ImageEffectDescriptor::addSupportedExtensions(const char*extensions[])
  {
    // only Tuttle support this property ( out of standard )
    //if( OFX::Private::gHostDescription.hostName == "TuttleOfx" ) {
    try {
      int n = _effectProps.propGetDimension( kTuttleOfxImageEffectPropSupportedExtensions );
      
      while (*extensions) {
        _effectProps.propSetString(kTuttleOfxImageEffectPropSupportedExtensions, *extensions, n);
        ++extensions;
        ++n;
      }
    } catch (OFX::Exception::PropertyUnknownToHost &e) {
      // ignore exception
    }
  }

  void ImageEffectDescriptor::setPluginEvaluation(double evaluation)
  {
    // This property is an extension, so it's optional.
    _effectProps.propSetDouble( kTuttleOfxImageEffectPropEvaluation, evaluation, false );
  }
#endif

  /** @brief Is the plugin single instance only ? */
  void ImageEffectDescriptor::setSingleInstance(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPluginPropSingleInstance, int(v));
  }

  /** @brief Does the plugin expect the host to perform per frame SMP threading */
  void ImageEffectDescriptor::setHostFrameThreading(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPluginPropHostFrameThreading, int(v));
  }

  /** @brief Does the plugin support multi resolution images */
  void ImageEffectDescriptor::setSupportsMultiResolution(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPropSupportsMultiResolution, int(v));
  }

  /** @brief Does the plugin support image tiling */
  void ImageEffectDescriptor::setSupportsTiles(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPropSupportsTiles, int(v));
  }

  /** @brief Does the plugin perform temporal clip access */
  void ImageEffectDescriptor::setTemporalClipAccess(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPropTemporalClipAccess, int(v));
  }

  /** @brief Does the plugin want to have render called twice per frame in all circumanstances for fielded images ? */
  void ImageEffectDescriptor::setRenderTwiceAlways(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPluginPropFieldRenderTwiceAlways, int(v));
  }

  /** @brief Does the plugin support inputs and output clips of differing depths */
  void ImageEffectDescriptor::setSupportsMultipleClipDepths(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPropSupportsMultipleClipDepths, int(v));
  }

  /** @brief Does the plugin support inputs and output clips of pixel aspect ratios */
  void ImageEffectDescriptor::setSupportsMultipleClipPARs(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectPropSupportsMultipleClipPARs, int(v));
  }

  /** @brief What kind of thread safety does the plugin have */
  void ImageEffectDescriptor::setRenderThreadSafety(RenderSafetyEnum v)
  {
    switch(v) 
    {
    case eRenderUnsafe :
      _effectProps.propSetString(kOfxImageEffectPluginRenderThreadSafety, kOfxImageEffectRenderUnsafe);
      break;
    case eRenderInstanceSafe :
      _effectProps.propSetString(kOfxImageEffectPluginRenderThreadSafety, kOfxImageEffectRenderInstanceSafe);
      break;
    case eRenderFullySafe :
      _effectProps.propSetString(kOfxImageEffectPluginRenderThreadSafety, kOfxImageEffectRenderFullySafe);
      break;
    }
  }

#ifdef OFX_EXTENSIONS_VEGAS
  void ImageEffectDescriptor::setPresetThumbnailHint(VegasPresetThumbnailEnum v)
  {
    switch(v) 
    {
    case eVegasPresetThumbnailDefault :
      _effectProps.propSetString(kOfxProbPluginVegasPresetThumbnail, kOfxProbPluginVegasPresetThumbnailDefault);
      break;
    case eVegasPresetThumbnailSolidImage :
      _effectProps.propSetString(kOfxProbPluginVegasPresetThumbnail, kOfxProbPluginVegasPresetThumbnailSolidImage);
      break;
    case eVegasPresetThumbnailImageWithAlpha :
      _effectProps.propSetString(kOfxProbPluginVegasPresetThumbnail, kOfxProbPluginVegasPresetThumbnailImageWithAlpha);
      break;
    }
  }
#endif

#ifdef OFX_SUPPORTS_OPENGLRENDER
    void ImageEffectDescriptor::setSupportsOpenGLRender(bool v) {
      if (gHostDescription.supportsOpenGLRender) {
        _effectProps.propSetString(kOfxImageEffectPropOpenGLRenderSupported, (v ? "true" : "false"));
      }
    }

    void ImageEffectDescriptor::setNeedsOpenGLRender(bool v) {
      if (gHostDescription.supportsOpenGLRender) {
        _effectProps.propSetString(kOfxImageEffectPropOpenGLRenderSupported, (v ? "needed" : "false"));
      }
    }

    void ImageEffectDescriptor::addOpenGLBitDepth(BitDepthEnum v) {
      int n = _effectProps.propGetDimension(kOfxImageEffectPropSupportedPixelDepths);
      std::string value = mapBitDepthEnumToStr(v);
      if (!value.empty()) {
        _effectProps.propSetString(kOfxOpenGLPropPixelDepth, value, n);
      }
    }

#endif

#ifdef OFX_EXTENSIONS_NUKE
  /** @brief indicate that a plugin or host can handle transform effects */
  void ImageEffectDescriptor::setCanTransform(bool v)
  {
    // the header says this property is on the effect instance, but on Nuke it only exists on the effect descriptor
    if (gHostDescription.canTransform) {
      _effectProps.propSetInt(kFnOfxImageEffectCanTransform, int(v), false);
    }
  }
    
  /** @brief Indicates that a host or plugin can fetch more than a type of image from a clip*/
  void ImageEffectDescriptor::setIsMultiPlanar(bool v)
  {
    if (gHostDescription.isMultiPlanar) {
      _effectProps.propSetInt(kFnOfxImageEffectPropMultiPlanar, int(v), false);
    }
  }
    
  void ImageEffectDescriptor::setPassThroughForNotProcessedPlanes(PassThroughLevelEnum v)
  {
    if (gHostDescription.isMultiPlanar) {
        _effectProps.propSetInt(kFnOfxImageEffectPropPassThroughComponents, int(v), false);
    }
  }
    
  /** @brief Indicates to the host that the plugin is view aware, in which case it will have to use the view calls*/
  void ImageEffectDescriptor::setIsViewAware(bool v)
  {
    if (OFX::Private::gImageEffectPlaneSuiteV2) {
      _effectProps.propSetInt(kFnOfxImageEffectPropViewAware, int(v), false);
    }
  }
    
  /** @brief Indicates to the host that a view aware plugin produces the same image independent of the view being rendered*/
  void ImageEffectDescriptor::setIsViewInvariant(bool v)
  {
    if (OFX::Private::gImageEffectPlaneSuiteV2) {
      _effectProps.propSetInt(kFnOfxImageEffectPropViewInvariance, int(v), false);
    }
  }
#endif

  /** @brief If the slave param changes the clip preferences need to be re-evaluated */
  void ImageEffectDescriptor::addClipPreferencesSlaveParam(ParamDescriptor &p)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropClipPreferencesSlaveParam);
    _effectProps.propSetString(kOfxImageEffectPropClipPreferencesSlaveParam, p.getName(), n);
  }

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief Add a guid to tell Vegas that this plug-in can uplift the guid of that older plug-in */
  void ImageEffectDescriptor::addVegasUpgradePath(const std::string &guidString)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropVegasUpliftGUID);
    _effectProps.propSetString(kOfxImageEffectPropVegasUpliftGUID, guidString.c_str(), n);
  }

  /** @brief sets the path to a help file, defaults to none, must be called at least once */
  void ImageEffectDescriptor::setHelpPath(const std::string &helpPathString)
  {
    _effectProps.propSetString(kOfxImageEffectPropHelpFile, helpPathString.c_str());
  }

  /** @brief sets the context ID to a help file if it's a .chm file, defaults to none, must be called at least once */
  void ImageEffectDescriptor::setHelpContextID(int helpContextID)
  {
    _effectProps.propSetInt(kOfxImageEffectPropHelpContextID, helpContextID);
  }
#endif

  /** @brief Create a clip, only callable from describe in context */
  ClipDescriptor *ImageEffectDescriptor::defineClip(const std::string &name)
  {
    // do we have the clip already
    std::map<std::string, ClipDescriptor *>::const_iterator search;
    search = _definedClips.find(name);
    if(search != _definedClips.end())
      return search->second;

    // no, so make it
    OfxPropertySetHandle propSet;
    OfxStatus stat = OFX::Private::gEffectSuite->clipDefine(_effectHandle, name.c_str(), &propSet);
    (void)stat;

    ClipDescriptor *clip = new ClipDescriptor(name, propSet);

    _definedClips[name] = clip;
    _clipComponentsPropNames[name] = std::string("OfxImageClipPropComponents_") + name;
    _clipDepthPropNames[name] = std::string("OfxImageClipPropDepth_") + name;
    _clipPARPropNames[name] = std::string("OfxImageClipPropPAR_") + name;
    _clipROIPropNames[name] = std::string("OfxImageClipPropRoI_") + name;
    _clipFrameRangePropNames[name] = std::string("OfxImageClipPropFrameRange_") + name;
#ifdef OFX_EXTENSIONS_NUKE
    _clipPlanesPropNames[name] = std::string(kFnOfxImageEffectActionGetClipComponentsPropString) + name;
    _clipFrameViewsPropNames[name] = std::string("OfxImageClipPropFrameRangeView_") + name;
#endif
    return clip;
  }

#ifdef OFX_EXTENSIONS_NATRON
  /** @brief indicate if the host may add a channel selector */
  void ImageEffectDescriptor::setChannelSelector(PixelComponentEnum v)
  {
    // the header says this property is on the effect instance, but on Nuke it only exists on the effect descriptor
    if (gHostDescription.supportsChannelSelector) {
      switch(v)
      {
      case ePixelComponentNone :
        _effectProps.propSetString(kNatronOfxImageEffectPropChannelSelector, kOfxImageComponentNone, false);
        break;

      case ePixelComponentRGBA :
        _effectProps.propSetString(kNatronOfxImageEffectPropChannelSelector, kOfxImageComponentNone, false);
        break;

      case ePixelComponentRGB :
        _effectProps.propSetString(kNatronOfxImageEffectPropChannelSelector, kOfxImageComponentNone, false);
        break;

      case ePixelComponentAlpha :
        _effectProps.propSetString(kNatronOfxImageEffectPropChannelSelector, kOfxImageComponentNone, false);
        break;

      default :
        break;
      }
    }
  }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  // wraps up an image  
  ImageBase::ImageBase(OfxPropertySetHandle props)
    : _imageProps(props)
  {
    OFX::Validation::validateImageBaseProperties(props);

    // and fetch all the properties
    _rowBytes         = _imageProps.propGetInt(kOfxImagePropRowBytes);
    _pixelAspectRatio = _imageProps.propGetDouble(kOfxImagePropPixelAspectRatio);;
      
    std::string str  = _imageProps.propGetString(kOfxImageEffectPropComponents);
    _pixelComponents = mapStrToPixelComponentEnum(str);

    switch (_pixelComponents) {
      case ePixelComponentAlpha:
        _pixelComponentCount = 1;
        break;
      case ePixelComponentNone:
        _pixelComponentCount = 0;
        break;
#ifdef OFX_EXTENSIONS_NUKE
      case ePixelComponentMotionVectors:
      case ePixelComponentStereoDisparity:
        _pixelComponentCount = 2;
        break;
#endif
      case ePixelComponentRGB:
        _pixelComponentCount = 3;
        break;
      case ePixelComponentRGBA:
        _pixelComponentCount = 4;
        break;
#ifdef OFX_EXTENSIONS_NATRON
      case ePixelComponentXY:
        _pixelComponentCount = 2;
        break;
#endif
      case ePixelComponentCustom:
      default:
#ifdef OFX_EXTENSIONS_NATRON
        // first element in the vector is the layer name (if any)
        _pixelComponentCount = std::max((int)mapPixelComponentCustomToLayerChannels(str).size() - 1, 0);
#else
        _pixelComponentCount = 0;
#endif
        break;
    }

    str = _imageProps.propGetString(kOfxImageEffectPropPixelDepth);
    _pixelDepth = mapStrToBitDepthEnum(str);

    // compute bytes per pixel
    _pixelBytes = _pixelComponentCount;

    switch(_pixelDepth) 
    {
    case eBitDepthNone   : _pixelBytes *= 0; break;
    case eBitDepthUByte  : _pixelBytes *= 1; break;
    case eBitDepthUShort : _pixelBytes *= 2; break;
    case eBitDepthHalf   : _pixelBytes *= 2; break;
    case eBitDepthFloat  : _pixelBytes *= 4; break;
#ifdef OFX_EXTENSIONS_VEGAS
    case eBitDepthUByteBGRA  : _pixelBytes *= 1; break;
    case eBitDepthUShortBGRA : _pixelBytes *= 2; break;
    case eBitDepthFloatBGRA  : _pixelBytes *= 4; break;
#endif
    case eBitDepthCustom : _pixelBytes *= 0; break;
    }

    str = _imageProps.propGetString(kOfxImageEffectPropPreMultiplication);
    _preMultiplication =  mapStrToPreMultiplicationEnum(str);

    _regionOfDefinition.x1 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 0);
    _regionOfDefinition.y1 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 1);
    _regionOfDefinition.x2 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 2);
    _regionOfDefinition.y2 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 3);

    _bounds.x1 = _imageProps.propGetInt(kOfxImagePropBounds, 0);
    _bounds.y1 = _imageProps.propGetInt(kOfxImagePropBounds, 1);
    _bounds.x2 = _imageProps.propGetInt(kOfxImagePropBounds, 2);
    _bounds.y2 = _imageProps.propGetInt(kOfxImagePropBounds, 3);

    str = _imageProps.propGetString(kOfxImagePropField);
    if(str == kOfxImageFieldNone) {
      _field = eFieldNone;
    }
    else if(str == kOfxImageFieldBoth) {
      _field = eFieldBoth;
    }
    else if(str == kOfxImageFieldLower) {
      _field = eFieldLower;
    }
    else if(str == kOfxImageFieldUpper) {
      _field = eFieldLower;
    }
    else {
      OFX::Log::error(true, "Unknown field state '%s' reported on an image", str.c_str());
      _field = eFieldNone;
    }

    _uniqueID = _imageProps.propGetString(kOfxImagePropUniqueIdentifier);

    _renderScale.x = _imageProps.propGetDouble(kOfxImageEffectPropRenderScale, 0);
    _renderScale.y = _imageProps.propGetDouble(kOfxImageEffectPropRenderScale, 1);
#ifdef OFX_EXTENSIONS_NUKE
    std::fill(_transform, _transform + 9, 0.);
    if (_imageProps.propGetDimension(kFnOfxPropMatrix2D, false) == 0) {
      // Host does not support transforms, just ignore
      _transformIsIdentity = true;
    } else {
      for (int i = 0; i < 9; ++i) {
        _transform[i] = _imageProps.propGetDouble(kFnOfxPropMatrix2D, i);
      }
      // check if the transform is identity (a zero matrix is considered identity)
      _transformIsIdentity = (_transform[1] == 0. && _transform[2] == 0. &&
                              _transform[3] == 0. && _transform[5] == 0. &&
                              _transform[6] == 0. && _transform[7] == 0. &&
                              _transform[0] == _transform[2] && _transform[0] == _transform[8]);
    }
#endif
  }

  ImageBase::~ImageBase()
  {
  }

  ////////////////////////////////////////////////////////////////////////////////
  // wraps up an image  
  Image::Image(OfxPropertySetHandle props)
    : ImageBase(props)
  {
    OFX::Validation::validateImageProperties(props);

    // and fetch all the properties
    _pixelData = _imageProps.propGetPointer(kOfxImagePropData);
  }

  Image::~Image()
  {
    OFX::Private::gEffectSuite->clipReleaseImage(_imageProps.propSetHandle());
  }

#ifdef OFX_SUPPORTS_OPENGLRENDER
  ////////////////////////////////////////////////////////////////////////////////
  // wraps up an OpenGL texture
  Texture::Texture(OfxPropertySetHandle props)
    : ImageBase(props)
  {
    OFX::Validation::validateTextureProperties(props);
  
    _index = _imageProps.propGetInt(kOfxImageEffectPropOpenGLTextureIndex, 0);
    _target = _imageProps.propGetInt(kOfxImageEffectPropOpenGLTextureTarget, 0);
  }

  Texture::~Texture()
  {
    OfxStatus stat = OFX::Private::gOpenGLRenderSuite->clipFreeTexture(_imageProps.propSetHandle());
    if (stat != kOfxStatOK) {
      throwSuiteStatusException(stat);
    }
  }
#endif

  /** @brief return a pixel pointer

  No attempt made to be uber efficient here.
  */
  void *Image::getPixelAddress(int x, int y)
  {
    // are we in the image bounds
    if(x < _bounds.x1 || x >= _bounds.x2 || y < _bounds.y1 || y >= _bounds.y2 || _pixelBytes == 0)
      return 0;

    char *pix = ((char *) _pixelData) + (size_t)(y - _bounds.y1) * _rowBytes;
    pix += (x - _bounds.x1) * _pixelBytes;
    return (void *) pix;   
  }

  const void *Image::getPixelAddress(int x, int y) const
  {
    // are we in the image bounds
    if(x < _bounds.x1 || x >= _bounds.x2 || y < _bounds.y1 || y >= _bounds.y2 || _pixelBytes == 0)
      return 0;

    const char *pix = ((const char *) _pixelData) + (size_t)(y - _bounds.y1) * _rowBytes;
    pix += (x - _bounds.x1) * _pixelBytes;
    return (const void *) pix;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // clip instance

  /** @brief hidden constructor */
  Clip::Clip(ImageEffect *effect, const std::string &name, OfxImageClipHandle handle, OfxPropertySetHandle props)
    : _clipName(name)
    , _clipProps(props)
    , _clipHandle(handle)
    , _effect(effect)
#ifdef OFX_EXTENSIONS_VEGAS
    , _pixelOrder(ePixelOrderRGBA)
#endif
  {
    OFX::Validation::validateClipInstanceProperties(_clipProps);
  }

  /** @brief fetch the label */
  void Clip::getLabel(std::string &label) const
  {
    label      = _clipProps.propGetString(kOfxPropLabel);
  }

  /** @brief fetch the labels */
  void Clip::getLabels(std::string &label, std::string &shortLabel, std::string &longLabel) const
  {
    getLabel(label);
    shortLabel = _clipProps.propGetString(kOfxPropShortLabel, false);
    longLabel  = _clipProps.propGetString(kOfxPropLongLabel, false);
  }

  /** @brief get the pixel depth */
  BitDepthEnum Clip::getPixelDepth(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageEffectPropPixelDepth);
    BitDepthEnum e;
    try {
      e = mapStrToBitDepthEnum(str);
      if(e == eBitDepthNone && isConnected()) {
        OFX::Log::error(true, "Clip %s is connected and has no pixel depth.", _clipName.c_str());
      }
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown pixel depth property '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
      e = eBitDepthNone;
    }
    return e;
  }

  /** @brief get the components in the image */
  PixelComponentEnum Clip::getPixelComponents(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageEffectPropComponents);
    PixelComponentEnum e;
    try {
      e = mapStrToPixelComponentEnum(str);
      if(e == ePixelComponentNone && isConnected()) {
        OFX::Log::error(true, "Clip %s is connected and has no pixel component type!", _clipName.c_str());
      }
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown  pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
      e = ePixelComponentNone;
    }
    return e;
  }

  /** @brief get the number of components in the image */
  int Clip::getPixelComponentCount(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageEffectPropComponents);
    PixelComponentEnum e;
    try {
      e = mapStrToPixelComponentEnum(str);
      if(e == ePixelComponentNone && isConnected()) {
        OFX::Log::error(true, "Clip %s is connected and has no pixel component type!", _clipName.c_str());
      }
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown  pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
      e = ePixelComponentNone;
    }

    switch (e) {
      case ePixelComponentAlpha:
        return 1;
      case ePixelComponentNone:
        return 0;
#ifdef OFX_EXTENSIONS_NUKE
      case ePixelComponentMotionVectors:
      case ePixelComponentStereoDisparity:
        return 2;
#endif
      case ePixelComponentRGB:
        return 3;
      case ePixelComponentRGBA:
        return 4;
#ifdef OFX_EXTENSIONS_NATRON
      case ePixelComponentXY:
        return 2;
#endif
      case ePixelComponentCustom:
      default:
#ifdef OFX_EXTENSIONS_NATRON
        // first element in the vector is the layer name (if any)
        return std::max((int)mapPixelComponentCustomToLayerChannels(str).size() - 1, 0);
#else
        return 0;
#endif
    }
  }

  /** @brief what is the actual pixel depth of the clip */
  BitDepthEnum Clip::getUnmappedPixelDepth(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageClipPropUnmappedPixelDepth);
    BitDepthEnum e;
    try {
      e = mapStrToBitDepthEnum(str);
      if(e == eBitDepthNone && !isConnected()) {
        OFX::Log::error(true, "Clip %s is connected and has no unmapped pixel depth.", _clipName.c_str());
      }
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown unmapped pixel depth property '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
      e = eBitDepthNone;
    }
    return e;
  }

  /** @brief what is the component type of the clip */
  PixelComponentEnum Clip::getUnmappedPixelComponents(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageClipPropUnmappedComponents);
    PixelComponentEnum e;
    try {
      e = mapStrToPixelComponentEnum(str);
      if(e == ePixelComponentNone && !isConnected()) {
        OFX::Log::error(true, "Clip %s is connected and has no unmapped pixel component type!", _clipName.c_str());
      }
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown unmapped pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
      e = ePixelComponentNone;
    }
    return e;
  }

  /** @brief get the components in the image */
  PreMultiplicationEnum Clip::getPreMultiplication(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageEffectPropPreMultiplication);
    PreMultiplicationEnum e;
    try {
      e = mapStrToPreMultiplicationEnum(str);
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown premultiplication type '%s' reported on clip %s!", str.c_str(), _clipName.c_str());
      e = eImageOpaque;
    }
    return e;
  }

  /** @brief which spatial field comes first temporally */
  FieldEnum Clip::getFieldOrder(void) const
  {
    std::string str = _clipProps.propGetString(kOfxImageClipPropFieldOrder);
    FieldEnum e;
    try {
      e = mapStrToFieldEnum(str);
      OFX::Log::error(e != eFieldNone && e != eFieldLower && e != eFieldUpper, 
        "Field order '%s' reported on a clip %s is invalid, it must be none, lower or upper.", str.c_str(), _clipName.c_str());
    }
    // gone wrong ?
    catch(std::invalid_argument) {
      OFX::Log::error(true, "Unknown field order '%s' reported on a clip %s.", str.c_str(), _clipName.c_str());
      e = eFieldNone;
    }
    return e;
  }

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief get the pixel order of this image */
  PixelOrderEnum Clip::getPixelOrder(void) const
  {
    // only vegas supports this so far, so ignore it if it doesn't work
    std::string str = _clipProps.propGetString(kOfxImagePropPixelOrder, false);
    PixelOrderEnum e;
    if(str == kOfxImagePixelOrderRGBA) {
      e = ePixelOrderRGBA;
    }
    else if(str == kOfxImagePixelOrderBGRA) {
      e = ePixelOrderBGRA;
    }
    else {
      e = ePixelOrderRGBA;
    }
    return e;
  }
#endif

  /** @brief is the clip connected */
  bool Clip::isConnected(void) const
  {
    return _clipProps.propGetInt(kOfxImageClipPropConnected) != 0;
  }

  /** @brief can the clip be continuously sampled */
  bool Clip::hasContinuousSamples(void) const
  {
    return _clipProps.propGetInt(kOfxImageClipPropContinuousSamples) != 0;
  }

  /** @brief get the scale factor that has been applied to this clip */
  double Clip::getPixelAspectRatio(void) const
  {
    try {
      return _clipProps.propGetDouble(kOfxImagePropPixelAspectRatio);
    } catch(...) {
      return 1.0;  // This error could happen in Eyeon Fusion.
    }
  }

  /** @brief get the frame rate, in frames per second on this clip, after any clip preferences have been applied */
  double Clip::getFrameRate(void) const
  {
    return _clipProps.propGetDouble(kOfxImageEffectPropFrameRate);
  }

  /** @brief return the range of frames over which this clip has images, after any clip preferences have been applied */
  OfxRangeD Clip::getFrameRange(void) const
  {
    OfxRangeD v;
    v.min = _clipProps.propGetDouble(kOfxImageEffectPropFrameRange, 0);
    v.max = _clipProps.propGetDouble(kOfxImageEffectPropFrameRange, 1);
    return v;
  }

  /** @brief get the frame rate, in frames per second on this clip, before any clip preferences have been applied */
  double Clip::getUnmappedFrameRate(void) const
  {
    return _clipProps.propGetDouble(kOfxImageEffectPropUnmappedFrameRate);
  }

  /** @brief return the range of frames over which this clip has images, before any clip preferences have been applied */
  OfxRangeD Clip::getUnmappedFrameRange(void) const
  {
    OfxRangeD v;
    v.min = _clipProps.propGetDouble(kOfxImageEffectPropUnmappedFrameRange, 0);
    v.max = _clipProps.propGetDouble(kOfxImageEffectPropUnmappedFrameRange, 1);
    return v;
  }

  /** @brief get the RoD for this clip in the cannonical coordinate system */
  OfxRectD Clip::getRegionOfDefinition(double t)
  {
    OfxRectD bounds;
    OfxStatus stat = OFX::Private::gEffectSuite->clipGetRegionOfDefinition(_clipHandle, t, &bounds);
    if(stat == kOfxStatFailed) {
      bounds.x1 = bounds.x2 = bounds.y1 = bounds.y2 = 0;
    }
    throwSuiteStatusException(stat);
    return bounds;
  }

  /** @brief fetch an image */
  Image *Clip::fetchImage(double t)
  {
    OfxPropertySetHandle imageHandle;
    OfxStatus stat = OFX::Private::gEffectSuite->clipGetImage(_clipHandle, t, NULL, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);

    return new Image(imageHandle);
  }

  /** @brief fetch an image, with a specific region in cannonical coordinates */
  Image *Clip::fetchImage(double t, const OfxRectD &bounds)
  {
    OfxPropertySetHandle imageHandle;
    OfxStatus stat = OFX::Private::gEffectSuite->clipGetImage(_clipHandle, t, &bounds, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);

    return new Image(imageHandle);
  }
    
#ifdef OFX_EXTENSIONS_NUKE
  OfxRectD Clip::getRegionOfDefinition(double t, int view)
  {
    if (!OFX::Private::gImageEffectPlaneSuiteV2) {
      return getRegionOfDefinition(t);
    }
    OfxRectD bounds;
    OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV2->clipGetRegionOfDefinition(_clipHandle, t, view,  &bounds);
    if(stat == kOfxStatFailed) {
      bounds.x1 = bounds.x2 = bounds.y1 = bounds.y2 = 0;
    }
    throwSuiteStatusException(stat);
    return bounds;
  }

  Image* Clip::fetchImagePlane(double t, int view, const char* plane)
  {
    if (!OFX::Private::gImageEffectPlaneSuiteV2) {
      throwHostMissingSuiteException("clipGetImagePlane");
    }
    OfxPropertySetHandle imageHandle;
    OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV2->clipGetImagePlane(_clipHandle, t, view, plane, NULL, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);
      
    return new Image(imageHandle);
  }
    
  Image* Clip::fetchImagePlane(double t, const char* plane)
  {
    if (!OFX::Private::gImageEffectPlaneSuiteV1) {
      throwHostMissingSuiteException("clipGetImagePlane");
    }
    OfxPropertySetHandle imageHandle;
    OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV1->clipGetImagePlane(_clipHandle, t, plane, NULL, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);
      
    return new Image(imageHandle);
  }
    
  Image* Clip::fetchImagePlane(double t, int view, const char* plane, const OfxRectD& bounds)
  {
    if (!OFX::Private::gImageEffectPlaneSuiteV2) {
      throwHostMissingSuiteException("clipGetImagePlane");
    }
    OfxPropertySetHandle imageHandle;
    OfxRectD boundsCopy = bounds;

    OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV2->clipGetImagePlane(_clipHandle, t, view, plane, &boundsCopy, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);

    return new Image(imageHandle);
  }
    
  Image* Clip::fetchImagePlane(double t, const char* plane, const OfxRectD& bounds)
  {
    if (!OFX::Private::gImageEffectPlaneSuiteV1) {
      throwHostMissingSuiteException("clipGetImagePlane");
    }
    OfxPropertySetHandle imageHandle;
    OfxRectD boundsCopy = bounds;

    OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV1->clipGetImagePlane(_clipHandle, t, plane, &boundsCopy, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);

    return new Image(imageHandle);
  }
    
  std::list<std::string> Clip::getComponentsPresent() const
  {
    std::list<std::string> ret = _clipProps.propGetNString(kFnOfxImageEffectPropComponentsPresent, false);
    return ret;
  }
#endif

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief fetch an image */
  Image *Clip::fetchStereoscopicImage(double t, int view)
  {
    if(!OFX::Private::gVegasStereoscopicImageSuite){ throwHostMissingSuiteException("clipGetStereoscopicImage"); }
    if(!OFX::Private::gVegasStereoscopicImageSuite->clipGetStereoscopicImage){ throwHostMissingSuiteException("clipGetStereoscopicImage"); }
    OfxPropertySetHandle imageHandle;
    OfxStatus stat = OFX::Private::gVegasStereoscopicImageSuite->clipGetStereoscopicImage(_clipHandle, t, view, NULL, &imageHandle);
    if(stat == kOfxStatFailed) {
      return NULL; // not an error, fetched images out of range/region, assume black and transparent
    }
    else
      throwSuiteStatusException(stat);

    return new Image(imageHandle);
  }
#endif

#ifdef OFX_SUPPORTS_OPENGLRENDER
  Texture *Clip::loadTexture(double t, BitDepthEnum format, const OfxRectD *region)
  {
    if (!gHostDescription.supportsOpenGLRender) {
      throwHostMissingSuiteException("loadTexture");
    }
    OfxPropertySetHandle hTex;
    OfxStatus stat = Private::gOpenGLRenderSuite->clipLoadTexture(_clipHandle, t, mapBitDepthEnumToStr(format).c_str(), region, &hTex);
    if (stat != kOfxStatOK) {
      throwSuiteStatusException(stat);
    }
    return new Texture(hTex);
  }
#endif
  ////////////////////////////////////////////////////////////////////////////////
  /// image effect 

  /** @brief ctor */
  ImageEffect::ImageEffect(OfxImageEffectHandle handle)
    : _effectHandle(handle)
    , _effectProps(0)
    , _context(eContextNone)
    , _progressStartSuccess(false)
  {
    // get the property handle
    _effectProps = OFX::Private::fetchEffectProps(handle);

    // Set this as the instance data pointer on the effect handle
    _effectProps.propSetPointer(kOfxPropInstanceData, this);

    // validate the plugin instance
    OFX::Validation::validatePluginInstanceProperties(_effectProps);

    // fetch the context
    std::string ctxt = _effectProps.propGetString(kOfxImageEffectPropContext);
    _context = mapToContextEnum(ctxt);

    // the param set daddy-oh
    OfxParamSetHandle paramSet;
    OfxStatus stat = OFX::Private::gEffectSuite->getParamSet(handle, &paramSet);
    throwSuiteStatusException(stat);
    setParamSetHandle(paramSet);      

  }

  /** @brief dtor */
  ImageEffect::~ImageEffect()
  {
    // clobber the instance data property on the effect handle
    _effectProps.propSetPointer(kOfxPropInstanceData, 0);

    // delete any clip instances we may have constructed
    std::map<std::string, Clip *>::iterator iter;
    for(iter = _fetchedClips.begin(); iter != _fetchedClips.end(); ++iter) {
      if(iter->second) {
        delete iter->second;
        iter->second = NULL;
      }
    }
  }

  /** @brief the context this effect was instantiate in */
  ContextEnum ImageEffect::getContext(void) const
  {
    return _context;
  }

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief the Vegas context this effect exists in */
  VegasContextEnum ImageEffect::getVegasContext(void)
  {
    // fetch the context
    std::string ctxt = _effectProps.propGetString(kOfxImageEffectPropVegasContext);
    return mapToVegasContextEnum(ctxt);
  }
#endif

  /** @brief size of the project */
  OfxPointD ImageEffect::getProjectSize(void) const
  {
    OfxPointD v;    
    v.x = _effectProps.propGetDouble(kOfxImageEffectPropProjectSize, 0);
    v.y = _effectProps.propGetDouble(kOfxImageEffectPropProjectSize, 1);
    return v;
  }

  /** @brief origin of the project */
  OfxPointD ImageEffect::getProjectOffset(void) const
  {
    OfxPointD v;    
    v.x = _effectProps.propGetDouble(kOfxImageEffectPropProjectOffset, 0);
    v.y = _effectProps.propGetDouble(kOfxImageEffectPropProjectOffset, 1);
    return v;
  }

  /** @brief extent of the project */
  OfxPointD ImageEffect::getProjectExtent(void) const
  {
    OfxPointD v;    
    v.x = _effectProps.propGetDouble(kOfxImageEffectPropProjectExtent, 0);
    v.y = _effectProps.propGetDouble(kOfxImageEffectPropProjectExtent, 1);
    return v;
  }

  /** @brief pixel aspect ratio of the project */
  double ImageEffect::getProjectPixelAspectRatio(void) const
  {
    return _effectProps.propGetDouble(kOfxImageEffectPropProjectPixelAspectRatio, 0);
  }

  /** @brief how long does the effect last */
  double ImageEffect::getEffectDuration(void) const
  {
    return _effectProps.propGetDouble(kOfxImageEffectInstancePropEffectDuration, 0);
  }

  /** @brief the frame rate of the project */
  double ImageEffect::getFrameRate(void) const
  {
    return _effectProps.propGetDouble(kOfxImageEffectPropFrameRate, 0);
  }

  /** @brief is the instance currently being interacted with */
  bool ImageEffect::isInteractive(void) const
  {
    return _effectProps.propGetInt(kOfxPropIsInteractive) != 0;
  }

  /** @brief set the instance to be sequentially renderred, this should have been part of clip preferences! */
  void ImageEffect::setSequentialRender(bool v)
  {
    _effectProps.propSetInt(kOfxImageEffectInstancePropSequentialRender, int(v));
  }

  /** @brief Have we informed the host we want to be seqentially renderred ? */
  bool ImageEffect::getSequentialRender(void) const
  {
    return _effectProps.propGetInt(kOfxImageEffectInstancePropSequentialRender) != 0;
  }

  /** @brief notify host that the internal data structures need syncing back to parameters for persistance and so on.  This is reset by the host after calling SyncPrivateData. */
  void ImageEffect::setParamSetNeedsSyncing()
  {
    _effectProps.propSetInt(kOfxPropParamSetNeedsSyncing, 1, false); // introduced in OFX 1.2
  }

  OFX::Message::MessageReplyEnum ImageEffect::sendMessage(OFX::Message::MessageTypeEnum type, const std::string& id, const std::string& msg)
  {   
    if(!OFX::Private::gMessageSuite){ throwHostMissingSuiteException("message"); }
    if(!OFX::Private::gMessageSuite->message){ throwHostMissingSuiteException("message"); }
    OfxStatus stat = OFX::Private::gMessageSuite->message(_effectHandle, mapMessageTypeEnumToStr(type), id.c_str(), msg.c_str());
    return mapToMessageReplyEnum(stat);
  }

  OFX::Message::MessageReplyEnum ImageEffect::setPersistentMessage(OFX::Message::MessageTypeEnum type, const std::string& id, const std::string& msg)
  {   
    if(!OFX::Private::gMessageSuiteV2){ throwHostMissingSuiteException("setPersistentMessage"); }
    if(!OFX::Private::gMessageSuiteV2->setPersistentMessage){ throwHostMissingSuiteException("setPersistentMessage"); }
    OfxStatus stat = OFX::Private::gMessageSuiteV2->setPersistentMessage(_effectHandle, mapMessageTypeEnumToStr(type), id.c_str(), msg.c_str());
    return mapToMessageReplyEnum(stat);
  }

  OFX::Message::MessageReplyEnum ImageEffect::clearPersistentMessage()
  {   
    if(!OFX::Private::gMessageSuiteV2){ throwHostMissingSuiteException("clearPersistentMessage"); }
    if(!OFX::Private::gMessageSuiteV2->clearPersistentMessage){ throwHostMissingSuiteException("clearPersistentMessage"); }
    OfxStatus stat = OFX::Private::gMessageSuiteV2->clearPersistentMessage(_effectHandle);
    return mapToMessageReplyEnum(stat);
  }

  /** @brief Fetch the named clip from this instance */
  Clip *ImageEffect::fetchClip(const std::string &name)
  {
    // do we have the clip already
    std::map<std::string, Clip *>::const_iterator search;
    search = _fetchedClips.find(name);
    if(search != _fetchedClips.end())
      return search->second;

    // fetch the property set handle of the effect
    OfxImageClipHandle clipHandle = 0;
    OfxPropertySetHandle propHandle = 0;
    OfxStatus stat = OFX::Private::gEffectSuite->clipGetHandle(_effectHandle, name.c_str(), &clipHandle, &propHandle);
    throwSuiteStatusException(stat);

    // and make one
    Clip *newClip = new Clip(this, name, clipHandle, propHandle);

    // add it in
    _fetchedClips[name] = newClip;

    // return it
    return newClip;
  }

#ifdef OFX_EXTENSIONS_NUKE
  /** @brief Fetch a camera param */
  CameraParam* ImageEffect::fetchCameraParam(const std::string& name) const
  {
    CameraParam *paramPtr;
    fetchAttribute(_effectHandle, name, paramPtr);
    return paramPtr;
  }
#endif

  /** @brief does the host want us to abort rendering? */
  bool ImageEffect::abort(void) const
  {
    return OFX::Private::gEffectSuite->abort(_effectHandle) != 0;
  }

  /** @brief adds a new interact to the set of interacts open on this effect */
  void ImageEffect::addOverlayInteract(OverlayInteract *interact)
  {
    // do we have it already ?
    std::list<OverlayInteract *>::iterator i;
    i = std::find(_overlayInteracts.begin(), _overlayInteracts.end(), interact);

    // we don't, put it in there
    if(i == _overlayInteracts.end()) {
      // we have a new one to add in here
      _overlayInteracts.push_back(interact);
    }
  }

  /** @brief removes an interact to the set of interacts open on this effect */
  void ImageEffect::removeOverlayInteract(OverlayInteract *interact)
  {
    // find it
    std::list<OverlayInteract *>::iterator i;
    i = std::find(_overlayInteracts.begin(), _overlayInteracts.end(), interact);

    // and remove it
    if(i != _overlayInteracts.end()) {
      _overlayInteracts.erase(i);
    }
  }

  /** @brief force all overlays on this interact to be redrawn */
  void ImageEffect::redrawOverlays(void)
  {
    // find it
    std::list<OverlayInteract *>::iterator i;
    for(i = _overlayInteracts.begin(); i != _overlayInteracts.end(); ++i) {
      (*i)->requestRedraw();
    }
  }

#ifdef OFX_SUPPORTS_OPENGLRENDER
    bool ImageEffect::flushOpenGLResources(void)
    {
      if (!gHostDescription.supportsOpenGLRender) {
        return false;
      }
      return Private::gOpenGLRenderSuite->flushResources() == kOfxStatOK;
    }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  // below are the default members for the base image effect


  /** @brief client is identity function, returns the clip and time for the identity function 
  */
  bool ImageEffect::isIdentity(const IsIdentityArguments &/*args*/, Clip * &/*identityClip*/, double &/*identityTime*/)
  {
    return false; // by default, we are not an identity operation
  }

  /** @brief The get RoD action */
  bool ImageEffect::getRegionOfDefinition(const RegionOfDefinitionArguments &/*args*/, OfxRectD &/*rod*/)
  {
    return false; // by default, we are not setting the RoD
  }

  /** @brief the get RoI action */
  void ImageEffect::getRegionsOfInterest(const RegionsOfInterestArguments &/*args*/, RegionOfInterestSetter &/*rois*/)
  {
    // fa niente
  }

  /** @brief the get frames needed action */
  void ImageEffect::getFramesNeeded(const FramesNeededArguments &/*args*/, FramesNeededSetter &/*frames*/)
  {
    // fa niente
  }

  /** @brief client begin sequence render function */
  void ImageEffect::beginSequenceRender(const BeginSequenceRenderArguments &/*args*/)
  {
    // fa niente
  }

  /** @brief client end sequence render function, this is one of the few that must be set */
  void ImageEffect::endSequenceRender(const EndSequenceRenderArguments &/*args*/)
  {
    // fa niente
  }

  /** @brief The purge caches action, a request for an instance to free up as much memory as possible in low memory situations */
  void ImageEffect::purgeCaches(void)
  {
    // fa niente
  }

  /** @brief The sync private data action, called when the effect needs to sync any private data to persistant parameters */
  void ImageEffect::syncPrivateData(void)
  {
    // fa niente
  }

  /** @brief get the clip preferences */
  void ImageEffect::getClipPreferences(ClipPreferencesSetter &/*clipPreferences*/)
  {
    // fa niente
  }

  /** @brief the effect is about to be actively edited by a user, called when the first user interface is opened on an instance */
  void ImageEffect::beginEdit(void)
  {
    // fa niente
  }

  /** @brief the effect is no longer being edited by a user, called when the last user interface is closed on an instance */
  void ImageEffect::endEdit(void)
  {
    // fa niente
  }

  /** @brief the effect is about to have some values changed */
  void ImageEffect::beginChanged(InstanceChangeReason /*reason*/)
  {
  }

  /** @brief called when a param has just had its value changed */
  void ImageEffect::changedParam(const InstanceChangedArgs &/*args*/, const std::string &/*paramName*/)
  {
  }

  /** @brief called when a clip has just been changed in some way (a rewire maybe) */
  void ImageEffect::changedClip(const InstanceChangedArgs &/*args*/, const std::string &/*clipName*/)
  {
  }

  /** @brief the effect has just had some values changed */
  void ImageEffect::endChanged(InstanceChangeReason /*reason*/)
  {
  }

  /** @brief get the time domain */
  bool ImageEffect::getTimeDomain(OfxRangeD &/*range*/)
  {
    // by default, do the default
    return false;
  }

#ifdef OFX_SUPPORTS_OPENGLRENDER
  /** @brief OpenGL context attached */
  void ImageEffect::contextAttached(void)
  {
    // fa niente
  }

  /** @brief OpenGL context detached */
  void ImageEffect::contextDetached(void)
  {
    // fa niente
  }
#endif

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief Vegas requires conversion of keyframe data */
  void ImageEffect::upliftVegasKeyframes(const SonyVegasUpliftArguments &/*upliftInfo*/)
  {
    // fa niente
  }

  /** @brief Vegas requests custom about dialog */
  bool ImageEffect::invokeAbout()
  {
    // by default, do nothing
    return false;
  }

  /** @brief Vegas requests custom help dialog */
  bool ImageEffect::invokeHelp()
  {
    // by default, do nothing
    return false;
  }
#endif

#ifdef OFX_EXTENSIONS_NUKE
  void ImageEffect::getClipComponents(const ClipComponentsArguments& /*args*/, ClipComponentsSetter& /*clipComponents*/)
  {
        // pass
  }
    
  void ImageEffect::getFrameViewsNeeded(const FrameViewsNeededArguments & /*args*/, FrameViewsNeededSetter & /*frameViews*/)
  {
        // pass
  }
    
  /** @brief recover a transform matrix from an effect */
  bool ImageEffect::getTransform(const TransformArguments &/*args*/, Clip * &/*transformClip*/, double /*transformMatrix*/[9])
  {
    // by default, do the default
    return false;
  }
    
    
  std::string ImageEffect::getViewName(int viewIndex) const
  {
      char* viewName;
      OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV2->getViewName(_effectHandle, viewIndex, &viewName);
      if(stat == kOfxStatFailed) {
          return std::string();
      }
      throwSuiteStatusException(stat);
      return std::string(viewName);
  }
    
  int ImageEffect::getViewCount() const
  {
      int viewCount;
      OfxStatus stat = OFX::Private::gImageEffectPlaneSuiteV2->getViewCount(_effectHandle, &viewCount);
      if(stat == kOfxStatFailed) {
          return 0;
      }
      throwSuiteStatusException(stat);
      return viewCount;
  }

#endif

  /** @brief called when a custom param needs to be interpolated */
  std::string ImageEffect::interpolateCustomParam(const InterpolateCustomArgs &args, const std::string &/*paramName*/)
  {
      return args.value1;
  }

  /// Start doing progress. 
  void ImageEffect::progressStart(const std::string &message)
  {
    if(OFX::Private::gProgressSuite) {
      OfxStatus stat = OFX::Private::gProgressSuite->progressStart((void *) _effectHandle, message.c_str());
      _progressStartSuccess = ( stat == kOfxStatOK );
    }
  }

#ifdef OFX_EXTENSIONS_VEGAS
  /// Start doing progress. 
  void ImageEffect::progressStart(const std::string &id, const std::string &message)
  {
    if(OFX::Private::gVegasProgressSuite) {
        OfxStatus stat = OFX::Private::gVegasProgressSuite->progressStart((void *) _effectHandle, message.c_str(), id.c_str());
      _progressStartSuccess = ( stat == kOfxStatOK );
    }
    else if(OFX::Private::gProgressSuite) {
      OfxStatus stat = OFX::Private::gProgressSuite->progressStart((void *) _effectHandle, message.c_str());
      _progressStartSuccess = ( stat == kOfxStatOK );
    }
  }
#endif

  /// finish yer progress
  void ImageEffect::progressEnd()
  {
    if(OFX::Private::gProgressSuite && _progressStartSuccess) {
      OFX::Private::gProgressSuite->progressEnd((void *) _effectHandle);
    }
  }

  /// set the progress to some level of completion, returns
  /// false if you should abandon processing, true to continue
  bool ImageEffect::progressUpdate(double t)
  {
#ifdef OFX_EXTENSIONS_VEGAS
    if(OFX::Private::gVegasProgressSuite && _progressStartSuccess) {
      OFX::Private::gVegasProgressSuite->progressEnd((void *) _effectHandle);
    }
    else
#endif
    if(OFX::Private::gProgressSuite && _progressStartSuccess) {
      OfxStatus stat = OFX::Private::gProgressSuite->progressUpdate((void *) _effectHandle, t);
      if(stat == kOfxStatReplyNo)
        return false;
    }
    return true;
  }

  /// get the current time on the timeline. This is not necessarily the same
  /// time as being passed to an action (eg render)
  double ImageEffect::timeLineGetTime()
  {
    if(OFX::Private::gTimeLineSuite) {
      double time;
      if(OFX::Private::gTimeLineSuite->getTime((void *) _effectHandle, &time) == kOfxStatOK)
        return time;
    }
    return 0;
  }

  /// set the timeline to a specific time
  void ImageEffect::timeLineGotoTime(double t)
  {
    if(OFX::Private::gTimeLineSuite) {
      OFX::Private::gTimeLineSuite->gotoTime((void *) _effectHandle, t);
    }
  }

  /// get the first and last times available on the effect's timeline
  void ImageEffect:: timeLineGetBounds(double &t1, double &t2)
  {
    if(OFX::Private::gTimeLineSuite) {
      OFX::Private::gTimeLineSuite->getTimeBounds((void *) _effectHandle, &t1, &t2);
      return;
    }
    t1 = t2 = 0;
  }

#ifdef OFX_EXTENSIONS_VEGAS
  ////////////////////////////////////////////////////////////////////////////////
  // Class used to uplift previous vegas keyframe data of the effect. */ 
  SonyVegasUpliftArguments::SonyVegasUpliftArguments(PropertySet args)
    : guidUplift()
    , keyframeCount(0)
    , commonData(0)
    , commonDataSize(0)
  {
      _argProps = args;
  }

  void*  SonyVegasUpliftArguments::getKeyframeData     (int keyframeIndex) const
  {
      return _argProps.propGetPointer(kOfxPropVegasUpliftKeyframeData, keyframeIndex);
  }

  int    SonyVegasUpliftArguments::getKeyframeDataSize (int keyframeIndex) const
  {
      return _argProps.propGetInt(kOfxPropVegasUpliftKeyframeDataLength, keyframeIndex);
  }

  double SonyVegasUpliftArguments::getKeyframeTime     (int keyframeIndex) const
  {
      return _argProps.propGetDouble(kOfxPropVegasUpliftKeyframeTime, keyframeIndex);
  }

  VegasInterpolationEnum SonyVegasUpliftArguments::getKeyframeInterpolation (int keyframeIndex) const
  {
      return mapToInterpolationEnum(_argProps.propGetString(kOfxPropVegasUpliftKeyframeInterpolation, keyframeIndex));
  }
#endif
    
#ifdef OFX_EXTENSIONS_NUKE
  const std::string& ClipComponentsSetter::extractValueForName(const StringStringMap& m, const std::string& name)
  {
      StringStringMap::const_iterator it = m.find(name);
      if(it==m.end())
          throw(Exception::PropertyUnknownToHost(name.c_str()));
      return it->second;
  }
    
  void ClipComponentsSetter::addClipComponents(Clip& clip, PixelComponentEnum comps)
  {
      _doneSomething = true;
      const std::string& propName = extractValueForName(_clipPlanesPropNames, clip.name());
      std::string compName;
      switch(comps)
      {
        case ePixelComponentNone :
            compName = kOfxImageComponentNone;
            break;
        case ePixelComponentRGBA :
            compName = kOfxImageComponentRGBA;
            break;
        case ePixelComponentRGB :
            compName = kOfxImageComponentRGB;
            break;
        case ePixelComponentAlpha :
            compName = kOfxImageComponentAlpha;
            break;
#ifdef OFX_EXTENSIONS_NUKE
        case ePixelComponentMotionVectors :
            compName = kFnOfxImageComponentMotionVectors;
            break;
        case ePixelComponentStereoDisparity :
            compName = kFnOfxImageComponentStereoDisparity;
            break;
#endif
#ifdef OFX_EXTENSIONS_NATRON
        case ePixelComponentXY:
            compName = kNatronOfxImageComponentXY;
            break;
#endif
        case ePixelComponentCustom :
            break;
      }
      if (!propName.empty()) {
          int dim = _outArgs.propGetDimension(propName.c_str());
          _outArgs.propSetString(propName.c_str(), compName, dim);
      }
  }
    
  void ClipComponentsSetter::addClipComponents(Clip& clip, const std::string& comps)
  {
     _doneSomething = true;
     const std::string& propName = extractValueForName(_clipPlanesPropNames, clip.name());
     if (!propName.empty()) {
        int dim = _outArgs.propGetDimension(propName.c_str());
        _outArgs.propSetString(propName.c_str(), comps, dim);
     }
  }
    
  void ClipComponentsSetter::setPassThroughClip(const Clip* clip,double time,int view)
  {
      _doneSomething = true;
      if (clip) {
        _outArgs.propSetString(kFnOfxImageEffectPropPassThroughClip, clip->name(), 0);
      } else {
        _outArgs.propSetString(kFnOfxImageEffectPropPassThroughClip, "", 0);
      }
      _outArgs.propSetDouble(kFnOfxImageEffectPropPassThroughTime, time, 0);
      _outArgs.propSetInt(kFnOfxImageEffectPropPassThroughView, view, 0);
  }

    
  const std::string& FrameViewsNeededSetter::extractValueForName(const StringStringMap& m, const std::string& name)
  {
        StringStringMap::const_iterator it = m.find(name);
        if(it==m.end())
            throw(Exception::PropertyUnknownToHost(name.c_str()));
        return it->second;
  }
    

  void FrameViewsNeededSetter::addFrameViewsNeeded(const Clip& clip,const OfxRangeD &range, int view)
  {
      _doneSomething = true;
      const std::string& propName = extractValueForName(_clipFrameViewsPropnames, clip.name());
      int dim = _outArgs.propGetDimension(propName.c_str());
      _outArgs.propSetDouble(propName.c_str(), range.min, dim);
      _outArgs.propSetDouble(propName.c_str(), range.max, dim + 1);
      _outArgs.propSetDouble(propName.c_str(), view, dim + 2);
  }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  // Class used to set the clip preferences of the effect. */ 

  const std::string& ClipPreferencesSetter::extractValueForName(const StringStringMap& m, const std::string& name)
  {
    StringStringMap::const_iterator it = m.find(name);
    if(it==m.end())
      throw(Exception::PropertyUnknownToHost(name.c_str()));
    return it->second;
  }

  /** @brief, force the host to set a clip's mapped component type to be \em comps.  */
  void ClipPreferencesSetter::setClipComponents(Clip &clip, PixelComponentEnum comps)
  {
    doneSomething_ = true;
    const std::string& propName = extractValueForName(clipComponentPropNames_, clip.name());

    switch(comps) 
    {
    case ePixelComponentNone :
      outArgs_.propSetString(propName.c_str(), kOfxImageComponentNone);
      break;
    case ePixelComponentRGBA : 
      outArgs_.propSetString(propName.c_str(), kOfxImageComponentRGBA); 
      break;
    case ePixelComponentRGB :
      outArgs_.propSetString(propName.c_str(), kOfxImageComponentRGB);
      break;
    case ePixelComponentAlpha : 
      outArgs_.propSetString(propName.c_str(), kOfxImageComponentAlpha); 
      break;
#ifdef OFX_EXTENSIONS_NUKE
    case ePixelComponentMotionVectors :
      outArgs_.propSetString(propName.c_str(), kFnOfxImageComponentMotionVectors);
      break;
    case ePixelComponentStereoDisparity :
      outArgs_.propSetString(propName.c_str(), kFnOfxImageComponentStereoDisparity);
      break;
#endif
#ifdef OFX_EXTENSIONS_NATRON
    case ePixelComponentXY:
      outArgs_.propSetString(propName.c_str(), kNatronOfxImageComponentXY);
      break;
#endif
    case ePixelComponentCustom :
      break;
    }
  }

  /** @brief, force the host to set a clip's mapped bit depth be \em bitDepth */
  void ClipPreferencesSetter::setClipBitDepth(Clip &clip, BitDepthEnum bitDepth)
  {
    doneSomething_ = true;
    const std::string& propName = extractValueForName(clipDepthPropNames_, clip.name());

    switch(bitDepth) 
    {
    case eBitDepthNone :
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthNone);
      break;
    case eBitDepthUByte : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthByte); 
      break;
    case eBitDepthUShort : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthShort); 
      break;
    case eBitDepthHalf :
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthHalf);
      break;
    case eBitDepthFloat :
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthFloat); 
      break;
#ifdef OFX_EXTENSIONS_VEGAS
    case eBitDepthUByteBGRA : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthByteBGR); 
      break;
    case eBitDepthUShortBGRA : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthShortBGR); 
      break;
    case eBitDepthFloatBGRA : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthFloatBGR); 
      break;
#endif
    case eBitDepthCustom :
      break;
    }
  }

  /** @brief, force the host to set a clip's mapped Pixel Aspect Ratio to be \em PAR */
  void ClipPreferencesSetter::setPixelAspectRatio(Clip &clip, double PAR)
  {
    doneSomething_ = true;
    const std::string& propName = extractValueForName(clipPARPropNames_, clip.name());
    outArgs_.propSetDouble(propName.c_str(), PAR);
  }

  /** @brief Allows an effect to change the output frame rate */
  void ClipPreferencesSetter::setOutputFrameRate(double v)
  {
    doneSomething_ = true;
    outArgs_.propSetDouble(kOfxImageEffectPropFrameRate, v);
  }

  /** @brief Set the premultiplication state of the output clip. */
  void ClipPreferencesSetter::setOutputPremultiplication(PreMultiplicationEnum v)
  {
    doneSomething_ = true;
    switch(v) 
    {
    case eImageOpaque : 
      outArgs_.propSetString(kOfxImageEffectPropPreMultiplication, kOfxImageOpaque); 
      break;
    case eImagePreMultiplied: 
      outArgs_.propSetString(kOfxImageEffectPropPreMultiplication, kOfxImagePreMultiplied); 
      break;
    case eImageUnPreMultiplied:  
      outArgs_.propSetString(kOfxImageEffectPropPreMultiplication, kOfxImageUnPreMultiplied); 
      break;
    }
  }

  /** @brief Set whether the effect can be continously sampled. */
  void ClipPreferencesSetter::setOutputHasContinousSamples(bool v)
  {
    doneSomething_ = true;
    outArgs_.propSetInt(kOfxImageClipPropContinuousSamples, int(v));
  }

  /** @brief Sets whether the effect will produce different images in all frames, even if the no params or input images are varying (eg: a noise generator). */
  void ClipPreferencesSetter::setOutputFrameVarying(bool v)
  {
    doneSomething_ = true;
    outArgs_.propSetInt(kOfxImageEffectFrameVarying, int(v));
  }


  void  ClipPreferencesSetter::setOutputFielding(FieldEnum v)
  {
    doneSomething_ = true;
    switch(v) 
    {
    case eFieldNone : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldNone, 0, false); break;
    case eFieldLower : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldLower, 0, false); break;
    case eFieldUpper : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldUpper, 0, false); break;
    case eFieldBoth : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldBoth, 0, false); break;
    case eFieldSingle : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldSingle, 0, false); break;
    case eFieldDoubled : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldDoubled, 0, false); break;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Class that skins image memory allocation */

  /** @brief ctor */
  ImageMemory::ImageMemory(size_t nBytes, ImageEffect *associatedEffect)
    : _handle(0)
  {
    OfxImageEffectHandle effectHandle = 0;
    if(associatedEffect != 0) {
      effectHandle = associatedEffect->_effectHandle;
    }

    OfxStatus stat = OFX::Private::gEffectSuite->imageMemoryAlloc(effectHandle, nBytes, &_handle);
    if(stat == kOfxStatErrMemory)
      throw std::bad_alloc();
    throwSuiteStatusException(stat);
  }

  /** @brief dtor */
  ImageMemory::~ImageMemory()
  {
    OfxStatus stat = OFX::Private::gEffectSuite->imageMemoryFree(_handle);
    // ignore status code for exception purposes
    (void)stat;
  }

  /** @brief lock the memory and return a pointer to it */
  void *ImageMemory::lock(void)
  {
    void *ptr;
    OfxStatus stat = OFX::Private::gEffectSuite->imageMemoryLock(_handle, &ptr);
    if(stat == kOfxStatErrMemory)
      throw std::bad_alloc();
    throwSuiteStatusException(stat);
    return ptr;
  }

  /** @brief unlock the memory */
  void ImageMemory::unlock(void)
  {
    OfxStatus stat = OFX::Private::gEffectSuite->imageMemoryUnlock(_handle);
    (void)stat;
  }



  /** @brief OFX::Private namespace, for things private to the support library code here generally calls image effect class members */
  namespace Private {

    /** @brief Creates the global host description and sets its properties */
    static
    void
      fetchHostDescription(OfxHost *host)
    {
      OFX::Log::error(OFX::gHostDescriptionHasInit, "Tried to create host description when we already have one.");
      if(!OFX::gHostDescriptionHasInit) {
        OFX::gHostDescriptionHasInit = true;
        // wrap the property handle up with a property set
        PropertySet hostProps(host->host);

        // and get some properties
        gHostDescription.APIVersionMajor            = hostProps.propGetInt(kOfxPropAPIVersion, 0, false); // OFX 1.2
        if (gHostDescription.APIVersionMajor == 0) {
          // assume OFX 1.0
          gHostDescription.APIVersionMajor = 1;
        }
        gHostDescription.APIVersionMinor            = hostProps.propGetInt(kOfxPropAPIVersion, 1, false); // OFX 1.2
        gHostDescription.hostName                   = hostProps.propGetString(kOfxPropName);
        gHostDescription.hostLabel                  = hostProps.propGetString(kOfxPropLabel);
        gHostDescription.versionMajor               = hostProps.propGetInt(kOfxPropVersion, 0, false); // OFX 1.2
        gHostDescription.versionMinor               = hostProps.propGetInt(kOfxPropVersion, 1, false); // OFX 1.2
        gHostDescription.versionMicro               = hostProps.propGetInt(kOfxPropVersion, 2, false); // OFX 1.2
        gHostDescription.versionLabel               = hostProps.propGetString(kOfxPropVersionLabel, false); // OFX 1.2
        gHostDescription.hostIsBackground           = hostProps.propGetInt(kOfxImageEffectHostPropIsBackground) != 0;
        gHostDescription.supportsOverlays           = hostProps.propGetInt(kOfxImageEffectPropSupportsOverlays) != 0;
        gHostDescription.supportsMultiResolution    = hostProps.propGetInt(kOfxImageEffectPropSupportsMultiResolution) != 0;
        gHostDescription.supportsTiles              = hostProps.propGetInt(kOfxImageEffectPropSupportsTiles) != 0;
        gHostDescription.temporalClipAccess         = hostProps.propGetInt(kOfxImageEffectPropTemporalClipAccess) != 0;
        gHostDescription.supportsMultipleClipDepths = hostProps.propGetInt(kOfxImageEffectPropSupportsMultipleClipDepths) != 0;
        gHostDescription.supportsMultipleClipPARs   = hostProps.propGetInt(kOfxImageEffectPropSupportsMultipleClipPARs) != 0;
        gHostDescription.supportsSetableFrameRate   = hostProps.propGetInt(kOfxImageEffectPropSetableFrameRate) != 0;
        gHostDescription.supportsSetableFielding    = hostProps.propGetInt(kOfxImageEffectPropSetableFielding) != 0;
        gHostDescription.sequentialRender           = hostProps.propGetInt(kOfxImageEffectInstancePropSequentialRender, false); // appeared in OFX 1.2
        gHostDescription.supportsStringAnimation    = hostProps.propGetInt(kOfxParamHostPropSupportsStringAnimation) != 0;
        gHostDescription.supportsCustomInteract     = hostProps.propGetInt(kOfxParamHostPropSupportsCustomInteract) != 0;
        gHostDescription.supportsChoiceAnimation    = hostProps.propGetInt(kOfxParamHostPropSupportsChoiceAnimation) != 0;
        gHostDescription.supportsBooleanAnimation   = hostProps.propGetInt(kOfxParamHostPropSupportsBooleanAnimation) != 0;
        gHostDescription.supportsCustomAnimation    = hostProps.propGetInt(kOfxParamHostPropSupportsCustomAnimation) != 0;
        gHostDescription.supportsParametricParameter = gParametricParameterSuite != 0;
#ifdef OFX_SUPPORTS_OPENGLRENDER
        gHostDescription.supportsOpenGLRender = gOpenGLRenderSuite != 0 && hostProps.propGetString(kOfxImageEffectPropOpenGLRenderSupported, 0, false) == "true";
#endif
#ifdef OFX_EXTENSIONS_NUKE
        gHostDescription.supportsCameraParameter    = gCameraParameterSuite != 0;
        gHostDescription.canTransform               = hostProps.propGetInt(kFnOfxImageEffectCanTransform, false) != 0;
        gHostDescription.isMultiPlanar              = hostProps.propGetInt(kFnOfxImageEffectPropMultiPlanar, false) != 0;
#endif
        gHostDescription.maxParameters              = hostProps.propGetInt(kOfxParamHostPropMaxParameters);
        gHostDescription.maxPages                   = hostProps.propGetInt(kOfxParamHostPropMaxPages);
        gHostDescription.pageRowCount               = hostProps.propGetInt(kOfxParamHostPropPageRowColumnCount, 0);
        gHostDescription.pageColumnCount            = hostProps.propGetInt(kOfxParamHostPropPageRowColumnCount, 1);
#ifdef OFX_EXTENSIONS_NATRON
        gHostDescription.isNatron                   = hostProps.propGetInt(kNatronOfxHostIsNatron, false) != 0;
        gHostDescription.supportsDynamicChoices     = hostProps.propGetInt(kNatronOfxParamHostPropSupportsDynamicChoices, false) != 0;
        gHostDescription.supportsCascadingChoices   = hostProps.propGetInt(kNatronOfxParamPropChoiceCascading, false) != 0;
        gHostDescription.supportsChannelSelector    = hostProps.propGetString(kNatronOfxImageEffectPropChannelSelector, false) == kOfxImageComponentRGBA;
#endif

        int numComponents = hostProps.propGetDimension(kOfxImageEffectPropSupportedComponents);
        for(int i=0; i<numComponents; ++i)
          gHostDescription._supportedComponents.push_back(mapStrToPixelComponentEnum(hostProps.propGetString(kOfxImageEffectPropSupportedComponents, i)));

        int numContexts = hostProps.propGetDimension(kOfxImageEffectPropSupportedContexts);
        for(int i=0; i<numContexts; ++i)
          gHostDescription._supportedContexts.push_back(mapToContextEnum(hostProps.propGetString(kOfxImageEffectPropSupportedContexts, i)));

        int numPixelDepths = hostProps.propGetDimension(kOfxImageEffectPropSupportedPixelDepths);
        for(int i=0; i<numPixelDepths; ++i)
          gHostDescription._supportedPixelDepths.push_back(mapStrToBitDepthEnum(hostProps.propGetString(kOfxImageEffectPropSupportedPixelDepths, i)));
      }
        
    }

    /** @brief fetch the effect property set from the ImageEffectHandle */
    OFX::PropertySet
      fetchEffectProps(OfxImageEffectHandle handle)
    {
      // get the property handle
      OfxPropertySetHandle propHandle;
      OfxStatus stat = OFX::Private::gEffectSuite->getPropertySet(handle, &propHandle);
      throwSuiteStatusException(stat);
      return OFX::PropertySet(propHandle);
    }

    /** @brief Keeps count of how many times load/unload have been called */
    int gLoadCount = 0;

    /** @brief Library side load action, this fetches all the suite pointers */
    void loadAction(void)
    {
      gLoadCount++;  

      //OfxStatus status = kOfxStatOK;

      // fetch the suites
      OFX::Log::error(gHost == 0, "Host pointer has not been set.");
      if(!gHost) throw OFX::Exception::Suite(kOfxStatErrBadHandle);

      if(gLoadCount == 1) {
        gEffectSuite    = (OfxImageEffectSuiteV1 *) fetchSuite(kOfxImageEffectSuite, 1);
        gPropSuite      = (OfxPropertySuiteV1 *)    fetchSuite(kOfxPropertySuite, 1);
        gParamSuite     = (OfxParameterSuiteV1 *)   fetchSuite(kOfxParameterSuite, 1);
        gMemorySuite    = (OfxMemorySuiteV1 *)      fetchSuite(kOfxMemorySuite, 1);
        gThreadSuite    = (OfxMultiThreadSuiteV1 *) fetchSuite(kOfxMultiThreadSuite, 1);
        gMessageSuite   = (OfxMessageSuiteV1 *)     fetchSuite(kOfxMessageSuite, 1);
        gMessageSuiteV2 = (OfxMessageSuiteV2 *)     fetchSuite(kOfxMessageSuite, 2, true);
        gProgressSuite   = (OfxProgressSuiteV1 *)     fetchSuite(kOfxProgressSuite, 1, true);
        gTimeLineSuite   = (OfxTimeLineSuiteV1 *)     fetchSuite(kOfxTimeLineSuite, 1, true);
        gParametricParameterSuite = (OfxParametricParameterSuiteV1*) fetchSuite(kOfxParametricParameterSuite, 1, true);
#ifdef OFX_SUPPORTS_OPENGLRENDER
        gOpenGLRenderSuite = (OfxImageEffectOpenGLRenderSuiteV1*) fetchSuite(kOfxOpenGLRenderSuite, 1, true);
#endif
#ifdef OFX_EXTENSIONS_NUKE
        gCameraParameterSuite = (NukeOfxCameraSuiteV1*) fetchSuite(kNukeOfxCameraSuite, 1, true );
        gImageEffectPlaneSuiteV1 = (FnOfxImageEffectPlaneSuiteV1*) fetchSuite(kFnOfxImageEffectPlaneSuite, 1, true );
        gImageEffectPlaneSuiteV2 = (FnOfxImageEffectPlaneSuiteV2*) fetchSuite(kFnOfxImageEffectPlaneSuite, 2, true );
#endif
#ifdef OFX_EXTENSIONS_VEGAS
        gVegasProgressSuite   = (OfxVegasProgressSuiteV1 *)     fetchSuite(kOfxVegasProgressSuite, 1, true);
        gVegasStereoscopicImageSuite  = (OfxVegasStereoscopicImageSuiteV1 *) fetchSuite(kOfxVegasStereoscopicImageEffectSuite, 1, true);
        gVegasKeyframeSuite   = (OfxVegasKeyframeSuiteV1 *)     fetchSuite(kOfxVegasKeyframeSuite, 1, true);
#endif

        // OK check and fetch host information
        fetchHostDescription(gHost);

        /// and set some dendent flags
        OFX::gHostDescription.supportsMessageSuiteV2 = gMessageSuiteV2 != NULL;
#ifdef OFX_EXTENSIONS_VEGAS
        OFX::gHostDescription.supportsProgressSuite = gProgressSuite != NULL || gVegasProgressSuite != NULL;
#else
        OFX::gHostDescription.supportsProgressSuite = gProgressSuite != NULL;
#endif
        OFX::gHostDescription.supportsTimeLineSuite = gTimeLineSuite != NULL;

        // fetch the interact suite if the host supports interaction
        if(OFX::gHostDescription.supportsOverlays || OFX::gHostDescription.supportsCustomInteract)
          gInteractSuite  = (OfxInteractSuiteV1 *)    fetchSuite(kOfxInteractSuite, 1);

#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
        gHWNDInteractSuite  = (OfxHWNDInteractSuiteV1 *)    fetchSuite(kOfxHWndInteractSuite, 1, true);
#endif // #if defined(WIN32) || defined(WIN64)
#endif
      }

      // initialise the validation code
      OFX::Validation::initialise();

      // validate the host
      OFX::Validation::validateHostProperties(gHost);

    }

    /** @brief Library side unload action, this fetches all the suite pointers */
    static
    void unloadAction(const char* id)
    {
      gLoadCount--;
      if (gLoadCount<0) {
        OFX::Log::warning(true, "OFX Plugin '%s' is already unloaded.", id);
        return;
      }

      if(gLoadCount==0)
      {
        // force these to null
        gEffectSuite = 0;
        gPropSuite = 0;
        gParamSuite = 0;
        gMemorySuite = 0;
        gThreadSuite = 0;
        gMessageSuite = 0;
        gMessageSuiteV2 = 0;
        gInteractSuite = 0;
        gParametricParameterSuite = 0;
#ifdef OFX_EXTENSIONS_NUKE
        gCameraParameterSuite = 0;
        gImageEffectPlaneSuiteV1 = 0;
        gImageEffectPlaneSuiteV2 = 0;
#endif
#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
        gHWNDInteractSuite  = 0;
#endif // #if defined(WIN32) || defined(WIN64)
        gVegasStereoscopicImageSuite  = 0;
        gVegasKeyframeSuite  = 0;
#endif
      }

      {
        EffectDescriptorMap::iterator it = gEffectDescriptors.find(id);
        EffectContextMap& toBeDeleted = it->second;
        for(EffectContextMap::iterator it2 = toBeDeleted.begin(); it2 != toBeDeleted.end(); ++it2)
        {
          OFX::ImageEffectDescriptor* desc = it2->second;
          delete desc;
        }
        toBeDeleted.clear();
      }
      { 
        OFX::OfxPlugInfoMap::iterator it = OFX::plugInfoMap.find(id);
        OfxPlugin* plug = it->second._plug;
        OFX::OfxPluginArray::iterator it2 = std::find(ofxPlugs.begin(), ofxPlugs.end(), plug);
        if (it2 != ofxPlugs.end()) {
          (*it2) = 0;
        }
        delete plug;
      }
    }


    /** @brief fetches our pointer out of the props on the handle */
    ImageEffect *retrieveImageEffectPointer(OfxImageEffectHandle handle) 
    {
      ImageEffect *instance;

      // get the prop set on the handle
      OfxPropertySetHandle propHandle;
      OfxStatus stat = OFX::Private::gEffectSuite->getPropertySet(handle, &propHandle);
      throwSuiteStatusException(stat);

      // make our wrapper object
      PropertySet props(propHandle);

      // fetch the instance data out of the properties
      instance = (ImageEffect *) props.propGetPointer(kOfxPropInstanceData);

      OFX::Log::error(instance == 0, "Instance data handle in effect instance properties is NULL!");

      // need to throw something here

      // and dance to the music
      return instance;
    }

    /** @brief Checks the handles passed into the plugin's main entry point */
    static
    void
      checkMainHandles(const std::string &action,  const void *handle, 
      OfxPropertySetHandle inArgsHandle,  OfxPropertySetHandle outArgsHandle,
      bool handleCanBeNull, bool inArgsCanBeNull, bool outArgsCanBeNull)
    {
      if(handleCanBeNull)
        OFX::Log::warning(handle != 0, "Handle passed to '%s' is not null.", action.c_str());
      else
        OFX::Log::error(handle == 0, "'Handle passed to '%s' is null.", action.c_str());

      if(inArgsCanBeNull)
        OFX::Log::warning(inArgsHandle != 0, "'inArgs' Handle passed to '%s' is not null.", action.c_str());
      else
        OFX::Log::error(inArgsHandle == 0, "'inArgs' handle passed to '%s' is null.", action.c_str());

      if(outArgsCanBeNull)
        OFX::Log::warning(outArgsHandle != 0, "'outArgs' Handle passed to '%s' is not null.", action.c_str());
      else
        OFX::Log::error(outArgsHandle == 0, "'outArgs' handle passed to '%s' is null.", action.c_str());

      // validate the property sets on the arguments
      OFX::Validation::validateActionArgumentsProperties(action, inArgsHandle, outArgsHandle);

      // throw exceptions if null when not meant to be null
      if(!handleCanBeNull && !handle)         throwSuiteStatusException(kOfxStatErrBadHandle);
      if(!inArgsCanBeNull && !inArgsHandle)   throwSuiteStatusException(kOfxStatErrBadHandle);
      if(!outArgsCanBeNull && !outArgsHandle) throwSuiteStatusException(kOfxStatErrBadHandle);
    }


    /** @brief Fetches the arguments used in a render action 'inargs' property set into a POD struct */
    static void
      getRenderActionArguments(RenderArguments &args,  OFX::PropertySet inArgs)
    {
      args.time = inArgs.propGetDouble(kOfxPropTime);

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.renderWindow.x1 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 0);
      args.renderWindow.y1 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 1);
      args.renderWindow.x2 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 2);
      args.renderWindow.y2 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 3);

      // Don't throw an exception if the following inArgs are not present:
      // They appeared in OFX 1.2, and they are not in the IsIdentity args
      args.sequentialRenderStatus = inArgs.propGetInt(kOfxImageEffectPropSequentialRenderStatus, false) != 0;
      args.interactiveRenderStatus = inArgs.propGetInt(kOfxImageEffectPropInteractiveRenderStatus, false) != 0;

#ifdef OFX_EXTENSIONS_VEGAS
      args.viewsToRender = inArgs.propGetInt(kOfxImageEffectPropViewsToRender, 0, false);
      args.renderView = inArgs.propGetInt(kOfxImageEffectPropRenderView, 0, false);
#endif
        
#ifdef OFX_EXTENSIONS_NUKE
        args.renderView = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);
        int numPlanes = inArgs.propGetDimension(kFnOfxImageEffectPropComponentsPresent, false);
        for (int i = 0; i < numPlanes; ++i) {
            args.planes.push_back(inArgs.propGetString(kFnOfxImageEffectPropComponentsPresent, i, false));
        }
#endif

      std::string str = inArgs.propGetString(kOfxImageEffectPropFieldToRender);
      try {
        args.fieldToRender = mapStrToFieldEnum(str);
      }
      catch (std::invalid_argument) {
        // dud field?
        OFX::Log::error(true, "Unknown field to render '%s'", str.c_str());

        // HACK need to throw something to cause a failure
      }

#ifdef OFX_EXTENSIONS_VEGAS
      std::string strQuality = inArgs.propGetString(kOfxImageEffectPropRenderQuality, /*throwOnFailure*/false);
      try {
        args.renderQuality = mapToVegasRenderQualityEnum(strQuality);
      }
      catch (std::invalid_argument) {
        // dud field?
        OFX::Log::error(true, "Unknown render quality '%s'", str.c_str());

        // HACK need to throw something to cause a failure
      }
#endif
    }

    /** @brief Library side render action, fetches relevant properties and calls the client code */
    static
    void
      renderAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      RenderArguments args;

      // get the arguments 
      getRenderActionArguments(args, inArgs);

      // and call the plugin client render code
      effectInstance->render(args);
    }

    /** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
    static
    void
      beginSequenceRenderAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      BeginSequenceRenderArguments args;

      args.frameRange.min = inArgs.propGetDouble(kOfxImageEffectPropFrameRange, 0);
      args.frameRange.max = inArgs.propGetDouble(kOfxImageEffectPropFrameRange, 1);

      args.frameStep      = inArgs.propGetDouble(kOfxImageEffectPropFrameStep, 0);

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.isInteractive = inArgs.propGetInt(kOfxPropIsInteractive) != 0;
      // Don't throw an exception if the following inArgs are not present:
      // They appeared in OFX 1.2
      args.sequentialRenderStatus = inArgs.propGetInt(kOfxImageEffectPropSequentialRenderStatus, false) != 0;
      args.interactiveRenderStatus = inArgs.propGetInt(kOfxImageEffectPropInteractiveRenderStatus, false) != 0;
        
#ifdef OFX_EXTENSIONS_NUKE
      args.view = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);
#endif

      // and call the plugin client render code
      effectInstance->beginSequenceRender(args);
    }

    /** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
    static
    void
      endSequenceRenderAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      EndSequenceRenderArguments args;

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.isInteractive = inArgs.propGetInt(kOfxPropIsInteractive) != 0;
      // Don't throw an exception if the following inArgs are not present:
      // They appeared in OFX 1.2
      args.sequentialRenderStatus = inArgs.propGetInt(kOfxImageEffectPropSequentialRenderStatus, false) != 0;
      args.interactiveRenderStatus = inArgs.propGetInt(kOfxImageEffectPropInteractiveRenderStatus, false) != 0;

#ifdef OFX_EXTENSIONS_NUKE
      args.view = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);
#endif
        
      // and call the plugin client render code
      effectInstance->endSequenceRender(args);
    }


    /** @brief Fetches the arguments used in a isIdentity action 'inargs' property set into a POD struct */
    static void
      getIsIdentityActionArguments(IsIdentityArguments &args,  OFX::PropertySet inArgs)
    {
      args.time = inArgs.propGetDouble(kOfxPropTime);

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.renderWindow.x1 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 0);
      args.renderWindow.y1 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 1);
      args.renderWindow.x2 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 2);
      args.renderWindow.y2 = inArgs.propGetInt(kOfxImageEffectPropRenderWindow, 3);

#ifdef OFX_EXTENSIONS_NUKE
      args.view = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);
#endif
        
      std::string str = inArgs.propGetString(kOfxImageEffectPropFieldToRender);
      try {
        args.fieldToRender = mapStrToFieldEnum(str);
      }
      catch (std::invalid_argument) {
        // dud field?
        OFX::Log::error(true, "Unknown field to render '%s'", str.c_str());

        // HACK need to throw something to cause a failure
      }
    }

    /** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
    static
    bool
      isIdentityAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      IsIdentityArguments args;

      // get the arguments 
      getIsIdentityActionArguments(args, inArgs);

      // and call the plugin client isIdentity code
      Clip *identityClip = 0;
      double identityTime = args.time;
      bool v = effectInstance->isIdentity(args, identityClip, identityTime);

      if(v && identityClip) {
        outArgs.propSetString(kOfxPropName, identityClip->name());
        outArgs.propSetDouble(kOfxPropTime, identityTime);                
        return true;
      }
      return false;
    }

    /** @brief Library side get region of definition function */
    static
    bool
      regionOfDefinitionAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      RegionOfDefinitionArguments args;

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.time = inArgs.propGetDouble(kOfxPropTime);

#ifdef OFX_EXTENSIONS_NUKE
      args.view = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);
#endif
        
      // and call the plugin client code
      OfxRectD rod;
      bool v = effectInstance->getRegionOfDefinition(args, rod);

      if(v) {
        outArgs.propSetDouble(kOfxImageEffectPropRegionOfDefinition, rod.x1, 0);
        outArgs.propSetDouble(kOfxImageEffectPropRegionOfDefinition, rod.y1, 1);
        outArgs.propSetDouble(kOfxImageEffectPropRegionOfDefinition, rod.x2, 2);
        outArgs.propSetDouble(kOfxImageEffectPropRegionOfDefinition, rod.y2, 3);
        return true;
      }
      return false;
    }

    /** @brief Library side get regions of interest function */
    static
    bool
      regionsOfInterestAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs, const char* plugname)
    {
      /** @brief local class to set the roi of a clip */
      class LOCAL ActualROISetter : public OFX::RegionOfInterestSetter {
        OFX::PropertySet &outArgs_;
        bool doneSomething_;
        const std::map<std::string, std::string>& clipROIPropNames_;
      public :
        /** @brief ctor */
        ActualROISetter(OFX::PropertySet &args, const std::map<std::string, std::string>& clipROIPropNames) 
          : outArgs_(args)
          , doneSomething_(false) 
          , clipROIPropNames_(clipROIPropNames)
        { }

        /** @brief did we set something ? */
        bool didSomething(void) const {return doneSomething_;}

        /** @brief set the RoI of the clip */
        virtual void setRegionOfInterest(const Clip &clip, const OfxRectD &roi)
        {
          std::map<std::string, std::string>::const_iterator it = clipROIPropNames_.find(clip.name());
          if(it==clipROIPropNames_.end())
            throw(Exception::PropertyUnknownToHost(clip.name().c_str()));

          // construct the name of the property
          const std::string& propName = it->second;

          // and set it
          outArgs_.propSetDouble(propName.c_str(), roi.x1, 0);
          outArgs_.propSetDouble(propName.c_str(), roi.y1, 1);
          outArgs_.propSetDouble(propName.c_str(), roi.x2, 2);
          outArgs_.propSetDouble(propName.c_str(), roi.y2, 3);      

          // and record the face we have done something
          doneSomething_ = true;
        }
      }; // end of local class

      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      RegionsOfInterestArguments args;

      // fetch in arguments from the prop handle
      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.regionOfInterest.x1 = inArgs.propGetDouble(kOfxImageEffectPropRegionOfInterest, 0);
      args.regionOfInterest.y1 = inArgs.propGetDouble(kOfxImageEffectPropRegionOfInterest, 1);
      args.regionOfInterest.x2 = inArgs.propGetDouble(kOfxImageEffectPropRegionOfInterest, 2);
      args.regionOfInterest.y2 = inArgs.propGetDouble(kOfxImageEffectPropRegionOfInterest, 3);

      args.time = inArgs.propGetDouble(kOfxPropTime);
        
#ifdef OFX_EXTENSIONS_NUKE
      args.view = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);
#endif

      // make a roi setter object
      ActualROISetter setRoIs(outArgs, gEffectDescriptors[plugname][effectInstance->getContext()]->getClipROIPropNames());

      // and call the plugin client code
      effectInstance->getRegionsOfInterest(args, setRoIs);

      // did we do anything ?
      if(setRoIs.didSomething()) 
        return true;
      return false;
    }

    /** @brief Library side frames needed action */
    static
    bool
      framesNeededAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs, const char* plugname)
    {
      /** @brief local class to set the frames needed from a clip */
      class LOCAL ActualSetter : public OFX::FramesNeededSetter {
        OFX::PropertySet &outArgs_;                                  /**< @brief property set to set values in */
        std::map<std::string, std::vector<OfxRangeD> > frameRanges_;  /**< @brief map holding a bunch of frame ranges, one for each clip */
        const std::map<std::string, std::string>& _clipFrameRangePropNames;
      public :
        /** @brief ctor */
        ActualSetter(OFX::PropertySet &args, const std::map<std::string, std::string>& clipFrameRangePropNames) 
          : outArgs_(args), _clipFrameRangePropNames(clipFrameRangePropNames) 
        { }

        /** @brief set the RoI of the clip */
        virtual void setFramesNeeded(const Clip &clip, const OfxRangeD &range) 
        {
          // insert this into the vector which is in the map
          frameRanges_[clip.name()].push_back(range);
        }

        /** @brief write frameRanges_ back to the property set */
        bool setOutProperties(void) 
        {
          bool didSomething = false;

          std::map<std::string, std::vector<OfxRangeD> >::iterator i;

          for(i = frameRanges_.begin(); i != frameRanges_.end(); ++i) {
            if(i->first != kOfxImageEffectOutputClipName) {
              didSomething = true;

              // Make the property name we are setting
              const std::map<std::string, std::string>::const_iterator it = _clipFrameRangePropNames.find(i->first);
              if(it==_clipFrameRangePropNames.end())
                throw(Exception::PropertyUnknownToHost(i->first.c_str()));

              const std::string& propName = it->second;

              // fetch the list of frame ranges
              std::vector<OfxRangeD> &clipRange = i->second;
              std::vector<OfxRangeD>::iterator j;
              int n = 0;

              // and set 'em
              for(j = clipRange.begin(); j < clipRange.end(); ++j) {
                outArgs_.propSetDouble(propName.c_str(), j->min, n++);
                outArgs_.propSetDouble(propName.c_str(), j->max, n++);
              }
            }
          }

          return didSomething;
        }

      }; // end of local class

      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      FramesNeededArguments args;

      // fetch in arguments from the prop handle
      args.time = inArgs.propGetDouble(kOfxPropTime);

      // make a roi setter object
      ActualSetter setFrames(outArgs, gEffectDescriptors[plugname][effectInstance->getContext()]->getClipFrameRangePropNames());

      // and call the plugin client code
      effectInstance->getFramesNeeded(args, setFrames);

      // Write it back to the properties and see if we set anything
      if(setFrames.setOutProperties()) 
        return true;
      return false;
    }

    /** @brief Library side get regions of interest function */
    static
    bool
      getTimeDomainAction(OfxImageEffectHandle handle, OFX::PropertySet &outArgs)
    {
      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      // we can only be a general context effect, so check that this is true
#ifdef OFX_EXTENSIONS_TUTTLE
      OFX::Log::error(effectInstance->getContext() != eContextGeneral &&
                      effectInstance->getContext() != eContextReader &&
                      effectInstance->getContext() != eContextGenerator, "Calling kOfxImageEffectActionGetTimeDomain on an effect that is not a 'general', 'reader' or 'generator' context effect.");
#else
      OFX::Log::error(effectInstance->getContext() != eContextGeneral, "Calling kOfxImageEffectActionGetTimeDomain on an effect that is not a general context effect.");
#endif

      OfxRangeD timeDomain;

      // and call the plugin client code
      bool v = effectInstance->getTimeDomain(timeDomain);

      if(v) {
        outArgs.propSetDouble(kOfxImageEffectPropFrameRange, timeDomain.min, 0);
        outArgs.propSetDouble(kOfxImageEffectPropFrameRange, timeDomain.max, 1);
      }

      return v;
    }

    /** @brief Library side get regions of interest function */
    static
    bool
      clipPreferencesAction(OfxImageEffectHandle handle, OFX::PropertySet &outArgs, const char* plugname)
    {
      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      // set up our clip preferences setter
      ImageEffectDescriptor* desc = gEffectDescriptors[plugname][effectInstance->getContext()];
      ClipPreferencesSetter prefs(outArgs, desc->getClipDepthPropNames(), desc->getClipComponentPropNames(), desc->getClipPARPropNames());

      // and call the plug-in client code
      effectInstance->getClipPreferences(prefs);

      // did we do anything ?
      if(prefs.didSomething()) 
        return true;
      return false;
    }

    /** @brief Library side begin instance changed action */
    static
    void
      beginInstanceChangedAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      std::string reasonStr = inArgs.propGetString(kOfxPropChangeReason);
      InstanceChangeReason reason = mapToInstanceChangedReason(reasonStr);

      // and call the plugin client code
      effectInstance->beginChanged(reason);
    }

    /** @brief Library side instance changed action */
    static
    void
      instanceChangedAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      InstanceChangedArgs args;

      // why did it change
      std::string reasonStr = inArgs.propGetString(kOfxPropChangeReason);
      args.reason = mapToInstanceChangedReason(reasonStr);
      args.time = inArgs.propGetDouble(kOfxPropTime);
      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      // what changed
      std::string changedType = inArgs.propGetString(kOfxPropType);
      std::string changedName = inArgs.propGetString(kOfxPropName);

      if(changedType == kOfxTypeParameter) {
        // and call the plugin client code
        effectInstance->changedParam(args, changedName);
      }
      else if(changedType == kOfxTypeClip) {
        // and call the plugin client code
        effectInstance->changedClip(args, changedName);
      }
      else {
        OFX::Log::error(true, "Instance Changed called with unknown type '%s' of object '%s'", changedType.c_str(), changedName.c_str());
      }
    }

    /** @brief Library side end instance changed action */
    static
    void
      endInstanceChangedAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      std::string reasonStr = inArgs.propGetString(kOfxPropChangeReason);
      InstanceChangeReason reason = mapToInstanceChangedReason(reasonStr);

      // and call the plugin client code
      effectInstance->endChanged(reason);
    }

#ifdef OFX_EXTENSIONS_VEGAS
    /** @brief Library side uplift vegas keyframe action */
    static
    void
      upliftVegasKeyframeAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      SonyVegasUpliftArguments upliftArgs(inArgs);

      upliftArgs.keyframeCount = inArgs.propGetDimension(kOfxPropVegasUpliftKeyframeData);
      upliftArgs.guidUplift = inArgs.propGetString(kOfxImageEffectPropVegasUpliftGUID);

      upliftArgs.commonData = inArgs.propGetPointer(kOfxPropVegasUpliftData, false);
      upliftArgs.commonDataSize = inArgs.propGetInt(kOfxPropVegasUpliftDataLength, false);

      // and call the plugin client code
      effectInstance->upliftVegasKeyframes(upliftArgs);
    }

    /** @brief Library side invoke About function */
    static
    bool
      invokeAbout(OfxImageEffectHandle handle, const char* /*plugname*/)
    {
      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      // and call the plug-in client code
      return effectInstance->invokeAbout();
    }

    /** @brief Library side invoke Help function */
    static
    bool
      invokeHelp(OfxImageEffectHandle handle, const char* /*plugname*/)
    {
      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      // and call the plug-in client code
      return effectInstance->invokeHelp();
    }
#endif
#ifdef OFX_EXTENSIONS_NUKE

    static
    bool
    getFrameViewsNeededAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs, const char* plugname)
    {
        ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
        FrameViewsNeededArguments args;
        args.time = inArgs.propGetDouble(kOfxPropTime);
        args.view = inArgs.propGetInt(kFnOfxImageEffectPropView);
        
        ImageEffectDescriptor* desc = gEffectDescriptors[plugname][effectInstance->getContext()];
        FrameViewsNeededSetter setter(outArgs,desc->getClipFrameViewsPropNames());
        effectInstance->getFrameViewsNeeded(args,setter);
        return setter.didSomething();
    }
      
    static
    bool
    getClipComponentsAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs, const char* plugname)
    {
          ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
          ClipComponentsArguments args;
          args.time = inArgs.propGetDouble(kOfxPropTime);
          args.view = inArgs.propGetInt(kFnOfxImageEffectPropView);
          
          ImageEffectDescriptor* desc = gEffectDescriptors[plugname][effectInstance->getContext()];
          ClipComponentsSetter setter(outArgs,desc->getClipPlanesPropNames());
          effectInstance->getClipComponents(args,setter);
          return setter.didSomething();
    }
      
    /** @brief Action called in place of a render to recover a transform matrix from an effect. */
    static
    bool
      getTransform(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      TransformArguments args;

      // get the arguments 
      args.time = inArgs.propGetDouble(kOfxPropTime);

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.renderView = inArgs.propGetInt(kFnOfxImageEffectPropView, 0, false);

      std::string str = inArgs.propGetString(kOfxImageEffectPropFieldToRender);
      try {
        args.fieldToRender = eFieldBoth;
        args.fieldToRender = mapStrToFieldEnum(str);
      } catch (std::invalid_argument) {
        // dud field?
        OFX::Log::error(true, "Unknown field to render '%s'", str.c_str());

        // HACK need to throw something to cause a failure
      }

      // and call the plugin client getTransform code
      Clip *transformClip = 0;
      double transformMatrix[9];
      bool v = effectInstance->getTransform(args, transformClip, transformMatrix);

      if(v && transformClip) {
        outArgs.propSetString(kOfxPropName, transformClip->name());
        outArgs.propSetDoubleN(kFnOfxPropMatrix2D, transformMatrix, 9);
        return true; // the transfrom and clip name were set and can be used to modify the named image appropriately
      }
      return false; // don't attempt to use the transform matrix, but render the image as per normal
    }
#endif

    /** @brief The main entry point for the plugin
    */
    OfxStatus mainEntryStr(const char    *actionRaw,
      const void    *handleRaw,
      OfxPropertySetHandle   inArgsRaw,
      OfxPropertySetHandle   outArgsRaw,
      const char* plugname)
    {
      OFX::Log::print("********************************************************************************");
      OFX::Log::print("START mainEntry (%s)", actionRaw);
      OFX::Log::indent();
      OfxStatus stat = kOfxStatReplyDefault;
      try {

        OfxPlugInfoMap::iterator it = plugInfoMap.find(plugname);
        if(it==plugInfoMap.end())
          throw;

        OFX::PluginFactory* factory = it->second._factory;

        // Cast the raw handle to be an image effect handle, because that is what it is
        OfxImageEffectHandle handle = (OfxImageEffectHandle) handleRaw;

        // Turn the arguments into wrapper objects to make our lives easier
        OFX::PropertySet inArgs(inArgsRaw);
        OFX::PropertySet outArgs(outArgsRaw);

        // turn the action into a std::string
        std::string action(actionRaw);

        // figure the actions
        if (action == kOfxActionLoad) {
          // call the support load function, param-less
          OFX::Private::loadAction(); 

          // call the plugin side load action, param-less
          factory->load();

          // got here, must be good
          stat = kOfxStatOK;
        }

        // figure the actions
        else if (action == kOfxActionUnload) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, true, true, true);

          // call the plugin side unload action, param-less, should be called, eve if the stat above failed!
          factory->unload();

          // call the support unload function, param-less
          OFX::Private::unloadAction(plugname); 

          // got here, must be good
          stat = kOfxStatOK;
        }

        else if(action == kOfxActionDescribe) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // make the plugin descriptor
          ImageEffectDescriptor *desc = new ImageEffectDescriptor(handle);

          // validate the host
          OFX::Validation::validatePluginDescriptorProperties(fetchEffectProps(handle));

          //  and pass it to the plugin to do something with it

          factory->describe(*desc);

          // add it to our map
          gEffectDescriptors[plugname][eContextNone] = desc;

          // got here, must be good
          stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionDescribeInContext) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // make the plugin descriptor and pass it to the plugin to do something with it
          ImageEffectDescriptor *desc = new ImageEffectDescriptor(handle);

          // figure the context and map it to an enum
          std::string contextStr = inArgs.propGetString(kOfxImageEffectPropContext);
          ContextEnum context = mapToContextEnum(contextStr);

          // validate the host
          OFX::Validation::validatePluginDescriptorProperties(fetchEffectProps(handle));

          // call plugin describe in context
          factory->describeInContext(*desc, context);

          // add it to our map
          gEffectDescriptors[plugname][context] = desc;

          // got here, must be good
          stat = kOfxStatOK;
        }
        else if(action == kOfxActionCreateInstance) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch the effect props to figure the context
          PropertySet effectProps = fetchEffectProps(handle);

          // get the context and turn it into an enum
          std::string str = effectProps.propGetString(kOfxImageEffectPropContext);
          ContextEnum context = mapToContextEnum(str);

          // make the image effect instance for this context
          ImageEffect *instance = factory->createInstance(handle, context);
          (void)instance;

          // validate the plugin handle's properties
          OFX::Validation::validatePluginInstanceProperties(fetchEffectProps(handle));

          // got here, must be good
          stat = kOfxStatOK;
        }
        else if(action == kOfxActionDestroyInstance) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // kill it
          delete instance;

          // got here, must be good
          stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionRender) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the render action skin
          renderAction(handle, inArgs);

          // got here, must be good
          stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionBeginSequenceRender) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the begin render action skin
          beginSequenceRenderAction(handle, inArgs);
        }
        else if(action == kOfxImageEffectActionEndSequenceRender) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the begin render action skin
          endSequenceRenderAction(handle, inArgs);
        }
        else if(action == kOfxImageEffectActionIsIdentity) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the identity action, if it is, return OK
          if(isIdentityAction(handle, inArgs, outArgs))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionGetRegionOfDefinition) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the rod action, return OK if it does something
          if(regionOfDefinitionAction(handle, inArgs, outArgs))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionGetRegionsOfInterest) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the RoI action, return OK if it does something
          if(regionsOfInterestAction(handle, inArgs, outArgs, plugname))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionGetFramesNeeded) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the frames needed action, return OK if it does something
          if(framesNeededAction(handle, inArgs, outArgs, plugname))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionGetClipPreferences) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, false);

          // call the frames needed action, return OK if it does something
          if(clipPreferencesAction(handle, outArgs, plugname))
            stat = kOfxStatOK;
        }
        else if(action == kOfxActionPurgeCaches) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // purge 'em
          instance->purgeCaches();
        }
        else if(action == kOfxActionSyncPrivateData) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // and sync it
          instance->syncPrivateData();
        }
        else if(action == kOfxImageEffectActionGetTimeDomain) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, false);

          // call the instance changed action
          if(getTimeDomainAction(handle, outArgs))
            stat = kOfxStatOK;
        }
        else if(action == kOfxActionBeginInstanceChanged) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the instance changed action
          beginInstanceChangedAction(handle, inArgs);
        }
        else if(action == kOfxActionInstanceChanged) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the instance changed action
          instanceChangedAction(handle, inArgs);
        }
        else if(action == kOfxActionEndInstanceChanged) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the instance changed action
          endInstanceChangedAction(handle, inArgs);
        }
        else if(action == kOfxActionBeginInstanceEdit) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // call the begin edit function
          instance->beginEdit();
        }
        else if(action == kOfxActionEndInstanceEdit) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // call the end edit function
          instance->endEdit();
        }
#ifdef OFX_SUPPORTS_OPENGLRENDER
        else if(action == kOfxActionOpenGLContextAttached) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // call the context attached function
          instance->contextAttached();
        }
        else if(action == kOfxActionOpenGLContextDetached) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

          // call the context detached function
          instance->contextDetached();
        }
#endif
#ifdef OFX_EXTENSIONS_VEGAS
        else if(action == kOfxImageEffectActionVegasKeyframeUplift) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, true);

          // call the uplift vegas keyframes function
          upliftVegasKeyframeAction(handle, inArgs);
        }
        else if(action == kOfxImageEffectActionInvokeHelp) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // call the invoke help function
          if(invokeHelp(handle, plugname))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionInvokeAbout) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, true);

          // call the invoke help function
          if(invokeAbout(handle, plugname))
            stat = kOfxStatOK;
        }
#endif
#ifdef OFX_EXTENSIONS_NUKE

        else if(action == kFnOfxImageEffectActionGetClipComponents) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the clip components function, return OK if it does something
          // the spec is not clear as to whether it is allowed to do nothing but
          // this action should always be implemented for multi-planes effects.
          if (getClipComponentsAction(handle, inArgs, outArgs, plugname)) {
              stat = kOfxStatOK;
          }
        }
        else if(action == kFnOfxImageEffectActionGetFrameViewsNeeded) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the frames views needed action, return OK if it does something
          if (getFrameViewsNeededAction(handle, inArgs, outArgs, plugname)) {
              stat = kOfxStatOK;
          }
        }
        else if(action == kFnOfxImageEffectActionGetTransform) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the get transform function
          if(getTransform(handle, inArgs, outArgs))
            stat = kOfxStatOK;
        }
#endif
        else if(actionRaw) {
          OFX::Log::error(true, "Unknown action '%s'.", actionRaw);
        }
        else {
          OFX::Log::error(true, "Requested action was a null pointer.");
        }
      }

      // catch suite exceptions
      catch (const OFX::Exception::Suite &ex)
      {
#      ifdef DEBUG
        std::cout << "Caught OFX::Exception::Suite: " << ex.what() << std::endl;
#      endif
        stat = ex.status();
      }

      // catch host inadequate exceptions
      catch (const OFX::Exception::HostInadequate &e)
      {
#      ifdef DEBUG
        std::cout << "Caught OFX::Exception::HostInadequate: " << e.what() << std::endl;
#      endif
        stat = kOfxStatErrMissingHostFeature;
      }

      // catch exception due to a property being unknown to the host, implies something wrong with host if not caught further down
      catch (const OFX::Exception::PropertyUnknownToHost &e)
      {
#      ifdef DEBUG
        std::cout << "Caught OFX::Exception::PropertyUnknownToHost: " << e.what() << std::endl;
#      endif
        stat = kOfxStatErrMissingHostFeature;
      }

      // catch memory
      catch (std::bad_alloc)
      {
        stat = kOfxStatErrMemory;
      }

      // catch a custom client exception, if defined
#ifdef OFX_CLIENT_EXCEPTION_TYPE
      catch (OFX_CLIENT_EXCEPTION_TYPE &ex)
      {
        stat = OFX_CLIENT_EXCEPTION_HANDLER(ex, plugname);
      }
#endif
      // Catch anything else, unknown
      catch (const std::exception &e)
      {
#      ifdef DEBUG
        std::cout << "Caught exception: " << e.what() << std::endl;
#      endif
        stat = kOfxStatFailed;
      }
      catch (...)
      {
#      ifdef DEBUG
        std::cout << "Caught Unknown exception" << std::endl;
#      endif
        stat = kOfxStatFailed;
      }

      OFX::Log::outdent();
      OFX::Log::print("STOP mainEntry (%s)\n", actionRaw);
      return stat;
    }      


    OfxStatus customParamInterpolationV1Entry(
      const void*            handleRaw,
      OfxPropertySetHandle   inArgsRaw,
      OfxPropertySetHandle   outArgsRaw)
    {
      OFX::Log::print("********************************************************************************");
      OFX::Log::print("START customParamInterpolationV1Entry");
      OFX::Log::indent();
      OfxStatus stat = kOfxStatReplyDefault;
      try {
        // Cast the raw handle to be an image effect handle, because that is what it is
        OfxImageEffectHandle handle = (OfxImageEffectHandle) handleRaw;

        // Turn the arguments into wrapper objects to make our lives easier
        OFX::PropertySet inArgs(inArgsRaw);
        OFX::PropertySet outArgs(outArgsRaw);

        ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

        InterpolateCustomArgs interpArgs;

        interpArgs.time     = inArgs.propGetDouble(kOfxPropTime);
        interpArgs.value1   = inArgs.propGetString(kOfxParamPropCustomValue, 0);
        interpArgs.value2   = inArgs.propGetString(kOfxParamPropCustomValue, 1);
        interpArgs.keytime1 = inArgs.propGetDouble(kOfxParamPropInterpolationTime, 0);
        interpArgs.keytime2 = inArgs.propGetDouble(kOfxParamPropInterpolationTime, 1);
        interpArgs.amount   = inArgs.propGetDouble(kOfxParamPropInterpolationAmount);

        std::string paramName = inArgs.propGetString(kOfxPropName);

        // and call the plugin client code
        std::string output = effectInstance->interpolateCustomParam(interpArgs, paramName);

        outArgs.propSetString(kOfxParamPropCustomValue, output);
      }

      // catch suite exceptions
      catch (OFX::Exception::Suite &ex)
      {
#      ifdef DEBUG
        std::cout << "Caught OFX::Exception::Suite" << std::endl;
#      endif
        stat = ex.status();
      }

      // catch host inadequate exceptions
      catch (OFX::Exception::HostInadequate)
      {
#      ifdef DEBUG
        std::cout << "Caught OFX::Exception::HostInadequate" << std::endl;
#      endif
        stat = kOfxStatErrMissingHostFeature;
      }

      // catch exception due to a property being unknown to the host, implies something wrong with host if not caught further down
      catch (OFX::Exception::PropertyUnknownToHost)
      {
#      ifdef DEBUG
        std::cout << "Caught OFX::Exception::PropertyUnknownToHost" << std::endl;
#      endif
        stat = kOfxStatErrMissingHostFeature;
      }

      // catch memory
      catch (std::bad_alloc)
      {
        stat = kOfxStatErrMemory;
      }

      // catch a custom client exception, if defined
#ifdef OFX_CLIENT_EXCEPTION_TYPE
      catch (OFX_CLIENT_EXCEPTION_TYPE &ex)
      {
        stat = OFX_CLIENT_EXCEPTION_HANDLER(ex, plugname);
      }
#endif
      // Catch anything else, unknown
      catch (...)
      {
#      ifdef DEBUG
        std::cout << "Caught Unknown exception" << std::endl;
#      endif
        stat = kOfxStatFailed;
      }

      OFX::Log::outdent();
      OFX::Log::print("STOP customParamInterpolationV1Entry\n");
      return stat;
    }

    /** @brief The plugin function that gets passed the host structure. */
    void setHost(OfxHost *host)
    {
      gHost = host;
    }

  }; // namespace Private

  /** @brief Fetch's a suite from the host and logs errors */
  const void * fetchSuite(const char *suiteName, int suiteVersion, bool optional)
  {
    const void *suite = Private::gHost->fetchSuite(Private::gHost->host, suiteName, suiteVersion);
    if(suite==0)
    {
      if(optional)
        OFX::Log::warning(suite == 0, "Could not fetch the optional suite '%s' version %d.", suiteName, suiteVersion);
      else
        OFX::Log::error(suite == 0, "Could not fetch the mandatory suite '%s' version %d.", suiteName, suiteVersion);
    }
    if(!optional && suite == 0) throw OFX::Exception::HostInadequate(suiteName);
    return suite;
  }

}; // namespace OFX

static
OFX::OfxPlugInfo generatePlugInfo(OFX::PluginFactory* factory, std::string& newID)
{
  newID = factory->getUID();
  std::auto_ptr<OfxPlugin> ofxPlugin(new OfxPlugin());
  ofxPlugin->pluginApi  = kOfxImageEffectPluginApi;
  ofxPlugin->apiVersion = 1;
  ofxPlugin->pluginIdentifier   = factory->getID().c_str();
  ofxPlugin->pluginVersionMajor = factory->getMajorVersion();
  ofxPlugin->pluginVersionMinor = factory->getMinorVersion();
  ofxPlugin->setHost    = OFX::Private::setHost;
  ofxPlugin->mainEntry  = factory->getMainEntry();
  return OFX::OfxPlugInfo(factory, ofxPlugin.release());
}

bool gHasInit = false;

static
void init()
{
  if(gHasInit)
    return;

  OFX::Plugin::getPluginIDs(OFX::plugIDs);
  if(OFX::ofxPlugs.empty())
    OFX::ofxPlugs.resize(OFX::plugIDs.size());

  int counter = 0;
  for (OFX::PluginFactoryArray::const_iterator it = OFX::plugIDs.begin(); it != OFX::plugIDs.end(); ++it, ++counter)
  {
    std::string newID;
    OFX::OfxPlugInfo info = generatePlugInfo(*it, newID);
    OFX::plugInfoMap[newID] = info;
    OFX::ofxPlugs[counter] = info._plug;
  }
  gHasInit = true;
}

/** @brief, mandated function returning the number of plugins, which is always 1 */
EXPORT int OfxGetNumberOfPlugins(void)
{
  init();
  return (int)OFX::plugIDs.size();
}

/** @brief, mandated function returning the nth plugin 

We call the plugin side defined OFX::Plugin::getPluginIDs function to find out what to set.
*/

EXPORT OfxPlugin* OfxGetPlugin(int nth)
{
  init();
  int numPlugs = (int)OFX::plugInfoMap.size();
  OFX::Log::error(nth >= numPlugs, "Host attempted to get plugin %d, when there is only %d plugin(s), so it should have asked for 0.", nth, numPlugs);
  if(OFX::ofxPlugs[nth] == 0)
  {
    std::string newID;
    OFX::OfxPlugInfo info = generatePlugInfo(OFX::plugIDs[nth], newID);
    OFX::plugInfoMap[newID] = info;
    OFX::ofxPlugs[nth] = info._plug;
  }
  return OFX::ofxPlugs[nth];
}
