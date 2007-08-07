/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004-2007 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Foundry Visionmongers Ltd, nor the names of its 
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

The Foundry Visionmongers Ltd
1 Wardour St
London W1D 6PA
England


*/

/** @brief This file contains code that skins the ofx effect suite */

#include "ofxsSupportPrivate.h"
#include <algorithm> // for find


/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries. */
namespace OFX {
  
  /** @brief the global host description */
  ImageEffectHostDescription gHostDescription;
  bool gHostDescriptionHasInit = false;

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
    OfxProgressSuiteV1     *gProgressSuite = 0;
    OfxTimeLineSuiteV1     *gTimeLineSuite = 0;
  

    /** @brief the set of descriptors, one per context used by kOfxActionDescribeInContext,  'eContextNone' is the one used by the kOfxActionDescribe */
    std::map<ContextEnum, ImageEffectDescriptor *> gEffectDescriptors;
  };

  /** @brief map a std::string to a context */
  ContextEnum 
  mapToContextEnum(const std::string &s) throw(std::invalid_argument)
  {
    if(s == kOfxImageEffectContextGenerator) return eContextGenerator;
    if(s == kOfxImageEffectContextFilter) return eContextFilter;
    if(s == kOfxImageEffectContextTransition) return eContextTransition;
    if(s == kOfxImageEffectContextPaint) return eContextPaint;
    if(s == kOfxImageEffectContextGeneral) return eContextGeneral;
    if(s == kOfxImageEffectContextRetimer) return eContextRetimer;
    OFX::Log::error(true, "Unknown image effect context '%s'", s.c_str());
    throw std::invalid_argument(s);
  }

  const char* mapToMessageTypeEnum(OFX::Message::MessageTypeEnum type)
  {
    if(type == OFX::Message::eMessageFatal)
      return kOfxMessageFatal;
    else if(type == OFX::Message::eMessageError)
      return kOfxMessageError;
    else if(type == OFX::Message::eMessageMessage)
      return kOfxMessageMessage;
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
  InstanceChangeReason
  mapToInstanceChangedReason(const std::string &s) throw(std::invalid_argument)
  {
    if(s == kOfxChangePluginEdited) return eChangePluginEdit;
    if(s == kOfxChangeUserEdited) return eChangeUserEdit;
    if(s == kOfxChangeTime) return eChangeTime;
    OFX::Log::error(true, "Unknown instance changed reason '%s'", s.c_str());
    throw std::invalid_argument(s);
  }

  /** @brief turns a bit depth string into and enum */
  static BitDepthEnum
  mapStrToBitDepthEnum(const std::string &str) throw(std::invalid_argument)
  {
    if(str == kOfxBitDepthByte) {
      return eBitDepthUByte;
    }
    else if(str == kOfxBitDepthShort) {
      return eBitDepthUShort;
    }
    else if(str == kOfxBitDepthFloat) {
      return eBitDepthFloat;
    }
    else if(str == kOfxBitDepthNone) {
      return eBitDepthNone;
    }
    else {
      return eBitDepthCustom;
    }
  }

  /** @brief turns a pixel component string into and enum */
  static PixelComponentEnum
  mapStrToPixelComponentEnum(const std::string &str) throw(std::invalid_argument)
  {
    if(str == kOfxImageComponentRGBA) {
      return ePixelComponentRGBA;
    }
    else if(str == kOfxImageComponentAlpha) {
      return ePixelComponentAlpha;
    }
    else if(str == kOfxImageComponentNone) {
      return ePixelComponentNone;
    }
    else {
      return ePixelComponentCustom;
    }
  }

  /** @brief turns a premultiplication string into and enum */
  static PreMultiplicationEnum
  mapStrToPreMultiplicationEnum(const std::string &str) throw(std::invalid_argument)
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
  void ClipDescriptor::setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel)
  {
    _clipProps.propSetString(kOfxPropLabel, label);
    _clipProps.propSetString(kOfxPropShortLabel, shortLabel, false);
    _clipProps.propSetString(kOfxPropLongLabel, longLabel, false);
  }

  /** @brief set how fielded images are extracted from the clip defaults to eFieldExtractDoubled */
  void ClipDescriptor::setFieldExtraction(FieldExtractionEnum v)
  {
    switch(v) {
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
    switch(v) {
    case ePixelComponentRGBA :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGBA, n);
      break;

    case ePixelComponentAlpha :
      _clipProps.propSetString(kOfxImageEffectPropSupportedComponents, kOfxImageComponentAlpha, n);
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

    // set the overlay interact function only if it supports interacts
    if(OFX::gHostDescription.supportsOverlays)
      _effectProps.propSetPointer(kOfxImageEffectPluginPropOverlayInteractV1, (void *) OFX::Private::overlayInteractMainEntry);
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
  void ImageEffectDescriptor::setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel)
  {
    _effectProps.propSetString(kOfxPropLabel, label);
    _effectProps.propSetString(kOfxPropShortLabel, shortLabel, false);
    _effectProps.propSetString(kOfxPropLongLabel, longLabel, false);
  }

  /** @brief Set the plugin grouping */
  void ImageEffectDescriptor::setPluginGrouping(const std::string &group)
  {
    _effectProps.propSetString(kOfxImageEffectPluginPropGrouping, group);
  }

  /** @brief Add a context to those supported */
  void ImageEffectDescriptor::addSupportedContext(ContextEnum v)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropSupportedContexts);
    switch (v) {
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
    }
  }

  /** @brief Add a pixel depth to those supported */
  void ImageEffectDescriptor::addSupportedBitDepth(BitDepthEnum v)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropSupportedPixelDepths);
    switch(v) {
    case eBitDepthUByte :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthByte  , n);
      break;
    case eBitDepthUShort :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthShort  , n);
      break;
    case eBitDepthFloat :
      _effectProps.propSetString(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthFloat  , n);
      break;
    }
  }

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
    switch(v) {
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

  /** @brief If the slave param changes the clip preferences need to be re-evaluated */
  void ImageEffectDescriptor::addClipPreferencesSlaveParam(ParamDescriptor &p)
  {
    int n = _effectProps.propGetDimension(kOfxImageEffectPropClipPreferencesSlaveParam);
    _effectProps.propSetString(kOfxImageEffectPropClipPreferencesSlaveParam, p.getName(), n);
  }

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

    ClipDescriptor *clip = new ClipDescriptor(name, propSet);

    _definedClips[name] = clip;
    _clipComponentsPropNames[name] = std::string("OfxImageClipPropComponents_") + name;
    _clipDepthPropNames[name] = std::string("OfxImageClipPropDepth_") + name;
    _clipPARPropNames[name] = std::string("OfxImageClipPropPAR_") + name;
    _clipROIPropNames[name] = std::string("OfxImageClipPropRoI_") + name;
    _clipFrameRangePropNames[name] = std::string("OfxImageClipPropFrameRange_") + name;
    return clip;
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  // wraps up an image  
  Image::Image(OfxPropertySetHandle props)
    : _imageProps(props)
  {
    OFX::Validation::validateImageProperties(props);

    // and fetch all the properties
    _pixelData = _imageProps.propGetPointer(kOfxImagePropData);
        
    _rowBytes         = _imageProps.propGetInt(kOfxImagePropRowBytes);
    _pixelAspectRatio = _imageProps.propGetDouble(kOfxImagePropPixelAspectRatio);;

    std::string str  = _imageProps.propGetString(kOfxImageEffectPropComponents);
    _pixelComponents = mapStrToPixelComponentEnum(str);

    str = _imageProps.propGetString(kOfxImageEffectPropPixelDepth);
    _pixelDepth = mapStrToBitDepthEnum(str);
        
    // compute bytes per pixel
    _pixelBytes = 0;
    switch(_pixelComponents) {
    case ePixelComponentRGBA  : _pixelBytes = 4; break;
    case ePixelComponentAlpha : _pixelBytes = 1; break;
    case ePixelComponentCustom : _pixelBytes = 0; break;
    }

    switch(_pixelDepth) {
    case eBitDepthUByte  : _pixelBytes *= 1; break;
    case eBitDepthUShort : _pixelBytes *= 2; break;
    case eBitDepthFloat  : _pixelBytes *= 4; break;
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
  }

  Image::~Image()
  {
    OFX::Private::gEffectSuite->clipReleaseImage(_imageProps.propSetHandle());
  }

  /** @brief return a pixel pointer

      No attempt made to be uber efficient here.
  */
  void *Image::getPixelAddress(int x, int y)
  {
    // are we in the image bounds
    if(x < _bounds.x1 || x >= _bounds.x2 || y < _bounds.y1 || y > _bounds.y2 || _pixelBytes == 0)
      return 0;
     
    char *pix = (char *) (((char *) _pixelData) + (y - _bounds.y1) * _rowBytes);
    pix += (x - _bounds.x1) * _pixelBytes;
    return (void *) pix;   
  }

  ////////////////////////////////////////////////////////////////////////////////
  // clip instance

  /** @brief hidden constructor */
  Clip::Clip(ImageEffect *effect, const std::string &name, OfxImageClipHandle handle, OfxPropertySetHandle props)
    : _clipName(name)
    , _clipProps(props)
    , _clipHandle(handle)
    , _effect(effect)
  {
    OFX::Validation::validateClipInstanceProperties(_clipProps);
  }

  /** @brief fetch the labels */
  void Clip::getLabels(std::string &label, std::string &shortLabel, std::string &longLabel) const
  {
    label      = _clipProps.propGetString(kOfxPropLabel);
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
      if(e == eBitDepthNone && !isConnected()) {
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
      if(e == ePixelComponentNone && !isConnected()) {
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
    return _clipProps.propGetDouble(kOfxImagePropPixelAspectRatio);
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
  Image *Clip::fetchImage(double t, OfxRectD bounds)
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

  ////////////////////////////////////////////////////////////////////////////////
  /// image effect 

  /** @brief ctor */
  ImageEffect::ImageEffect(OfxImageEffectHandle handle)
    : _effectHandle(handle)
    , _effectProps(0)
    , _context(eContextNone)
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
    
  OFX::Message::MessageReplyEnum ImageEffect::sendMessage(OFX::Message::MessageTypeEnum type, const std::string& id, const std::string& msg)
  {   
    if(!OFX::Private::gMessageSuite->message){ throwHostMissingSuiteException("message"); }
    OfxStatus stat = OFX::Private::gMessageSuite->message(_effectHandle, mapToMessageTypeEnum(type), id.c_str(), msg.c_str());
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

  ////////////////////////////////////////////////////////////////////////////////
  // below are the default members for the base image effect


  /** @brief client is identity function, returns the clip and time for the identity function 
   */
  bool ImageEffect::isIdentity(const RenderArguments &args, Clip * &identityClip, double &identityTime)
  {
    return false; // by default, we are not an identity operation
  }

  /** @brief The get RoD action */
  bool ImageEffect::getRegionOfDefinition(const RegionOfDefinitionArguments &args, OfxRectD &rod)
  {
    return false; // by default, we are not setting the RoD
  }

  /** @brief the get RoI action */
  void ImageEffect::getRegionsOfInterest(const RegionsOfInterestArguments &args, RegionOfInterestSetter &rois)
  {
    // fa niente
  }

  /** @brief the get frames needed action */
  void ImageEffect::getFramesNeeded(const FramesNeededArguments &args, FramesNeededSetter &frames)
  {
    // fa niente
  }

  /** @brief client begin sequence render function */
  void ImageEffect::beginSequenceRender(const BeginSequenceRenderArguments &args)
  {
    // fa niente
  }

  /** @brief client end sequence render function, this is one of the few that must be set */
  void ImageEffect::endSequenceRender(const EndSequenceRenderArguments &args)
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
  void ImageEffect::getClipPreferences(ClipPreferencesSetter &clipPreferences)
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
  void ImageEffect::beginChanged(InstanceChangeReason reason)
  {
  }

  /** @brief called when a param has just had its value changed */
  void ImageEffect::changedParam(const InstanceChangedArgs &args, const std::string &paramName)
  {
  }

  /** @brief called when a clip has just been changed in some way (a rewire maybe) */
  void ImageEffect::changedClip(const InstanceChangedArgs &args, const std::string &clipName)
  {
  }

  /** @brief the effect has just had some values changed */
  void ImageEffect::endChanged(InstanceChangeReason reason)
  {
  }

  /** @brief get the time domain */
  bool ImageEffect::getTimeDomain(OfxRangeD &range)
  {
    // by default, do the default
    return false;
  }

  /** @brief create the interact */
  OverlayInteract *ImageEffect::createOverlayInteract(OfxInteractHandle handle)
  {
    // by default don't make one
    return NULL;
  }

  /// Start doing progress. 
  void ImageEffect::progressStart(const std::string &message)
  {
    if(OFX::Private::gProgressSuite) {
      OFX::Private::gProgressSuite->progressStart((void *) _effectHandle, message.c_str());
    }
  }
  
  /// finish yer progress
  void ImageEffect::progressEnd()
  {
    if(OFX::Private::gProgressSuite) {
      OFX::Private::gProgressSuite->progressEnd((void *) _effectHandle);
    }
  }
  
  /// set the progress to some level of completion, returns
  /// false if you should abandon processing, true to continue
  bool ImageEffect::progressUpdate(double t)
  {
    if(OFX::Private::gProgressSuite) {
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
    }
    t1 = t2 = 0;
  }
      
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
    
    switch(comps) {
    case ePixelComponentRGBA : 
      outArgs_.propSetString(propName.c_str(), kOfxImageComponentRGBA); 
      break;
    case ePixelComponentAlpha : 
      outArgs_.propSetString(propName.c_str(), kOfxImageComponentAlpha); 
      break;
    }
  }

  /** @brief, force the host to set a clip's mapped bit depth be \em bitDepth */
  void ClipPreferencesSetter::setClipBitDepth(Clip &clip, BitDepthEnum bitDepth)
  {
    doneSomething_ = true;
    const std::string& propName = extractValueForName(clipDepthPropNames_, clip.name());
    
    switch(bitDepth) {
    case eBitDepthUByte : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthByte); 
      break;
    case eBitDepthUShort : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthShort); 
      break;
    case eBitDepthFloat : 
      outArgs_.propSetString(propName.c_str(), kOfxBitDepthFloat); 
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
    switch(v) {
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
    switch(v) {
    case eFieldNone : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldNone, 0, false); break;
    case eFieldLower : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldLower, 0, false); break;
    case eFieldUpper : outArgs_.propSetString(kOfxImageClipPropFieldOrder, kOfxImageFieldUpper, 0, false); break;
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
  }



  /** @brief OFX::Private namespace, for things private to the support library code here generally calls image effect class members */
  namespace Private {

    /** @brief Creates the global host description and sets its properties */
    void
    fetchHostDescription(OfxHost *host)
    {
      OFX::Log::error(OFX::gHostDescriptionHasInit, "Tried to create host description when we already have one.");
      if(!OFX::gHostDescriptionHasInit) {
        OFX::gHostDescriptionHasInit = true;
        // wrap the property handle up with a property set
        PropertySet hostProps(host->host);

        // and get some properties
        gHostDescription.hostName                   = hostProps.propGetString(kOfxPropName);
        gHostDescription.hostIsBackground           = hostProps.propGetInt(kOfxImageEffectHostPropIsBackground) != 0;
				
				int licenseIsBackground;
 				OfxStatus stat = gPropSuite->propGetInt(hostProps.propSetHandle(),kFnOfxImageEffectHostPropLicenseIsBackground,0,&licenseIsBackground);
				gHostDescription.licenseIsBackground        = stat==kOfxStatOK? licenseIsBackground:gHostDescription.hostIsBackground;

        gHostDescription.supportsOverlays           = hostProps.propGetInt(kOfxImageEffectPropSupportsOverlays) != 0;
        gHostDescription.supportsMultiResolution    = hostProps.propGetInt(kOfxImageEffectPropSupportsMultiResolution) != 0;
        gHostDescription.supportsTiles              = hostProps.propGetInt(kOfxImageEffectPropSupportsTiles) != 0;
        gHostDescription.temporalClipAccess         = hostProps.propGetInt(kOfxImageEffectPropTemporalClipAccess) != 0;
        gHostDescription.supportsMultipleClipDepths = hostProps.propGetInt(kOfxImageEffectPropSupportsMultipleClipDepths) != 0;
        gHostDescription.supportsMultipleClipPARs   = hostProps.propGetInt(kOfxImageEffectPropSupportsMultipleClipPARs) != 0;
        gHostDescription.supportsSetableFrameRate   = hostProps.propGetInt(kOfxImageEffectPropSetableFrameRate) != 0;
        gHostDescription.supportsSetableFielding    = hostProps.propGetInt(kOfxImageEffectPropSetableFielding) != 0;
        gHostDescription.supportsStringAnimation    = hostProps.propGetInt(kOfxParamHostPropSupportsStringAnimation) != 0;
        gHostDescription.supportsCustomInteract     = hostProps.propGetInt(kOfxParamHostPropSupportsCustomInteract) != 0;
        gHostDescription.supportsChoiceAnimation    = hostProps.propGetInt(kOfxParamHostPropSupportsChoiceAnimation) != 0;
        gHostDescription.supportsBooleanAnimation   = hostProps.propGetInt(kOfxParamHostPropSupportsBooleanAnimation) != 0;
        gHostDescription.supportsCustomAnimation    = hostProps.propGetInt(kOfxParamHostPropSupportsCustomAnimation) != 0;
        gHostDescription.maxParameters              = hostProps.propGetInt(kOfxParamHostPropMaxParameters);
        gHostDescription.maxPages                   = hostProps.propGetInt(kOfxParamHostPropMaxPages);
        gHostDescription.pageRowCount               = hostProps.propGetInt(kOfxParamHostPropPageRowColumnCount, 0);
        gHostDescription.pageColumnCount            = hostProps.propGetInt(kOfxParamHostPropPageRowColumnCount, 1);
        int numComponents = hostProps.propGetDimension(kOfxImageEffectPropSupportedComponents);
        for(int i=0; i<numComponents; ++i)
          gHostDescription._supportedComponents.push_back(mapStrToPixelComponentEnum(hostProps.propGetString(kOfxImageEffectPropSupportedComponents, i)));

        int numContexts = hostProps.propGetDimension(kOfxImageEffectPropSupportedContexts);
        for(int i=0; i<numContexts; ++i)
          gHostDescription._supportedContexts.push_back(mapToContextEnum(hostProps.propGetString(kOfxImageEffectPropSupportedContexts, i)));
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
      OFX::Log::error(gLoadCount != 0, "Load action called more than once without unload being called.");
      gLoadCount++;  
  
      OfxStatus status = kOfxStatOK;
  
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
        gProgressSuite   = (OfxProgressSuiteV1 *)     fetchSuite(kOfxProgressSuite, 1, true);
        gTimeLineSuite   = (OfxTimeLineSuiteV1 *)     fetchSuite(kOfxTimeLineSuite, 1, true);
      
        // OK check and fetch host information
        fetchHostDescription(gHost);

        /// and set some dendent flags
        OFX::gHostDescription.supportsProgressSuite = gProgressSuite != NULL;
        OFX::gHostDescription.supportsTimeLineSuite = gTimeLineSuite != NULL;
              
        // fetch the interact suite if the host supports interaction
        if(OFX::gHostDescription.supportsOverlays || OFX::gHostDescription.supportsCustomInteract)
          gInteractSuite  = (OfxInteractSuiteV1 *)    fetchSuite(kOfxInteractSuite, 1);
      }

      // initialise the validation code
      OFX::Validation::initialise();

      // validate the host
      OFX::Validation::validateHostProperties(gHost);

    }

    /** @brief Library side unload action, this fetches all the suite pointers */
    void
    unloadAction(void)
    {
      gLoadCount--;
      OFX::Log::error(gLoadCount != 0, "UnLoad action called without a corresponding load action having been called.");
  
      // force these to null
      gEffectSuite = 0;
      gPropSuite = 0;
      gParamSuite = 0;
      gMemorySuite = 0;
      gThreadSuite = 0;
      gMessageSuite = 0;
      gInteractSuite = 0;

      // delete the effect descriptors
      std::map<ContextEnum, ImageEffectDescriptor *>::iterator iter;
      for(iter = gEffectDescriptors.begin(); iter != gEffectDescriptors.end(); ++iter) {
        if(iter->second) {
          delete iter->second;
          iter->second = NULL;
        }
      }

      // and clobber the map
      gEffectDescriptors.clear();
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

    /** @brief Library side render action, fetches relevant properties and calls the client code */
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

      // and call the plugin client render code
      effectInstance->beginSequenceRender(args);
    }
        
    /** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
    void
    endSequenceRenderAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      
      EndSequenceRenderArguments args;

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.isInteractive = inArgs.propGetInt(kOfxPropIsInteractive) != 0;

      // and call the plugin client render code
      effectInstance->endSequenceRender(args);
    }

    /** @brief Library side render begin sequence render action, fetches relevant properties and calls the client code */
    bool
    isIdentityAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      RenderArguments args;
            
      // get the arguments 
      getRenderActionArguments(args, inArgs);

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
    bool
    regionOfDefinitionAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      RegionOfDefinitionArguments args;

      args.renderScale.x = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 0);
      args.renderScale.y = inArgs.propGetDouble(kOfxImageEffectPropRenderScale, 1);

      args.time = inArgs.propGetDouble(kOfxPropTime);

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
    bool
    regionsOfInterestAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      /** @brief local class to set the roi of a clip */
      class ActualROISetter : public OFX::RegionOfInterestSetter {
        bool doneSomething_;
        OFX::PropertySet &outArgs_;
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
            
      // make a roi setter object
      ActualROISetter setRoIs(outArgs, gEffectDescriptors[effectInstance->getContext()]->getClipROIPropNames());

      // and call the plugin client code
      effectInstance->getRegionsOfInterest(args, setRoIs);

      // did we do anything ?
      if(setRoIs.didSomething()) 
        return true;
      return false;
    }
        
    /** @brief Library side frames needed action */
    bool
    framesNeededAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs, OFX::PropertySet &outArgs)
    {
      /** @brief local class to set the frames needed from a clip */
      class ActualSetter : public OFX::FramesNeededSetter {
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
            if(i->first != "Output") {
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
      ActualSetter setFrames(outArgs, gEffectDescriptors[effectInstance->getContext()]->getClipFrameRangePropNames());

      // and call the plugin client code
      effectInstance->getFramesNeeded(args, setFrames);

      // Write it back to the properties and see if we set anything
      if(setFrames.setOutProperties()) 
        return true;
      return false;
    }

    /** @brief Library side get regions of interest function */
    bool
    getTimeDomainAction(OfxImageEffectHandle handle, OFX::PropertySet &outArgs)
    {
      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      // we can only be a general context effect, so check that this is true
      OFX::Log::error(effectInstance->getContext() != eContextGeneral, "Calling kOfxImageEffectActionGetTimeDomain on an effect that is not a general context effect.");

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
    bool
    clipPreferencesAction(OfxImageEffectHandle handle, OFX::PropertySet &outArgs)
    {
      // fetch our effect pointer 
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);

      // set up our clip preferences setter
      ImageEffectDescriptor* desc = gEffectDescriptors[effectInstance->getContext()];
      ClipPreferencesSetter prefs(outArgs, desc->getClipDepthPropNames(), desc->getClipComponentPropNames(), desc->getClipPARPropNames());
            
      // and call the plug-in client code
      effectInstance->getClipPreferences(prefs);

      // did we do anything ?
      if(prefs.didSomething()) 
        return true;
      return false;
    }

    /** @brief Library side begin instance changed action */
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
    void
    endInstanceChangedAction(OfxImageEffectHandle handle, OFX::PropertySet inArgs)
    {
      ImageEffect *effectInstance = retrieveImageEffectPointer(handle);
      
      std::string reasonStr = inArgs.propGetString(kOfxPropChangeReason);
      InstanceChangeReason reason = mapToInstanceChangedReason(reasonStr);

      // and call the plugin client code
      effectInstance->endChanged(reason);
    }
        

    /** @brief The main entry point for the plugin
     */
    OfxStatus
    mainEntry(const char    *actionRaw,
              const void    *handleRaw,
              OfxPropertySetHandle   inArgsRaw,
              OfxPropertySetHandle   outArgsRaw)
    {
      OFX::Log::print("********************************************************************************");
      OFX::Log::print("START mainEntry (%s)", actionRaw);
      OFX::Log::indent();
      OfxStatus stat = kOfxStatReplyDefault;
      try {
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
          OFX::Plugin::loadAction();

          // got here, must be good
          stat = kOfxStatOK;
        }

        // figure the actions
        else if (action == kOfxActionUnload) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, true, true, true);

          // call the support unload function, param-less
          OFX::Private::unloadAction(); 
      
          // call the plugin side unload action, param-less, should be called, eve if the stat above failed!
          OFX::Plugin::unloadAction();

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
          OFX::Plugin::describe(*desc);

          // add it to our map
          gEffectDescriptors[eContextNone] = desc;

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

          // call plugin descibe in context
          OFX::Plugin::describeInContext(*desc, context);

          // add it to our map
          gEffectDescriptors[context] = desc;

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
          ImageEffect *instance = OFX::Plugin::createInstance(handle, context);

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
          if(regionsOfInterestAction(handle, inArgs, outArgs))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionGetFramesNeeded) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, false, false);

          // call the frames needed action, return OK if it does something
          if(framesNeededAction(handle, inArgs, outArgs))
            stat = kOfxStatOK;
        }
        else if(action == kOfxImageEffectActionGetClipPreferences) {
          checkMainHandles(actionRaw, handleRaw, inArgsRaw, outArgsRaw, false, true, false);

          // call the frames needed action, return OK if it does something
          if(clipPreferencesAction(handle, outArgs))
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

          // fetch our pointer out of the props on the handle
          ImageEffect *instance = retrieveImageEffectPointer(handle);

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
        else if(actionRaw) {
          OFX::Log::error(true, "Unknown action '%s'.", actionRaw);
        }
        else {
          OFX::Log::error(true, "Requested action was a null pointer.");
        }
      }

      // catch suite exceptions
      catch (OFX::Exception::Suite &ex)
        {
          std::cout << "Caught OFX::Exception::Suite" << std::endl;
          stat = ex.status();
        }

      // catch host inadequate exceptions
      catch (OFX::Exception::HostInadequate)
        {
          std::cout << "Caught OFX::Exception::HostInadequate" << std::endl;
          stat = kOfxStatErrMissingHostFeature;
        }

      // catch exception due to a property being unknown to the host, implies something wrong with host if not caught further down
      catch (OFX::Exception::PropertyUnknownToHost)
        {
          std::cout << "Caught OFX::Exception::PropertyUnknownToHost" << std::endl;
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
          stat = OFX_CLIENT_EXCEPTION_HANDLER(ex);
        }
#endif
      // Catch anything else, unknown
      catch (...)
        {
          std::cout << "Caught Unknown exception" << std::endl;
          stat = kOfxStatFailed;
        }
      
      OFX::Log::outdent();
      OFX::Log::print("STOP mainEntry (%s)\n", actionRaw);
      return stat;
    }      


    /** @brief The plugin function that gets passed the host structure. */
    void setHost(OfxHost *host)
    {
      gHost = host;
    }
    
  }; // namespace Private

  /** @brief Fetch's a suite from the host and logs errors */
  void * fetchSuite(char *suiteName, int suiteVersion, bool optional)
  {
    void *suite = Private::gHost->fetchSuite(Private::gHost->host, suiteName, suiteVersion);
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

/** @brief, mandated function returning the number of plugins, which is always 1 */
OfxExport int 
OfxGetNumberOfPlugins(void)
{
  return 1;
}

/** @brief, mandated function returning the nth plugin 

  We call the plugin side defined OFX::Plugin::getPluginID function to find out what to set.
*/

OfxExport OfxPlugin *
OfxGetPlugin(int nth)
{
  OFX::Log::error(nth != 0, "Host attempted to get plugin %d, when there is only 1 plugin, so it should have asked for 0", nth);
  // the raw OFX plugin struct returned to the host
  static OfxPlugin      ofxPlugin;

  // struct identifying the plugin to the support lib 
  static OFX::PluginID id;

  // API is always an image effect plugin
  ofxPlugin.pluginApi  = kOfxImageEffectPluginApi;
  ofxPlugin.apiVersion = 1;

  // call the plugin defined id function
  OFX::Plugin::getPluginID(id);

  // set identifier, version major and version minor 
  ofxPlugin.pluginIdentifier   = id.pluginIdentifier.c_str();
  ofxPlugin.pluginVersionMajor = id.pluginVersionMajor;
  ofxPlugin.pluginVersionMinor = id.pluginVersionMinor;

  // set up our two routines
  ofxPlugin.setHost    = OFX::Private::setHost;
  ofxPlugin.mainEntry  = OFX::Private::mainEntry;

  return &ofxPlugin;
}
