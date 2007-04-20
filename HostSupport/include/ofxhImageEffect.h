#ifndef OFX_IMAGE_EFFECT_H
#define OFX_IMAGE_EFFECT_H

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
      public:        

        // constructor based on clip descriptor
        Instance(ImageEffectPlugin* plugin,
                 Descriptor &other, 
                 const std::string &context);

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

        // create a clip instance
        virtual OfxStatus createInstanceAction();
        
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


      };

    }

  }

}

#endif // OFX_IMAGE_EFFECT_H