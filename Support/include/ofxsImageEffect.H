#ifndef _ofxsImageEffect_H_
#define _ofxsImageEffect_H_
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

/** @file This file contains core code that wraps OFX 'objects' with C++ classes.

This file only holds code that is visible to a plugin implementation, and so hides much
of the direct OFX objects and any library side only functions.
 */
#include "ofxsParam.h"

#include <vector>
#include <string>
#include <map>

/** @brief Nasty macro used to define empty protected copy ctors and assign ops */
#define mDeclareProtectedAssignAndCC(CLASS) \
  CLASS &operator=(const CLASS &v1) {assert(false); return *this;}	\
  CLASS(const CLASS &v) {assert(false); } 

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
 */
namespace OFX {
  /** forward class declarations */
  class ClipDescriptor;
  class ImageEffectDescriptor;

  class Image;
  class Clip;
  class ImageEffect;

  /** @brief Enumerates the contexts a plugin can be used in */
  enum ContextEnum {eContextNone,
		    eContextGenerator,
		    eContextFilter,
		    eContextTransition,
		    eContextPaint,
		    eContextGeneral,
		    eContextRetimer};

  /** @brief Enumerates the pixel depths supported */
  enum BitDepthEnum {eBitDepthNone, /**< @brief bit depth that indicates no data is present */
		     eBitDepthUByte,
		     eBitDepthUShort,
		     eBitDepthFloat};

  /** @brief Enumerates the component types supported */
  enum PixelComponentEnum {ePixelComponentNone,
			   ePixelComponentRGBA,
			   ePixelComponentAlpha};

  /** @brief Enumerates the ways a fielded image can be extracted from a clip */
  enum FieldExtractionEnum {eFieldExtractBoth,   /**< @brief extract both fields */
			    eFieldExtractSingle, /**< @brief extracts a single field, so you have a half height image */
			    eFieldExtractDoubled /**< @brief extracts a single field, but doubles up the field, so you have a full height image */
  };
  
  /** @brief Enumerates the kind of render thread safety a plugin has */
  enum RenderSafetyEnum {eRenderUnsafe,       /**< @brief can only render a single instance at any one time */
			 eRenderInstanceSafe, /**< @brief can call a single render on an instance, but can render multiple instances simultaneously */
			 eRenderFullySafe     /**< @brief can call render any number of times on an instance, and render multiple instances simultaneously */
  };
  
  /** @brief Enumerates the fields present in an image */
  enum FieldEnum {eFieldNone,   /**< @brief unfielded image */
		  eFieldBoth,   /**< @brief fielded image with both fields present */
		  eFieldLower,  /**< @brief only the spatially lower field is present */
		  eFieldUpper   /**< @brief only the spatially upper field is present  */
  };

  enum PreMultiplicationEnum { eImageOpaque,          /**< @brief the image is opaque and so has no premultiplication state */
			       eImagePreMultiplied,   /**< @brief the image is premultiplied by it's alpha */
			       eImageUnPreMultiplied, /**< @brief the image is unpremultiplied */
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief A class that lists all the properties of a host */
  struct ImageEffectHostDescription {
  public :
    std::string hostName;
    bool hostIsBackground;
    bool supportsOverlays;
    bool supportsMultiResolution;
    bool supportsTiles;
    bool temporalClipAccess;
    bool supportsMultipleClipDepths;
    bool supportsMultipleClipPARs;
    bool supportsSetableFrameRate;
    bool supportsSetableFielding;
    bool supportsStringAnimation;
    bool supportsCustomInteract;
    bool supportsChoiceAnimation;
    bool supportsBooleanAnimation;
    bool supportsCustomAnimation;
    int maxParameters;
    int maxPages;
    int pageRowCount;
    int pageColumnCount;
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up a clip */
  class ClipDescriptor {
  protected :
    mDeclareProtectedAssignAndCC(ClipDescriptor);
    ClipDescriptor(void) {assert(false);}
    
  protected :
    /** @brief name of the clip */
    std::string _clipName;

    /** @brief properties for this clip */
    PropertySet _clipProps;

  protected :
    /** @brief hidden constructor */
    ClipDescriptor(const std::string &name, OfxPropertySetHandle props);

    friend class ImageEffectDescriptor;

  public :
    /** @brief set the label properties */
    void setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel);

    /** @brief set how fielded images are extracted from the clip defaults to eFieldExtractDoubled */
    void setFieldExtraction(FieldExtractionEnum v);

    /** @brief set which components are supported, defaults to none set, this must be called at least once! */
    void addSupportedComponent(PixelComponentEnum v);

    /** @brief say whether we are going to do random temporal access on this clip, defaults to false */
    void setTemporalClipAccess(bool v);

    /** @brief say whether if the clip is optional, defaults to false */
    void setOptional(bool v);

    /** @brief say whether this clip supports tiling, defaults to true */
    void setSupportsTiles(bool v);

    /** @brief say whether this clip is a 'mask', so the host can know to replace with a roto or similar, defaults to false */
    void setIsMask(bool v);    
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an effect descriptor, used in the describe actions */
  class ImageEffectDescriptor : public ParamSetDescriptor
  {
  protected :
    mDeclareProtectedAssignAndCC(ImageEffectDescriptor);
    ImageEffectDescriptor(void) {assert(false);}
    
  protected :
    /** @brief The effect handle */
    OfxImageEffectHandle _effectHandle;

    /** @brief properties for this clip */
    PropertySet _effectProps;

    /** @brief Set of all previously defined parameters, defined on demand */
    std::map<std::string, ClipDescriptor *> _definedClips;

  public :
    /** @brief ctor */
    ImageEffectDescriptor(OfxImageEffectHandle handle);
    
    /** @brief dtor */
    ~ImageEffectDescriptor();

    /** @brief, set the label properties in a plugin */
    void setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel);

    /** @brief Set the plugin grouping, defaults to "" */
    void setPluginGrouping(const std::string &group);

    /** @brief Add a context to those supported, defaults to none, must be called at least once */
    void addSupportedContext(ContextEnum v);

    /** @brief Add a pixel depth to those supported, defaults to none, must be called at least once */
    void addSupportedBitDepth(BitDepthEnum v);

    /** @brief Is the plugin single instance only ? defaults to false */
    void setSingleInstance(bool v);

    /** @brief Does the plugin expect the host to perform per frame SMP threading defaults to true */
    void setHostFrameThreading(bool v);
    
    /** @brief Does the plugin support multi resolution images, defaults to true */
    void setSupportsMultiResolution(bool v);

    /** @brief Does the plugin support image tiling, defaults to true */
    void setSupportsTiles(bool v);

    /** @brief Does the plugin perform temporal clip access, defaults to false */
    void setTemporalClipAccess(bool v);
    
    /** @brief Does the plugin want to have render called twice per frame in all circumanstances for fielded images ? defaults to true */
    void setRenderTwiceAlways(bool v);

    /** @brief Does the plugin support inputs and output clips of differing depths, defaults to false */
    void setSupportsMultipleClipDepths(bool v);

    /** @brief Does the plugin support inputs and output clips of pixel aspect ratios, defaults to false */
    void setSupportsMultipleClipPARs(bool v);

    /** @brief How thread safe is the plugin, defaults to eRenderInstanceSafe */
    void setRenderThreadSafety(RenderSafetyEnum v);

    /** @brief If the slave  param changes the clip preferences need to be re-evaluated */
    void addClipPreferencesSlaveParam(ParamDescriptor &p);

    /** @brief Create a clip, only callable from describe in context */
    ClipDescriptor *defineClip(const std::string &name);
  };  

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an image */
  class Image {
  protected :
    /** @brief the handle that holds this image */
    PropertySet _imageProps;

    /** @brief hidden ctor */
    Image(OfxPropertySetHandle props);

    /** @brief friend so we get access to ctor */
    friend class Clip;

  public :
    /** @brief dtor */
    virtual ~Image();

    /** @brief get the pixel depth */
    BitDepthEnum pixelDepth(void) const;

    /** @brief get the components in the image */
    PixelComponentEnum pixelComponents(void) const;

    /** @brief get the components in the image */
    PreMultiplicationEnum preMultiplication(void) const;
    
    /** @brief get the scale factor that has been applied to this image */
    OfxPointD renderScale(void) const;

    /** @brief get the scale factor that has been applied to this image */
    double pixelAspectRatio(void) const;

    /** @brief get the pixel data for this image */
    void *pixelData(void) const;

    /** @brief get the region of definition (in pixel coordinates) of this image */
    OfxRectI regionOfDefinition(void) const;

    /** @brief get the row bytes, may be negative */
    int rowBytes(void) const;

    /** @brief get the fielding of this image */
    FieldEnum field(void) const;

    /** @brief the unique ID of this image */
    std::string UniqueIdentifier(void) const;
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up a clip instance */
  class Clip {
  protected :
    mDeclareProtectedAssignAndCC(Clip);

    /** @brief name of the clip */
    std::string _clipName;

    /** @brief properties for this clip */
    PropertySet _clipProps;

    /** @brief handle for this clip */
    OfxImageClipHandle _clipHandle;

    /** @brief effect instance that owns this clip */
    ImageEffect *_effect;

    /** @brief hidden constructor */
    Clip(ImageEffect *effect, const std::string &name, OfxImageClipHandle handle, OfxPropertySetHandle props);

    /** @brief so one can be made */
    friend class ImageEffect;

  public :
    /** @brief get the name */
    const std::string &name(void) const {return _clipName;}

    /** @brief fetch the labels */
    void getLabels(std::string &label, std::string &shortLabel, std::string &longLabel) const;

    /** @brief what is the pixel depth images will be given to us as */
    BitDepthEnum pixelDepth(void) const;

    /** @brief what is the components images will be given to us as */
    PixelComponentEnum pixelComponents(void) const;
    
    /** @brief what is the actual pixel depth of the clip */
    BitDepthEnum unmappedPixelDepth(void) const;

    /** @brief what is the component type of the clip */
    PixelComponentEnum unmappedPixelComponents(void) const;
    
    /** @brief get the components in the image */
    PreMultiplicationEnum preMultiplication(void) const;

    /** @brief which spatial field comes first temporally */
    FieldEnum fieldOrder(void) const;

    /** @brief is the clip connected */
    bool isConnected(void) const;

    /** @brief can the clip be continuously sampled */
    bool hasContinuousSamples(void) const;

    /** @brief get the scale factor that has been applied to this clip */
    double pixelAspectRatio(void) const;
      
    /** @brief get the frame rate, in frames per second on this clip, after any clip preferences have been applied */
    double frameRate(void) const;
      
    /** @brief return the range of frames over which this clip has images, after any clip preferences have been applied */
    OfxRangeD frameRange(void) const;

    /** @brief get the frame rate, in frames per second on this clip, before any clip preferences have been applied */
    double unmappedFrameRate(void) const;
      
    /** @brief return the range of frames over which this clip has images, before any clip preferences have been applied */
    OfxRangeD unmappedFrameRange(void) const;

    /** @brief get the RoD for this clip in the cannonical coordinate system */
    OfxRectD regionOfDefinition(double t);

    /** @brief fetch an image */
    Image *fetchImage(double t);

    /** @brief fetch an image, with a specific region in cannonical coordinates */
    Image *fetchImage(double t, OfxRectD bounds);
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an effect descriptor, used in the describe actions */
  class ImageEffect : public ParamSet
  {
  protected :
    mDeclareProtectedAssignAndCC(ImageEffect);

  protected :
    /** @brief The effect handle */
    OfxImageEffectHandle _effectHandle;

    /** @brief properties for this clip */
    PropertySet _effectProps;

    /** @brief the context of the effect */
    ContextEnum _context;

    /** @brief Set of all previously defined parameters, defined on demand */
    std::map<std::string, Clip *> _fetchedClips;

    /** @brief client instance data */
    void *_instanceData;

  public :
    /** @brief ctor */
    ImageEffect(OfxImageEffectHandle handle);

    /** @brief the context this effect was instantiate in */
    ContextEnum context(void) const;

    /** @brief size of the project */
    OfxPointD projectSize(void) const;
    
    /** @brief origin of the project */
    OfxPointD projectOffset(void) const;

    /** @brief extent of the project */
    OfxPointD projectExtent(void) const;

    /** @brief pixel aspect ratio of the project */
    double projectPixelAspectRatio(void) const;

    /** @brief how long does the effect last */
    double effectDuration(void) const;

    /** @brief the frame rate of the project */
    double frameRate(void) const;

    /** @brief is the instance currently being interacted with */
    bool isInteractive(void) const;

    /** @brief set the instance to be sequentially renderred, this should have been part of clip preferences! */
    void sequentialRender(bool v);

    /** @brief Have we informed the host we want to be seqentially renderred ? */
    bool sequentialRender(void) const;
    
    /** @brief Fetch the named clip from this instance */
    Clip *fetchClip(const std::string &name);

    /** @brief does the host want us to abort rendering? */
    bool abort(void) const;

    /** @brief get some image memory */
    OfxImageMemoryHandle allocImageMemory(size_t nBytes);

    /** @brief free image */
    void freeImageMemory(OfxImageMemoryHandle handle);

    /** @brief lock the image memory */
    void *lockImageMemory(OfxImageMemoryHandle handle);

    /** @brief unlock the image memory */
    void unlockImageMemory(OfxImageMemoryHandle handle);
  };  

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief map a std::string to a context enum */
  ContextEnum mapToContextEnum(const std::string &s);

  /** @brief Global description of the host */
  extern ImageEffectHostDescription *gHostDescription;

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief POD struct to pass rendering arguments into the client code */
  struct RenderArguments {
    double    time;
    OfxPointD renderScale;
    OfxRectI  renderWindow;
    FieldEnum fieldToRender;
  };
  
  ////////////////////////////////////////////////////////////////////////////////
  /** @brief The OFX::Plugin namespace. All the functions in here needs to be defined by each plugin that uses the support libs.
   */
  namespace Plugin {
    /** @brief Plugin side function used to identify the plugin to the support library */
    void getPluginID(OFX::PluginID &id);

    /** @brief Plugin side function called during the load action */
    void loadAction(void);

    /** @brief Plugin side function called during the unload action */
    void unloadAction(void);

    /** @brief The basic describe function, passed a plugin descriptor */
    void describe(OFX::ImageEffectDescriptor &desc);

    /** @brief The describe in context function, passed a plugin descriptor and a context */
    void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context);

    /** @brief The create instance function */
    void createInstance(OFX::ImageEffect &effect);

    /** @brief The destroy instance function */
    void destroyInstance(OFX::ImageEffect &effect);

    /** @brief The render function */
    void render(OFX::ImageEffect &effect, const RenderArguments &args);
  };

};

// undeclare the protected assign and CC macro
#undef mDeclareProtectedAssignAndCC

#endif
