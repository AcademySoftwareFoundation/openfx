#ifndef HOST_DEMO_PARAMSET_INSTANCE_H
#define HOST_DEMO_PARAMSET_INSTANCE_H

namespace MyHost {

  class MyParamSetInstance : public OFX::Host::Param::SetInstance {
  protected:
    MyEffectInstance* _effect;
  public:
    MyParamSetInstance(MyEffectInstance* effect, OFX::Host::Param::SetDescriptor& descriptor);

    // make a parameter instance
    virtual OFX::Host::Param::Instance* newParam(const OFX::Host::Param::Descriptor& Descriptor);

    virtual OfxStatus editBegin(const std::string& name);
    virtual OfxStatus editEnd();
  };

}

#endif // HOST_DEMO_PARAMSET_INSTANCE_H
