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
      
      /// properties common on an effect and a descriptor
      static Property::PropSpec effectDescriptorStuff[] = {
        /* name                                 type                   dim. r/o default value */
        { kOfxPropType,                         Property::eString,     1, true,  kOfxTypeImageEffect },
        { kOfxPropLabel,                        Property::eString,     1, false, "" },
        { kOfxPropShortLabel,                   Property::eString,     1, false, "" },
        { kOfxPropLongLabel,                    Property::eString,     1, false, "" },
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

      //
      // Base
      //

      Base::Base(const Property::Set &set) 
        : _properties(set) 
      {}

      Base::Base(const Property::PropSpec * propSpec)
        : _properties(propSpec) 
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

      /// name of the clip
      const std::string &Base::getShortLabel() const
      {
        const std::string &s = _properties.getStringProperty(kOfxPropShortLabel);
        if(s == "") {
          const std::string &s2 = _properties.getStringProperty(kOfxPropLabel);
          if(s2 == "") {
            return _properties.getStringProperty(kOfxPropName);
          }
        }
        return s;
      }
        
      /// name of the clip
      const std::string &Base::getLabel() const
      {
        const std::string &s = _properties.getStringProperty(kOfxPropShortLabel);
        if(s == "") {
          return _properties.getStringProperty(kOfxPropName);
        }
        return s;
      }
        
      /// name of the clip
      const std::string &Base::getLongLabel() const
      {
        const std::string &s = _properties.getStringProperty(kOfxPropLongLabel);
        if(s == "") {
          const std::string &s2 = _properties.getStringProperty(kOfxPropLabel);
          if(s2 == "") {
            return _properties.getStringProperty(kOfxPropName);
          }
        }
        return s;
      }

      /// is the given context supported
      bool Base::isContextSupported(const std::string &s) const
      {
        return _properties.findStringPropValueIndex(kOfxImageEffectPropSupportedContexts, s) != -1;
      }

      /// what is the name of the group the plug-in belongs to
      const std::string &Base::getPluginGrouping() const
      {
        return _properties.getStringProperty(kOfxImageEffectPluginPropGrouping);
      }

      /// is the effect single instance
      bool Base::isSingleInstance() const
      {
        return _properties.getIntProperty(kOfxImageEffectPluginPropSingleInstance) != 0;
      }

      /// what is the thread safety on this effect
      const std::string &Base::getRenderThreadSaftey() const
      {
        return _properties.getStringProperty(kOfxImageEffectPluginRenderThreadSafety);
      }

      /// should the host attempt to managed multi-threaded rendering if it can
      /// via tiling or some such
      bool Base::getHostFrameThreading() const
      {
        return _properties.getIntProperty(kOfxImageEffectPluginPropHostFrameThreading) != 0;
      }

      /// get the overlay interact main entry if it exists
      OfxPluginEntryPoint *Base::getOverlayInteractMainEntry() const
      {
        return (OfxPluginEntryPoint *)(_properties.getPointerProperty(kOfxImageEffectPluginPropOverlayInteractV1));
      }

      /// does the effect support images of differing sizes
      bool Base::supportsMultiResolution() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropSupportsMultiResolution) != 0;
      }

      /// does the effect support tiled rendering
      bool Base::supportsTiles() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropSupportsTiles) != 0;
      }

      /// does this effect need random temporal access
      bool Base::temporalAccess() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropTemporalClipAccess) != 0;
      }

      /// is the given RGBA/A pixel depth supported by the effect
      bool Base::isPixelDepthSupported(const std::string &s) const
      {
        return _properties.findStringPropValueIndex(kOfxImageEffectPropSupportedPixelDepths, s) != -1;
      }

      /// when field rendering, does the effect need to be called
      /// twice to render a frame in all Base::circumstances (with different fields)
      bool Base::fieldRenderTwiceAlways() const
      {
        return _properties.getIntProperty(kOfxImageEffectPluginPropFieldRenderTwiceAlways) != 0;
      }
        
      /// does the effect support multiple clip depths
      bool Base::supportsMultipleClipDepths() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropSupportsMultipleClipDepths) != 0;
      }
        
      /// does the effect support multiple clip pixel aspect ratios
      bool Base::supportsMultipleClipPARs() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropSupportsMultipleClipPARs) != 0;
      }
        
      /// does changing the named param re-tigger a clip preferences action
      bool Base::isClipPreferencesSlaveParam(const std::string &s) const
      {
        return _properties.findStringPropValueIndex(kOfxImageEffectPropClipPreferencesSlaveParam, s) != -1;
      }



      ////////////////////////////////////////////////////////////////////////////////
      // descriptor


      // Descriptor
      Descriptor::Descriptor(const Descriptor &other)
        : Base(other._properties)
        , _clips(other._clips)
        , _params(other._params)
      {
      }

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
        Clip::Descriptor *c = new Clip::Descriptor(name);
        _clips[name] = c;
        _clipsByOrder.push_back(c);
        return c;
      }

      /// get the clips
      std::map<std::string, Clip::Descriptor*> &Descriptor::getClips() {
        return _clips;
      }

      void Descriptor::addClip(const std::string &name, Clip::Descriptor *clip) {
        _clips[name] = clip;
        _clipsByOrder.push_back(clip);
      }

      //
      // Instance
      //

      static Property::PropSpec effectInstanceStuff[] = {
        /* name                                 type                   dim.   r/o    default value */
        { kOfxPropType,                         Property::eString,     1, true,  kOfxTypeImageEffect },
        { kOfxImageEffectPropContext,           Property::eString,     1, true, "" },
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
                         Descriptor         &other, 
                         const std::string  &context,
                         bool               interactive) 
        : Base(effectInstanceStuff),
          _plugin(plugin), 
          _context(context),
          _descriptor(&other),
          _interactive(interactive),
          _created(false)
        , _continuousSamples(false)
        , _frameVarying(false)
      {
        int i = 0;

        _properties.setStringProperty(kOfxImageEffectPropContext,context);
        _properties.setIntProperty(kOfxPropIsInteractive,interactive);

        // copy is sequential over
        bool sequential = other.getProps().getIntProperty(kOfxImageEffectInstancePropSequentialRender);
        _properties.setIntProperty(kOfxImageEffectInstancePropSequentialRender,sequential);

        while(effectInstanceStuff[i].name) {
          
          // don't set hooks for context or isinteractive

          if(strcmp(effectInstanceStuff[i].name,kOfxImageEffectPropContext) ||
             strcmp(effectInstanceStuff[i].name,kOfxPropIsInteractive) ||
             strcmp(effectInstanceStuff[i].name,kOfxImageEffectInstancePropSequentialRender) )
            {
              Property::PropSpec& spec = effectInstanceStuff[i];

              switch (spec.type) {
              case Property::eDouble:
                _properties.setGetHook(spec.name, this);
                break;
              default:
                break;
              }

            }

          i++;
        }
      }

      /// called after construction to populate clips and params
      OfxStatus Instance::populate() 
      {        
        const std::vector<Clip::Descriptor*>& clips = _descriptor->getClipsByOrder();

        for(std::vector<Clip::Descriptor*>::const_iterator it=clips.begin();
            it!=clips.end();
            it++)
          {
            const std::string name =  (*it)->getName();
            // foreach clip descriptor make a clip instance
            Clip::Instance* instance = newClipInstance(this, *it);   
            if(!instance) return kOfxStatFailed;

            _clips[name] = instance;
          }        

        // make the param set instance from the param instances
        _params = newParamSetInstance(this,_descriptor->getParams());

        std::map<std::string,Param::Descriptor*>& map = _descriptor->getParams().getParams();

        std::map<std::string,std::vector<Param::Instance*> > parameters;
        std::map<std::string, Param::Instance*> groups;

        for(std::map<std::string,Param::Descriptor*>::iterator it=map.begin();
            it!=map.end();
            it++)
          {
            // name of the parameter
            std::string name = it->first;
          
            // get the param descriptor
            Param::Descriptor* descriptor = it->second;
            if(!descriptor) return kOfxStatErrValue;

            // get a param instance from a param descriptor
            Param::Instance* instance = _params->newParam(name,*descriptor);
            if(!instance) return kOfxStatFailed;
          
            // add the value into the param set instance
            OfxStatus st = _params->addParam(name,instance);
            if(st != kOfxStatOK) return st;

            std::string parent = instance->getParentName();
          
            if(parent!="")
              parameters[parent].push_back(instance);

            if(instance->getType()==kOfxParamTypeGroup){
              groups[instance->getName()]=instance;
            }
          }

        // for each group parameter made
        for(std::map<std::string,Param::Instance*>::iterator it=groups.begin();
            it!=groups.end();
            it++)
          {
            // cast to a group instance
            Param::GroupInstance* group = dynamic_cast<Param::GroupInstance*>(it->second);

            // if cast ok
            if(group){
              // find the parameters whose parent was this group
              std::map<std::string,std::vector<Param::Instance*> >::iterator it2 = parameters.find(group->getName());
              if(it2!=parameters.end()){
                // associate the group with its children, and the children with its parent group
                group->setChildren(it2->second);
              }
            }
          }

        return kOfxStatOK;
      }

      // do nothing
      int Instance::getDimension(const std::string &name) OFX_EXCEPTION_SPEC {
        printf("failing in %s with name=%s\n", __PRETTY_FUNCTION__, name.c_str());
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      int Instance::upperGetDimension(const std::string &name) {
        return _properties.getDimension(name);
      }

      void Instance::notify(const std::string &name, bool singleValue, int indexOrN) OFX_EXCEPTION_SPEC 
      { 
        printf("failing in %s\n", __PRETTY_FUNCTION__);
      }

      // don't know what to do
      void Instance::reset(const std::string &name) OFX_EXCEPTION_SPEC {
        printf("failing in %s\n", __PRETTY_FUNCTION__);
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      // get the virutals for viewport size, pixel scale, background colour
      double Instance::getDoubleProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC
      {
        if(name==kOfxImageEffectPropProjectSize){
          if(index>=2) throw Property::Exception(kOfxStatErrBadIndex);
          double values[2];
          getProjectSize(values[0],values[1]);
          return values[index];
        }
        else if(name==kOfxImageEffectPropProjectOffset){
          if(index>=2) throw Property::Exception(kOfxStatErrBadIndex);
          double values[2];
          getProjectOffset(values[0],values[1]);
          return values[index];
        }
        else if(name==kOfxImageEffectPropProjectExtent){
          if(index>=2) throw Property::Exception(kOfxStatErrBadIndex);
          double values[2];
          getProjectExtent(values[0],values[1]);
          return values[index];
        }
        else if(name==kOfxImageEffectPropProjectPixelAspectRatio){
          if(index>=1) throw Property::Exception(kOfxStatErrBadIndex);
          return getProjectPixelAspectRatio();
        }
        else if(name==kOfxImageEffectInstancePropEffectDuration){
          if(index>=1) throw Property::Exception(kOfxStatErrBadIndex);
          return getEffectDuration();
        }
        else if(name==kOfxImageEffectPropFrameRate){
          if(index>=1) throw Property::Exception(kOfxStatErrBadIndex);
          return getFrameRate();
        }
        else
          throw Property::Exception(kOfxStatErrUnknown);        
      }

      void Instance::getDoublePropertyN(const std::string &name, double* first, int n) OFX_EXCEPTION_SPEC
      {
        if(name==kOfxImageEffectPropProjectSize){
          if(n>2) throw Property::Exception(kOfxStatErrBadIndex);
          getProjectSize(first[0],first[1]);
        }
        else if(name==kOfxImageEffectPropProjectOffset){
          if(n>2) throw Property::Exception(kOfxStatErrBadIndex);
          getProjectOffset(first[0],first[1]);
        }
        else if(name==kOfxImageEffectPropProjectExtent){
          if(n>2) throw Property::Exception(kOfxStatErrBadIndex);
          getProjectExtent(first[0],first[1]);
        }
        else if(name==kOfxImageEffectPropProjectPixelAspectRatio){
          if(n>1) throw Property::Exception(kOfxStatErrBadIndex);
          *first = getProjectPixelAspectRatio();
        }
        else if(name==kOfxImageEffectInstancePropEffectDuration){
          if(n>1) throw Property::Exception(kOfxStatErrBadIndex);
          *first = getEffectDuration();
        }
        else if(name==kOfxImageEffectPropFrameRate){
          if(n>1) throw Property::Exception(kOfxStatErrBadIndex);
          *first = getFrameRate();
        }
        else
          throw Property::Exception(kOfxStatErrUnknown);
      }

      Instance::~Instance(){
        // destroy the instance, only if succesfully created
        if (_created) {
          mainEntry(kOfxActionDestroyInstance,this->getHandle(),0,0);
        }
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

      // call the interactive entry point
      OfxStatus Instance::overlayEntry(const char *action, 
                                       const void *handle, 
                                       OfxPropertySetHandle inArgs, 
                                       OfxPropertySetHandle outArgs)
      {
        OfxPluginEntryPoint *overlayInteractV1Entry = 0;

        overlayInteractV1Entry = (OfxPluginEntryPoint*)_descriptor->getProps().getPointerProperty(kOfxImageEffectPluginPropOverlayInteractV1);

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
        if(_plugin){
          PluginHandle* pHandle = _plugin->getPluginHandle();
          if(pHandle){
            OfxPlugin* ofxPlugin = pHandle->getOfxPlugin();
            if(ofxPlugin){
              return ofxPlugin->mainEntry(action,handle,inArgs,outArgs);        
            }
            return kOfxStatFailed;
          }
          return kOfxStatFailed;
        }
        return kOfxStatFailed;
      }

      // get the nth clip, in order of declaration
      Clip::Instance* Instance::getNthClip(int index)
      {
        const std::string name = _descriptor->getClipsByOrder()[index]->getName();
        return _clips[name];
      }

      Clip::Instance* Instance::getClip(const std::string& name){
        std::map<std::string,Clip::Instance*>::iterator it = _clips.find(name);
        if(it!=_clips.end()){
          return it->second;
        }
        return 0;
      }

      // create a clip instance
      OfxStatus Instance::createInstanceAction() 
      {
        // now tell the plug-in to create instance
        OfxStatus st = mainEntry(kOfxActionCreateInstance,this->getHandle(),0,0);

        if (st == kOfxStatOK) {
          _created = true;
        }

        return st;
      }

      // begin/change/end instance changed
      OfxStatus Instance::beginInstanceChangedAction(std::string why)
      {
        Property::PropSpec stuff[] = {
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { 0 }
        };

        Property::Set inArgs(stuff);

        return mainEntry(kOfxActionBeginInstanceChanged,this->getHandle(),inArgs.getHandle(),0);
      }

      OfxStatus Instance::paramInstanceChangedAction(std::string paramName,
                                                     std::string why,
                                                     OfxTime     time,
                                                     double      renderScaleX,
                                                     double      renderScaleY)
      {        
        Param::Instance* param = _params->getParam(paramName);
        if (!param) {
          return kOfxStatFailed;
        }

        Property::PropSpec stuff[] = {
          { kOfxPropType, Property::eString, 1, true, kOfxTypeParameter },
          { kOfxPropName, Property::eString, 1, true, paramName.c_str() },
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);

        // add the second dimension of the render scale
        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);
        
        if (paramName == "presets") {
          printf("presets changed.\n");
        }

        return mainEntry(kOfxActionInstanceChanged,this->getHandle(),inArgs.getHandle(),0);
      }


      //        if(param)
      //          return param->instanceChangedAction(why,time,renderScaleX,renderScaleY);
      //        else
      //          return kOfxStatFailed;
      //      }

      OfxStatus Instance::clipInstanceChangedAction(std::string clipName,
                                                    std::string why,
                                                    OfxTime     time,
                                                    double      renderScaleX,
                                                    double      renderScaleY)
      {
        std::map<std::string,Clip::Instance*>::iterator it=_clips.find(clipName);
        if(it!=_clips.end())
          return (it->second)->instanceChangedAction(why,time,renderScaleX,renderScaleY);
        else
          return kOfxStatFailed;
      }

      OfxStatus Instance::endInstanceChangedAction(std::string why)
      {
        Property::PropSpec whyStuff[] = {
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { 0 }
        };

        Property::Set inArgs(whyStuff);

        return mainEntry(kOfxActionEndInstanceChanged,this->getHandle(),inArgs.getHandle(),0);
      }

      // purge your caches
      OfxStatus Instance::purgeCachesAction(){
        return mainEntry(kOfxActionPurgeCaches ,this->getHandle(),0,0);
      }

      // sync your private data
      OfxStatus Instance::syncPrivateDataAction(){
        return mainEntry(kOfxActionSyncPrivateData,this->getHandle(),0,0);
      }

      // begin/end edit instance
      OfxStatus Instance::beginInstanceEditAction(){
        return mainEntry(kOfxActionBeginInstanceEdit,this->getHandle(),0,0);
      }

      OfxStatus Instance::endInstanceEditAction(){
        return mainEntry(kOfxActionEndInstanceEdit,this->getHandle(),0,0);
      }

      OfxStatus Instance::beginRenderAction(OfxTime  startFrame,
                                            OfxTime  endFrame,
                                            OfxTime  step,
                                            bool     interactive,
                                            double   renderScaleX,
                                            double   renderScaleY) {
        Property::PropSpec stuff[] = {
          { kOfxImageEffectPropFrameRange, Property::eDouble, 2, true, "0" },
          { kOfxImageEffectPropFrameStep, Property::eDouble, 1, true, "0" }, 
          { kOfxPropIsInteractive, Property::eInt, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);

        // set up second dimension for frame range and render scale
        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,startFrame, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,endFrame, 1);

        inArgs.setDoubleProperty(kOfxImageEffectPropFrameStep,step);

        inArgs.setDoubleProperty(kOfxPropIsInteractive,interactive);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);

        return  mainEntry(kOfxImageEffectActionBeginSequenceRender,this->getHandle(),inArgs.getHandle(),0);        
      }

      OfxStatus Instance::renderAction(OfxTime      time,
                                       std::string  field,
                                       double       x1,
                                       double       y1,
                                       double       x2,
                                       double       y2,
                                       double       renderScaleX,
                                       double       renderScaleY) {
        Property::PropSpec stuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropFieldToRender, Property::eString, 1, true, field.c_str() }, 
          { kOfxImageEffectPropRenderWindow, Property::eInt, 4, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);
        
        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,x1, 0);
        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,y1, 1);
        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,x2, 2);
        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,y2, 3);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);

        return mainEntry(kOfxImageEffectActionRender,this->getHandle(),inArgs.getHandle(),0);        
      }

      OfxStatus Instance::endRenderAction(OfxTime  startFrame,
                                          OfxTime  endFrame,
                                          OfxTime  step,
                                          bool     interactive,
                                          double   renderScaleX,
                                          double   renderScaleY) {
        Property::PropSpec stuff[] = {
          { kOfxImageEffectPropFrameRange, Property::eDouble, 2, true, "0" },
          { kOfxImageEffectPropFrameStep, Property::eDouble, 1, true, "0" }, 
          { kOfxPropIsInteractive, Property::eInt, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);

        inArgs.setDoubleProperty(kOfxImageEffectPropFrameStep,step);

        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,startFrame, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,endFrame, 1);

        inArgs.setDoubleProperty(kOfxPropIsInteractive,interactive);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);

        return mainEntry(kOfxImageEffectActionEndSequenceRender,this->getHandle(),inArgs.getHandle(),0);        
      }

      OfxStatus Instance::getRegionOfDefinitionAction(OfxTime  time,
                                                      double   renderScaleX,
                                                      double   renderScaleY,
                                                      double   &x1,
                                                      double   &y1,
                                                      double   &x2,
                                                      double   &y2) {
        Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::PropSpec outStuff[] = {
          { kOfxImageEffectPropRegionOfDefinition , Property::eDouble, 4, false, "0" },
          { 0 }
        };

        Property::Set inArgs(inStuff);
        Property::Set outArgs(outStuff);
        
        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);


        mainEntry(kOfxImageEffectActionGetRegionOfDefinition,
                  this->getHandle(),
                  inArgs.getHandle(),
                  outArgs.getHandle());

        x1 = outArgs.getDoubleProperty(kOfxImageEffectPropRegionOfDefinition,0);
        y1 = outArgs.getDoubleProperty(kOfxImageEffectPropRegionOfDefinition,1);
        x2 = outArgs.getDoubleProperty(kOfxImageEffectPropRegionOfDefinition,2);
        y2 = outArgs.getDoubleProperty(kOfxImageEffectPropRegionOfDefinition,3);

        return kOfxStatOK;
      }

      OfxStatus Instance::getRegionOfInterestAction(OfxTime  time,
                                                    double   renderScaleX,
                                                    double   renderScaleY,
                                                    double   x1,
                                                    double   y1,
                                                    double   x2,
                                                    double   y2,
                                                    std::map<std::string,OfxRectD>& rois) {
        Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::PropSpec outStuff[] = {
          { kOfxImageEffectPropRegionOfDefinition , Property::eDouble, 4, false, 0 },
          { 0 }
        };

        Property::Set inArgs(inStuff);
        Property::Set outArgs(outStuff);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);

        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setDoubleProperty(kOfxImageEffectPropRegionOfInterest,x1, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRegionOfInterest,y1, 1);
        inArgs.setDoubleProperty(kOfxImageEffectPropRegionOfInterest,x2, 2);
        inArgs.setDoubleProperty(kOfxImageEffectPropRegionOfInterest,y2, 3);

        mainEntry(kOfxImageEffectActionGetRegionsOfInterest,
                  this->getHandle(),
                  inArgs.getHandle(),
                  outArgs.getHandle());

        for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
            it!=_clips.end();
            it++)
          {
            std::string name = "OfxImageClipPropRoI_"+it->first;
          
            OfxRectD roi;

            roi.x1 = outArgs.getDoubleProperty(name,0);
            roi.y1 = outArgs.getDoubleProperty(name,1);
            roi.x2 = outArgs.getDoubleProperty(name,2);
            roi.y2 = outArgs.getDoubleProperty(name,3);

            rois[it->first] = roi;

          }
  
        return kOfxStatOK;
      }

      OfxStatus Instance::getFrameNeededAction(OfxTime time, 
                                               RangeMap &rangeMap)
      {
        if(!temporalAccess()) {
          return kOfxStatReplyDefault;
        }

        Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },          
          { 0 }
        };
        Property::Set inArgs(inStuff);       
        inArgs.setDoubleProperty(kOfxPropTime,time);
        
        
        Property::Set outArgs;
        for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
            it!=_clips.end();
            it++) { 
          Property::PropSpec s;
          std::string name = "OfxImageClipPropFrameRange_"+it->first;

          s.name = name.c_str();
          s.type = Property::eDouble;
          s.dimension = 0;
          s.readonly = false;
          s.defaultValue = "";
          outArgs.createProperty(s);
        }

        OfxStatus stat = mainEntry(kOfxImageEffectActionGetFramesNeeded,
                                   this->getHandle(),
                                   inArgs.getHandle(),
                                   outArgs.getHandle());
        
        if(stat == kOfxStatOK) {
          for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
              it!=_clips.end();
              it++) {
            Clip::Instance *clip = it->second;

            std::string name = "OfxImageClipPropFrameRange_"+it->first;

            int nRanges = outArgs.getDimension(name);
            if(nRanges%2 != 0)
              return kOfxStatFailed; // bad! needs to be divisible by 2

            std::vector<OfxRangeD> ranges;

            for(int r=0;r<nRanges;){
              double min = outArgs.getDoubleProperty(name,r);
              double max = outArgs.getDoubleProperty(name,r+1);
              r += 2;

              OfxRangeD range;
              range.min = min;
              range.max = max;
              rangeMap[clip].push_back(range);
            }
          }
        }

        return stat;
      }

      OfxStatus Instance::isIdentityAction(OfxTime     &time,
                                           std::string  field,
                                           double       x1,
                                           double       y1,
                                           double       x2,
                                           double       y2,
                                           double       renderScaleX,
                                           double       renderScaleY,
                                           std::string &clip)
      {
        static Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropFieldToRender, Property::eString, 1, true, field.c_str() }, 
          { kOfxImageEffectPropRenderWindow, Property::eInt, 4, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        static Property::PropSpec outStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, false, "0.0" },
          { kOfxPropName, Property::eString, 1, false, "" },
          { 0 }
        };

        Property::Set inArgs(inStuff);        

        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,x1, 0);
        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,y1, 1);
        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,x2, 2);
        inArgs.setIntProperty(kOfxImageEffectPropRenderWindow,y2, 3);

        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);

        Property::Set outArgs(outStuff);

        OfxStatus st = mainEntry(kOfxImageEffectActionIsIdentity,
                                 this->getHandle(),
                                 inArgs.getHandle(),
                                 outArgs.getHandle());        

        if(st==kOfxStatOK){
          time = outArgs.getDoubleProperty(kOfxPropTime);
          clip = outArgs.getStringProperty(kOfxPropName);        
        }
        
        return st;
      }

      OfxStatus Instance::getClipPreferenceAction(std::map<std::string,std::string> &clipComponents,
                                                  std::map<std::string,std::string> &clipDepth,
                                                  std::map<std::string,double>      &clipPARs,
                                                  double                            &outputFrameRate,
                                                  double                            &outputPAR,
                                                  std::string                       &outputPremult,
                                                  int                               &outputContinuousSamples,
                                                  int                               &outputFrameVarying)
      {

        int max = _clips.size();

        Property::PropSpec* outStuff = new Property::PropSpec[(3*max)+5];               

        int i = 0;

        for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
            it!=_clips.end();
            it++)
          {
            std::string componentParamName = "OfxImageClipPropComponents_"+it->first;

            outStuff[i].name = componentParamName.c_str();
            outStuff[i].type = Property::eString;
            outStuff[i].dimension = 0;
            outStuff[i].readonly = false;
            outStuff[i++].defaultValue = 0;

            std::string depthParamName = "OfxImageClipPropDepth_"+it->first;

            outStuff[i].name = depthParamName.c_str();
            outStuff[i].type = Property::eString;
            outStuff[i].dimension = 0;
            outStuff[i].readonly = false;
            outStuff[i++].defaultValue = 0;

            std::string parParamName = "OfxImageClipPropPAR_"+it->first;

            outStuff[i].name = parParamName.c_str();
            outStuff[i].type = Property::eDouble;
            outStuff[i].dimension = 0;
            outStuff[i].readonly = false;
            outStuff[i++].defaultValue = 0;
          }
        
        outStuff[i].name = kOfxImageEffectPropFrameRate;
        outStuff[i].type = Property::eDouble;
        outStuff[i].dimension = 0;
        outStuff[i].readonly = false;
        outStuff[i++].defaultValue = 0;

        outStuff[i].name = kOfxImagePropPixelAspectRatio;
        outStuff[i].type = Property::eDouble;
        outStuff[i].dimension = 0;
        outStuff[i].readonly = false;
        outStuff[i++].defaultValue = 0;

        outStuff[i].name = kOfxImageEffectPropPreMultiplication;
        outStuff[i].type = Property::eString;
        outStuff[i].dimension = 0;
        outStuff[i].readonly = false;
        outStuff[i++].defaultValue = 0;

        outStuff[i].name = kOfxImageClipPropContinuousSamples;
        outStuff[i].type = Property::eInt;
        outStuff[i].dimension = 0;
        outStuff[i].readonly = false;
        outStuff[i++].defaultValue = 0;

        outStuff[i].name = kOfxImageEffectFrameVarying;
        outStuff[i].type = Property::eInt;
        outStuff[i].dimension = 0;
        outStuff[i].readonly = false;
        outStuff[i++].defaultValue = 0;

        Property::Set outArgs(outStuff);

        OfxStatus st = mainEntry(kOfxImageEffectActionGetClipPreferences,
                                 this->getHandle(),
                                 0,
                                 outArgs.getHandle());
        if(st!=kOfxStatOK) return st;

        for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
            it!=_clips.end();
            it++)
          {
            std::string componentParamName = "OfxImageClipPropComponents_"+it->first;
            std::string depthParamName = "OfxImageClipPropDepth_"+it->first;
            std::string parParamName = "OfxImageClipPropPAR_"+it->first;

            clipComponents[it->first] = outArgs.getStringProperty(componentParamName);
            clipDepth[it->first]      = outArgs.getStringProperty(depthParamName,0);
            clipPARs[it->first]       = outArgs.getDoubleProperty(parParamName,0);
          }

        outputFrameRate = outArgs.getDoubleProperty(kOfxImageEffectPropFrameRate);
        outputPAR = outArgs.getDoubleProperty(kOfxImagePropPixelAspectRatio);
        outputPremult = outArgs.getStringProperty(kOfxImageEffectPropPreMultiplication);
        outputContinuousSamples = outArgs.getIntProperty(kOfxImageClipPropContinuousSamples);
        outputFrameVarying = outArgs.getIntProperty(kOfxImageEffectFrameVarying);

        return kOfxStatOK;
      }

      OfxStatus Instance::getTimeDomainAction(OfxRangeD& range)
      {
        Property::PropSpec outStuff[] = {
          { kOfxImageEffectPropFrameRange , Property::eDouble, 2, false, "0.0" },
          { 0 }
        };

        Property::Set outArgs(outStuff);  

        OfxStatus st = mainEntry(kOfxImageEffectActionGetTimeDomain,
                                 this->getHandle(),
                                 0,
                                 outArgs.getHandle());
        if(st!=kOfxStatOK) return st;

        range.min = outArgs.getDoubleProperty(kOfxImageEffectActionGetTimeDomain,0);
        range.max = outArgs.getDoubleProperty(kOfxImageEffectActionGetTimeDomain,1);

        return kOfxStatOK;
      }

    } // ImageEffect

  } // Host

} // OFX
