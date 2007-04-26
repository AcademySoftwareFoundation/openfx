#ifndef OFX_IMAGE_EFFECT_H
#define OFX_IMAGE_EFFECT_H

#include "ofxCore.h"
#include "ofxImageEffect.h"

#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"

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
        Base(const Property::PropSpec& propSpec);
        virtual ~Base();

        /// obtain a handle on this for passing to the C api
        OfxImageEffectHandle getHandle();

        /// get the properties set
        Property::Set &getProps();
      };

      /// an image effect plugin descriptor
      class Descriptor : public Base {
      protected:
        std::map<std::string, Clip::Descriptor*>  _clips;
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

        void addClip(const std::string &name, Clip::Descriptor *clip);

      };      

      /// an image effect plugin descriptor
      class Instance : public Base {
      protected:
        OFX::Host::ImageEffect::ImageEffectPlugin    *_plugin;
        std::string                                   _context;
        Descriptor                                   *_descriptor;
        std::map<std::string, Clip::Instance*>        _clips;
        Param::SetInstance                           *_params;
        bool                                          _created;
      public:        

        // constructor based on clip descriptor
        Instance(ImageEffectPlugin* plugin,
          Descriptor &other, 
          const std::string &context);

        virtual ~Instance();

        /// get the parameters set
        Param::SetInstance &getParams();

        // pure virtuals that must  be overriden
        virtual Clip::Instance* getClip(const std::string& name);

        // override this to make processing abort, return 1 to abort processing
        virtual int abort();

        // override this to use your own memory instance - must inherrit from memory::instance
        virtual Memory::Instance* newMemoryInstance(size_t nBytes);

        // return an memory::instance calls makeMemoryInstance that can be overriden
        Memory::Instance* imageMemoryAlloc(size_t nBytes);

        // make a clip
        virtual Clip::Instance* newClipInstance(ImageEffect::Instance* plugin,
          const std::string& name, 
          Clip::Descriptor* descriptor) = 0;

        // make a param set
        virtual Param::SetInstance* newParamSetInstance(ImageEffect::Instance* plugin, 
          Param::SetDescriptor& descriptor) = 0;

        // vmessage
        virtual OfxStatus vmessage(const char* type,
          const char* id,
          const char* format,
          va_list args) = 0;       

        // call the interactive entry point
        OfxStatus overlayEntry(const char *action, 
          const void *handle, 
          OfxPropertySetHandle inArgs, 
          OfxPropertySetHandle outArgs);

        // call the effect entry point
        OfxStatus mainEntry(const char *action, 
          const void *handle, 
          OfxPropertySetHandle inArgs,                        
          OfxPropertySetHandle outArgs);     

        //
        // actions
        //

        // create a clip instance
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
        virtual OfxStatus beginRenderAction(OfxTime  startName,
                                            OfxTime  endName,
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

        virtual OfxStatus endRenderAction(OfxTime  startName,
                                          OfxTime  endName,
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
                                                    double   &x1,
                                                    double   &y1,
                                                    double   &x2,
                                                    double   &y2);

        // frames needed
        virtual OfxStatus getFrameNeededAction(OfxTime time, 
                                                std::map<std::string,std::vector<OfxRangeD> > rangeMap);

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
