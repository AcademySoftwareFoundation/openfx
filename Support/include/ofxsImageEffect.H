#ifndef _ofxsImageEffect_H_
#define _ofxsImageEffect_H_
/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004-2005 The Foundry Visionmongers Ltd
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
#include "ofxsParam.H"
#include "ofxsInteract.H"

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
    class ImageMemory;

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

    /** @brief Global description of the host */
    extern ImageEffectHostDescription *gHostDescription;

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

        /** @brief Create a clip, only callable from describe in context 
        
        The returned clip \em must not be deleted by the client code. This is all managed by the ImageEffectDescriptor itself.
        */
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

        void     *_pixelData;                    /**< @brief the base address of the image */
        PixelComponentEnum _pixelComponents;     /**< @brief get the components in the image */
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

    public :
        /** @brief dtor */
        virtual ~Image();

        /** @brief get the pixel depth */
        BitDepthEnum pixelDepth(void) const {return _pixelDepth;}

        /** @brief get the components in the image */
        PixelComponentEnum pixelComponents(void) const { return _pixelComponents;}

        /** @brief premultiplication on the image */
        PreMultiplicationEnum preMultiplication(void) const { return _preMultiplication;}
    
        /** @brief get the scale factor that has been applied to this image */
        OfxPointD renderScale(void) const { return _renderScale;}

        /** @brief get the scale factor that has been applied to this image */
        double pixelAspectRatio(void) const { return _pixelAspectRatio;}

        /** @brief get the pixel data for this image */
        void *pixelData(void) const { return _pixelData;}

        /** @brief get the region of definition (in pixel coordinates) of this image */
        OfxRectI regionOfDefinition(void) const { return _regionOfDefinition;}

        /** @brief get the bounds on the image data (in pixel coordinates) of this image */
        OfxRectI bounds(void) const { return _bounds;}

        /** @brief get the row bytes, may be negative */
        int rowBytes(void) const { return _rowBytes;}

        /** @brief get the fielding of this image */
        FieldEnum field(void) const { return _field;}

        /** @brief the unique ID of this image */
        std::string uniqueIdentifier(void) const { return _uniqueID;}

        /** @brief return a pixel pointer, returns NULL if (x,y) is outside the image bounds
        
        x and y are in pixel coordinates
        */
        void *pixelAddress(int x, int y);
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

        /** @brief fetch an image
        
        When finished with, the client code must delete the image.

        If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
        */
        Image *fetchImage(double t);

        /** @brief fetch an image, with a specific region in cannonical coordinates
        
        When finished with, the client code must delete the image.

        If the same image is fetched twice, it must be deleted in each case, they will not be the same pointer.
        */
        Image *fetchImage(double t, OfxRectD bounds);
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
    /** @brief POD struct to pass rendering arguments into @ref ImageEffect::render and @ref OFX::ImageEffect::isIdentity */
    struct RenderArguments {
        double    time;
        OfxPointD renderScale;
        OfxRectI  renderWindow;
        FieldEnum fieldToRender;
    };

    /** @brief POD struct to pass arguments into  @ref OFX::ImageEffect::render */
    struct BeginSequenceRenderArguments {
        OfxRangeD frameRange;
        double    frameStep;
        bool      isInteractive;
        OfxPointD renderScale;
    };

    /** @brief POD struct to pass arguments into  @ref OFX::ImageEffect::beginSequenceRender */
    struct EndSequenceRenderArguments {
        bool      isInteractive;
        OfxPointD renderScale;
    };

    /** @brief POD struct to pass arguments into  @ref OFX::ImageEffect::getRegionOfDefinition */
    struct RegionOfDefinitionArguments {
        double    time;
        OfxPointD renderScale;
    };

    /** @brief POD struct to pass arguments into @ref OFX::ImageEffect::getRegionsOfInterest */
    struct RegionsOfInterestArguments {
        double    time;
        OfxPointD renderScale;
        OfxRectD  regionOfInterest;
    };

    /** @brief Class used to set regions of interest on a clip in @ref OFX::ImageEffect::getRegionsOfInterest

    This is a base class, the actual class is private and you don't need to see the glue involved.
    */ 
    class RegionOfInterestSetter {
    public :
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
        /** @brief function to set the frames needed on a clip, the range is min <= time <= max */
        virtual void setFramesNeeded(const Clip &clip, const OfxRangeD &range) = 0;
    };

    /** @brief Class used to set the clip preferences of the effect.
    */ 
    class ClipPreferencesSetter {
        OFX::PropertySet outArgs_;
        bool doneSomething_;
    public :
        ClipPreferencesSetter( OFX::PropertySet props) 
          : outArgs_(props)
          , doneSomething_(false)
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
        
        /** @brief Set whether the effect can be continously sampled.
        
        Defaults to false. 
        */
        void setOutputHasContinousSamples(bool v);

        /** @brief Sets whether the effect will produce different images in all frames, even if the no params or input images are varying (eg: a noise generator).

        Defaults to false.
        */
        void setOutputFrameVarying(bool v);
    };

    /** @brief POD data structure passing in the instance changed args */
    struct InstanceChangedArgs {
        InstanceChangeReason reason;      /**< @brief why did it change */
        double               time;        /**< time of the change */
        OfxPointD            renderScale; /**< the renderscale on the instance */
    };

    ////////////////////////////////////////////////////////////////////////////////
    /** @brief Wraps up an effect instance, plugin implementations need to inherit from this */
    class ImageEffect : public ParamSet
    {
    protected :
        mDeclareProtectedAssignAndCC(ImageEffect);

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
        
    public :
        /** @brief ctor */
        ImageEffect(OfxImageEffectHandle handle);

        /** @brief dtor */
        virtual ~ImageEffect();

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
    
        /** @brief Fetch the named clip from this instance

        The returned clip \em must not be deleted by the client code. This is all managed by the ImageEffect itself.
        */
        Clip *fetchClip(const std::string &name);

        /** @brief does the host want us to abort rendering? */
        bool abort(void) const;

        /** @brief adds a new interact to the set of interacts open on this effect */
        void addOverlayInteract(OverlayInteract *interact);

        /** @brief removes an interact to the set of interacts open on this effect */
        void removeOverlayInteract(OverlayInteract *interact);

        /** @brief force all overlays on this interact to be redrawn */
        void redrawOverlays(void);

        ////////////////////////////////////////////////////////////////////////////////
        // these are actions that need to be overridden by a plugin that implements an effect host

        /** @brief The purge caches action, a request for an instance to free up as much memory as possible in low memory situations */
        virtual void purgeCaches(void);

        /** @brief The sync private data action, called when the effect needs to sync any private data to persistant parameters */
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
        virtual bool isIdentity(const RenderArguments &args, Clip * &identityClip, double &identityTime);

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

        /** @brief what is the time domain of this effect, valid only in the general context

        return true is range was set, otherwise the default (the union of the time domain of all input clips) is used
        */
        virtual bool getTimeDomain(OfxRangeD &range);

        /** @brief override this to create an interact for the effect */
        virtual OverlayInteract *createOverlayInteract(OfxInteractHandle handle);
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

        /** @brief The describe function, passed a plugin descriptor */
        void describe(OFX::ImageEffectDescriptor &desc);

        /** @brief The describe in context function, passed a plugin descriptor and a context */
        void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context);

        /** @brief The create instance function, the plugin must return an object derived from the \ref OFX::ImageEffect class */
        ImageEffect *createInstance(OfxImageEffectHandle handle, ContextEnum context);
    };

};

// undeclare the protected assign and CC macro
#undef mDeclareProtectedAssignAndCC

#endif
