#ifndef OFX_IMAGE_EFFECT_H
#define OFX_IMAGE_EFFECT_H

#include "ofxCore.h"
#include "ofxImageEffect.h"

#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"

#if defined(WIN32) || defined(WIN64)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace OFX {

  namespace Host {

    // forward declare    
    class Plugin;

    namespace ImageEffect {

      // forward declare
      class ImageEffectPlugin;
      class Instance;
      class Descriptor;

      // this is the entry point to the OFX Host support library, this pointer is passed
      // into the newInstance function that also needs to be implemented.
      extern void* gNewInstancePtr;

     // implement a function OFX::Host::ImageEffect::newInstance to make new instances
      // of effects
      extern ImageEffect::Instance* newInstance(void* ptr,
                                                ImageEffectPlugin* plugin,
                                                Descriptor& desc,
                                                const std::string& context);


      class Base {
      protected:        
        Property::Set   _properties;

      public:
        Base(const Property::Set &set);
        Base(const Property::PropSpec * propSpec);
        virtual ~Base();

        /// obtain a handle on this for passing to the C api
        OfxImageEffectHandle getHandle();

        /// get the properties set
        Property::Set &getProps();

        /// name of the clip
        const std::string &getShortLabel() const;
        
        /// name of the clip
        const std::string &getLabel() const;
        
        /// name of the clip
        const std::string &getLongLabel() const;

        /// is the given context supported
        bool isContextSupported(const std::string &s) const;

        /// what is the name of the group the plug-in belongs to
        const std::string &getPluginGrouping() const;

        /// is the effect single instance
        bool isSingleInstance() const;

        /// what is the thread safety on this effect
        const std::string &getRenderThreadSaftey() const;

        /// should the host attempt to managed multi-threaded rendering if it can
        /// via tiling or some such
        bool getHostFrameThreading() const;

        /// get the overlay interact main entry if it exists
        OfxPluginEntryPoint *getOverlayInteractMainEntry() const;

        /// does the effect support images of differing sizes
        bool supportsMultiResolution() const;

        /// does the effect support tiled rendering
        bool supportsTiles() const;

        /// does this effect need random temporal access
        bool temporalAccess() const;

        /// is the given RGBA/A pixel depth supported by the effect
        bool isPixelDepthSupported(const std::string &s) const;

        /// when field rendering, does the effect need to be called
        /// twice to render a frame in all circumstances (with different fields)
        bool fieldRenderTwiceAlways() const;
        
        /// does the effect support multiple clip depths
        bool supportsMultipleClipDepths() const;
        
        /// does the effect support multiple clip pixel aspect ratios
        bool supportsMultipleClipPARs() const;
        
        /// does changing the named param re-tigger a clip preferences action
        bool isClipPreferencesSlaveParam(const std::string &s) const;
      };

      /// an image effect plugin descriptor
      class Descriptor : public Base {
      protected:
        std::map<std::string, Clip::Descriptor*>  _clips; ///< clips descriptors by name
        std::vector<Clip::Descriptor*>            _clipsByOrder; ///< clip descriptors in order of declaration
        Param::SetDescriptor                      _params;
      public:

        Descriptor(const Descriptor &other);

        /// constructor
        Descriptor(Plugin *plug);        
        Descriptor(const std::string &bundlePath);

        /// get the parameters set
        Param::SetDescriptor &getParams();

        /// create a new clip and add this to the clip map
        Clip::Descriptor *defineClip(const std::string &name);

        /// get the clips
        std::map<std::string, Clip::Descriptor*> &getClips();

        /// add a new clip
        void addClip(const std::string &name, Clip::Descriptor *clip);

        /// get the clips in order of construction
        const std::vector<Clip::Descriptor*> &getClipsByOrder() 
        {
          return _clipsByOrder;
        }
      };      

      /// a map used to specify needed frame ranges on set of clips
      typedef std::map<Clip::Instance *, std::vector<OfxRangeD> > RangeMap;

      /// an image effect plugin descriptor
      class Instance : public Base,
                       private Property::NotifyHook, 
                       private Property::GetHook
      {
      protected:
        OFX::Host::ImageEffect::ImageEffectPlugin    *_plugin;
        std::string                                   _context;
        Descriptor                                   *_descriptor;
        std::map<std::string, Clip::Instance*>        _clips;
        Param::SetInstance                           *_params;
        bool                                          _interactive;
        bool                                          _created;
        bool                                          _continuousSamples;
        bool                                          _frameVarying;
      public:        

        /// constructor based on clip descriptor
        Instance(ImageEffectPlugin* plugin,
                 Descriptor         &other, 
                 const std::string  &context,
                 bool               interactive);

        virtual ~Instance();

        /// called after construction to populate the various members
        /// ideally should be called in the ctor, but it relies on 
        /// virtuals so has to be delayed until after the effect is
        /// constructed
        OfxStatus populate();

        /// get the parameters set
        Param::SetInstance &getParams();

        /// get the nth clip, in order of declaration
        Clip::Instance* getNthClip(int index);

        /// get the nth clip, in order of declaration
        int getNClips() const
        {
          return _clips.size();
        }

        /// are all the non optional clips connected
        bool checkClipConnectionStatus() const;

        /// can this this instance render images at arbitrary times, not just frame boundaries
        /// set by getClipPreferenceAction()
        bool continuousSamples() const {return _continuousSamples;}

        /// does this instance generate a different picture on a frame change, even if the
        /// params and input images are exactly the same. eg: random noise generator
        bool isFrameVarying() const {return _frameVarying;}

        /// pure virtuals that must  be overriden
        virtual Clip::Instance* getClip(const std::string& name);

        /// override this to make processing abort, return 1 to abort processing
        virtual int abort();

        /// override this to use your own memory instance - must inherrit from memory::instance
        virtual Memory::Instance* newMemoryInstance(size_t nBytes);

        // return an memory::instance calls makeMemoryInstance that can be overriden
        Memory::Instance* imageMemoryAlloc(size_t nBytes);

        /// make a clip
        virtual Clip::Instance* newClipInstance(ImageEffect::Instance* plugin,
                                                Clip::Descriptor* descriptor) = 0;

        /// make a param set
        virtual Param::SetInstance* newParamSetInstance(ImageEffect::Instance* plugin, 
                                                        Param::SetDescriptor& descriptor) = 0;

        /// vmessage
        virtual OfxStatus vmessage(const char* type,
                                   const char* id,
                                   const char* format,
                                   va_list args) = 0;       

        /// call the interactive entry point
        OfxStatus overlayEntry(const char *action, 
                               const void *handle, 
                               OfxPropertySetHandle inArgs, 
                               OfxPropertySetHandle outArgs);

        /// call the effect entry point
        OfxStatus mainEntry(const char *action, 
                            const void *handle, 
                            OfxPropertySetHandle inArgs,                        
                            OfxPropertySetHandle outArgs);     

        int upperGetDimension(const std::string &name);

        /// overridden from Property::Notify
        virtual void notify(const std::string &name, bool singleValue, int indexOrN) OFX_EXCEPTION_SPEC;

        /// overridden from gethook,  get the virutals for viewport size, pixel scale, background colour
        virtual double getDoubleProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC;

        /// overridden from gethook,  get the virutals for viewport size, pixel scale, background colour
        virtual void getDoublePropertyN(const std::string &name, double *values, int count) OFX_EXCEPTION_SPEC;
        
        /// overridden from gethook, don't know what to do
        virtual void reset(const std::string &name) OFX_EXCEPTION_SPEC;

        //// overridden from gethook
        virtual int getDimension(const std::string &name)  OFX_EXCEPTION_SPEC;

        //
        // live parameters
        //

        // The size of the current project in canonical coordinates. 
        // The size of a project is a sub set of the kOfxImageEffectPropProjectExtent. For example a 
        // project may be a PAL SD project, but only be a letter-box within that. The project size is 
        // the size of this sub window. 
        virtual void getProjectSize(double& xSize, double& ySize) = 0;

        // The offset of the current project in canonical coordinates. 
        // The offset is related to the kOfxImageEffectPropProjectSize and is the offset from the origin 
        // of the project 'subwindow'. For example for a PAL SD project that is in letterbox form, the
        // project offset is the offset to the bottom left hand corner of the letter box. The project 
        // offset is in canonical coordinates. 
        virtual void getProjectOffset(double& xOffset, double& yOffset) = 0;

        // The extent of the current project in canonical coordinates. 
        // The extent is the size of the 'output' for the current project. See ProjectCoordinateSystems 
        // for more infomation on the project extent. The extent is in canonical coordinates and only 
        // returns the top right position, as the extent is always rooted at 0,0. For example a PAL SD 
        // project would have an extent of 768, 576. 
        virtual void getProjectExtent(double& xSize, double& ySize) = 0;

        // The pixel aspect ratio of the current project 
        virtual double getProjectPixelAspectRatio() = 0;

        // The duration of the effect 
        // This contains the duration of the plug-in effect, in frames. 
        virtual double getEffectDuration() = 0;

        // For an instance, this is the frame rate of the project the effect is in. 
        virtual double getFrameRate() = 0;

        //
        // actions
        //

        /// create an instance. This needs to be called _after_ construction and
        /// _after_ the host populates it's params and clips with the 'correct'
        /// values (either persisted ones or the defaults)
        virtual OfxStatus createInstanceAction();

        // begin/change/end instance changed

        //
        // why -
        //
        // kOfxChangeUserEdited   - the user or host changed the instance somehow and 
        //                          caused a change to something, this includes undo/redos, 
        //                          resets and loading values from files or presets,
        // kOfxChangePluginEdited - the plugin itself has changed the value of the instance 
        //                          in some action
        // kOfxChangeTime         - the time has changed and this has affected the value 
        //                          of the object because it varies over time
        //
        virtual OfxStatus beginInstanceChangedAction(std::string why);

        virtual OfxStatus paramInstanceChangedAction(std::string paramName,
                                                     std::string why,
                                                     OfxTime     time,
                                                     double      renderScaleX,
                                                     double      renderScaleY);

        virtual OfxStatus clipInstanceChangedAction(std::string clipName,
                                                    std::string why,
                                                    OfxTime     time,
                                                    double      renderScaleX,
                                                    double      renderScaleY);

        virtual OfxStatus endInstanceChangedAction(std::string why);

        // purge your caches
        virtual OfxStatus purgeCachesAction();

        // sync your private data
        virtual OfxStatus syncPrivateDataAction();

        // begin/end edit instance
        virtual OfxStatus beginInstanceEditAction();
        virtual OfxStatus endInstanceEditAction();

        // render action
        virtual OfxStatus beginRenderAction(OfxTime  startFrame,
                                            OfxTime  endFrame,
                                            OfxTime  step,
                                            bool     interactive,
                                            double   renderScaleX,
                                            double   renderScaleY);

        virtual OfxStatus renderAction(OfxTime      time,
                                       std::string  field,
                                       double       x1,
                                       double       y1,
                                       double       x2,
                                       double       y2,
                                       double       renderScaleX,
                                       double       renderScaleY);

        virtual OfxStatus endRenderAction(OfxTime  startFrame,
                                          OfxTime  endFrame,
                                          OfxTime  step,
                                          bool     interactive,
                                          double   renderScaleX,
                                          double   renderScaleY);

        // roi/rod
        virtual OfxStatus getRegionOfDefinitionAction(OfxTime  time,
                                                      double   renderScaleX,
                                                      double   renderScaleY,
                                                      double   &x1,
                                                      double   &y1,
                                                      double   &x2,
                                                      double   &y2);

        virtual OfxStatus getRegionOfInterestAction(OfxTime  time,
                                                    double   renderScaleX,
                                                    double   renderScaleY,
                                                    double   x1,
                                                    double   y1,
                                                    double   x2,
                                                    double   y2,
                                                    std::map<Clip::Instance *, OfxRectD> &rois);

        // get frames needed to render the given frame
        virtual OfxStatus getFrameNeededAction(OfxTime time, 
                                               RangeMap &rangeMap);
 
        // is identity
        virtual OfxStatus isIdentityAction(OfxTime     &time,
                                           std::string  field,
                                           double       x1,
                                           double       y1,
                                           double       x2,
                                           double       y2,
                                           double       renderScaleX,
                                           double       renderScaleY,
                                           std::string &clip);

        // clip preferences
        virtual OfxStatus getClipPreferenceAction(std::map<std::string,std::string> &clipComponents,
                                                  std::map<std::string,std::string> &clipDepth,
                                                  std::map<std::string,double>      &clipPARs,
                                                  double                            &outputFrameRate,
                                                  double                            &outputPAR,
                                                  std::string                       &outputPremult,
                                                  int                               &outputContinuousSamples,
                                                  int                               &outputFrameVarying);

        // time domain
        virtual OfxStatus getTimeDomainAction(OfxRangeD& range);

      };

    }

  }

}

#endif // OFX_IMAGE_EFFECT_H
