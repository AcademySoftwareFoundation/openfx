// ofx 
#include "ofxKeySyms.h"
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhImageEffect.h"
#include "ofxhInteract.h"

namespace OFX {

  namespace Host {

    namespace Interact {

      //
      // descriptor
      //
      static Property::PropSpec interactDescriptorStuffs[] = {
        { kOfxInteractPropHasAlpha , Property::eInt, 1, false, "0" },
        { kOfxInteractPropBitDepth , Property::eInt, 1, false, "0" },
        { 0 },
      };

      Descriptor::Descriptor() : _properties(interactDescriptorStuffs) {
      }

      Descriptor::Descriptor(const Descriptor& desc) : _properties(desc.getProperties()) {
      }

      static Property::PropSpec interactInstanceStuffs[] = {
        { kOfxPropEffectInstance, Property::ePointer, 1, false, "0" },
        { kOfxPropInstanceData, Property::ePointer, 1, false, "0" },
        { kOfxInteractPropPixelScale, Property::eDouble, 2, false, "0.0f" },
        { kOfxInteractPropBackgroundColour , Property::eDouble, 3, false, "0.0f" },
        { kOfxInteractPropViewportSize, Property::eDouble, 2, false, "0.0f" },
        { kOfxInteractPropSlaveToParam , Property::eString, 0, false, ""},
        { kOfxPropTime, Property::eDouble, 1, false, "0.0" },
        { kOfxImageEffectPropRenderScale, Property::eDouble, 2, false, "0.0" },
        { 0 },
      };

      static Property::PropSpec interactArgsStuffs[] = {
        { kOfxPropEffectInstance, Property::ePointer, 1, false, "0" },
        { kOfxPropInstanceData, Property::ePointer, 1, false, "0" },
        { kOfxPropTime, Property::eDouble, 1, false, "0.0" },
        { kOfxImageEffectPropRenderScale, Property::eDouble, 2, false, "0.0" },
        { kOfxInteractPropPenPosition, Property::eDouble, 2, false, "0.0" },
        { kOfxInteractPropPenPressure, Property::eDouble, 1, false, "0.0" },
        { kOfxPropKeyString, Property::eString, 1, false, "" },
        { kOfxPropKeySym, Property::eInt, 1, false, "0" },
        { 0 },
      };

      // instance

      Instance::Instance(Descriptor& desc, ImageEffect::Instance& effect) 
        : Descriptor(desc)
        , _effect(effect)
        , _argProperties(interactArgsStuffs)
      {
        _properties.addProperties(interactInstanceStuffs);
        _properties.setGetHook(kOfxInteractPropPixelScale, this);
        _properties.setGetHook(kOfxInteractPropBackgroundColour,this);
        _properties.setGetHook(kOfxInteractPropViewportSize,this);
      }

      Instance::~Instance(){
        _effect.overlayEntry(kOfxActionDestroyInstance,this->getHandle(),0,0);
      }

      // do nothing
      int Instance::getDimension(const std::string &name) OFX_EXCEPTION_SPEC
      {
        if(name == kOfxInteractPropPixelScale){
          return 2;
        }
        else if(name == kOfxInteractPropBackgroundColour){
          return 3;
        }
        else if(name == kOfxInteractPropViewportSize){
          return 2;
        }
        else
          throw Property::Exception(kOfxStatErrValue);
      }
        
      // do nothing function
      void Instance::reset(const std::string &name) OFX_EXCEPTION_SPEC
      {
        // no-op
      }

      double Instance::getDoubleProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC
      {   
        if(name == kOfxInteractPropPixelScale){
          if(index>=2) throw Property::Exception(kOfxStatErrBadIndex);
          double first[2];
          getPixelScale(first[0],first[1]);
          return first[index];
        }
        else if(name == kOfxInteractPropBackgroundColour){
          if(index>=3) throw Property::Exception(kOfxStatErrBadIndex);
          double first[3];
          getBackgroundColour(first[0],first[1],first[2]);
          return first[index];
        }
        else if(name == kOfxInteractPropViewportSize){
          if(index>=2) throw Property::Exception(kOfxStatErrBadIndex);
          double first[2];
          getViewportSize(first[0],first[1]);
          return first[index];
        }
        else
          throw Property::Exception(kOfxStatErrUnknown);
      }

      void Instance::getDoublePropertyN(const std::string &name, double *first, int n) OFX_EXCEPTION_SPEC
      {
        if(name == kOfxInteractPropPixelScale){
          if(n>2) throw Property::Exception(kOfxStatErrBadIndex);
          getPixelScale(first[0],first[1]);
        }
        else if(name == kOfxInteractPropBackgroundColour){
          if(n>3) throw Property::Exception(kOfxStatErrBadIndex);
          getBackgroundColour(first[0],first[1],first[2]);
        }
        else if(name == kOfxInteractPropViewportSize){
          if(n>2) throw Property::Exception(kOfxStatErrBadIndex);
          getViewportSize(first[0],first[1]);
        }
        else
          throw Property::Exception(kOfxStatErrUnknown);
      }

      void Instance::getSlaveToParam(std::vector<std::string>& params)
      {        
        int nSlaveParams = _properties.getDimension(kOfxInteractPropSlaveToParam);
                    
        for(int i=0;i<nSlaveParams;i++){
          std::string param = _properties.getStringProperty(kOfxInteractPropSlaveToParam, i);
          params.push_back(param);
        }
      }    
      
      /// initialise the argument properties
      void Instance::initArgProp(OfxTime time, 
                                 double  renderScaleX, 
                                 double  renderScaleY)
      {
        _argProperties.setPointerProperty(kOfxPropEffectInstance, &_effect);
        _argProperties.setPointerProperty(kOfxPropInstanceData, _instanceData);
        _argProperties.setDoubleProperty(kOfxPropTime,time);
        _argProperties.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        _argProperties.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);
      }
               
      void Instance::setPenArgProps(double  penX, 
                                    double  penY,
                                    double  pressure)
      {
        _argProperties.setDoubleProperty(kOfxInteractPropPenPosition,penX, 0);
        _argProperties.setDoubleProperty(kOfxInteractPropPenPosition,penY, 1);
        _argProperties.setDoubleProperty(kOfxInteractPropPenPressure,pressure);
      }

      void Instance::setKeyArgProps(int     key,
                                     char*   keyString)
      {
        _argProperties.setIntProperty(kOfxPropKeySym,key);
        _argProperties.setStringProperty(kOfxPropKeyString,keyString);
      }

      OfxStatus Instance::drawAction(OfxTime time, 
                                     double  renderScaleX, 
                                     double  renderScaleY)
      {        
        initArgProp(time, renderScaleX, renderScaleY);
        return _effect.overlayEntry(kOfxInteractActionDraw,this->getHandle(), _argProperties.getHandle(),0);
      }

      OfxStatus Instance::penMotionAction(OfxTime time, 
                                          double  renderScaleX, 
                                          double  renderScaleY, 
                                          double  penX, 
                                          double  penY,
                                          double  pressure)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        setPenArgProps(penX, penY, pressure);
        return _effect.overlayEntry(kOfxInteractActionPenMotion,this->getHandle(),_argProperties.getHandle(),0);
      }

      OfxStatus Instance::penUpAction(OfxTime time, 
                                      double renderScaleX, 
                                      double renderScaleY, 
                                      double penX, 
                                      double penY,
                                      double pressure)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        setPenArgProps(penX, penY, pressure);
        return _effect.overlayEntry(kOfxInteractActionPenUp,this->getHandle(),_argProperties.getHandle(),0);
      }

      OfxStatus Instance::penDownAction(OfxTime time, 
                                        double renderScaleX, 
                                        double renderScaleY, 
                                        double penX, 
                                        double penY,
                                        double pressure)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        setPenArgProps(penX, penY, pressure);
        return _effect.overlayEntry(kOfxInteractActionPenDown,this->getHandle(),_argProperties.getHandle(),0);
      }

      OfxStatus Instance::keyDownAction(OfxTime time, 
                                        double  renderScaleX,
                                        double  renderScaleY, 
                                        int     key,
                                        char*   keyString)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        setKeyArgProps(key, keyString);
        return _effect.overlayEntry(kOfxInteractActionKeyDown,this->getHandle(),_argProperties.getHandle(),0);
      }

      OfxStatus Instance::keyUpAction(OfxTime time, 
                                      double  renderScaleX,
                                      double  renderScaleY, 
                                      int     key,
                                      char*   keyString)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        setKeyArgProps(key, keyString);
        return _effect.overlayEntry(kOfxInteractActionKeyUp,this->getHandle(),_argProperties.getHandle(),0);
      }

      OfxStatus Instance::keyRepeatAction(OfxTime time,
                                          double  renderScaleX,
                                          double  renderScaleY,
                                          int     key,
                                          char*   keyString)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        setKeyArgProps(key, keyString);
        return _effect.overlayEntry(kOfxInteractActionKeyRepeat,this->getHandle(),_argProperties.getHandle(),0);
      }
      
      OfxStatus Instance::gainFocusAction(OfxTime time,
                                          double  renderScaleX, 
                                          double  renderScaleY)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        return _effect.overlayEntry(kOfxInteractActionGainFocus,this->getHandle(),_argProperties.getHandle(),0);
      }

      OfxStatus Instance::loseFocusAction(OfxTime  time,
                                          double   renderScaleX, 
                                          double   renderScaleY)
      {
        initArgProp(time, renderScaleX, renderScaleY);
        return _effect.overlayEntry(kOfxInteractActionLoseFocus,this->getHandle(),_argProperties.getHandle(),0);
      }

    } // Interact

  } // Host

} // OFX
