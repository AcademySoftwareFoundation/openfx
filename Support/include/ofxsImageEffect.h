#ifndef _ofxsImageEffect_H_
#define _ofxsImageEffect_H_
/*
OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
Copyright (C) 2004-2005 The Open Effects Association Ltd
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

/** @file This file contains core code that wraps OFX 'objects' with C++ classes.

This file only holds code that is visible to a plugin implementation, and so hides much
of the direct OFX objects and any library side only functions.
*/
#include <map>
#include <string>
#include <sstream> // stringstream
#include <memory>
#include "ofxsParam.h"
#include "ofxsInteract.h"
#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
#include "ofxsHWNDInteract.h"
#endif
#endif
#include "ofxsMessage.h"
#include "ofxProgress.h"
#include "ofxTimeLine.h"
#ifdef OFX_EXTENSIONS_VEGAS
#include "ofxSonyVegas.h"
#endif
#include "ofxParametricParam.h"
#ifdef OFX_EXTENSIONS_NUKE
#include "nuke/camera.h"
#include "nuke/fnOfxExtensions.h"
#include "nuke/fnPublicOfxExtensions.h"
#endif
#ifdef OFX_EXTENSIONS_TUTTLE
#include "tuttle/ofxReadWrite.h"
#endif
#ifdef OFX_EXTENSIONS_NATRON
#include "ofxNatron.h"
#endif

/** @brief Nasty macro used to define empty protected copy ctors and assign ops */
#define mDeclareProtectedAssignAndCC(CLASS) \
  CLASS &operator=(const CLASS &) {assert(false); return *this;}	\
  CLASS(const CLASS &) {assert(false); } 
#define mDeclareProtectedAssignAndCCBase(CLASS,BASE) \
  CLASS &operator=(const CLASS &) {assert(false); return *this;}	\
  CLASS(const CLASS &c) : BASE(c) {assert(false); }

namespace OFX
{
  namespace Private
  {
    OfxStatus mainEntryStr(const char    *actionRaw,
      const void    *handleRaw,
      OfxPropertySetHandle   inArgsRaw,
      OfxPropertySetHandle   outArgsRaw,
      const char* plugname);

    OfxStatus customParamInterpolationV1Entry(
      const void*            handleRaw,
      OfxPropertySetHandle   inArgsRaw,
      OfxPropertySetHandle   outArgsRaw);
  }
}


/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
*/
namespace OFX {
  /** forward class declarations */
  class ClipDescriptor;
  class ImageEffectDescriptor;

  class Image;
  class Clip;
  class ImageEffect;
  class ImageMemory;

  /** @brief Enumerates the contexts a plugin can be used in */
  enum ContextEnum {eContextNone,
    eContextGenerator,
    eContextFilter,
    eContextTransition,
    eContextPaint,
    eContextGeneral,
    eContextRetimer,
#ifdef OFX_EXTENSIONS_TUTTLE
    eContextReader,
    eContextWriter,
#endif
#ifdef OFX_EXTENSIONS_NATRON
    eContextTracker
#endif
  };

  /** @brief Enumerates the pixel depths supported */
  enum BitDepthEnum {eBitDepthNone, /**< @brief bit depth that indicates no data is present */
    eBitDepthUByte,
    eBitDepthUShort,
    eBitDepthHalf,
    eBitDepthFloat,
    eBitDepthCustom, ///< some non standard bit depth
#ifdef OFX_EXTENSIONS_VEGAS
    eBitDepthUByteBGRA,   /// added support for sony vegas bgra ordered pixels
    eBitDepthUShortBGRA,
    eBitDepthFloatBGRA,
#endif
  };

  /** @brief Enumerates the component types supported */
  enum PixelComponentEnum {ePixelComponentNone,
    ePixelComponentRGBA,
    ePixelComponentRGB,
    ePixelComponentAlpha,
#ifdef OFX_EXTENSIONS_NUKE
    ePixelComponentMotionVectors,
    ePixelComponentStereoDisparity,
#endif
#ifdef OFX_EXTENSIONS_NATRON
    ePixelComponentXY,
#endif
    ePixelComponentCustom ///< some non standard pixel type
  };
    
#ifdef OFX_EXTENSIONS_NUKE
  enum PassThroughLevelEnum {
    //all planes not specified in output by the getClipComponents action will not be visible by effects down-stream
    ePassThroughLevelBlockAllNonRenderedPlanes = 0,
      
    //all planes not specified in output by the getClipComponents action will be pass-through from the input pass-through clip
    ePassThroughLevelPassThroughNonRenderedPlanes = 1,
      
    //all planes requested by the host are rendered, regardless of the getClipComponents action.
    //In this mode, the render action will be called once
    //for every plane (instead of a single time with all planes in parameter) and the plug-in is expected to use the regular
    //image effect suite, i.e: clipGetImage. The pixel components property of the image returned by clipGetImage is expected to
    //match what is returned by the pixel components property of the clip. This value is useful for instance for Transform effects:
    //all planes will be transformed with minimalist changes to the plug-in code.
    ePassThroughLevelRenderAllRequestedPlanes = 2
  };
    
  enum ViewInvarianceLevelEnum {
      
    // All views produced are different from each other
    eViewInvarianceAllViewsVariant = 0,
      
    // All views are similar except for pass-through planes
    eViewInvarianceOnlyPassThroughPlanesVariant,
      
    // Result is similar across all views
    eViewInvarianceAllViewsInvariant,
  };
#endif

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
    eFieldUpper,  /**< @brief only the spatially upper field is present  */
    eFieldSingle, /**< @brief image that consists of a single field, and so is half height  */
    eFieldDoubled /**< @brief image that consists of a single field, but each scan line is double, and so is full height  */
  };

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief Enumerates the pixel order in an image */
  enum PixelOrderEnum {
    ePixelOrderRGBA,   /**< @brief pixel order is RGBA (ofx typical) */
    ePixelOrderBGRA    /**< @brief pixel order is BGRA (added for Sony Vegas) */
  };
#endif

  enum PreMultiplicationEnum { eImageOpaque,          /**< @brief the image is opaque and so has no premultiplication state */
    eImagePreMultiplied,   /**< @brief the image is premultiplied by it's alpha */
    eImageUnPreMultiplied, /**< @brief the image is unpremultiplied */
  };

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief Enumerates the vegas contexts a plugin is being used in */
  enum VegasRenderQualityEnum {
    eVegasRenderQualityUnknown,
    eVegasRenderQualityDraft,
    eVegasRenderQualityPreview,
    eVegasRenderQualityGood,
    eVegasRenderQualityBest
    };

  /** @brief Enumerates the vegas contexts a plugin is being used in */
  enum VegasContextEnum {
    eVegasContextUnknown,
    eVegasContextMedia,
    eVegasContextTrack,
    eVegasContextEvent,
    eVegasContextEventFadeIn,
    eVegasContextEventFadeOut,
    eVegasContextProject,
    eVegasContextGenerator
    };

  /** @brief Enumerates the hint for vegas to display thumbnails */
  enum VegasPresetThumbnailEnum {
    eVegasPresetThumbnailDefault,
    eVegasPresetThumbnailSolidImage,
    eVegasPresetThumbnailImageWithAlpha
    };
#endif

  enum NativeOriginEnum {
    eNativeOriginBottomLeft,
    eNativeOriginTopLeft,
    eNativeOriginCenter
  };

#ifdef OFX_EXTENSIONS_NATRON

  enum ShortcutModifierEnum
  {
    eShortcutModifierNone = 0x0,
    eShortcutModifierShift = 0x1,
    eShortcutModifierCtrl = 0x2,
    eShortcutModifierAlt = 0x4,
    eShortcutModifierMeta = 0x8,
  };

#endif

  /** @brief turns a field string into and enum */
  FieldEnum mapStrToFieldEnum(const std::string &str)  throw(std::invalid_argument);

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief map a std::string to a RenderQuality */
  VegasRenderQualityEnum mapToVegasRenderQualityEnum(const std::string &s) throw(std::invalid_argument);

  /** @brief map a std::string to a context */
  VegasContextEnum mapToVegasContextEnum(const std::string &s) throw(std::invalid_argument);
#endif

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief map a std::string to a context enum */
  ContextEnum mapToContextEnum(const std::string &s) throw(std::invalid_argument);

  const char* mapContextEnumToStr(ContextEnum context) throw(std::invalid_argument);

  const char* mapMessageTypeEnumToStr(OFX::Message::MessageTypeEnum type);

  OFX::Message::MessageReplyEnum mapToMessageReplyEnum(OfxStatus stat);

  InstanceChangeReason mapToInstanceChangedReason(const std::string &s) throw(std::invalid_argument);

  BitDepthEnum mapStrToBitDepthEnum(const std::string &str) throw(std::invalid_argument);

  const char* mapBitDepthEnumToStr(BitDepthEnum bitDepth) throw(std::invalid_argument);

  PixelComponentEnum mapStrToPixelComponentEnum(const std::string &str) throw(std::invalid_argument);

  const char* mapPixelComponentEnumToStr(PixelComponentEnum pixelComponent) throw(std::invalid_argument);

#if defined(OFX_EXTENSIONS_NATRON)
  /** @brief extract layer name (first element) and channel names (other elements) from the kOfxImageEffectPropComponents property value, @see getPixelComponentsProperty() */
  std::vector<std::string> mapPixelComponentCustomToLayerChannels(const std::string& comp);
#endif

  class PluginFactory
  {
  public:
    /** @brief virtual destructor */
    virtual ~PluginFactory() {};
    virtual void load() {}
    virtual void unload() {}
    virtual void describe(OFX::ImageEffectDescriptor &desc) = 0;
    virtual void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context) = 0;
    virtual ImageEffect* createInstance(OfxImageEffectHandle handle, ContextEnum context) = 0;
    virtual const std::string& getID() const = 0;
    virtual const std::string& getUID() const = 0;
    virtual unsigned int getMajorVersion() const = 0;
    virtual unsigned int getMinorVersion() const = 0;
    virtual OfxPluginEntryPoint* getMainEntry() = 0;
  };

  template<class FACTORY>
  class FactoryMainEntryHelper
  {
  protected:
    const std::string& getHelperID() const { return _id; }
    unsigned int getHelperMajorVersion() const  { return  _maj; }
    unsigned int getHelperMinorVersion() const  { return  _min; }
    std::string toString(unsigned int val)
    {
      std::ostringstream ss;
      ss << val;
      return ss.str();
    }
    FactoryMainEntryHelper(const std::string& id, unsigned int maj, unsigned int min): _id(id), _maj(maj), _min(min)
    {
      assert(uid().empty()); // constructor should only be called once
      uid() = id + toString(maj) + toString(min);
    }
    const std::string& getHelperUID() const { return uid(); }
    static OfxStatus mainEntry(const char *action, const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out)
    { 
      return OFX::Private::mainEntryStr(action, handle, in, out, uid().c_str());
    }

  private:
    static std::string& uid() {
      // _uid can not be a static class member, because in that case it may be
      // initialized after the constructor is called (plugin factories are declared as global variables).

      // since only one instance of FactoryMainEntryHelp<FACTORY> is constructed
      // (see the assert() above), there is no deinitialization order issue.
      // see https://isocpp.org/wiki/faq/ctors#construct-on-first-use-v2
      static std::string _uid;
      return _uid;
    }
    std::string _id;
    unsigned int _maj;
    unsigned int _min;
  };

  template<class FACTORY>
  class PluginFactoryHelper : public FactoryMainEntryHelper<FACTORY>, public PluginFactory
  {
  public:
    PluginFactoryHelper(const std::string& id, unsigned int maj, unsigned int min): FactoryMainEntryHelper<FACTORY>(id, maj, min)
    {}
    OfxPluginEntryPoint* getMainEntry() { return FactoryMainEntryHelper<FACTORY>::mainEntry; }
    const std::string& getID() const { return FactoryMainEntryHelper<FACTORY>::getHelperID(); }
    const std::string& getUID() const { return FactoryMainEntryHelper<FACTORY>::getHelperUID(); }
    unsigned int getMajorVersion() const { return FactoryMainEntryHelper<FACTORY>::getHelperMajorVersion(); }
    unsigned int getMinorVersion() const { return FactoryMainEntryHelper<FACTORY>::getHelperMinorVersion(); }
  };

#define mDeclarePluginFactory(CLASS, LOADFUNCDEF, UNLOADFUNCDEF) \
  class CLASS : public OFX::PluginFactoryHelper<CLASS> \
  { \
  public: \
  CLASS(const std::string& id, unsigned int verMaj, unsigned int verMin):OFX::PluginFactoryHelper<CLASS>(id, verMaj, verMin){} \
  virtual void load() LOADFUNCDEF ;\
  virtual void unload() UNLOADFUNCDEF ;\
  virtual void describe(OFX::ImageEffectDescriptor &desc); \
  virtual void describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context); \
  virtual OFX::ImageEffect* createInstance(OfxImageEffectHandle handle, OFX::ContextEnum context); \
  };

  /** @brief Declare a plugin factory with a template majorVersion parameters.
   *
   * Allows an easy definition of multiple versions of the same plugin, for backward compatibility.
   * Keep in mind that the members of PluginFactoryHelper must be accessed through this->, 
   * because when a class template derives from a base class template, the base members are
   * not visible in the derived class template definition. (This makes sense; until you specialize,
   * there is no class, and so there are no members. Explicit specializations can always change 
   * the meaning of any given template class.)
   */
#define mDeclarePluginFactoryVersioned(CLASS, LOADFUNCDEF, UNLOADFUNCDEF) \
  template<unsigned int majorVersion> \
  class CLASS : public OFX::PluginFactoryHelper<CLASS<majorVersion> > \
  { \
  public: \
  CLASS<majorVersion>(const std::string& id, unsigned int verMin):OFX::PluginFactoryHelper<CLASS>(id, majorVersion, verMin){} \
  virtual void load() LOADFUNCDEF ;\
  virtual void unload() UNLOADFUNCDEF ;\
  virtual void describe(OFX::ImageEffectDescriptor &desc); \
  virtual void describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context); \
  virtual OFX::ImageEffect* createInstance(OfxImageEffectHandle handle, OFX::ContextEnum context); \
  };

  typedef std::vector<PluginFactory*> PluginFactoryArray;
  struct PluginFactories {
    static OFX::PluginFactoryArray& plugIDs() {
      // Use a static variable in a member function to make sure plugIDs is initialized before PluginFactories.
      // This is OK, since s_plugIDs is not used in the PluginFactories destructor.
      // see https://isocpp.org/wiki/faq/ctors#construct-on-first-use-v2
      static OFX::PluginFactoryArray _plugIDs;
      return _plugIDs;
    }
    PluginFactories(PluginFactory* pf) { plugIDs().push_back(pf); }
  };
#define mRegisterPluginFactoryInstance(pf) static OFX::PluginFactories mypluginfactory_##pf(&pf);

  /** @brief Fetch's a suite from the host and logs errors 

  All the standard suites are fetched by the support code, you should use this
  to fetch any extra non-standard suites.
  */
  const void * fetchSuite(const char *suiteName, int suiteVersion, bool optional = false);

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief A class that lists all the properties of a host */
  struct ImageEffectHostDescription {
  public :
    int APIVersionMajor;
    int APIVersionMinor;
    std::string hostName;
    std::string hostLabel;
    int versionMajor;
    int versionMinor;
    int versionMicro;
    std::string versionLabel;
    bool hostIsBackground;
    bool supportsOverlays;
    bool supportsMultiResolution;
    bool supportsTiles;
    bool temporalClipAccess;
    bool supportsMultipleClipDepths;
    bool supportsMultipleClipPARs;
    bool supportsSetableFrameRate;
    bool supportsSetableFielding;
    int sequentialRender;
    bool supportsStringAnimation;
    bool supportsCustomInteract;
    bool supportsChoiceAnimation;
    bool supportsBooleanAnimation;
    bool supportsCustomAnimation;
    void* osHandle;
    bool supportsParametricParameter;
    bool supportsParametricAnimation;
    bool supportsRenderQualityDraft;
    NativeOriginEnum nativeOrigin;
#ifdef OFX_SUPPORTS_OPENGLRENDER
    bool supportsOpenGLRender;
#endif
#ifdef OFX_EXTENSIONS_NUKE
    bool supportsCameraParameter;
    bool canTransform;
    bool isMultiPlanar;
#endif
    int maxParameters;
    int maxPages;
    int pageRowCount;
    int pageColumnCount;
    typedef std::vector<PixelComponentEnum> PixelComponentArray;
    PixelComponentArray _supportedComponents;
    typedef std::vector<ContextEnum> ContextArray;
    ContextArray _supportedContexts;
    typedef std::vector<BitDepthEnum> PixelDepthArray;
    PixelDepthArray _supportedPixelDepths;
    bool supportsProgressSuite;
    bool supportsTimeLineSuite;
    bool supportsMessageSuiteV2;
#ifdef OFX_EXTENSIONS_NATRON
    bool isNatron;
    bool supportsDynamicChoices;
    bool supportsCascadingChoices;
    bool supportsChannelSelector;
    bool canDistort;

    struct NativeOverlayHandle
    {
      // Uniquely identifies the overlay handle in the host
      std::string identifier;

      // A vector of parameters, where each parameter contains a pair <parameterHint, parameterType>
      // The parameterHint is a hint of the host as to what kind of parameter should be used for that role
      // The parameterType allows the host to contrain which parameter type should be passed to the native overlay handle
      std::vector<std::pair<std::string,std::string> > parameters;
    };
    std::vector<NativeOverlayHandle> nativeInteracts;
#endif

  public:
    bool supportsPixelComponent(const PixelComponentEnum component) const;
    bool supportsBitDepth( const BitDepthEnum bitDepth) const;
    bool supportsContext(const ContextEnum context) const;
	
    /** @return default pixel depth supported by host application. */
    BitDepthEnum getDefaultPixelDepth() const;
	
    /** @return default pixel component supported by host application. */
    PixelComponentEnum getDefaultPixelComponent() const;
  };


  /// retrieve the host description
  ImageEffectHostDescription* getImageEffectHostDescription();

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
    const PropertySet &getPropertySet() const {return _clipProps;}

    PropertySet &getPropertySet() {return _clipProps;}


    /** @brief set the label properties */
    void setLabel(const std::string &label);

    /** @brief set the label properties */
    void setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel);

#ifdef OFX_EXTENSIONS_NATRON
    /** @brief set the secretness of the clip, defaults to false */
    void setIsSecret(bool v);

    /** @brief set the clip hint */
    void setHint(const std::string &hint);

    /** @brief say whether this clip may contain images with a distortion function attached */
    void setCanDistort(bool v);
#endif

    /** @brief set how fielded images are extracted from the clip defaults to eFieldExtractDoubled */
    void setFieldExtraction(FieldExtractionEnum v);

    /** @brief set which components are supported, defaults to none set, this must be called at least once! */
    void addSupportedComponent(PixelComponentEnum v);

    /** @brief set which components are supported. This version adds by the raw C-string label, allowing you to add
    custom component types */
    void addSupportedComponent(const std::string &comp);

    /** @brief say whether we are going to do random temporal access on this clip, defaults to false */
    void setTemporalClipAccess(bool v);

    /** @brief say whether if the clip is optional, defaults to false */
    void setOptional(bool v);

    /** @brief say whether this clip supports tiling, defaults to true */
    void setSupportsTiles(bool v);

    /** @brief say whether this clip is a 'mask', so the host can know to replace with a roto or similar, defaults to false */
    void setIsMask(bool v);

#ifdef OFX_EXTENSIONS_NUKE
    /** @brief say whether this clip may contain images with a transform attached */
    void setCanTransform(bool v);
#endif
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an effect descriptor, used in the describe actions */
  class ImageEffectDescriptor : public ParamSetDescriptor
  {
  protected :
    mDeclareProtectedAssignAndCCBase(ImageEffectDescriptor,ParamSetDescriptor);
    ImageEffectDescriptor(void) {assert(false);}

  protected :
    /** @brief The effect handle */
    OfxImageEffectHandle _effectHandle;

    /** @brief properties for this clip */
    PropertySet _effectProps;

    /** @brief Set of all previously defined parameters, defined on demand */
    std::map<std::string, ClipDescriptor *> _definedClips;

    /** @brief Set of strings for clip preferences action (stored in here so the array persists and can be used in a property name)*/
    std::map<std::string, std::string> _clipComponentsPropNames;
    std::map<std::string, std::string> _clipDepthPropNames;
    std::map<std::string, std::string> _clipPARPropNames;
    std::map<std::string, std::string> _clipROIPropNames;
    std::map<std::string, std::string> _clipFrameRangePropNames;
#ifdef OFX_EXTENSIONS_NUKE
    std::map<std::string, std::string> _clipPlanesPropNames;
    std::map<std::string, std::string> _clipFrameViewsPropNames;
#endif

    std::auto_ptr<EffectOverlayDescriptor> _overlayDescriptor;
#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
    std::auto_ptr<HWNDInteractDescriptor>  _hwndInteractDescriptor;
#endif // #if defined(WIN32) || defined(WIN64)
#endif
  public :
    /** @brief ctor */
    ImageEffectDescriptor(OfxImageEffectHandle handle);

    /** @brief dtor */
    ~ImageEffectDescriptor();

    const PropertySet &getPropertySet() const {return _effectProps;}

    PropertySet &getPropertySet() {return _effectProps;}

    /** @brief, set the label properties in a plugin */
    void setLabel(const std::string &label);

    /** @brief, set the label properties in a plugin */
    void setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel);

    /** @brief, set the version properties in a plugin */
    void setVersion(int major, int minor, int micro, int build, const std::string &versionLabel);

    /** @brief Set the plugin grouping, defaults to "" */
    void setPluginGrouping(const std::string &group);

    /** @brief Set the plugin description, defaults to "" */
    void setPluginDescription(const std::string &description, bool validate = true);

    /** @brief Add a context to those supported, defaults to none, must be called at least once */
    void addSupportedContext(ContextEnum v);

    /** @brief Add a pixel depth to those supported, defaults to none, must be called at least once */
    void addSupportedBitDepth(BitDepthEnum v);

    /** @brief Add a pixel depth to those supported for OpenGL rendering, defaults to all */
    void addSupportedOpenGLBitDepth(BitDepthEnum v);

#ifdef OFX_EXTENSIONS_TUTTLE
    /** @brief Add a file extension to those supported, defaults to none */
    void addSupportedExtension(const std::string& extension);
    void addSupportedExtensions(const std::vector<std::string>& extensions);
    void addSupportedExtensions(const char* extensions[]); // NULL-terminated array of char*
    void setPluginEvaluation(double evaluation);
#endif

    /** @brief Is the plugin single instance only ? defaults to false */
    void setSingleInstance(bool v);

    /** @brief Does the plugin expect the host to perform per frame SMP threading defaults to true */
    void setHostFrameThreading(bool v);

    /** @brief Does the plugin support multi resolution images, defaults to true */
    void setSupportsMultiResolution(bool v);

    /** @brief set the instance to be sequentially renderred, this should have been part of clip preferences! */
    void setSequentialRender(bool v);

    /** @brief Have we informed the host we want to be seqentially renderred ? */
    bool getSequentialRender(void) const;

    /** @brief Does the plugin support image tiling, defaults to true */
    void setSupportsTiles(bool v);

    /** @brief Does the plugin handle render quality, defaults to false */
    void setSupportsRenderQuality(bool v);

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

#ifdef OFX_SUPPORTS_OPENGLRENDER
    /** @brief Does the plugin support OpenGL accelerated rendering (but is also capable of CPU rendering) ? */
    void setSupportsOpenGLRender(bool v);

    /** @brief Does the plugin require OpenGL accelerated rendering ? */
    void setNeedsOpenGLRender(bool v);
    void addOpenGLBitDepth(BitDepthEnum bitDepth);
#endif

#ifdef OFX_EXTENSIONS_VEGAS
    /** @brief Add a guid upgrade path, defaults to none, must be called at least once */
    void addVegasUpgradePath(const std::string &guidString);

    /** @brief sets the path to a help file, defaults to none, must be called at least once */
    void setHelpPath(const std::string &helpPathString);

    /** @brief sets the context ID to a help file if it's a .chm file, defaults to none, must be called at least once */
    void setHelpContextID(int helpContextID);

    void setPresetThumbnailHint(VegasPresetThumbnailEnum thumbnailHint);
#endif

#ifdef OFX_EXTENSIONS_NUKE
      /** @brief indicate that a plugin or host can handle transform effects */
      void setCanTransform(bool v);
      
      /** @brief Indicates that a host or plugin can fetch more than a type of image from a clip*/
      void setIsMultiPlanar(bool v);
      
      /** @brief Plugin indicates to the host that it should pass through any planes not modified by the plugin*/
      void setPassThroughForNotProcessedPlanes(PassThroughLevelEnum v);
      
      /** @brief Indicates to the host that the plugin is view aware, in which case it will have to use the view calls*/
      void setIsViewAware(bool v);
      
      /** @brief Indicates to the host that a view aware plugin produces the same image independent of the view being rendered*/
      void setIsViewInvariant(ViewInvarianceLevelEnum v);
#endif
      
#ifdef OFX_EXTENSIONS_NATRON
      /** @brief Indicates if the host may add a mask that will be handled automatically. */
      void setHostMaskingEnabled(bool enabled);
      
      /** @brief Indicates if the host may add a "Mix" double parameter that will dissolve
        between the source image at 0 and the full effect at 1. */
      void setHostMixingEnabled(bool enabled);

      /** @brief Indicates if the plug-in description is written in markdown or plain-text otherwise. */
      void setDescriptionIsMarkdown(bool markdown);

      /** @brief The current selection rectangle drawn by the user on the host viewport.

       This property is refreshed whenever calling the kOfxActionInstanceChanged action 
       for the parameter kNatronOfxParamSelectionRectangleState to let the plug-in a change
       to correctly synchronized its selection.
       */
      OfxRectI getSelectionRectangle();

      /** @brief Add a parameter that will be visible in a toolbar in the viewport of the host application. */
      void addInViewportParam(const std::string& paramName);

      /** @brief Add a shortcut for the given parameter and set its default value. The host could then display this shortcut in an editor. */
      void setDefaultParamInViewportShortcut(const std::string& paramName, int symbolKey, ShortcutModifierEnum modifiers);

      /** @brief Add a native overlay interact for the given parameters.
       
       This should be called in describeInContext().
       It sets the properties kOfxParamPropUseHostOverlayHandle on each parameter,
       and kNatronOfxPropNativeOverlays on the effect descriptor. */
      void addNativeOverlayInteractForParameters(const std::string& interactType, const std::list<ParamDescriptor*>& parameters);
#endif

    /** @brief Create a clip, only callable from describe in context

    The returned clip \em must not be deleted by the client code. This is all managed by the ImageEffectDescriptor itself.
    */
    ClipDescriptor *defineClip(const std::string &name);

    /** @brief Access to the string maps needed for runtime properties. Because the char array must persist after the call,
    we need these to be stored in the descriptor, which is only deleted on unload.*/

    const std::map<std::string, std::string>& getClipComponentPropNames() const { return _clipComponentsPropNames; }
    const std::map<std::string, std::string>& getClipDepthPropNames() const { return _clipDepthPropNames; }
    const std::map<std::string, std::string>& getClipPARPropNames() const { return _clipPARPropNames; }
    const std::map<std::string, std::string>& getClipROIPropNames() const { return _clipROIPropNames; }
    const std::map<std::string, std::string>& getClipFrameRangePropNames() const { return _clipFrameRangePropNames; }
#ifdef OFX_EXTENSIONS_NUKE
    const std::map<std::string, std::string>& getClipPlanesPropNames() const { return _clipPlanesPropNames; }
    const std::map<std::string, std::string>& getClipFrameViewsPropNames() const { return _clipFrameViewsPropNames; }
#endif
      
    /** @brief override this to create an interact for the effect */
    virtual void setOverlayInteractDescriptor(EffectOverlayDescriptor* desc);

#ifdef OFX_EXTENSIONS_VEGAS
#if defined(WIN32) || defined(WIN64)
    /** @brief override this to create an hwnd interact for the effect */
    virtual void setHWNDInteractDescriptor(HWNDInteractDescriptor* desc);
#endif // #if defined(WIN32) || defined(WIN64)
#endif
#ifdef OFX_EXTENSIONS_NATRON
  /** @brief indicate that a plugin or host can handle distortion function effects */
  void setCanDistort(bool v);

  /** @brief indicate if the host may add a channel selector */
  void setChannelSelector(PixelComponentEnum v);

  /** @brief indicate that the plugin is deprecated */
  void setIsDeprecated(bool v);
#endif
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an image */
  class ImageBase {
  protected :
    /** @brief the handle that holds this image */
    PropertySet _imageProps;

    /** @brief friend so we get access to ctor */
    //friend class Clip;

    PixelComponentEnum _pixelComponents;     /**< @brief get the components in the image */
    int       _pixelComponentCount;
    int       _rowBytes;                     /**< @brief the number of bytes per scanline */
    int       _pixelBytes;                   /**< @brief the number of bytes per pixel */
    BitDepthEnum _pixelDepth;                 /**< @brief get the pixel depth */
    PreMultiplicationEnum _preMultiplication; /**< @brief premultiplication on the image */
    OfxRectI  _regionOfDefinition;           /**< @brief the RoD in pixel coordinates, this may be more or less than the bounds! */
    OfxRectI  _bounds;                       /**< @brief the bounds on the pixel data */
    double    _pixelAspectRatio;             /**< @brief the pixel aspect ratio */
    FieldEnum _field;                        /**< @brief which field this represents */
    std::string _uniqueID;                   /**< @brief the unique ID of this image */
    OfxPointD _renderScale;                  /**< @brief any scaling factor applied to the image */
#ifdef OFX_EXTENSIONS_NUKE
    double _transform[9];                    /**< @brief a 2D transform to apply to the image */
    bool _transformIsIdentity;
#endif
#ifdef OFX_EXTENSIONS_NATRON
    OfxDistortionFunctionV1 _distortionFunction;
    const void* _distortionFunctionData;
#endif

  public :
    /** @brief ctor */
    ImageBase(OfxPropertySetHandle props);

    /** @brief dtor */
    virtual ~ImageBase();
      
    const PropertySet &getPropertySet() const {return _imageProps;}

    PropertySet &getPropertySet() {return _imageProps;}

    /** @brief get the pixel depth */
    BitDepthEnum getPixelDepth(void) const {return _pixelDepth;}

    /** @brief get the components in the image */
    PixelComponentEnum getPixelComponents(void) const { return _pixelComponents;}

    /** @brief get the number of components in the image */
    int getPixelComponentCount(void) const { return _pixelComponentCount; }

    /** @brief get the string representing the pixel components */
    std::string getPixelComponentsProperty(void) const { return _imageProps.propGetString(kOfxImageEffectPropComponents);}

    /** @brief premultiplication on the image */
    PreMultiplicationEnum getPreMultiplication(void) const { return _preMultiplication;}

    /** @brief get the scale factor that has been applied to this image */
    const OfxPointD& getRenderScale(void) const { return _renderScale;}

    /** @brief get the scale factor that has been applied to this image */
    double getPixelAspectRatio(void) const { return _pixelAspectRatio;}

    /** @brief get the region of definition (in pixel coordinates) of this image */
    const OfxRectI& getRegionOfDefinition(void) const { return _regionOfDefinition;}

    /** @brief get the bounds on the image data (in pixel coordinates) of this image */
    const OfxRectI& getBounds(void) const { return _bounds;}

    /** @brief get the row bytes, may be negative */
    int getRowBytes(void) const { return _rowBytes;}

    /** @brief get the number of bytes per pixel */
    int getPixelBytes(void) const { return _pixelBytes;}

    /** @brief get the fielding of this image */
    FieldEnum getField(void) const { return _field;}

    /** @brief the unique ID of this image */
    const std::string& getUniqueIdentifier(void) const { return _uniqueID;}

#ifdef OFX_EXTENSIONS_NUKE
    /** @brief the 2D transform attached to this image. */
    void getTransform(double t[9]) const { for (int i = 0; i < 9; ++i) { t[i] = _transform[i]; } }

    /** @brief is the transform identity? */
    bool getTransformIsIdentity() const { return _transformIsIdentity; }
#endif

#ifdef OFX_EXTENSIONS_NATRON
    /** @brief the 2D distortion function attached to this image. */
    OfxDistortionFunctionV1 getDistortionFunction(const void** distortionFunctionData) const {
      *distortionFunctionData = _distortionFunctionData;
      return _distortionFunction;
    }
#endif
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an image */
  class Image : public ImageBase {
  protected :
    void     *_pixelData;                    /**< @brief the base address of the image */

  public :
    /** @brief ctor */
    Image(OfxPropertySetHandle props);

    /** @brief dtor */
    virtual ~Image();

    /** @brief get the pixel data for this image */
    void *getPixelData(void) { return _pixelData;}

    /** @brief get the pixel data for this image */
    const void *getPixelData(void) const { return _pixelData;}

    /** @brief return a pixel pointer, returns NULL if (x,y) is outside the image bounds

    x and y are in pixel coordinates

    If the components are custom, then this will return NULL as the support code
    can't know the pixel size to do the work.
    */
    void *getPixelAddress(int x, int y);

    /** @brief return a pixel pointer, returns NULL if (x,y) is outside the image bounds

    x and y are in pixel coordinates

    If the components are custom, then this will return NULL as the support code
    can't know the pixel size to do the work.
    */
    const void *getPixelAddress(int x, int y) const;

    /** @brief return a pixel pointer, returns the nearest pixel if (x,y) is outside the image bounds

    x and y are in pixel coordinates

    If the components are custom, then this will return NULL as the support code
    can't know the pixel size to do the work.
    */
    void *getPixelAddressNearest(int x, int y);

    /** @brief return a pixel pointer, returns the nearest pixel if (x,y) is outside the image bounds

    x and y are in pixel coordinates

    If the components are custom, then this will return NULL as the support code
    can't know the pixel size to do the work.
    */
    const void *getPixelAddressNearest(int x, int y) const;
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an OpenGL texture */
  class Texture : public ImageBase {
  protected :
    int _index;
    int _target;

  public :
    /** @brief ctor */
    Texture(OfxPropertySetHandle props);

    /** @brief dtor */
    virtual ~Texture();

    /** @brief get OpenGL texture id (cast to GLuint) */
    inline int getIndex() const {return _index;}
      
    /** @brief get OpenGL texture target (cast to GLenum) */
    inline int getTarget() const {return _target;}
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

#ifdef OFX_EXTENSIONS_VEGAS
    PixelOrderEnum _pixelOrder;              /**< @brief the pixel order */
#endif

  public :
    /// get the underlying property set on this clip
    const PropertySet &getPropertySet() const {return _clipProps;}

    /// get the underlying property set on this clip
    PropertySet &getPropertySet() {return _clipProps;}

#ifdef OFX_EXTENSIONS_NATRON
    /** @brief set the label property in a clip */
    void setLabel(const std::string &label);

    /** @brief set the label properties in a clip */
    void setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel);

    /** @brief set the secretness of the clip, defaults to false */
    void setIsSecret(bool v);

    /** @brief set the clip hint */
    void setHint(const std::string &hint);

    /* @brief Get the clip format in pixel coordinates */
    void getFormat(OfxRectI &format) const;
#endif

    /// get the OFX clip handle
    OfxImageClipHandle getHandle() {return _clipHandle;}

    /** @brief get the name */
    const std::string &name(void) const {return _clipName;}

    /** @brief fetch the label */
    void getLabel(std::string &label) const;

    /** @brief fetch the labels */
    void getLabels(std::string &label, std::string &shortLabel, std::string &longLabel) const;

    /** @brief what is the pixel depth images will be given to us as */
    BitDepthEnum getPixelDepth(void) const;

    /** @brief what is the components images will be given to us as */
    PixelComponentEnum getPixelComponents(void) const;

    /** @brief get the number of components in the image */
    int getPixelComponentCount(void) const;

    /** @brief get the string representing the pixel components */
    std::string getPixelComponentsProperty(void) const { return _clipProps.propGetString(kOfxImageEffectPropComponents);}

    /** @brief what is the actual pixel depth of the clip */
    BitDepthEnum getUnmappedPixelDepth(void) const;

    /** @brief what is the component type of the clip */
    PixelComponentEnum getUnmappedPixelComponents(void) const;

    /** @brief get the string representing the pixel components */
    std::string getUnmappedPixelComponentsProperty(void) const { return _clipProps.propGetString(kOfxImageClipPropUnmappedComponents);}

    /** @brief get the components in the image */
    PreMultiplicationEnum getPreMultiplication(void) const;

    /** @brief which spatial field comes first temporally */
    FieldEnum getFieldOrder(void) const;

    /** @brief is the clip connected */
    bool isConnected(void) const;

    /** @brief can the clip be continuously sampled */
    bool hasContinuousSamples(void) const;

    /** @brief get the scale factor that has been applied to this clip */
    double getPixelAspectRatio(void) const;

    /** @brief get the frame rate, in frames per second on this clip, after any clip preferences have been applied */
    double getFrameRate(void) const;

    /** @brief return the range of frames over which this clip has images, after any clip preferences have been applied */
    OfxRangeD getFrameRange(void) const;

    /** @brief get the frame rate, in frames per second on this clip, before any clip preferences have been applied */
    double getUnmappedFrameRate(void) const;

    /** @brief return the range of frames over which this clip has images, before any clip preferences have been applied */
    OfxRangeD getUnmappedFrameRange(void) const;

    /** @brief get the RoD for this clip in the cannonical coordinate system */
    OfxRectD getRegionOfDefinition(double t);

#ifdef OFX_EXTENSIONS_NUKE
    
    /** @brief get the RoD for this clip in the cannonical coordinate system for the given view */
    OfxRectD getRegionOfDefinition(double t, int view);
      
    /** @brief fetch an image for the given plane and view
       
    When finished with, the client code must delete the image.
    
    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image* fetchImagePlane(double t, int view, const char* plane);
      
    /** @brief fetch an image plane, with a specific region in cannonical coordinates
       
    When finished with, the client code must delete the image.
    
    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image* fetchImagePlane(double t, int view, const char* plane, const OfxRectD& bounds);
      
    /** @brief fetch an image for the given plane and view
       
    When finished with, the client code must delete the image.
    
    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image* fetchImagePlane(double t, const char* plane);
      
    /** @brief fetch an image plane, with a specific region in cannonical coordinates
       
    When finished with, the client code must delete the image.
    
    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image* fetchImagePlane(double t, const char* plane, const OfxRectD& bounds);

    /** @brief Property set indicating the components present on something*/
    void getComponentsPresent(std::vector<std::string>* components) const;
      
#endif
      
#ifdef OFX_EXTENSIONS_VEGAS
    /** @brief get the pixel order of this image */
    PixelOrderEnum getPixelOrder(void) const;
#endif

    /** @brief fetch an image

    When finished with, the client code must delete the image.

    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image *fetchImage(double t);

#if defined(OFX_EXTENSIONS_VEGAS) || defined(OFX_EXTENSIONS_NUKE)
    /** @brief fetch an image

    When finished with, the client code must delete the image.

    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image *fetchStereoscopicImage(double t, int view);
#endif

    /** @brief fetch an image, with a specific region in cannonical coordinates

    When finished with, the client code must delete the image.

    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image *fetchImage(double t, const OfxRectD &bounds);

    /** @brief fetch an image, with a specific region in cannonical coordinates

    When finished with, the client code must delete the image.

    If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
    */
    Image *fetchImage(double t, const OfxRectD *bounds)
    {
      if(bounds) 
        return fetchImage(t, *bounds);
      else
        return fetchImage(t);
    }

#ifdef OFX_SUPPORTS_OPENGLRENDER
    Texture *loadTexture(double t, BitDepthEnum format = eBitDepthNone, const OfxRectD *region = NULL);
#endif
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Class that skins image memory allocation */
  class ImageMemory {
  protected :
    OfxImageMemoryHandle _handle;

  public :
    /** @brief ctor */
    ImageMemory(size_t nBytes, ImageEffect *associatedEffect = 0);

    /** @brief dtor */
    ~ImageMemory();

    /** @brief lock the memory and return a pointer to it */
    void *lock(void);

    /** @brief unlock the memory */
    void unlock(void);
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief POD struct to pass rendering arguments into @ref ImageEffect::render */
  struct RenderArguments {
    double    time;
    OfxPointD renderScale;
    OfxRectI  renderWindow;
    FieldEnum fieldToRender;
#ifdef OFX_SUPPORTS_OPENGLRENDER
    bool      openGLEnabled;
#ifdef OFX_EXTENSIONS_NATRON
    void*     openGLContextData;
#endif
#endif
#ifdef OFX_EXTENSIONS_VEGAS
    int         viewsToRender;      /// default is 1, for stereoscopic 3d: 2
#endif
#if defined(OFX_EXTENSIONS_VEGAS) || defined(OFX_EXTENSIONS_NUKE)
    int         renderView;         /// default is 0, for s3d left eye: 0, right eye: 1
#endif
#ifdef OFX_EXTENSIONS_VEGAS
    VegasRenderQualityEnum renderQuality;
#endif
    bool      sequentialRenderStatus;
    bool      interactiveRenderStatus;
    bool      renderQualityDraft;
#ifdef OFX_EXTENSIONS_NUKE
    std::list<std::string> planes;
#endif
  };

  /** @brief POD struct to pass rendering arguments into @ref OFX::ImageEffect::isIdentity */
  struct IsIdentityArguments {
    double    time;
    OfxPointD renderScale;
    OfxRectI  renderWindow;
    FieldEnum fieldToRender;
#ifdef OFX_EXTENSIONS_NUKE
    int view;
#endif
  };

  /** @brief POD struct to pass arguments into  @ref OFX::ImageEffect::beginSequenceRender */
  struct BeginSequenceRenderArguments {
    OfxRangeD frameRange;
    double    frameStep;
    bool      isInteractive;
    OfxPointD renderScale;
#ifdef OFX_SUPPORTS_OPENGLRENDER
    bool      openGLEnabled;
#ifdef OFX_EXTENSIONS_NATRON
    void*     openGLContextData;
#endif
#endif
    bool      sequentialRenderStatus;
    bool      interactiveRenderStatus;
    bool      renderQualityDraft;
#ifdef OFX_EXTENSIONS_NUKE
    int view;
#endif
  };

  /** @brief POD struct to pass arguments into  @ref OFX::ImageEffect::endSequenceRender */
  struct EndSequenceRenderArguments {
    bool      isInteractive;
    OfxPointD renderScale;
#ifdef OFX_SUPPORTS_OPENGLRENDER
    bool      openGLEnabled;
#ifdef OFX_EXTENSIONS_NATRON
    void*     openGLContextData;
#endif
#endif
    bool      sequentialRenderStatus;
    bool      interactiveRenderStatus;
    bool      renderQualityDraft;
#ifdef OFX_EXTENSIONS_NUKE
    int view;
#endif
  };

  /** @brief POD struct to pass arguments into  @ref OFX::ImageEffect::getRegionOfDefinition */
  struct RegionOfDefinitionArguments {
    double    time;
    OfxPointD renderScale;
#ifdef OFX_EXTENSIONS_NUKE
    int view;
#endif
  };

  /** @brief POD struct to pass arguments into @ref OFX::ImageEffect::getRegionsOfInterest */
  struct RegionsOfInterestArguments {
    double    time;
    OfxPointD renderScale;
    OfxRectD  regionOfInterest;
#ifdef OFX_EXTENSIONS_NUKE
    int view;
#endif
  };

#ifdef OFX_EXTENSIONS_VEGAS
  /** @brief POD struct to pass arguments into @ref OFX::ImageEffect::upliftVegasKeyframes */
  class SonyVegasUpliftArguments {
  protected:
      /** @brief properties for this clip */
      PropertySet _argProps;

  public:

      SonyVegasUpliftArguments(PropertySet args);

      std::string  guidUplift;
      int          keyframeCount;
      void*        commonData;
      int          commonDataSize;

      void*  getKeyframeData     (int keyframeIndex) const;
      int    getKeyframeDataSize (int keyframeIndex) const;
      double getKeyframeTime     (int keyframeIndex) const;
      VegasInterpolationEnum getKeyframeInterpolation (int keyframeIndex) const;
  };
#endif

#ifdef OFX_EXTENSIONS_NUKE
  /** @brief POD struct to pass arguments into @ref OFX::ImageEffect::getTransform */
  struct TransformArguments {
    double    time;
    OfxPointD renderScale;
    FieldEnum fieldToRender;
    int       renderView;
  };
#endif

#ifdef OFX_EXTENSIONS_NATRON
  /** @brief POD struct to pass arguments into @ref OFX::ImageEffect::getDistortion */
  struct DistortionArguments {
    double    time;
    OfxPointD renderScale;
    FieldEnum fieldToRender;
    int       renderView;
  };
#endif

  /** @brief Class used to set regions of interest on a clip in @ref OFX::ImageEffect::getRegionsOfInterest

  This is a base class, the actual class is private and you don't need to see the glue involved.
  */ 
  class RegionOfInterestSetter {
  public :
    /** @brief virtual destructor */
    virtual ~RegionOfInterestSetter() {}

    /** @brief function to set the RoI of a clip, pass in the clip to set the RoI of, and the RoI itself */
    virtual void setRegionOfInterest(const Clip &clip, const OfxRectD &RoI) = 0;
  };

  /** @brief POD struct to pass arguments into @ref OFX::ImageEffect::getFramesNeeded */
  struct FramesNeededArguments {
    double    time;
  };

  /** @brief Class used to set the frames needed to render a single frame of a clip in @ref OFX::ImageEffect::getFramesNeeded

  This is a base class, the actual class is private and you don't need to see the glue involved.
  */ 
  class FramesNeededSetter {
  public :
    /** @brief virtual destructor */
    virtual ~FramesNeededSetter() {}

    /** @brief function to set the frames needed on a clip, the range is min <= time <= max */
    virtual void setFramesNeeded(const Clip &clip, const OfxRangeD &range) = 0;
  };
    
#ifdef OFX_EXTENSIONS_NUKE
  struct ClipComponentsArguments {
    double time;
    int view;
  };
    
  class ClipComponentsSetter {
      
      OFX::PropertySet _outArgs;
      bool _doneSomething;
      typedef std::map<std::string, std::string> StringStringMap;
      const StringStringMap& _clipPlanesPropNames;
      std::map<std::string,std::vector<std::string> > _clipComponents;
      
      const std::string& extractValueForName(const StringStringMap& m, const std::string& name);
      
  public:
      
      ClipComponentsSetter(OFX::PropertySet props,
                           const StringStringMap& clipPlanesPropNames)
      : _outArgs(props)
      , _doneSomething(false)
      , _clipPlanesPropNames(clipPlanesPropNames)
      , _clipComponents()
      {
          
      }
      
      bool setOutProperties();
      
      void addClipComponents(Clip& clip, PixelComponentEnum comps);
      
      //Pass the raw-string, used by the ofxNatron.h extension
      void addClipComponents(Clip& clip, const std::string& comps);
      
      //Pass NULL into clip for non pass-through
      void setPassThroughClip(const Clip* clip,double time,int view);

  };
    
  struct FrameViewsNeededArguments {
      double time;
      int view;
  };
    
  class FrameViewsNeededSetter {
      OFX::PropertySet _outArgs;
      bool _doneSomething;
      typedef std::map<std::string, std::string> StringStringMap;
      const StringStringMap& _clipFrameViewsPropnames;
      
      // For each clip and for each view a vector of ranges
      std::map<std::string, std::map<int, std::vector<OfxRangeD> > > _frameViews;
      
      const std::string& extractValueForName(const StringStringMap& m, const std::string& name);
  public:
      
      FrameViewsNeededSetter(OFX::PropertySet props,
                             const StringStringMap& clipFrameViewsPropNames)
      : _outArgs(props)
      , _doneSomething(false)
      , _clipFrameViewsPropnames(clipFrameViewsPropNames)
      , _frameViews()
      {}
            
      bool setOutProperties();
      
      void addFrameViewsNeeded(const Clip& clip,const OfxRangeD &range, int view);
      
  };
#endif

  /** @brief Class used to set the clip preferences of the effect.
  */ 
  class ClipPreferencesSetter {
    OFX::PropertySet outArgs_;
    bool doneSomething_;
    typedef std::map<std::string, std::string> StringStringMap;
    const StringStringMap& clipDepthPropNames_;
    const StringStringMap& clipComponentPropNames_;
    const StringStringMap& clipPARPropNames_;
    const std::string& extractValueForName(const StringStringMap& m, const std::string& name);
  public :
    ClipPreferencesSetter( OFX::PropertySet props, 
      const StringStringMap& depthPropNames,
      const StringStringMap& componentPropNames,
      const StringStringMap& PARPropNames) 
      : outArgs_(props)
      , doneSomething_(false)
      , clipDepthPropNames_(depthPropNames)
      , clipComponentPropNames_(componentPropNames)
      , clipPARPropNames_(PARPropNames)
    {}

    bool didSomething(void) const {return doneSomething_;}

    /** @brief, force the host to set a clip's mapped component type to be \em comps. 

    Only callable on non optional clips in all contexts. Must set comps to be one of the types the effect says it supports on the given clip.

    See the OFX API documentation for the default values of this.
    */
    void setClipComponents(Clip &clip, PixelComponentEnum comps);

    /** @brief, force the host to set a clip's mapped bit depth be \em bitDepth

    Only callable if the OFX::ImageEffectHostDescription::supportsMultipleClipDepths is true.

    See the OFX API documentation for the default values of this.
    */
    void setClipBitDepth(Clip &clip, BitDepthEnum bitDepth);

    /** @brief, force the host to set a clip's mapped Pixel Aspect Ratio to be \em PAR

    Only callable if the OFX::ImageEffectHostDescription::supportsMultipleClipPARs is true.

    Default is up to the host, generally based on the input clips. 

    Not supported by most host applications.
    */
    void setPixelAspectRatio(Clip &clip, double PAR);

    /** @brief Allows an effect to change the output frame rate 

    Only callable if OFX::ImageEffectHostDescription::supportsSetableFrameRate is true.

    Default is controlled by the host, typically the framerate of the input clips.
    */
    void setOutputFrameRate(double v);

    /** @brief Set the premultiplication state of the output clip.

    Defaults to the premultiplication state of ???
    */
    void setOutputPremultiplication(PreMultiplicationEnum v);

    /** @brief Set whether the effect can be continuously sampled.

    Defaults to false. 
    */
    void setOutputHasContinuousSamples(bool v);

    /** @brief Sets whether the effect will produce different images in all frames, even if the no params or input images are varying (eg: a noise generator).

    Defaults to false.
    */
    void setOutputFrameVarying(bool v);

    /** @brief Sets the output fielding

    Default is host dependent, must be one of 
    - eFieldNone,  
    - eFieldLower, 
    - eFieldUpper  
    */
    void setOutputFielding(FieldEnum v);

#ifdef OFX_EXTENSIONS_NATRON
    /** @brief Sets the output format in pixel coordinates.
     Default to first non optional input clip format
     */
    void setOutputFormat(const OfxRectI& format);
#endif
  };

  /** @brief POD data structure passing in the instance changed args */
  struct InstanceChangedArgs {
    InstanceChangeReason reason;      /**< @brief why did it change */
    double               time;        /**< time of the change */
    OfxPointD            renderScale; /**< the renderscale on the instance */
  };

  /** @brief struct to pass arguments into @ref OFX::ImageEffect::interpolateCustomParam.
  It is non-POD (it contains std::string), but it is passed as const ref, so that does
  not matter */
  struct InterpolateCustomArgs {
    double      time;
    std::string value1;
    std::string value2;
    double      keytime1;
    double      keytime2;
    double      amount;
  };

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief Wraps up an effect instance, plugin implementations need to inherit from this */
  class ImageEffect : public ParamSet
  {
  protected :
    mDeclareProtectedAssignAndCCBase(ImageEffect,ParamSet);

  private :
    /** @brief to get access to the effect handle without exposing it generally via a function */
    friend class ImageMemory;

    /** @brief The effect handle */
    OfxImageEffectHandle _effectHandle;

    /** @brief properties for this clip */
    PropertySet _effectProps;

    /** @brief the context of the effect */
    ContextEnum _context;

    /** @brief Set of all previously defined parameters, defined on demand */
    std::map<std::string, Clip *> _fetchedClips;

    /** @brief the overlay interacts that are open on this image effect */
    std::list<OverlayInteract *> _overlayInteracts;

    /** @brief cached result of whether progress start succeeded. */
    bool _progressStartSuccess;
  public :
    /** @brief ctor */
    ImageEffect(OfxImageEffectHandle handle);

    /** @brief dtor */
    virtual ~ImageEffect();

    const PropertySet &getPropertySet() const {return _effectProps;}

    PropertySet &getPropertySet() {return _effectProps;}


    OfxImageEffectHandle getHandle(void) const {return _effectHandle;}

    /** @brief the context this effect was instantiate in */
    ContextEnum getContext(void) const;

#ifdef OFX_EXTENSIONS_VEGAS
    /** @brief the Vegas context this effect exists in */
    VegasContextEnum getVegasContext(void);
#endif

    /** @brief size of the project */
    OfxPointD getProjectSize(void) const;

    /** @brief origin of the project */
    OfxPointD getProjectOffset(void) const;

    /** @brief extent of the project */
    OfxPointD getProjectExtent(void) const;

    /** @brief pixel aspect ratio of the project */
    double getProjectPixelAspectRatio(void) const;

    /** @brief how long does the effect last */
    double getEffectDuration(void) const;

    /** @brief the frame rate of the project */
    double getFrameRate(void) const;

    /** @brief is the instance currently being interacted with */
    bool isInteractive(void) const;

    /** @brief set the instance to be sequentially renderred, this should have been part of clip preferences! */
    void setSequentialRender(bool v);

    /** @brief Have we informed the host we want to be seqentially renderred ? */
    bool getSequentialRender(void) const;

    /** @brief Does the plugin support image tiling ? Can only be called from changedParam or changedClip. */
    void setSupportsTiles(bool v);

    /** @brief Have we informed the host we support image tiling ? */
    bool getSupportsTiles(void) const;

    /** @brief get plugin file path */
    std::string getPluginFilePath(void) { return _effectProps.propGetString(kOfxPluginPropFilePath); }
    
#ifdef OFX_EXTENSIONS_NUKE
    /** @brief indicate that a plugin or host can handle transform effects */
    void setCanTransform(bool v);
    
    bool getCanTransform() const;
#endif

#ifdef OFX_EXTENSIONS_NATRON
    /** @brief indicate that a plugin or host can handle distortion function effects */
    void setCanDistort(bool v);

    bool getCanDistort() const;
#endif

#ifdef OFX_SUPPORTS_OPENGLRENDER
    /** @brief Does the plugin support OpenGL accelerated rendering (but is also capable of CPU rendering) ? Can only be called from changedParam or changedClip (OFX 1.4). */
    void setSupportsOpenGLRender(bool v);
#endif

    /** @brief notify host that the internal data structures need syncing back to parameters for persistence and so on.  This is reset by the host after calling SyncPrivateData. */
    void setParamSetNeedsSyncing();

    OFX::Message::MessageReplyEnum sendMessage(OFX::Message::MessageTypeEnum type, const std::string& id, const std::string& msg, bool throwIfMissing = false);

    OFX::Message::MessageReplyEnum setPersistentMessage(OFX::Message::MessageTypeEnum type, const std::string& id, const std::string& msg, bool throwIfMissing = false);
    OFX::Message::MessageReplyEnum clearPersistentMessage(bool throwIfMissing = false);

#ifdef OFX_SUPPORTS_DIALOG
    /** @brief Request the host to send a kOfxActionDialog to the plugin from its UI thread. */
    void requestDialog(OfxPropertySetHandle inArgs, void *instanceData);

    /** @brief Inform the host of redraw event so it can redraw itself */
    void notifyRedrawPending(OfxPropertySetHandle inArgs);
#endif

    /** @brief Fetch the named clip from this instance

    The returned clip \em must not be deleted by the client code. This is all managed by the ImageEffect itself.
    */
    Clip *fetchClip(const std::string &name);

#ifdef OFX_EXTENSIONS_NUKE
    /** @brief Fetch the named camera param from this instance

    The returned camera param \em must not be deleted by the client code. This is all managed by the ImageEffect itself.
    */
    CameraParam* fetchCameraParam(const std::string& name) const;
#endif

    /** @brief does the host want us to abort rendering? */
    bool abort(void) const;

    /** @brief adds a new interact to the set of interacts open on this effect */
    void addOverlayInteract(OverlayInteract *interact);

    /** @brief removes an interact to the set of interacts open on this effect */
    void removeOverlayInteract(OverlayInteract *interact);

    /** @brief force all overlays on this interact to be redrawn */
    void redrawOverlays(void);

#ifdef OFX_SUPPORTS_OPENGLRENDER
    bool flushOpenGLResources(void);
#endif

    ////////////////////////////////////////////////////////////////////////////////
    // these are actions that need to be overridden by a plugin that implements an effect host

    /** @brief The purge caches action, a request for an instance to free up as much memory as possible in low memory situations */
    virtual void purgeCaches(void);

    /** @brief The sync private data action, called when the effect needs to sync any private data to persistent parameters */
    virtual void syncPrivateData(void);

    /** @brief client render function, this is one of the few that must be overridden */
    virtual void render(const RenderArguments &args) = 0;

    /** @brief client begin sequence render function */
    virtual void beginSequenceRender(const BeginSequenceRenderArguments &args); 

    /** @brief client end sequence render function */
    virtual void endSequenceRender(const EndSequenceRenderArguments &args); 

    /** @brief client is identity function, returns the clip and time for the identity function 

    If the effect would do no processing for the given param set and render arguments, then this
    function should return true and set the \em identityClip pointer to point to the clip that is the identity
    and \em identityTime to be the time at which to access the clip for the identity operation.
    */
    virtual bool isIdentity(const IsIdentityArguments &args, Clip * &identityClip, double &identityTime);

    /** @brief The get RoD action. 

    If the effect wants change the rod from the default value (which is the union of RoD's of all input clips)
    it should set the \em rod argument and return true.

    This is all in cannonical coordinates.
    */
    virtual bool getRegionOfDefinition(const RegionOfDefinitionArguments &args, OfxRectD &rod);

    /** @brief the get region of interest action

    If the effect wants change its region of interest on any input clip from the default values (which is the same as the RoI in the arguments)
    it should do so by calling the OFX::RegionOfInterestSetter::setRegionOfInterest function on the \em rois argument.

    Note, everything is in \em cannonical \em coordinates.
    */
    virtual void getRegionsOfInterest(const RegionsOfInterestArguments &args, RegionOfInterestSetter &rois);

    /** @brief the get frames needed action

    If the effect wants change the frames needed on an input clip from the default values (which is the same as the frame to be renderred)
    it should do so by calling the OFX::FramesNeededSetter::setFramesNeeded function on the \em frames argument.
    */
    virtual void getFramesNeeded(const FramesNeededArguments &args, FramesNeededSetter &frames);

    /** @brief get the clip preferences */
    virtual void getClipPreferences(ClipPreferencesSetter &clipPreferences);
      
    /** @brief the effect is about to be actively edited by a user, called when the first user interface is opened on an instance */
    virtual void beginEdit(void);

    /** @brief the effect is no longer being edited by a user, called when the last user interface is closed on an instance */
    virtual void endEdit(void);

    /** @brief the effect is about to have some values changed */
    virtual void beginChanged(InstanceChangeReason reason);

    /** @brief called when a param has just had its value changed */
    virtual void changedParam(const InstanceChangedArgs &args, const std::string &paramName);

    /** @brief called when a clip has just been changed in some way (a rewire maybe) */
    virtual void changedClip(const InstanceChangedArgs &args, const std::string &clipName);

    /** @brief the effect has just had some values changed */
    virtual void endChanged(InstanceChangeReason reason);

#ifdef OFX_SUPPORTS_DIALOG
    /** @brief called in the host's UI thread after a plugin has requested a dialog @see requestDialog() */
    virtual void dialog(void *userData);
#endif

#ifdef OFX_EXTENSIONS_VEGAS
    /** @brief Vegas requires conversion of keyframe data */
    virtual void upliftVegasKeyframes(const SonyVegasUpliftArguments &upliftInfo);

    /** @brief Vegas invokes about dialog */
    virtual bool invokeAbout();

    /** @brief Vegas invokes help dialog */
    virtual bool invokeHelp();
#endif

#ifdef OFX_EXTENSIONS_NUKE
    /** @brief get the needed input components and produced output components*/
    virtual void getClipComponents(const ClipComponentsArguments& args, ClipComponentsSetter& clipComponents);
      
    /** @brief get the frame/views needed for input clips*/
    virtual void getFrameViewsNeeded(const FrameViewsNeededArguments& args, FrameViewsNeededSetter& frameViews);

    /** @brief recover a transform matrix from an effect */
    virtual bool getTransform(const TransformArguments &args, Clip * &transformClip, double transformMatrix[9]);
      
    /** @brief Returns the textual representation of a view*/
    std::string getViewName(int viewIndex) const;
    
    /** @brief Returns the number of views*/
    int getViewCount() const;
#endif

#ifdef OFX_EXTENSIONS_NATRON
    /** @brief Implement if you effect can apply a 2D distortion.
     In the generic form, the distortion function pointer must be set and a pointer to the data that should be passed back
     to the function. A free function must also be provided to free the data once the host does not need them any more.
     This let a chance to the host to concatenate distortion effects together filter only once.
     @param distortionFunctionDataSizeHintInBytes This should indicate the size in bytes of the data held by distortionFunctionData.
     Since distortionFunctionData may contain the result of heavy computations (such as a STMap), the host will attempt to cache these data.
     However the void* does not indicate much to the host as to "how heavy" these datas are in its cache, so this parameter should hint
     the host of the size of these datas in bytes.

     If the effect distortion can be represented as a 3x3 matrix, then leave the function pointer to NULL and fill the transform matrix.
     This will enable the host to better concatenate the distortion in such cases where 3x3 matrices can be multiplied together instead
     of multiplying the transformation matrices for each pixel.
     */
    virtual bool getDistortion(const DistortionArguments &args, Clip * &transformClip, double transformMatrix[9],
                               OfxDistortionFunctionV1* distortionFunction,
                               void** distortionFunctionData,
                               int* distortionFunctionDataSizeHintInBytes,
                               OfxDistortionFreeDataFunctionV1* freeDataFunction);
#endif

    /** @brief called when a custom param needs to be interpolated */
    virtual std::string interpolateCustomParam(const InterpolateCustomArgs &args, const std::string &paramName);

    /** @brief what is the time domain of this effect, valid only in the general context

    return true is range was set, otherwise the default (the union of the time domain of all input clips) is used
    */
    virtual bool getTimeDomain(OfxRangeD &range);

#ifdef OFX_SUPPORTS_OPENGLRENDER
    /** @brief OpenGL context attached (returns context-specific data if createContextData, or NULL if the plugin does not support multiple contexts) */
    virtual void* contextAttached(bool createContextData);

    /** @brief OpenGL context detached */
    virtual void contextDetached(void* contextData);
#endif

    /// Start doing progress.
    void progressStart(const std::string &message, const std::string &messageid = "");

    /// finish yer progress
    void progressEnd();

    /// set the progress to some level of completion, returns
    /// false if you should abandon processing, true to continue
    bool progressUpdate(double t);   

    /// get the current time on the timeline. This is not necessarily the same
    /// time as being passed to an action (eg render)
    double timeLineGetTime();

    /// set the timeline to a specific time
    void timeLineGotoTime(double t);

    /// get the first and last times available on the effect's timeline
    void timeLineGetBounds(double &t1, double &t2);  
  };  


  ////////////////////////////////////////////////////////////////////////////////
  /** @brief The OFX::Plugin namespace. All the functions in here needs to be defined by each plugin that uses the support libs.
  */  
  namespace Plugin {
    /** @brief Plugin side function used to identify the plugin to the support library.
     
     This was obsoleted by automatic plugin registration, using the macro mRegisterPluginFactoryInstance:
     static BasicExamplePluginFactory p("net.sf.openfx.basicPlugin", 1, 0);
     mRegisterPluginFactoryInstance(p)
     */
    void getPluginIDs(OFX::PluginFactoryArray &id);

    /// If the client has defined its own exception type, allow it to catch it in the main function
#ifdef OFX_CLIENT_EXCEPTION_TYPE
    OfxStatus catchException(OFX_CLIENT_EXCEPTION_TYPE &ex);
#endif
  };

};

// undeclare the protected assign and CC macro
#undef mDeclareProtectedAssignAndCC
#undef mDeclareProtectedAssignAndCCBase

#endif
