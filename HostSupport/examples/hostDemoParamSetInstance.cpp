#include <iostream>
#include <fstream>

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

// my host
#include "hostDemoHostDescriptor.h"
#include "hostDemoEffectInstance.h"
#include "hostDemoClipInstance.h"
#include "hostDemoParamSetInstance.h"
#include "hostDemoParamInstance.h"

namespace MyHost {

  MyParamSetInstance::MyParamSetInstance(MyEffectInstance* effect, OFX::Host::Param::SetDescriptor& descriptor) 
    : _effect(effect), _descriptor(descriptor), OFX::Host::Param::SetInstance(effect)
  {
  }

  // make a parameter instance
  OFX::Host::Param::Instance* MyParamSetInstance::newParam(const std::string& name, OFX::Host::Param::Descriptor& descriptor){
    if(descriptor.getType()==kOfxParamTypeInteger)
      return new MyIntegerInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeDouble)
      return new MyDoubleInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeBoolean)
      return new MyBooleanInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeChoice)
      return new MyChoiceInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeRGBA)
      return new MyRGBAInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeRGB)
      return new MyRGBInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeDouble2D)
      return new MyDouble2DInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeInteger2D)
      return new MyInteger2DInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypePushButton)
      return new MyPushbuttonInstance(_effect,this,name,descriptor);
    else if(descriptor.getType()==kOfxParamTypeGroup)
      return new OFX::Host::Param::GroupInstance(descriptor,this);
    else
      return 0;
  }

  OfxStatus MyParamSetInstance::editBegin(const std::string& name)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyParamSetInstance::editEnd(){
    return kOfxStatErrMissingHostFeature;
  }

}