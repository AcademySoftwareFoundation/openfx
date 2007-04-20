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

      // instance

      Instance::Instance(Descriptor& desc, ImageEffect::Instance& effect) : 
        Descriptor(desc),
          _effect(effect)
        {
          _properties.addProperties(interactInstanceStuffs);
          _properties.setGetHook(kOfxInteractPropPixelScale,this,this);
          _properties.setGetHook(kOfxInteractPropBackgroundColour,this,this);
          _properties.setGetHook(kOfxInteractPropViewportSize,this,this);
        }

        Instance::~Instance(){
          _effect.overlayEntry(kOfxActionDestroyInstance,this->getHandle(),0,0);
        }

        void Instance::getProperty(const std::string &name, double &ret, int index)
        {   
          int max = getDimension(name);        
          if(index>=max) throw Property::Exception(kOfxStatErrValue);

          std::auto_ptr<double> values = new double[max];
          getPropertyN(name,values.get(),max);
          ret = values.get()[index];
        }

        void Instance::getPropertyN(const std::string &name, double *first, int n)
        {
          int max = getDimension(name);        
          if(n>max) throw Property::Exception(kOfxStatErrValue);

          OfxStatus st = kOfxStatOK;

          if(name == kOfxInteractPropPixelScale){
            if(n>2) throw Property::Exception(kOfxStatErrValue);
            st = getPixelScale(first[0],first[1]);
          }
          else if(name == kOfxInteractPropBackgroundColour){
            if(n>3) throw Property::Exception(kOfxStatErrValue);
            st = getBackgroundColour(first[0],first[1],first[2]);
          }
          else if(name == kOfxInteractPropViewportSize){
            if(n>2) throw Property::Exception(kOfxStatErrValue);
            st = getViewportSize(first[0],first[1]);
          }
          else
            throw Property::Exception(kOfxStatErrValue);

          if(st!=kOfxStatOK) throw Property::Exception(st);
        }

        OfxStatus Instance::getSlaveToParam(std::vector<std::string>& params)
        {        
          int nSlaveParams = _properties.getDimension(kOfxInteractPropSlaveToParam);
                    
          for(int i=0;i<nSlaveParams;i++){
            std::string param = _properties.getProperty<Property::StringValue>(kOfxInteractPropSlaveToParam,i);
            params.push_back(param);
          }

          return kOfxStatOK;
        }    

        OfxStatus Instance::drawAction(OfxTime time, 
                                       double  renderScaleX, 
                                       double  renderScaleY)
        {        
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          return _effect.overlayEntry(kOfxInteractActionDraw,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::penMotionAction(OfxTime time, 
                                            double  renderScaleX, 
                                            double  renderScaleY, 
                                            double  penX, 
                                            double  penY,
                                            double  pressure)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPosition,0,penX);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPosition,1,penY);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPressure,0,pressure);
          return _effect.overlayEntry(kOfxInteractActionPenMotion,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::penUpAction(OfxTime time, 
                                        double renderScaleX, 
                                        double renderScaleY, 
                                        double penX, 
                                        double penY,
                                        double pressure)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPosition,0,penX);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPosition,1,penY);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPressure,0,pressure);
          return _effect.overlayEntry(kOfxInteractActionPenUp,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::penDownAction(OfxTime time, 
                                          double renderScaleX, 
                                          double renderScaleY, 
                                          double penX, 
                                          double penY,
                                          double pressure)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPosition,0,penX);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPosition,1,penY);
          inArgs.setProperty<Property::DoubleValue>(kOfxInteractPropPenPressure,0,pressure);
          return _effect.overlayEntry(kOfxInteractActionPenDown,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::keyDownAction(OfxTime time, 
                                          double  renderScaleX,
                                          double  renderScaleY, 
                                          int     key,
                                          char*   keyString)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          inArgs.setProperty<Property::IntValue>(kOfxPropKeySym,1,key);
          inArgs.setProperty<Property::StringValue>(kOfxPropKeyString,0,keyString);
          return _effect.overlayEntry(kOfxInteractActionKeyDown,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::keyUpAction(OfxTime time, 
                                        double  renderScaleX,
                                        double  renderScaleY, 
                                        int     key,
                                        char*   keyString)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          inArgs.setProperty<Property::IntValue>(kOfxPropKeySym,1,key);
          inArgs.setProperty<Property::StringValue>(kOfxPropKeyString,0,keyString);
          return _effect.overlayEntry(kOfxInteractActionKeyUp,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::keyRepeatAction(OfxTime time,
                                            double  renderScaleX,
                                            double  renderScaleY,
                                            int     key,
                                            char*   keyString)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          inArgs.setProperty<Property::IntValue>(kOfxPropKeySym,1,key);
          inArgs.setProperty<Property::StringValue>(kOfxPropKeyString,0,keyString);
          return _effect.overlayEntry(kOfxInteractActionKeyRepeat,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::gainFocusAction(OfxTime time,
                                            double  renderScaleX, 
                                            double  renderScaleY)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          return _effect.overlayEntry(kOfxInteractActionGainFocus,this->getHandle(),inArgs.getHandle(),0);
        }

        OfxStatus Instance::loseFocusAction(OfxTime  time,
                                            double   renderScaleX, 
                                            double   renderScaleY)
        {
          Property::Set inArgs = getProperties();
          inArgs.setProperty<Property::PointerValue>(kOfxPropEffectInstance, 0, &_effect);
          inArgs.setProperty<Property::PointerValue>(kOfxPropInstanceData, 0, _instanceData);
          inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
          inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);
          return _effect.overlayEntry(kOfxInteractActionLoseFocus,this->getHandle(),inArgs.getHandle(),0);
        }

    } // Interact

  } // Host

} // OFX