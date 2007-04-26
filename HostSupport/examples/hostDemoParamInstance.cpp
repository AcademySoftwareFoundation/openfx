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

  //
  // MyIntegerInstance
  //

  MyIntegerInstance::MyIntegerInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::IntegerInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyIntegerInstance::get(int&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyIntegerInstance::get(OfxTime time, int&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyIntegerInstance::set(int)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyIntegerInstance::set(OfxTime time, int) {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyDoubleInstance
  //

  MyDoubleInstance::MyDoubleInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::DoubleInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyDoubleInstance::get(double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDoubleInstance::get(OfxTime time, double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDoubleInstance::set(double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDoubleInstance::set(OfxTime time, double) 
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDoubleInstance::derive(OfxTime time, double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDoubleInstance::integrate(OfxTime time1, OfxTime time2, double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyBooleanInstance
  //

  MyBooleanInstance::MyBooleanInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::BooleanInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyBooleanInstance::get(bool&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyBooleanInstance::get(OfxTime time, bool&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyBooleanInstance::set(bool)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyBooleanInstance::set(OfxTime time, bool) {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyChoiceInteger
  //

  MyChoiceInstance::MyChoiceInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::ChoiceInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyChoiceInstance::get(int&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyChoiceInstance::get(OfxTime time, int&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyChoiceInstance::set(int)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyChoiceInstance::set(OfxTime time, int) 
  {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyRGBAInstance
  //

  MyRGBAInstance::MyRGBAInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::RGBAInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyRGBAInstance::get(double&,double&,double&,double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyRGBAInstance::get(OfxTime time, double&,double&,double&,double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyRGBAInstance::set(double,double,double,double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyRGBAInstance::set(OfxTime time, double,double,double,double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyRGBInstance
  //

  MyRGBInstance::MyRGBInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::RGBInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyRGBInstance::get(double&,double&,double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyRGBInstance::get(OfxTime time, double&,double&,double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyRGBInstance::set(double,double,double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyRGBInstance::set(OfxTime time, double,double,double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyDouble2DInstance
  //

  MyDouble2DInstance::MyDouble2DInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::Double2DInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyDouble2DInstance::get(double&,double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDouble2DInstance::get(OfxTime time,double&,double&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDouble2DInstance::set(double,double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyDouble2DInstance::set(OfxTime time,double,double)
  {
    return kOfxStatErrMissingHostFeature;
  }

  //
  // MyInteger2DInstance
  //

  MyInteger2DInstance::MyInteger2DInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::Integer2DInstance(descriptor,paramSet)
  {
  }

  OfxStatus MyInteger2DInstance::get(int&,int&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyInteger2DInstance::get(OfxTime time,int&,int&)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyInteger2DInstance::set(int,int)
  {
    return kOfxStatErrMissingHostFeature;
  }

  OfxStatus MyInteger2DInstance::set(OfxTime time,int,int)
  {
    return kOfxStatErrMissingHostFeature;
  }

 //
  // MyInteger2DInstance
  //

  MyPushbuttonInstance::MyPushbuttonInstance(MyEffectInstance* effect, 
    MyParamSetInstance* paramSet, 
    const std::string& name, 
    OFX::Host::Param::Descriptor& descriptor)
    : _effect(effect), _paramSet(paramSet), _descriptor(descriptor), OFX::Host::Param::PushbuttonInstance(descriptor,paramSet)
  {
  }

}