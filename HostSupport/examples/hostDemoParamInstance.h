// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
#ifndef HOST_DEMO_PARAM_INSTANCE_H
#define HOST_DEMO_PARAM_INSTANCE_H

namespace MyHost {

  class MyPushbuttonInstance : public OFX::Host::Param::PushbuttonInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyPushbuttonInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Param::Descriptor& descriptor);
  };

  class MyIntegerInstance : public OFX::Host::Param::IntegerInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyIntegerInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(int&);
    OfxStatus get(OfxTime time, int&);
    OfxStatus set(int);
    OfxStatus set(OfxTime time, int);
  };

  class MyDoubleInstance : public OFX::Host::Param::DoubleInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyDoubleInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(double&);
    OfxStatus get(OfxTime time, double&);
    OfxStatus set(double);
    OfxStatus set(OfxTime time, double);
    OfxStatus derive(OfxTime time, double&);
    OfxStatus integrate(OfxTime time1, OfxTime time2, double&);
  };

  class MyBooleanInstance : public OFX::Host::Param::BooleanInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyBooleanInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(bool&);
    OfxStatus get(OfxTime time, bool&);
    OfxStatus set(bool);
    OfxStatus set(OfxTime time, bool);
  };

  class MyChoiceInstance : public OFX::Host::Param::ChoiceInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyChoiceInstance(MyEffectInstance* effect,  const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(int&);
    OfxStatus get(OfxTime time, int&);
    OfxStatus set(int);
    OfxStatus set(OfxTime time, int);
  };

  class MyRGBAInstance : public OFX::Host::Param::RGBAInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyRGBAInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(double&,double&,double&,double&);
    OfxStatus get(OfxTime time, double&,double&,double&,double&);
    OfxStatus set(double,double,double,double);
    OfxStatus set(OfxTime time, double,double,double,double);
  };


  class MyRGBInstance : public OFX::Host::Param::RGBInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyRGBInstance(MyEffectInstance* effect,  const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(double&,double&,double&);
    OfxStatus get(OfxTime time, double&,double&,double&);
    OfxStatus set(double,double,double);
    OfxStatus set(OfxTime time, double,double,double);
  };

  class MyDouble2DInstance : public OFX::Host::Param::Double2DInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyDouble2DInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(double&,double&);
    OfxStatus get(OfxTime time,double&,double&);
    OfxStatus set(double,double);
    OfxStatus set(OfxTime time,double,double);
  };

  class MyInteger2DInstance : public OFX::Host::Param::Integer2DInstance {
  protected:
    MyEffectInstance*   _effect;
    OFX::Host::Param::Descriptor& _descriptor;
  public:
    MyInteger2DInstance(MyEffectInstance* effect,  const std::string& name, OFX::Host::Param::Descriptor& descriptor);
    OfxStatus get(int&,int&);
    OfxStatus get(OfxTime time,int&,int&);
    OfxStatus set(int,int);
    OfxStatus set(OfxTime time,int,int);
  };


}

#endif // HOST_DEMO_PARAM_INSTANCE_H
