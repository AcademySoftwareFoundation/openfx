/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  The Foundry Visionmongers Ltd
  35-36 Gt Marlborough St 
  London W1V 7FN
  England
*/

/** @brief This file contains code that skins the ofx effect suite */

#include "./ofxsSupportPrivate.H"

 
/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries. */
namespace OFX {
  
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
    _clipProps.propSetString(kOfxPropShortLabel, shortLabel);
    _clipProps.propSetString(kOfxPropLongLabel, longLabel);
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
    throwStatusException(stat);
    _effectProps.propSetHandle(props);

    OFX::Validation::validatePluginDescriptorProperties(props);

    // fetch the param set handle and set it in our ParamSetDescriptor base
    OfxParamSetHandle paramSetHandle;
    stat = OFX::Private::gEffectSuite->getParamSet(handle, &paramSetHandle);
    throwStatusException(stat);
    setParamSetHandle(paramSetHandle);
  }
  
  /** @brief dtor */
  ImageEffectDescriptor::~ImageEffectDescriptor()
  {
    // delete any descriptor we may have constructed
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
    _effectProps.propSetString(kOfxPropShortLabel, shortLabel);
    _effectProps.propSetString(kOfxPropLongLabel, longLabel);
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
    _effectProps.propSetString(kOfxImageEffectPropClipPreferencesSlaveParam, p.name(), n);
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
    return clip;
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  // wraps up an image  
  Image::Image(OfxPropertySetHandle props)
    : _imageProps(props)
  {
    OFX::Validation::validateImageProperties(props);
  }

  Image::~Image()
  {
    OFX::Private::gEffectSuite->clipReleaseImage(_imageProps.propSetHandle());
  }

  /** @brief get the pixel depth */
  BitDepthEnum Image::pixelDepth(void) const
  {
    std::string str = _imageProps.propGetString(kOfxImageEffectPropPixelDepth);
    if(str == kOfxBitDepthByte) {
      return eBitDepthUByte;
    }
    else if(str == kOfxBitDepthShort) {
      return eBitDepthUShort;
    }
    else if(str == kOfxBitDepthFloat) {
      return eBitDepthFloat;
    }
    else {
      OFX::Log::error(true, "Unknown pixel depth '%s' reported on an image!", str.c_str());
      return eBitDepthNone;
    }
  }

  /** @brief get the components in the image */
  PixelComponentEnum Image::pixelComponents(void) const
  {
    std::string str = _imageProps.propGetString(kOfxImageEffectPropComponents);
    if(str == kOfxImageComponentRGBA) {
      return ePixelComponentRGBA;
    }
    else if(str == kOfxImageComponentAlpha) {
      return ePixelComponentAlpha;
    }
    else {
      OFX::Log::error(true, "Unknown pixel component type '%s' reported on an image!", str.c_str());
      return ePixelComponentNone;
    }
  }

  /** @brief get the components in the image */
  PreMultiplicationEnum Image::preMultiplication(void) const
  {
    std::string str = _imageProps.propGetString(kOfxImageEffectPropPreMultiplication);
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
      OFX::Log::error(true, "Unknown premultiplication state '%s' reported on an image", str.c_str());
      return eImageOpaque;
    }
  }
    
  /** @brief get the scale factor that has been applied to this image */
  OfxPointD Image::renderScale(void) const
  {
    OfxPointD scale;
    scale.x = _imageProps.propGetDouble(kOfxImageEffectPropRenderScale, 0);
    scale.y = _imageProps.propGetDouble(kOfxImageEffectPropRenderScale, 1);
    return scale;
  }

  /** @brief get the scale factor that has been applied to this image */
  double Image::pixelAspectRatio(void) const
  {
    return _imageProps.propGetDouble(kOfxImagePropPixelAspectRatio);
  }

  /** @brief get the pixel data for this image */
  void *Image::pixelData(void) const
  {
    return _imageProps.propGetPointer(kOfxImagePropData);
  }

  /** @brief get the region of definition (in pixel coordinates) of this image */
  OfxRectI Image::regionOfDefinition(void) const
  {
    OfxRectI rod;
    rod.x1 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 0);
    rod.y1 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 1);
    rod.x2 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 2);
    rod.y2 = _imageProps.propGetInt(kOfxImagePropRegionOfDefinition, 3);
    return rod;
  }

  /** @brief get the row bytes, may be negative */
  int Image::rowBytes(void) const
  {
    return _imageProps.propGetInt(kOfxImagePropRowBytes);
  }

  /** @brief get the fielding of this image */
  FieldEnum Image::field(void) const
  {
    std::string str = _imageProps.propGetString(kOfxImagePropField);
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
      return eFieldLower;
    }
    else {
      OFX::Log::error(true, "Unknown field state '%s' reported on an image", str.c_str());
      return eFieldNone;
    }
  }

  /** @brief the unique ID of this image */
  std::string Image::UniqueIdentifier(void) const
  {
    return _imageProps.propGetString(kOfxImagePropUniqueIdentifier);
  }

};
