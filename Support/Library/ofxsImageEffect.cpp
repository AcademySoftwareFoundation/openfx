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
            throw std::invalid_argument("");
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
            throw std::invalid_argument("");
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
        BitDepthEnum e;
        try {
            e = mapStrToBitDepthEnum(str);
            if(e == eBitDepthNone) {
                OFX::Log::error(true, "Image with no pixel depth.");
            }
        }
        // gone wrong ?
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown pixel depth property '%s' reported on an image!", str.c_str());
            e = eBitDepthNone;
        }
        return e;
    }
  
    /** @brief get the components in the image */
    PixelComponentEnum Image::pixelComponents(void) const
    {
        std::string str = _imageProps.propGetString(kOfxImageEffectPropComponents);
        PixelComponentEnum e;
        try {
            e = mapStrToPixelComponentEnum(str);
            if(e == ePixelComponentNone) {
                OFX::Log::error(true, "Image with no pixel component type.");
            }
        }
        // gone wrong ?
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown  pixel component type '%s' reported on an image!", str.c_str());
            e = ePixelComponentNone;
        }
        return e;
    }

    /** @brief get the components in the image */
    PreMultiplicationEnum Image::preMultiplication(void) const
    {
        std::string str = _imageProps.propGetString(kOfxImageEffectPropPreMultiplication);
        PreMultiplicationEnum e;
        try {
            e = mapStrToPreMultiplicationEnum(str);
        }
        // gone wrong ?
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown premultiplication type '%s' reported on an image!", str.c_str());
            e = eImageOpaque;
        }
        return e;
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
        shortLabel = _clipProps.propGetString(kOfxPropShortLabel);
        longLabel  = _clipProps.propGetString(kOfxPropLongLabel);
    }

    /** @brief get the pixel depth */
    BitDepthEnum Clip::pixelDepth(void) const
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
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown pixel depth property '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
            e = eBitDepthNone;
        }
        return e;
    }

    /** @brief get the components in the image */
    PixelComponentEnum Clip::pixelComponents(void) const
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
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown  pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
            e = ePixelComponentNone;
        }
        return e;
    }

    /** @brief what is the actual pixel depth of the clip */
    BitDepthEnum Clip::unmappedPixelDepth(void) const
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
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown unmapped pixel depth property '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
            e = eBitDepthNone;
        }
        return e;
    }

    /** @brief what is the component type of the clip */
    PixelComponentEnum Clip::unmappedPixelComponents(void) const
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
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown unmapped pixel component type '%s' reported on clip '%s'", str.c_str(), _clipName.c_str());
            e = ePixelComponentNone;
        }
        return e;
    }

    /** @brief get the components in the image */
    PreMultiplicationEnum Clip::preMultiplication(void) const
    {
        std::string str = _clipProps.propGetString(kOfxImageEffectPropPreMultiplication);
        PreMultiplicationEnum e;
        try {
            e = mapStrToPreMultiplicationEnum(str);
        }
        // gone wrong ?
        catch(std::invalid_argument &ex) {
            OFX::Log::error(true, "Unknown premultiplication type '%s' reported on clip %s!", str.c_str(), _clipName.c_str());
            e = eImageOpaque;
        }
        return e;
    }
    
    /** @brief which spatial field comes first temporally */
    FieldEnum Clip::fieldOrder(void) const
    {
        std::string str = _clipProps.propGetString(kOfxImageClipPropFieldOrder);
        FieldEnum e;
        try {
            e = mapStrToFieldEnum(str);
            OFX::Log::error(e != eFieldNone && e != eFieldLower && e != eFieldUpper, 
                            "Field order '%s' reported on a clip %s is invalid, it must be none, lower or upper.", str.c_str(), _clipName.c_str());
        }
        // gone wrong ?
        catch(std::invalid_argument &ex) {
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
    double Clip::pixelAspectRatio(void) const
    {
        return _clipProps.propGetDouble(kOfxImagePropPixelAspectRatio);
    }
      
    /** @brief get the frame rate, in frames per second on this clip, after any clip preferences have been applied */
    double Clip::frameRate(void) const
    {
        return _clipProps.propGetDouble(kOfxImageEffectPropFrameRate);
    }
      
    /** @brief return the range of frames over which this clip has images, after any clip preferences have been applied */
    OfxRangeD Clip::frameRange(void) const
    {
        OfxRangeD v;
        v.min = _clipProps.propGetDouble(kOfxImageEffectPropFrameRange, 0);
        v.max = _clipProps.propGetDouble(kOfxImageEffectPropFrameRange, 1);
        return v;
    }

    /** @brief get the frame rate, in frames per second on this clip, before any clip preferences have been applied */
    double Clip::unmappedFrameRate(void) const
    {
        return _clipProps.propGetDouble(kOfxImageEffectPropUnmappedFrameRate);
    }
      
    /** @brief return the range of frames over which this clip has images, before any clip preferences have been applied */
    OfxRangeD Clip::unmappedFrameRange(void) const
    {
        OfxRangeD v;
        v.min = _clipProps.propGetDouble(kOfxImageEffectPropUnmappedFrameRange, 0);
        v.max = _clipProps.propGetDouble(kOfxImageEffectPropUnmappedFrameRange, 1);
        return v;
    }

    /** @brief get the RoD for this clip in the cannonical coordinate system */
    OfxRectD Clip::regionOfDefinition(double t)
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

        // Set this as the instance data pointer on the effect handle
        _effectProps.propSetPointer(kOfxPropInstanceData, this);
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
    ContextEnum ImageEffect::context(void) const
    {
        return _context;
    }

    /** @brief size of the project */
    OfxPointD ImageEffect::projectSize(void) const
    {
        OfxPointD v;    
        v.x = _effectProps.propGetDouble(kOfxImageEffectPropProjectSize, 0);
        v.y = _effectProps.propGetDouble(kOfxImageEffectPropProjectSize, 1);
        return v;
    }
    
    /** @brief origin of the project */
    OfxPointD ImageEffect::projectOffset(void) const
    {
        OfxPointD v;    
        v.x = _effectProps.propGetDouble(kOfxImageEffectPropProjectOffset, 0);
        v.y = _effectProps.propGetDouble(kOfxImageEffectPropProjectOffset, 1);
        return v;
    }

    /** @brief extent of the project */
    OfxPointD ImageEffect::projectExtent(void) const
    {
        OfxPointD v;    
        v.x = _effectProps.propGetDouble(kOfxImageEffectPropProjectExtent, 0);
        v.y = _effectProps.propGetDouble(kOfxImageEffectPropProjectExtent, 1);
        return v;
    }

    /** @brief pixel aspect ratio of the project */
    double ImageEffect::projectPixelAspectRatio(void) const
    {
        return _effectProps.propGetDouble(kOfxImageEffectPropProjectPixelAspectRatio, 0);
    }

    /** @brief how long does the effect last */
    double ImageEffect::effectDuration(void) const
    {
        return _effectProps.propGetDouble(kOfxImageEffectInstancePropEffectDuration, 0);
    }

    /** @brief the frame rate of the project */
    double ImageEffect::frameRate(void) const
    {
        return _effectProps.propGetDouble(kOfxImageEffectPropFrameRate, 0);
    }

    /** @brief is the instance currently being interacted with */
    bool ImageEffect::isInteractive(void) const
    {
        return _effectProps.propGetInt(kOfxPropIsInteractive) != 0;
    }

    /** @brief set the instance to be sequentially renderred, this should have been part of clip preferences! */
    void ImageEffect::sequentialRender(bool v)
    {
        _effectProps.propSetInt(kOfxImageEffectInstancePropSequentialRender, int(v));
    }

    /** @brief Have we informed the host we want to be seqentially renderred ? */
    bool ImageEffect::sequentialRender(void) const
    {
        return _effectProps.propGetInt(kOfxImageEffectInstancePropSequentialRender) != 0;
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
        OfxImageClipHandle clipHandle;
        OfxPropertySetHandle propHandle;
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
  
    /** @brief default sync the private data function, does nothing */
    void ImageEffect::syncPrivateData(void)
    {
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
};
