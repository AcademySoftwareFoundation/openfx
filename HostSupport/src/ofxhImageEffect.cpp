// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhImageEffect.h"
#include "ofxhPluginAPICache.h"
#include "ofxhPluginCache.h"
#include "ofxhHost.h"
#include "ofxhImageEffectAPI.h"

namespace OFX {

  namespace Host {

    namespace ImageEffect {

      //
      // Base
      //

      Base::Base(const Property::Set &set) 
        : _properties(set) 
      {}

      Base::Base(const Property::PropSpec& propSpec)
        : _properties(&propSpec) 
      {}

      Base::~Base() {}

      /// obtain a handle on this for passing to the C api
      OfxImageEffectHandle Base::getHandle() {
        return (OfxImageEffectHandle)this;
      }

      /// get the properties set
      Property::Set &Base::getProps() {
        return _properties;
      }

      // Descriptor
      Descriptor::Descriptor(const Descriptor &other)
        : _clips(other._clips)
        , _params(other._params)
        , Base(other._properties)
      {
      }

      static Property::PropSpec effectDescriptorStuff[] = {
        /* name                                 type                   dim. r/o default value */
        { kOfxPropType,											    Property::eString,     1, true,  kOfxTypeImageEffect },
        { kOfxPropLabel, 									      Property::eString,     1, false, "" },
        { kOfxPropShortLabel,  						      Property::eString,     1, false, "" },
        { kOfxPropLongLabel,  						      Property::eString,     1, false, "" },
        { kOfxImageEffectPropSupportedContexts, Property::eString,     0, false, "" },
        { kOfxImageEffectPluginPropGrouping,    Property::eString,     1, false, "" },
        { kOfxImageEffectPluginPropSingleInstance, Property::eInt,     1, false, "0" },
        { kOfxImageEffectPluginRenderThreadSafety, Property::eString,  1, false, kOfxImageEffectRenderInstanceSafe },
        { kOfxImageEffectPluginPropHostFrameThreading, Property::eInt, 1, false, "1" },
        { kOfxImageEffectPluginPropOverlayInteractV1, Property::ePointer, 1, false, "0" },
        { kOfxImageEffectPropSupportsMultiResolution, Property::eInt,  1, false, "1" } ,
        { kOfxImageEffectPropSupportsTiles,     Property::eInt,        1, false, "1" }, 
        { kOfxImageEffectPropTemporalClipAccess, Property::eInt,       1, false, "0" },
        { kOfxImageEffectPropSupportedPixelDepths, Property::eString,  0, false, "" }, 
        { kOfxImageEffectPluginPropFieldRenderTwiceAlways, Property::eInt, 1, false, "1" } ,
        { kOfxImageEffectPropSupportsMultipleClipDepths, Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropSupportsMultipleClipPARs,   Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropClipPreferencesSlaveParam, Property::eString, 0, false, "" },
        { 0 }
      };

      Descriptor::Descriptor(Plugin *plug) : Base(effectDescriptorStuff) {

        Property::PropSpec filePath[] = {
          { kOfxPluginPropFilePath, Property::eString, 1, true, plug->getBinary()->getBundlePath().c_str() },
          { 0 }
        };

        _properties.addProperties(filePath);
      }

      Descriptor::Descriptor(const std::string &bundlePath) : Base(effectDescriptorStuff) {

        Property::PropSpec filePath[] = {
          { kOfxPluginPropFilePath, Property::eString, 1, true, bundlePath.c_str() },
          { 0 }
        };

        _properties.addProperties(filePath);
      }

      /// get the parameters set
      Param::SetDescriptor &Descriptor::getParams() {
        return _params;
      }

      /// create a new clip and add this to the clip map
      Clip::Descriptor *Descriptor::defineClip(const std::string &name) {
        Clip::Descriptor *c = new Clip::Descriptor();
        _clips[name] = c;
        return c;
      }

      /// get the clips
      std::map<std::string, Clip::Descriptor*> &Descriptor::getClips() {
        return _clips;
      }

      void Descriptor::addClip(const std::string &name, Clip::Descriptor *clip) {
        _clips[name] = clip;
      }

      //
      // Instance
      //

      static Property::PropSpec effectInstanceStuff[] = {
        /* name                                 type                   dim.   r/o    default value */
        { kOfxPropType,											    Property::eString,     1, true,  kOfxTypeImageEffect },
        { kOfxImageEffectPropContext, 				  Property::eString,     1, true, "" },
        { kOfxPropInstanceData,                 Property::ePointer,    1, false, "0" },
        { kOfxImageEffectPropProjectSize,       Property::eDouble,     2, true,  "0" },
        { kOfxImageEffectPropProjectOffset,     Property::eDouble,     2, true,  "0" },
        { kOfxImageEffectPropProjectExtent,     Property::eDouble,     2, true,  "0" },
        { kOfxImageEffectPropProjectPixelAspectRatio, Property::eDouble, 1, true,  "0" },
        { kOfxImageEffectInstancePropEffectDuration, Property::eDouble, 1, true,  "0" },
        { kOfxImageEffectInstancePropSequentialRender, Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropFrameRate ,        Property::eDouble,     1, true,  "0" },
        { kOfxPropIsInteractive,                Property::eInt,        1, true, "0" },
        { 0 }
      };

      Instance::Instance(ImageEffectPlugin* plugin,
                         Descriptor &other, 
                         const std::string &context) 
        : _plugin(plugin), 
          _context(context),
          _descriptor(&other),
          Base(effectInstanceStuff)
      {
      }

      /// get the parameters set
      Param::SetInstance &Instance::getParams() {
        return *_params;
      }

      // override this to make processing abort, return 1 to abort processing
      int Instance::abort() { 
        return 0; 
      }

      // override this to use your own memory instance - must inherrit from memory::instance
      Memory::Instance* Instance::newMemoryInstance(size_t nBytes) { 
        return 0; 
      }

      // return an memory::instance calls makeMemoryInstance that can be overriden
      Memory::Instance* Instance::imageMemoryAlloc(size_t nBytes){
        Memory::Instance* instance = newMemoryInstance(nBytes);
        if(instance)
          return instance;
        else{
          Memory::Instance* instance = new Memory::Instance;
          instance->alloc(nBytes);
          return instance;
        }
      }

      // create a clip instance
      OfxStatus Instance::createInstanceAction() 
      {
        std::map<std::string,Clip::Descriptor*>& clips = _descriptor->getClips();

        for(std::map<std::string, Clip::Descriptor*>::iterator it=clips.begin();
          it!=clips.end();
          it++)
        {
          // foreach clip descriptor make a clip instance
          Clip::Instance* instance = newClipInstance(this,it->first,it->second);   
          if(!instance) return kOfxStatFailed;

          _clips[it->first] = instance;
        }        

        // make the param set instance from the param instances
        _params = newParamSetInstance(this,_descriptor->getParams());

        std::map<std::string,Param::Descriptor*>& map = _descriptor->getParams().getParams();

        for(std::map<std::string,Param::Descriptor*>::iterator it=map.begin();
            it!=map.end();
            it++)
        {
          // name of the parameter
          std::string name = it->first;
          
          // get the param descriptor
          Param::Descriptor* descriptor = it->second;
          if(!descriptor) return kOfxStatErrValue;

          // TODO - this is maybe not brilliant
          if(descriptor->getType()!=kOfxParamTypeGroup && 
             descriptor->getType()!=kOfxParamTypePage)
          {
            // get a param instance from a param descriptor
            Param::Instance* instance = _params->newParam(name,*descriptor);
            if(!instance) return kOfxStatFailed;

            // add the value into the param set instance
            OfxStatus st = _params->addParam(name,instance);
            if(st!=kOfxStatOK) return st;
          }

        }

        // now tell the plug-in to create instance
        mainEntry(kOfxActionCreateInstance,this->getHandle(),0,0);

        return kOfxStatOK;
      }

      // call the interactive entry point
      OfxStatus Instance::overlayEntry(const char *action, 
        const void *handle, 
        OfxPropertySetHandle inArgs, 
        OfxPropertySetHandle outArgs)
      {
        OfxPluginEntryPoint *overlayInteractV1Entry = 0;

        overlayInteractV1Entry = (OfxPluginEntryPoint*)_descriptor->getProps().getProperty<Property::PointerValue>(kOfxImageEffectPluginPropOverlayInteractV1,0);

        if(overlayInteractV1Entry){
          overlayInteractV1Entry(action,handle,inArgs,outArgs);
        }
        else
          return kOfxStatFailed;
        
        return kOfxStatOK;
      }

      // call the effect entry point
      OfxStatus Instance::mainEntry(const char *action, 
        const void *handle, 
        OfxPropertySetHandle inArgs,                        
        OfxPropertySetHandle outArgs)
      {
        return _plugin->getPluginHandle()->getOfxPlugin()->mainEntry(action,handle,inArgs,outArgs);        
      }

      Clip::Instance* Instance::getClip(const std::string& name){
        std::map<std::string,Clip::Instance*>::iterator it = _clips.find(name);
        if(it!=_clips.end()){
          return it->second;
        }
        return 0;
      }

    } // ImageEffect

  } // Host

} // OFX