/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.

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
*/

#include <math.h>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"
#ifdef OFX_EXTENSIONS_VEGAS
#include "ofxSonyVegas.h"
#endif

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
#include "ofxhUtilities.h"

#include <string.h>
#include <stdarg.h>

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
        { kOfxImageEffectPluginPropOverlayInteractV1, Property::ePointer, 1, false, NULL },
        { kOfxImageEffectPropSupportsMultiResolution, Property::eInt,  1, false, "1" } ,
        { kOfxImageEffectPropSupportsTiles,     Property::eInt,        1, false, "1" }, 
        { kOfxImageEffectPropTemporalClipAccess, Property::eInt,       1, false, "0" },
        { kOfxImageEffectPropSupportedPixelDepths, Property::eString,  0, false, "" }, 
        { kOfxImageEffectPluginPropFieldRenderTwiceAlways, Property::eInt, 1, false, "1" } ,
        { kOfxImageEffectPropSupportsMultipleClipDepths, Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropSupportsMultipleClipPARs,   Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropClipPreferencesSlaveParam, Property::eString, 0, false, "" },
        { kOfxPluginPropFilePath, Property::eString, 1, true, ""},
        { kOfxPropPluginDescription, Property::eString, 1,false, ""}, /*Added by Alex on 09/23/13*/
        Property::propSpecEnd
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
      OfxImageEffectHandle Base::getHandle() const {
        return (OfxImageEffectHandle)this;
      }
      
      /// get the properties set
      Property::Set &Base::getProps() {
        return _properties;
      }

      /// get the properties set, const version
      const Property::Set &Base::getProps() const {
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
        const std::string &s = _properties.getStringProperty(kOfxPropLabel);
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
      const std::string &Base::getRenderThreadSafety() const
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

      Descriptor::Descriptor(Plugin *plug) 
        : Base(effectDescriptorStuff)
        , _plugin(plug)
      {
        _properties.setStringProperty(kOfxPluginPropFilePath, plug->getBinary()->getBundlePath());
        gImageEffectHost->initDescriptor(this);
      }

      Descriptor::Descriptor(const Descriptor &other, Plugin *plug) 
        : Base(other._properties)
        , _plugin(plug)
      {
        _properties.setStringProperty(kOfxPluginPropFilePath, plug->getBinary()->getBundlePath());
        gImageEffectHost->initDescriptor(this);
      }

      Descriptor::Descriptor(const std::string &bundlePath, Plugin *plug) 
        : Base(effectDescriptorStuff) 
        , _plugin(plug)
      {
        _properties.setStringProperty(kOfxPluginPropFilePath, bundlePath);
        gImageEffectHost->initDescriptor(this);
      }

      Descriptor::~Descriptor()	
      {	
        for(std::map<std::string, ClipDescriptor*>::iterator it = _clips.begin(); it != _clips.end(); ++it)	
          delete it->second;
        _clips.clear();
      }		


      /// create a new clip and add this to the clip map
      ClipDescriptor *Descriptor::defineClip(const std::string &name) {
        ClipDescriptor *c = new ClipDescriptor(name);
        _clips[name] = c;
        _clipsByOrder.push_back(c);
        return c;
      }

      /// implemented for Param::SetDescriptor
      Property::Set &Descriptor::getParamSetProps()
      {
        return _properties;
      }

      /// get the interact description, this will also call describe on the interact
      Interact::Descriptor &Descriptor::getOverlayDescriptor(int bitDepthPerComponent, bool hasAlpha)
      {
        if(_overlayDescriptor.getState() == Interact::eUninitialised) {
          // OK, we need to describe it, set the entry point and describe away
          _overlayDescriptor.setEntryPoint(getOverlayInteractMainEntry());
          _overlayDescriptor.describe(bitDepthPerComponent, hasAlpha);
        }

        return _overlayDescriptor;
      }

      /// get the clips
      const std::map<std::string, ClipDescriptor*> &Descriptor::getClips() const {
        return _clips;
      }

      void Descriptor::addClip(const std::string &name, ClipDescriptor *clip) {
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
        { kOfxPropInstanceData,                 Property::ePointer,    1, false, NULL },
        { kOfxImageEffectPropProjectSize,       Property::eDouble,     2, true,  "0" },
        { kOfxImageEffectPropProjectOffset,     Property::eDouble,     2, true,  "0" },
        { kOfxImageEffectPropProjectExtent,     Property::eDouble,     2, true,  "0" },
        { kOfxImageEffectPropProjectPixelAspectRatio, Property::eDouble, 1, true,  "0" },
        { kOfxImageEffectInstancePropEffectDuration, Property::eDouble, 1, true,  "0" },
        { kOfxImageEffectInstancePropSequentialRender, Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropFrameRate ,        Property::eDouble,     1, true,  "0" },
        { kOfxPropIsInteractive,                Property::eInt,        1, true, "0" },
        { kOfxPropPluginDescription,            Property::eString,     1, true, ""},/*Added by Alex on 09/23/13*/
        Property::propSpecEnd
      };

      Instance::Instance(ImageEffectPlugin* plugin,
                         Descriptor         &other, 
                         const std::string  &context,
                         bool               interactive) 
        : Base(effectInstanceStuff)
        , _plugin(plugin)
        , _context(context)
        , _descriptor(&other)
        , _interactive(interactive)
        , _created(false)
        , _clipPrefsDirty(true)
        , _continuousSamples(false)
        , _frameVarying(false)
        , _outputFrameRate(24)
      {
        int i = 0;
        _properties.setChainedSet(&other.getProps());

        /*Added by Alex on 09/23/13*/
        _properties.setStringProperty(kOfxPropPluginDescription, other.getProps().getStringProperty(kOfxPropPluginDescription));
          
        _properties.setStringProperty(kOfxImageEffectPropContext,context);
        _properties.setIntProperty(kOfxPropIsInteractive,interactive);

        // copy is sequential over
        bool sequential = other.getProps().getIntProperty(kOfxImageEffectInstancePropSequentialRender) != 0;
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

      /// implemented for Param::SetDescriptor
      Property::Set &Instance::getParamSetProps()
      {
        return _properties;
      }

      /// called after construction to populate clips and params
      OfxStatus Instance::populate() 
      {        
        const std::vector<ClipDescriptor*>& clips = _descriptor->getClipsByOrder();

        int counter = 0;
        for(std::vector<ClipDescriptor*>::const_iterator it=clips.begin();
            it!=clips.end();
            ++it, ++counter) {
            const std::string &name =  (*it)->getName();
            // foreach clip descriptor make a clip instance
            ClipInstance* instance = newClipInstance(this, *it, counter);   
            if(!instance) return kOfxStatFailed;

            _clips[name] = instance;
          }        

        const std::list<Param::Descriptor*>& map = _descriptor->getParamList();

        std::map<std::string,std::vector<Param::Instance*> > parameters;
        std::map<std::string, Param::Instance*> groups;

        for(std::list<Param::Descriptor*>::const_iterator it=map.begin();
            it!=map.end();
            ++it) {
            Param::Descriptor* descriptor = (*it);
            // get the param descriptor
            if(!descriptor) return kOfxStatErrValue;

            // name of the parameter
            std::string name = descriptor->getName();

            // get a param instance from a param descriptor
            Param::Instance* instance = newParam(name,*descriptor);
            if(!instance) return kOfxStatFailed;
          
            // add the value into the param set instance
            OfxStatus st = addParam(name,instance);
            if(st != kOfxStatOK) return st;

            std::string parent = instance->getParentName();
          
            if(parent!="")
              parameters[parent].push_back(instance);

            if(instance->getType()==kOfxParamTypeGroup){
              groups[instance->getName()]=instance;
            }
          }

        // for each group parameter made
        for(std::map<std::string, Param::Instance*>::iterator it=groups.begin();
            it!=groups.end();
            ++it) {
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
      int Instance::getDimension(const std::string &name) const OFX_EXCEPTION_SPEC {
        printf("failing in %s with name=%s\n", __PRETTY_FUNCTION__, name.c_str());
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      int Instance::upperGetDimension(const std::string &name) {
        return _properties.getDimension(name);
      }

      void Instance::notify(const std::string &/*name*/, bool /*singleValue*/, int /*indexOrN*/) OFX_EXCEPTION_SPEC
      { 
        printf("failing in %s\n", __PRETTY_FUNCTION__);
      }

      // don't know what to do
      void Instance::reset(const std::string &/*name*/) OFX_EXCEPTION_SPEC {
        printf("failing in %s\n", __PRETTY_FUNCTION__);
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      // get the virutals for viewport size, pixel scale, background colour
      double Instance::getDoubleProperty(const std::string &name, int index) const OFX_EXCEPTION_SPEC
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

      void Instance::getDoublePropertyN(const std::string &name, double* first, int n) const OFX_EXCEPTION_SPEC
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
        
        /// clobber my clips
        std::map<std::string, ClipInstance*>::iterator i;
        for(i = _clips.begin(); i != _clips.end(); ++i) {
          if(i->second)
            delete i->second;
          i->second = NULL;
        }
      }

      /// this is used to populate with any extra action in argumnents that may be needed
      void Instance::setCustomInArgs(const std::string &/*action*/, Property::Set &/*inArgs*/)
      {
      }
      
      /// this is used to populate with any extra action out argumnents that may be needed
      void Instance::setCustomOutArgs(const std::string &/*action*/, Property::Set &/*outArgs*/)
      {
      }

      /// this is used to populate with any extra action out argumnents that may be needed
      void Instance::examineOutArgs(const std::string &/*action*/, OfxStatus, const Property::Set &/*outArgs*/)
      {
      }

      /// check for connection
      bool Instance::checkClipConnectionStatus() const
      {
        std::map<std::string, ClipInstance*>::const_iterator i;      
        for(i = _clips.begin(); i != _clips.end(); ++i) {
          if(!i->second->isOptional() && !i->second->getConnected()) {
            return false;
          }
        }
        return true;
      }

      // override this to make processing abort, return 1 to abort processing
      int Instance::abort() { 
        return 0; 
      }

      // override this to use your own memory instance - must inherrit from memory::instance
      Memory::Instance* Instance::newMemoryInstance(size_t /*nBytes*/) {
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

      // call the effect entry point
      OfxStatus Instance::mainEntry(const char *action, 
                                    const void *handle, 
                                    Property::Set *inArgs,
                                    Property::Set *outArgs)
      {
        if(_plugin){
          PluginHandle* pHandle = _plugin->getPluginHandle();
          if(pHandle){
            OfxPlugin* ofxPlugin = pHandle->getOfxPlugin();
            if(ofxPlugin){
              
              OfxPropertySetHandle inHandle = 0;
              if(inArgs) {
                setCustomInArgs(action, *inArgs);
                inHandle = inArgs->getHandle();
              }
              
              OfxPropertySetHandle outHandle = 0;
              if(outArgs) {
                setCustomOutArgs(action, *outArgs);
                outHandle = outArgs->getHandle();
              }
                
              OfxStatus stat = ofxPlugin->mainEntry(action, handle, inHandle, outHandle);

              if(outArgs) 
                examineOutArgs(action, stat, *outArgs);

              return stat;
            }
            return kOfxStatFailed;
          }
          return kOfxStatFailed;
        }
        return kOfxStatFailed;
      }

      // get the nth clip, in order of declaration
      ClipInstance* Instance::getNthClip(int index)
      {
        const std::string name = _descriptor->getClipsByOrder()[index]->getName();
        return _clips[name];
      }

      ClipInstance* Instance::getClip(const std::string& name) const {
        std::map<std::string,ClipInstance*>::const_iterator it = _clips.find(name);
        if(it!=_clips.end()){
          return it->second;
        }
        return 0;
      }

      // create a clip instance
      OfxStatus Instance::createInstanceAction() 
      {
        /// we need to init the clips before we call create instance incase
        /// they try and fetch something in create instance, which they are allowed
        setDefaultClipPreferences();

        // now tell the plug-in to create instance
        OfxStatus st = mainEntry(kOfxActionCreateInstance,this->getHandle(),0,0);

        if (st == kOfxStatOK) {
          _created = true;
        }

        return st;
      }

      // begin/change/end instance changed
      OfxStatus Instance::beginInstanceChangedAction(const std::string & why)
      {
        Property::PropSpec stuff[] = {
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          Property::propSpecEnd
        };

        Property::Set inArgs(stuff);

        return mainEntry(kOfxActionBeginInstanceChanged,this->getHandle(), &inArgs, 0);
      }

      OfxStatus Instance::paramInstanceChangedAction(const std::string & paramName,
                                                     const std::string & why,
                                                     OfxTime     time,
                                                     OfxPointD   renderScale)
      {        
        Param::Instance* param = getParam(paramName);

        if(isClipPreferencesSlaveParam(paramName))
          _clipPrefsDirty = true;

        if (!param) {
          return kOfxStatFailed;
        }

        Property::PropSpec stuff[] = {
          { kOfxPropType, Property::eString, 1, true, kOfxTypeParameter },
          { kOfxPropName, Property::eString, 1, true, paramName.c_str() },
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          Property::propSpecEnd
        };

        Property::Set inArgs(stuff);

        // add the second dimension of the render scale
        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);
        
        return mainEntry(kOfxActionInstanceChanged,this->getHandle(), &inArgs, 0);
      }

      OfxStatus Instance::clipInstanceChangedAction(const std::string & clipName,
                                                    const std::string & why,
                                                    OfxTime     time,
                                                    OfxPointD   renderScale)
      {
        _clipPrefsDirty = true;
        std::map<std::string,ClipInstance*>::iterator it=_clips.find(clipName);
        if(it!=_clips.end())
          return (it->second)->instanceChangedAction(why,time,renderScale);
        else
          return kOfxStatFailed;
      }

      OfxStatus Instance::endInstanceChangedAction(const std::string & why)
      {
        Property::PropSpec whyStuff[] = {
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          Property::propSpecEnd
        };

        Property::Set inArgs(whyStuff);

        return mainEntry(kOfxActionEndInstanceChanged,this->getHandle(), &inArgs, 0);
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
                                            OfxPointD   renderScale) {
        Property::PropSpec stuff[] = {
          { kOfxImageEffectPropFrameRange, Property::eDouble, 2, true, "0" },
          { kOfxImageEffectPropFrameStep, Property::eDouble, 1, true, "0" }, 
          { kOfxPropIsInteractive, Property::eInt, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          Property::propSpecEnd
        };

        Property::Set inArgs(stuff);

        // set up second dimension for frame range and render scale
        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,startFrame, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,endFrame, 1);

        inArgs.setDoubleProperty(kOfxImageEffectPropFrameStep,step);

        inArgs.setDoubleProperty(kOfxPropIsInteractive,interactive);

        inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);

        return  mainEntry(kOfxImageEffectActionBeginSequenceRender, this->getHandle(), &inArgs, 0);        
      }

      OfxStatus Instance::renderAction(OfxTime      time,
                                       const std::string &  field,
                                       const OfxRectI    &renderRoI,
                                       OfxPointD   renderScale
#ifdef OFX_EXTENSIONS_VEGAS
                                       ,
                                       int view,
                                       int nViews
#endif
                                       ) {
        Property::PropSpec stuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropFieldToRender, Property::eString, 1, true, "" }, 
          { kOfxImageEffectPropRenderWindow, Property::eInt, 4, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
#ifdef OFX_EXTENSIONS_VEGAS
          { kOfxImageEffectPropRenderView, Property::eInt, 1, true, "0" },
          { kOfxImageEffectPropViewsToRender, Property::eInt, 1, true, "1" },
#endif
          Property::propSpecEnd
        };

        Property::Set inArgs(stuff);
        
        inArgs.setStringProperty(kOfxImageEffectPropFieldToRender,field);
        inArgs.setDoubleProperty(kOfxPropTime,time);
        inArgs.setIntPropertyN(kOfxImageEffectPropRenderWindow, &renderRoI.x1, 4);
        inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);
#ifdef OFX_EXTENSIONS_VEGAS
        inArgs.setIntProperty(kOfxImageEffectPropRenderView,view);
        inArgs.setIntProperty(kOfxImageEffectPropViewsToRender,nViews);
#endif


        return mainEntry(kOfxImageEffectActionRender,this->getHandle(), &inArgs, 0);        
      }

      OfxStatus Instance::endRenderAction(OfxTime  startFrame,
                                          OfxTime  endFrame,
                                          OfxTime  step,
                                          bool     interactive,
                                          OfxPointD   renderScale) {
        Property::PropSpec stuff[] = {
          { kOfxImageEffectPropFrameRange, Property::eDouble, 2, true, "0" },
          { kOfxImageEffectPropFrameStep, Property::eDouble, 1, true, "0" }, 
          { kOfxPropIsInteractive, Property::eInt, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          Property::propSpecEnd
        };

        Property::Set inArgs(stuff);

        inArgs.setDoubleProperty(kOfxImageEffectPropFrameStep,step);

        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,startFrame, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropFrameRange,endFrame, 1);
        inArgs.setDoubleProperty(kOfxPropIsInteractive,interactive);
        inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);

        return mainEntry(kOfxImageEffectActionEndSequenceRender,this->getHandle(), &inArgs, 0);        
      }

      /// calculate the default rod for this effect instance
      OfxRectD Instance::calcDefaultRegionOfDefinition(OfxTime  time,
                                                       OfxPointD   /*renderScale*/)
      {
        OfxRectD rod;

        // figure out the default contexts
        if(_context == kOfxImageEffectContextGenerator) {
          // generator is the extent
          rod.x1 = rod.y1 = 0;
          getProjectExtent(rod.x2, rod.y2);
        }                                     
        else if(_context == kOfxImageEffectContextFilter ||
                _context == kOfxImageEffectContextPaint) {
          // filter and paint default to the input clip
          ClipInstance *clip = getClip(kOfxImageEffectSimpleSourceClipName);
          if(clip) {
            rod = clip->getRegionOfDefinition(time);
          }
        }
        else if(_context == kOfxImageEffectContextTransition) {
          // transition is the union of the two clips
          ClipInstance *clipFrom = getClip(kOfxImageEffectTransitionSourceFromClipName);
          ClipInstance *clipTo = getClip(kOfxImageEffectTransitionSourceToClipName);
          if(clipFrom && clipTo) {
            rod = clipFrom->getRegionOfDefinition(time);
            rod = Union(rod, clipTo->getRegionOfDefinition(time));
          }
        }
        else if(_context == kOfxImageEffectContextGeneral) {
          // general context is the union of all the non optional clips
          bool gotOne = false;
          for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
              it!=_clips.end();
              ++it) {
            ClipInstance *clip = it->second;
            if(!clip->isOutput() && !clip->isOptional()) {
              if(!gotOne)
                rod = clip->getRegionOfDefinition(time);
              else
                rod = Union(rod, clip->getRegionOfDefinition(time));
              gotOne = true;
            }
          }
            
          if(!gotOne) {
            /// no non optionals? then be the extent
            rod.x1 = rod.y1 = 0;
            getProjectExtent(rod.x2, rod.y2);
          }
            
        }
        else if(_context == kOfxImageEffectContextRetimer) {
          // retimer
          ClipInstance *clip = getClip(kOfxImageEffectSimpleSourceClipName);
          if(clip) {
            Param::DoubleInstance *param = dynamic_cast<Param::DoubleInstance *>(getParam(kOfxImageEffectRetimerParamName));
            if(param) {
              rod = clip->getRegionOfDefinition(floor(time));
              rod = Union(rod, clip->getRegionOfDefinition(floor(time) + 1));
            }
          }
        }

        return rod;
      }

      ////////////////////////////////////////////////////////////////////////////////
      // RoD call
      OfxStatus Instance::getRegionOfDefinitionAction(OfxTime  time,
                                                      OfxPointD   renderScale,
                                                      OfxRectD &rod)
      {
        Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          Property::propSpecEnd
        };

        Property::PropSpec outStuff[] = {
          { kOfxImageEffectPropRegionOfDefinition , Property::eDouble, 4, false, "0" },
          Property::propSpecEnd
        };

        Property::Set inArgs(inStuff);
        Property::Set outArgs(outStuff);
        
        inArgs.setDoubleProperty(kOfxPropTime,time);

        inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);

        OfxStatus stat = mainEntry(kOfxImageEffectActionGetRegionOfDefinition,
                                   this->getHandle(),
                                   &inArgs,
                                   &outArgs);

        if(stat == kOfxStatOK) {
          outArgs.getDoublePropertyN(kOfxImageEffectPropRegionOfDefinition, &rod.x1, 4);
        }
        else if(stat == kOfxStatReplyDefault) {
          rod = calcDefaultRegionOfDefinition(time, renderScale);
        }        

        return stat;
      }

      /// get the region of interest for each input and return it in the given std::map
      OfxStatus Instance::getRegionOfInterestAction(OfxTime  time,
                                                    OfxPointD   renderScale,
                                                    const OfxRectD &roi,
                                                    std::map<ClipInstance *, OfxRectD>& rois) 
      {
        OfxStatus stat = kOfxStatReplyDefault;

        // reset the map
        rois.clear();

        if(!supportsTiles()) {
          /// No tiling support on the effect at all. So set the roi of each input clip to be the RoD of that clip.
          for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
              it!=_clips.end();
              ++it) {
            if(!it->second->isOutput()) {
              OfxRectD roi = it->second->getRegionOfDefinition(time);
              rois[it->second] = roi;
            }
          }
          stat = kOfxStatOK;
        }
        else {
          /// set up the in args 
          static Property::PropSpec inStuff[] = {
            { kOfxPropTime, Property::eDouble, 1, true, "0" },
            { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
            { kOfxImageEffectPropRegionOfInterest , Property::eDouble, 4, true, 0 },
            Property::propSpecEnd
          };
          Property::Set inArgs(inStuff);

          inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);
          inArgs.setDoubleProperty(kOfxPropTime,time);
          inArgs.setDoublePropertyN(kOfxImageEffectPropRegionOfInterest, &roi.x1, 4);

          Property::Set outArgs;
          for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
              it!=_clips.end();
              ++it) {
            if(!it->second->isOutput()) {
              Property::PropSpec s;
              std::string name = "OfxImageClipPropRoI_"+it->first;
            
              s.name = name.c_str();
              s.type = Property::eDouble;
              s.dimension = 4;
              s.readonly = false;
              s.defaultValue = "";
              outArgs.createProperty(s);
            
              /// initialise to the default
              outArgs.setDoublePropertyN(s.name, &roi.x1, 4);
            }
          }

          /// call the action
          stat = mainEntry(kOfxImageEffectActionGetRegionsOfInterest,
                           this->getHandle(),
                           &inArgs,
                           &outArgs);

          /// set the thing up
          for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
              it!=_clips.end();
              ++it) {
              if(!it->second->isOutput()) {
                OfxRectD rod = it->second->getRegionOfDefinition(time);
                if(it->second->supportsTiles()) {
                  std::string name = "OfxImageClipPropRoI_"+it->first;
                  OfxRectD thisRoi;
                  thisRoi.x1 = outArgs.getDoubleProperty(name,0);
                  thisRoi.y1 = outArgs.getDoubleProperty(name,1);
                  thisRoi.x2 = outArgs.getDoubleProperty(name,2);
                  thisRoi.y2 = outArgs.getDoubleProperty(name,3);
                  
                  /// and clamp it to the clip's rod
                  thisRoi = Clamp(thisRoi, rod);
                  rois[it->second] = thisRoi;
                }
                else {
                  /// not supporting tiles on this input, so set it to the rod
                  rois[it->second] = rod;
                }
              }
            }
        }
  
        return stat;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// see how many frames are needed from each clip to render the indicated frame
      OfxStatus Instance::getFrameNeededAction(OfxTime time, 
                                               RangeMap &rangeMap)
      {
        OfxStatus stat = kOfxStatReplyDefault;
        Property::Set outArgs;
      
        if(temporalAccess()) {
          Property::PropSpec inStuff[] = {
            { kOfxPropTime, Property::eDouble, 1, true, "0" },          
            Property::propSpecEnd
          };
          Property::Set inArgs(inStuff);       
          inArgs.setDoubleProperty(kOfxPropTime,time);
        
        
          for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
              it!=_clips.end();
              ++it) {
            if(!it->second->isOutput()) {
              Property::PropSpec s;
              std::string name = "OfxImageClipPropFrameRange_"+it->first;
              
              s.name = name.c_str();
              s.type = Property::eDouble;
              s.dimension = 0;
              s.readonly = false;
              s.defaultValue = "";
              outArgs.createProperty(s);
              /// intialise it to the current frame
              outArgs.setDoubleProperty(name, time, 0);
              outArgs.setDoubleProperty(name, time, 1);
            }
          }

          stat = mainEntry(kOfxImageEffectActionGetFramesNeeded,
                           this->getHandle(),
                           &inArgs,
                           &outArgs);
        }
        
        OfxRangeD defaultRange;
        defaultRange.min = 
          defaultRange.max = time;

        for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
            it!=_clips.end();
            ++it) {
          ClipInstance *clip = it->second;
          
          if(!clip->isOutput()) {
            if(stat != kOfxStatOK) {
              rangeMap[clip].push_back(defaultRange);
            }
            else {
              std::string name = "OfxImageClipPropFrameRange_"+it->first;
          
              int nRanges = outArgs.getDimension(name);
              if(nRanges%2 != 0)
                return kOfxStatFailed; // bad! needs to be divisible by 2

              if(nRanges == 0) {
                rangeMap[clip].push_back(defaultRange);
              }
              else {
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
          }
        }

        return stat;
      }

      OfxStatus Instance::isIdentityAction(OfxTime     &time,
                                           const std::string &  field,
                                           const OfxRectI &renderRoI,
                                           OfxPointD   renderScale,
                                           std::string &clip)
      {
        static Property::PropSpec inStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropFieldToRender, Property::eString, 1, true, "" }, 
          { kOfxImageEffectPropRenderWindow, Property::eInt, 4, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          Property::propSpecEnd
        };

        static Property::PropSpec outStuff[] = {
          { kOfxPropTime, Property::eDouble, 1, false, "0.0" },
          { kOfxPropName, Property::eString, 1, false, "" },
          Property::propSpecEnd
        };

        Property::Set inArgs(inStuff);        

        inArgs.setStringProperty(kOfxImageEffectPropFieldToRender,field);
        inArgs.setDoubleProperty(kOfxPropTime,time);
        inArgs.setIntPropertyN(kOfxImageEffectPropRenderWindow, &renderRoI.x1, 4);
        inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);

        Property::Set outArgs(outStuff);

        OfxStatus st = mainEntry(kOfxImageEffectActionIsIdentity,
                                 this->getHandle(),
                                 &inArgs,
                                 &outArgs);        

        if(st==kOfxStatOK){
          time = outArgs.getDoubleProperty(kOfxPropTime);
          clip = outArgs.getStringProperty(kOfxPropName);        
        }
        
        return st;
      }

      /// Get whether the component is a supported 'chromatic' component (RGBA or alpha) in
      /// the base API.
      /// Override this if you have extended your chromatic colour types (eg RGB) and want
      /// the clip preferences logic to still work
      bool Instance::isChromaticComponent(const std::string &str) const
      {
        if(str == kOfxImageComponentRGBA)
          return true;
        if(str == kOfxImageComponentRGB)
          return true;
        if(str == kOfxImageComponentAlpha)
          return true;
        return false;
      }
      
      /// function to check for multiple bit depth support
      /// The answer will depend on host, plugin and context
      bool Instance::canCurrentlyHandleMultipleClipDepths() const
      {
        /// does the host support 'em
        bool hostSupports = gImageEffectHost->getProperties().getIntProperty(kOfxImageEffectPropSupportsMultipleClipDepths) != 0;;

        /// does the plug-in support 'em
        bool pluginSupports = supportsMultipleClipDepths();

        /// no support, so no
        if(!hostSupports || !pluginSupports)
          return false;

        /// if filter context, no support, tempted to change this though
        if(_context == kOfxImageEffectContextFilter)
          return false;

        /// if filter context, no support
        if(_context == kOfxImageEffectContextGenerator ||
           _context == kOfxImageEffectContextTransition ||
           _context == kOfxImageEffectContextPaint ||
           _context == kOfxImageEffectContextRetimer)
          return false;
        
        /// OK we're cool
        return true;
      }
      
      /// Setup the default clip preferences on the clips
      void Instance::setDefaultClipPreferences()
      {
        /// is there multiple bit depth support? Depends on host, plugin and context
        bool multiBitDepth = canCurrentlyHandleMultipleClipDepths();

        /// OK find the deepest chromatic component on our input clips and the one with the
        /// most components
          bool hasInputs = false;
          
        std::string deepestBitDepth = kOfxBitDepthNone;
        std::string mostComponents  = kOfxImageComponentNone;       
        double frameRate = 0; 
        std::string premult = kOfxImageOpaque;
        for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
            it!=_clips.end();
            ++it) {
          ClipInstance *clip = it->second;

          if(!clip->isOutput()) {
              hasInputs = true;
            frameRate = Maximum(frameRate, clip->getFrameRate());

            std::string rawComp  = clip->getUnmappedComponents(); 
            rawComp = clip->findSupportedComp(rawComp); // turn that into a comp the plugin expects on that clip

            const std::string &rawDepth = clip->getUnmappedBitDepth();
            const std::string &rawPreMult = clip->getPremult();            
              
            if(isChromaticComponent(rawComp)) {
              if(rawPreMult == kOfxImagePreMultiplied)
                premult = kOfxImagePreMultiplied;
              else if(rawPreMult == kOfxImageUnPreMultiplied && premult != kOfxImagePreMultiplied)
                premult = kOfxImageUnPreMultiplied;                
                
              deepestBitDepth = FindDeepestBitDepth(deepestBitDepth, rawDepth);
              mostComponents  = findMostChromaticComponents(mostComponents, rawComp);
            }
          }
        }

        /// set some stuff up
        _outputFrameRate           = frameRate;
        _outputFielding            = getDefaultOutputFielding();
        _outputPreMultiplication   = premult;
        _continuousSamples         = false;
        _frameVarying              = false;

        /// now find the best depth that the plugin supports
        deepestBitDepth = bestSupportedDepth(deepestBitDepth);
          
          /*edit on 08/29/13 default value when there's no inputs for a plugin*/
          if(!hasInputs){
              mostComponents = kOfxImageComponentRGBA;
              deepestBitDepth = kOfxBitDepthFloat;
          }

        /// now add the clip gubbins to the out args
        for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
            it!=_clips.end();
            ++it) {
          ClipInstance *clip = it->second;

          std::string comp, depth;

          std::string rawComp  = clip->getUnmappedComponents();
          rawComp = clip->findSupportedComp(rawComp); // turn that into a comp the plugin expects on that clip
          const std::string &rawDepth = clip->getUnmappedBitDepth();

          if(isChromaticComponent(rawComp)) {
                
            if(clip->isOutput()) {
              depth = deepestBitDepth;
              comp = clip->findSupportedComp(mostComponents);
                  
              clip->setPixelDepth(depth);
              clip->setComponents(comp);
            }
            else {                 
              comp  = rawComp;
              depth = multiBitDepth ? bestSupportedDepth(rawDepth) : deepestBitDepth;
                  
              clip->setPixelDepth(depth);
              clip->setComponents(comp);
            }
          }
          else {
            /// hmm custom component type, don't touch it and pass it through
            clip->setPixelDepth(rawDepth);
            clip->setComponents(rawComp);
          }
        }
      }

      /// Initialise the clip preferences arguments, override this to do
      /// stuff with wierd components etc...
      void Instance::setupClipPreferencesArgs(Property::Set &outArgs)
      {
        /// reset all the clip prefs stuff to their defaults
        setDefaultClipPreferences();

        static Property::PropSpec clipPrefsStuffs []= 
          {
            { kOfxImageEffectPropFrameRate,          Property::eDouble,  1, false,  "1" },
            { kOfxImageEffectPropPreMultiplication,  Property::eString,  1, false,  "" },
            { kOfxImageClipPropFieldOrder,           Property::eString,  1, false,  "" },
            { kOfxImageClipPropContinuousSamples,    Property::eInt,     1, false,  "0" },
            { kOfxImageEffectFrameVarying,           Property::eInt,     1, false,  "0" },
            Property::propSpecEnd
          };
        
        outArgs.addProperties(clipPrefsStuffs);
        
        /// set the default for those

        /// is there multiple bit depth support? Depends on host, plugin and context
        bool multiBitDepth = canCurrentlyHandleMultipleClipDepths();

        outArgs.setStringProperty(kOfxImageClipPropFieldOrder, _outputFielding);
        outArgs.setStringProperty(kOfxImageEffectPropPreMultiplication, _outputPreMultiplication);
        outArgs.setDoubleProperty(kOfxImageEffectPropFrameRate, _outputFrameRate);

        /// now add the clip gubbins to the out args
        for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
            it!=_clips.end();
            ++it) {
          ClipInstance *clip = it->second;

          std::string componentParamName = "OfxImageClipPropComponents_"+it->first;
          std::string depthParamName     = "OfxImageClipPropDepth_"+it->first;
          std::string parParamName       = "OfxImageClipPropPAR_"+it->first;

          Property::PropSpec specComp = {componentParamName.c_str(),  Property::eString, 0, false,          ""}; // note the support for multi-planar clips
          outArgs.createProperty(specComp);
          outArgs.setStringProperty(componentParamName.c_str(), clip->getComponents().c_str()); // as it is variable dimension, there is no default value, so we have to set it explicitly

          Property::PropSpec specDep = {depthParamName.c_str(),       Property::eString, 1, !multiBitDepth, clip->getPixelDepth().c_str()};
          outArgs.createProperty(specDep);

          Property::PropSpec specPAR = {parParamName.c_str(),         Property::eDouble, 1, false,          "1"};
          outArgs.createProperty(specPAR);
        }
      }

      /// the idea here is the clip prefs live as active props on the effect
      /// and are set up by clip preferences. The action manages the clip
      /// preferences bits. We also monitor clip and param changes and
      /// flag when clip prefs is dirty.      
      /// call the clip preferences action
      bool Instance::getClipPreferences()
      {      
        /// create the out args with the stuff that does not depend on individual clips
        Property::Set outArgs;

        setupClipPreferencesArgs(outArgs);

        OfxStatus st = mainEntry(kOfxImageEffectActionGetClipPreferences,
                                 this->getHandle(),
                                 0,
                                 &outArgs);

        if(st!=kOfxStatOK && st!=kOfxStatReplyDefault) {
          /// ouch
          return false;
        }

        /// OK, go pump the components/depths back into the clips themselves
        for(std::map<std::string, ClipInstance*>::iterator it=_clips.begin();
            it!=_clips.end();
            ++it) {
            ClipInstance *clip = it->second;

            std::string componentParamName = "OfxImageClipPropComponents_"+it->first;
            std::string depthParamName = "OfxImageClipPropDepth_"+it->first;
            std::string parParamName = "OfxImageClipPropPAR_"+it->first;


            clip->setPixelDepth(outArgs.getStringProperty(depthParamName));
            clip->setComponents(outArgs.getStringProperty(componentParamName));
            //            clip->setPixelAspect(outArgs.getStringProperty(componentParamName));
          }

        _outputFrameRate           = outArgs.getDoubleProperty(kOfxImageEffectPropFrameRate);
        _outputFielding            = outArgs.getStringProperty(kOfxImageClipPropFieldOrder);
        _outputPreMultiplication   = outArgs.getStringProperty(kOfxImageEffectPropPreMultiplication);
        _continuousSamples = outArgs.getIntProperty(kOfxImageClipPropContinuousSamples) != 0;
        _frameVarying      = outArgs.getIntProperty(kOfxImageEffectFrameVarying) != 0;

        _clipPrefsDirty  = false;

        return true;
      }

      /// find the most chromatic components out of the two. Override this if you define
      /// more chromatic components
      const std::string &Instance::findMostChromaticComponents(const std::string &a, const std::string &b) const
      {
        if(a == kOfxImageComponentNone)
          return b;
        if(a == kOfxImageComponentRGBA)
          return a;
        if(b == kOfxImageComponentRGBA)
          return b;
        if(a == kOfxImageComponentRGB)
          return a;
        if(b == kOfxImageComponentRGB)
          return b;
        return a;
      }

      /// given the bit depth, find the best match for it.
      const std::string &Instance::bestSupportedDepth(const std::string &depth) const
      {
        static const std::string none(kOfxBitDepthNone);
        static const std::string bytes(kOfxBitDepthByte);
        static const std::string shorts(kOfxBitDepthShort);
        static const std::string floats(kOfxBitDepthFloat);

        if(depth == none)
          return none;

        if(isPixelDepthSupported(depth))
          return depth;
        
        if(depth == floats) {
          if(isPixelDepthSupported(shorts))
            return shorts;
          if(isPixelDepthSupported(bytes))
            return bytes;
        }
          
        if(depth == shorts) {
          if(isPixelDepthSupported(floats))
            return floats;
          if(isPixelDepthSupported(bytes))
            return bytes;
        }

        if(depth == bytes) {
          if(isPixelDepthSupported(shorts))
            return shorts;
          if(isPixelDepthSupported(floats))
            return floats;
        }
        
        /// WTF? Something wrong here
        return none;          
      }


      /// get the interact description, this will also call describe on the interact
      Interact::Descriptor &Instance::getOverlayDescriptor(int bitDepthPerComponent, bool hasAlpha)
      {
        return _descriptor->getOverlayDescriptor(bitDepthPerComponent, hasAlpha);
      }

      OfxStatus Instance::getTimeDomainAction(OfxRangeD& range)
      {
        Property::PropSpec outStuff[] = {
          { kOfxImageEffectPropFrameRange , Property::eDouble, 2, false, "0.0" },
          Property::propSpecEnd
        };

        Property::Set outArgs(outStuff);  
        outArgs.setDoubleProperty(kOfxImageEffectPropFrameRange, range.min, 0);
        outArgs.setDoubleProperty(kOfxImageEffectPropFrameRange, range.max, 1);

        OfxStatus st = mainEntry(kOfxImageEffectActionGetTimeDomain,
                                 this->getHandle(),
                                 0,
                                 &outArgs);
        if(st!=kOfxStatOK) return st;

        range.min = outArgs.getDoubleProperty(kOfxImageEffectActionGetTimeDomain,0);
        range.max = outArgs.getDoubleProperty(kOfxImageEffectActionGetTimeDomain,1);

        return kOfxStatOK;
      }

      
      /// implemented for Param::SetInstance
      void Instance::paramChangedByPlugin(Param::Instance *param)
      {
        double frame  = getFrameRecursive();
        OfxPointD renderScale; getRenderScaleRecursive(renderScale.x, renderScale.y);

        beginInstanceChangedAction(kOfxChangePluginEdited);
        paramInstanceChangedAction(param->getName(), kOfxChangePluginEdited, frame, renderScale);
        endInstanceChangedAction(kOfxChangePluginEdited);
      }

      ////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////
      /// The image effect suite functions
      static OfxStatus getPropertySet(OfxImageEffectHandle h1, 
                                      OfxPropertySetHandle *h2)
      {        
        Base *effectBase = reinterpret_cast<Base*>(h1);

        if (!effectBase || !effectBase->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        *h2 = effectBase->getProps().getHandle();
        return kOfxStatOK;
      }

      static OfxStatus getParamSet(OfxImageEffectHandle h1, 
                                   OfxParamSetHandle *h2)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(h1);

        if (!effectBase || !effectBase->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        ImageEffect::Descriptor *effectDescriptor = dynamic_cast<ImageEffect::Descriptor*>(effectBase);

        if(effectDescriptor) {
          *h2 = effectDescriptor->getParamSetHandle();
          return kOfxStatOK;
        }

        ImageEffect::Instance *effectInstance = dynamic_cast<ImageEffect::Instance*>(effectBase);

        if(effectInstance) {
          *h2 = effectInstance->getParamSetHandle();
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle;
      }
      
      static OfxStatus clipDefine(OfxImageEffectHandle h1, 
                                  const char *name, 
                                  OfxPropertySetHandle *h2)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(h1);
        
        if (!effectBase || !effectBase->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        ImageEffect::Descriptor *effectDescriptor = dynamic_cast<ImageEffect::Descriptor*>(effectBase);
        
        if(effectDescriptor){ 
          ClipDescriptor *clip = effectDescriptor->defineClip(name);
          *h2 = clip->getPropHandle();        
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle;
      }
      
      static OfxStatus clipGetPropertySet(OfxImageClipHandle clip,
                                          OfxPropertySetHandle *propHandle){        
        ClipInstance *clipInstance = reinterpret_cast<ClipInstance*>(clip);

        if (!clipInstance || !clipInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        if(clipInstance) {
          *propHandle = clipInstance->getPropHandle();
          return kOfxStatOK;
        }
        
        return kOfxStatErrBadHandle;
      }
      
      static OfxStatus clipGetImage(OfxImageClipHandle h1, 
                                    OfxTime time, 
                                    OfxRectD *h2, 
                                    OfxPropertySetHandle *h3)
      {
        ClipInstance *clipInstance = reinterpret_cast<ClipInstance*>(h1);

        if (!clipInstance || !clipInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        if(clipInstance){
          Image* image = clipInstance->getImage(time,h2);
          if(!image) {
            h3 = NULL;
            return kOfxStatFailed;
          }

          *h3 = image->getPropHandle();

          return kOfxStatOK;
        }
        
        return kOfxStatErrBadHandle;
      }

#ifdef OFX_EXTENSIONS_VEGAS
      static OfxStatus clipGetStereoscopicImage(OfxImageClipHandle h1,
                                                OfxTime time,
                                                int view,
                                                OfxRectD *h2,
                                                OfxPropertySetHandle *h3)
      {
        ClipInstance *clipInstance = reinterpret_cast<ClipInstance*>(h1);

        if (!clipInstance || !clipInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        if(clipInstance){
          Image* image = clipInstance->getStereoscopicImage(time,view,h2);
          if(!image) {
            h3 = NULL;
            return kOfxStatFailed;
          }

          *h3 = image->getPropHandle();

          return kOfxStatOK;
        }
        
        return kOfxStatErrBadHandle;
      }
#endif
      
      static OfxStatus clipReleaseImage(OfxPropertySetHandle h1)
      {
        Property::Set *pset = reinterpret_cast<Property::Set*>(h1);

        if (!pset || !pset->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        Image *image = dynamic_cast<Image*>(pset);

        if(image){
          // clip::image has a virtual destructor for derived classes
          image->releaseReference();
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrBadHandle;
      }
      
      static OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect,
                                     const char *name,
                                     OfxImageClipHandle *clip,
                                     OfxPropertySetHandle *propertySet)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(imageEffect);

        if (!effectBase || !effectBase->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        ImageEffect::Instance *effectInstance = reinterpret_cast<ImageEffect::Instance*>(effectBase);

        if(effectInstance){
          ClipInstance* instance = effectInstance->getClip(name);
          *clip = instance->getHandle();
          if(propertySet)
            *propertySet = instance->getPropHandle();
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle;
      }
      
      static OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip,
                                                 OfxTime time,
                                                 OfxRectD *bounds)
      {
        ClipInstance *clipInstance = reinterpret_cast<ClipInstance*>(clip);

        if (!clipInstance || !clipInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        if(clipInstance) {
          *bounds = clipInstance->getRegionOfDefinition(time);
          return kOfxStatOK;
        }
         
        return kOfxStatErrBadHandle;
      }

      // should processing be aborted?
      static int abort(OfxImageEffectHandle imageEffect)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(imageEffect);

        if (!effectBase || !effectBase->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        ImageEffect::Instance *effectInstance = dynamic_cast<ImageEffect::Instance*>(effectBase);

        if(effectInstance) 
          return effectInstance->abort();
        else 
          return kOfxStatErrBadHandle;        
      }
      
      static OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
                                        size_t nBytes,
                                        OfxImageMemoryHandle *memoryHandle)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(instanceHandle);
        ImageEffect::Instance *effectInstance = reinterpret_cast<ImageEffect::Instance*>(effectBase);
        Memory::Instance* memory;

        if(effectInstance){

          if (!effectInstance->verifyMagic()) {
            return kOfxStatErrBadHandle;
          }

          memory = effectInstance->imageMemoryAlloc(nBytes);
        }
        else {
          memory = new Memory::Instance;
          memory->alloc(nBytes);
        }

        *memoryHandle = memory->getHandle();
        return kOfxStatOK;
      }
      
      static OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle){
        Memory::Instance *memoryInstance = reinterpret_cast<Memory::Instance*>(memoryHandle);

        if(memoryInstance && memoryInstance->verifyMagic()) {
          memoryInstance->freeMem();
          delete memoryInstance;
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrBadHandle; 
      }
      
      OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                                void **returnedPtr){
        Memory::Instance *memoryInstance = reinterpret_cast<Memory::Instance*>(memoryHandle);

        if(memoryInstance && memoryInstance->verifyMagic()) {
          memoryInstance->lock();          
          *returnedPtr = memoryInstance->getPtr();
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle; 
      }
      
      static OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle){
        Memory::Instance *memoryInstance = reinterpret_cast<Memory::Instance*>(memoryHandle);

        if(memoryInstance && memoryInstance->verifyMagic()){
          memoryInstance->unlock();          
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle; 
      }

      static struct OfxImageEffectSuiteV1 gImageEffectSuite = {
        getPropertySet,
        getParamSet,
        clipDefine,
        clipGetHandle,
        clipGetPropertySet,
        clipGetImage,
        clipReleaseImage,
        clipGetRegionOfDefinition,
        abort,
        imageMemoryAlloc,
        imageMemoryFree,
        imageMemoryLock,
        imageMemoryUnlock
      };

#ifdef OFX_EXTENSIONS_VEGAS
      static struct OfxVegasStereoscopicImageEffectSuiteV1 gVegasStereoscopicImageEffectSuite = {
        clipGetStereoscopicImage
      };
#endif

      /// message suite function for an image effect
      static OfxStatus message(void *handle, const char *type, const char *id, const char *format, ...)
      {
        ImageEffect::Instance *effectInstance = reinterpret_cast<ImageEffect::Instance*>(handle);
        if(effectInstance){
          va_list args;
          va_start(args,format);
          effectInstance->vmessage(type,id,format,args);
          va_end(args);
        }
        else{
          va_list args;
          va_start(args,format);
          vprintf(format,args);
          va_end(args);
        }
        return kOfxStatOK;
      }

      /// message suite for an image effect plugin
      static struct OfxMessageSuiteV1 gMessageSuite = {
        message
      };


      ////////////////////////////////////////////////////////////////////////////////
      /// make an overlay interact for an image effect
      OverlayInteract::OverlayInteract(ImageEffect::Instance &effect, int bitDepthPerComponent, bool hasAlpha)
        :  Interact::Instance(effect.getOverlayDescriptor(bitDepthPerComponent, hasAlpha),
                              (void *)(effect.getHandle()))
        , _instance(effect)
      {        
      }

      ////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////
      // Progress suite functions

      /// begin progressing
      static OfxStatus ProgressStart(void *effectInstance,
                              const char *label)
      {
        Instance *me = reinterpret_cast<Instance *>(effectInstance);
        me->progressStart(label);
        return kOfxStatOK;
      }
      
      /// finish progressing
      static OfxStatus ProgressEnd(void *effectInstance)
      {
        Instance *me = reinterpret_cast<Instance *>(effectInstance);
        me->progressEnd();
        return kOfxStatOK;
      }

      /// update progressing
      static OfxStatus ProgressUpdate(void *effectInstance, double progress)
      {
        Instance *me = reinterpret_cast<Instance *>(effectInstance);
        bool v = me->progressUpdate(progress);
        return v ? kOfxStatOK : kOfxStatReplyNo;          
      }

      /// our progress suite
      struct OfxProgressSuiteV1 gProgressSuite = {
        ProgressStart,
        ProgressUpdate,
        ProgressEnd
      };


      ////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////
      // timeline suite functions

      /// timeline suite function
      static OfxStatus TimeLineGetTime(void *effectInstance, double *time)
      {
        Instance *me = reinterpret_cast<Instance *>(effectInstance);
        *time = me->timeLineGetTime();
        return kOfxStatOK;
      }

      /// timeline suite function
      static OfxStatus TimeLineGotoTime(void *effectInstance, double time)
      {
        Instance *me = reinterpret_cast<Instance *>(effectInstance);
        me->timeLineGotoTime(time);
        return kOfxStatOK;
      }
      
      /// timeline suite function
      static OfxStatus TimeLineGetBounds(void *effectInstance, double *firstTime, double *lastTime)
      {
        Instance *me = reinterpret_cast<Instance *>(effectInstance);
        me->timeLineGetBounds(*firstTime, *lastTime);
        return kOfxStatOK;
      }

      /// our progress suite
      struct OfxTimeLineSuiteV1 gTimelineSuite = {
        TimeLineGetTime,
        TimeLineGotoTime,
        TimeLineGetBounds
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// a simple multithread suite
      static OfxStatus multiThread(OfxThreadFunctionV1 func,
                                   unsigned int /*nThreads*/,
                                   void *customArg)
      {
        func(0,1,customArg);
        return kOfxStatOK;
      }

      static OfxStatus multiThreadNumCPUs(unsigned int *nCPUs)
      {
        *nCPUs = 1;
        return kOfxStatOK;
      }

      static OfxStatus multiThreadIndex(unsigned int *threadIndex){
        *threadIndex = 0;
        return kOfxStatOK;
      }

      static int multiThreadIsSpawnedThread(void){
        return false;
      }

      static OfxStatus mutexCreate(const OfxMutexHandle */*mutex*/, int /*lockCount*/)
      {
        // do nothing single threaded
        //mutex = 0;
        return kOfxStatOK;
      }

      static OfxStatus mutexDestroy(const OfxMutexHandle /*mutex*/)
      {
        // do nothing single threaded
        return kOfxStatOK;
      }

      static OfxStatus mutexLock(const OfxMutexHandle /*mutex*/){
        // do nothing single threaded
        return kOfxStatOK;
      }
       
      static OfxStatus mutexUnLock(const OfxMutexHandle /*mutex*/){
        // do nothing single threaded
        return kOfxStatOK;
      }       

      static OfxStatus mutexTryLock(const OfxMutexHandle /*mutex*/){
        // do nothing single threaded
        return kOfxStatOK;
      }
       
      static struct OfxMultiThreadSuiteV1 gSingleThreadedSuite = {
        multiThread,
        multiThreadNumCPUs,
        multiThreadIndex,
        multiThreadIsSpawnedThread,
        mutexCreate,
        mutexDestroy,
        mutexLock,
        mutexUnLock,
        mutexTryLock
      };
      

      ////////////////////////////////////////////////////////////////////////////////
      /// The image effect host

      /// properties for the image effect host
      static Property::PropSpec hostStuffs[] = {
        { kOfxImageEffectHostPropIsBackground, Property::eInt, 1, true, "0" },
        { kOfxImageEffectPropSupportsOverlays, Property::eInt, 1, true, "1" },
        { kOfxImageEffectPropSupportsMultiResolution, Property::eInt, 1, true, "1" },
        { kOfxImageEffectPropSupportsTiles, Property::eInt, 1, true, "1" },
        { kOfxImageEffectPropTemporalClipAccess, Property::eInt, 1, true, "1"  },
      
        

        /// xxx this needs defaulting manually
        { kOfxImageEffectPropSupportedComponents, Property::eString, 0, true, "" },
        /// xxx this needs defaulting manually

        { kOfxImageEffectPropSupportedPixelDepths, Property::eString, 0, true, "" },

        /// xxx this needs defaulting manually
  
        { kOfxImageEffectPropSupportedContexts, Property::eString, 0, true, "" },
        /// xxx this needs defaulting manually

        { kOfxImageEffectPropSupportsMultipleClipDepths, Property::eInt, 1, true, "1" },
        { kOfxImageEffectPropSupportsMultipleClipPARs, Property::eInt, 1, true, "0" },
        { kOfxImageEffectPropSetableFrameRate, Property::eInt, 1, true, "0" },
        { kOfxImageEffectPropSetableFielding, Property::eInt, 1, true, "0"  },
        { kOfxParamHostPropSupportsCustomInteract, Property::eInt, 1, true, "0" },
        { kOfxParamHostPropSupportsStringAnimation, Property::eInt, 1, true, "0" },
        { kOfxParamHostPropSupportsChoiceAnimation, Property::eInt, 1, true, "0"  },
        { kOfxParamHostPropSupportsBooleanAnimation, Property::eInt, 1, true, "0" },
        { kOfxParamHostPropSupportsCustomAnimation, Property::eInt, 1, true, "0" },
        { kOfxParamHostPropMaxParameters, Property::eInt, 1, true, "-1" },
        { kOfxParamHostPropMaxPages, Property::eInt, 1, true, "0" },
        { kOfxParamHostPropPageRowColumnCount, Property::eInt, 2, true, "0" },
        Property::propSpecEnd
      };    

      /// ctor
      Host::Host() 
      {
        /// add the properties for an image effect host, derived classs to set most of them
        _properties.addProperties(hostStuffs);
      }

      /// optionally over-ridden function to register the creation of a new descriptor in the host app
      void Host::initDescriptor(Descriptor* /*desc*/)
      {
      }

      /// Use this in any dialogue etc... showing progress
      void Host::loadingStatus(const std::string &)
      {
      }

      bool Host::pluginSupported(ImageEffectPlugin */*plugin*/, std::string &/*reason*/) const
      {
        return true;
      }

      /// our suite fetcher
      void *Host::fetchSuite(const char *suiteName, int suiteVersion)
      {
        if (strcmp(suiteName, kOfxImageEffectSuite)==0) {
          if(suiteVersion==1)
            return (void *)&gImageEffectSuite;
          else
            return NULL;
        }
        else if (strcmp(suiteName, kOfxParameterSuite)==0) {
          return Param::GetSuite(suiteVersion);
        }
        else if (strcmp(suiteName, kOfxMessageSuite)==0) {
          if(suiteVersion==1)
            return (void *)&gMessageSuite;
          else 
            return NULL;
        }
        else if (strcmp(suiteName, kOfxInteractSuite)==0) {
          return Interact::GetSuite(suiteVersion);
        }
        else if (strcmp(suiteName, kOfxProgressSuite)==0) {
          if(suiteVersion==1) 
            return (void*)&gProgressSuite;
          else
            return 0;
        }
        else if (strcmp(suiteName, kOfxTimeLineSuite)==0) {
          if(suiteVersion==1) 
            return (void*)&gTimelineSuite;
          else
            return 0;
        }
        else if (strcmp(suiteName, kOfxMultiThreadSuite)==0) {
          if(suiteVersion == 1)
            return (void*)&gSingleThreadedSuite;
          else 
            return NULL;
        }
        else  /// otherwise just grab the base class one, which is props and memory
          return OFX::Host::Host::fetchSuite(suiteName, suiteVersion);
      }

    } // ImageEffect

  } // Host

} // OFX
