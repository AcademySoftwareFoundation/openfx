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
        : Base(other._properties)
        , _clips(other._clips)
        , _params(other._params)
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
        : Base(effectInstanceStuff),
          _plugin(plugin), 
          _context(context),
          _descriptor(&other),
          _created(false)
      {
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
        if(_plugin){
          PluginHandle* pHandle = _plugin->getPluginHandle();
          if(handle){
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

          // get a param instance from a param descriptor
          Param::Instance* instance = _params->newParam(name,*descriptor);
          if(!instance) return kOfxStatFailed;
          
          // add the value into the param set instance
          OfxStatus st = _params->addParam(name,instance);
          if(st != kOfxStatOK) return st;
        }

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
        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        return mainEntry(kOfxActionBeginInstanceChanged,this->getHandle(),inArgs.getHandle(),0);
      }

      OfxStatus Instance::clipInstanceChangedAction(std::string paramName,
                                          std::string why,
                                          OfxTime     time,
                                          double      renderScaleX,
                                          double      renderScaleY)
      {
        Property::PropSpec stuff[] = {
          { kOfxPropType, Property::eString, 1, true, kOfxTypeClip },
          { kOfxPropName, Property::eString, 1, true, paramName.c_str() },
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);

        // add the second dimension of the render scale
        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        return mainEntry(kOfxActionBeginInstanceChanged,this->getHandle(),inArgs.getHandle(),0);
      }

      OfxStatus Instance::endInstanceChangedAction(std::string why)
      {
        Property::PropSpec whyStuff[] = {
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { 0 }
        };

        Property::Set inArgs(whyStuff);

        return mainEntry(kOfxActionCreateInstance,this->getHandle(),inArgs.getHandle(),0);
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
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropFrameRange,0,startFrame);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropFrameRange,1,endFrame);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropFrameStep,0,step);

        inArgs.setProperty<Property::DoubleValue>(kOfxPropIsInteractive,0,interactive);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

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
          { kOfxImageEffectPropRenderWindow, Property::eDouble, 4, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);
        
        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,0,x1);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,1,y1);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,2,x2);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,3,y2);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        return mainEntry(kOfxImageEffectActionEndSequenceRender,this->getHandle(),inArgs.getHandle(),0);        
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

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropFrameStep,0,step);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropFrameRange,0,startFrame);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropFrameRange,1,endFrame);

        inArgs.setProperty<Property::DoubleValue>(kOfxPropIsInteractive,0,interactive);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

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
        
        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        mainEntry(kOfxImageEffectActionGetRegionOfDefinition,
                  this->getHandle(),
                  inArgs.getHandle(),
                  outArgs.getHandle());

        x1 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,0);
        y1 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,1);
        x2 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,2);
        y2 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,3);

        return kOfxStatOK;
      }

      OfxStatus Instance::getRegionOfInterestAction(OfxTime  time,
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
          { kOfxImageEffectPropRegionOfDefinition , Property::eDouble, 4, false, 0 },
          { 0 }
        };

        Property::Set inArgs(inStuff);
        Property::Set outArgs(outStuff);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);

        mainEntry(kOfxImageEffectActionGetRegionsOfInterest,
                  this->getHandle(),
                  inArgs.getHandle(),
                  outArgs.getHandle());

        x1 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,0);
        y1 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,1);
        x2 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,2);
        y2 = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropRegionOfDefinition,3);

        return kOfxStatOK;
      }

      OfxStatus Instance::getFrameNeededAction(OfxTime time, 
                                               std::map<std::string,std::vector<OfxRangeD> > rangeMap)
      {
        Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },          
          { 0 }
        };
        
        int max = _clips.size();

        Property::PropSpec* outStuff = new Property::PropSpec[max];

        int i = 0;

        for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
            it!=_clips.end();
            it++)
        {
          std::string name = "OfxImageClipPropFrameRange_"+it->first;

          outStuff[i].name = name.c_str();
          outStuff[i].type = Property::eDouble,
          outStuff[i].dimension = 0;
          outStuff[i].readonly = false;
          outStuff[i].defaultValue = "";

          i++;
        }


        Property::Set inArgs(inStuff);       

        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);

        Property::Set outArgs(outStuff);

        mainEntry(kOfxImageEffectActionGetFramesNeeded,
                  this->getHandle(),
                  inArgs.getHandle(),
                  outArgs.getHandle());

        for(std::map<std::string, Clip::Instance*>::iterator it=_clips.begin();
            it!=_clips.end();
            it++)
        {
          std::string name = "OfxImageClipPropFrameRange_"+it->first;

          int nRanges = outArgs.getDimension(name);

          std::vector<OfxRangeD> ranges;

          for(int r=0;r<nRanges;){
            double min = outArgs.getProperty<Property::DoubleValue>(name,r);
            r++;
            double max = outArgs.getProperty<Property::DoubleValue>(name,r);
            r++;

            OfxRangeD range;
            range.min = min;
            range.max = max;

            ranges.push_back(range);
          }

          rangeMap[it->first] = ranges;
        }

        return kOfxStatOK;
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
        Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropFieldToRender, Property::eString, 1, true, field.c_str() }, 
          { kOfxImageEffectPropRenderWindow, Property::eDouble, 4, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::PropSpec outStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, false, "0.0" },
          { kOfxPropName, Property::eString, 1, false, "" },
          { 0 }
        };

        Property::Set inArgs(inStuff);        

        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,0,x1);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,1,y1);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,2,x2);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderWindow,3,y2);

        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        Property::Set outArgs(outStuff);

        OfxStatus st = mainEntry(kOfxImageEffectActionIsIdentity,
                                 this->getHandle(),
                                 inArgs.getHandle(),
                                 outArgs.getHandle());        

        if(st==kOfxStatOK){
          time = outArgs.getProperty<Property::DoubleValue>(kOfxPropTime,0);
          clip = outArgs.getProperty<Property::StringValue>(kOfxPropName,0);        
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

          std::string component = outArgs.getProperty<Property::StringValue>(componentParamName,0);
          std::string depth = outArgs.getProperty<Property::StringValue>(depthParamName,0);
          double PAR = outArgs.getProperty<Property::DoubleValue>(parParamName,0);

          clipComponents[it->first] = component;
          clipDepth[it->first]      = depth;
          clipPARs[it->first]       = PAR;
        }

        outputFrameRate = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectPropFrameRate,0);
        outputPAR = outArgs.getProperty<Property::DoubleValue>(kOfxImagePropPixelAspectRatio,0);
        outputPremult = outArgs.getProperty<Property::StringValue>(kOfxImageEffectPropPreMultiplication,0);
        outputContinuousSamples = outArgs.getProperty<Property::IntValue>(kOfxImageClipPropContinuousSamples,0);
        outputFrameVarying = outArgs.getProperty<Property::IntValue>(kOfxImageEffectFrameVarying,0);

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

        range.min = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectActionGetTimeDomain,0);
        range.max = outArgs.getProperty<Property::DoubleValue>(kOfxImageEffectActionGetTimeDomain,1);

        return kOfxStatOK;
      }

    } // ImageEffect

  } // Host

} // OFX
