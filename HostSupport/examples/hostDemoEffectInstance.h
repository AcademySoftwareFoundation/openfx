#ifndef HOST_DEMO_EFFECT_INSTANCE_H
#define HOST_DEMO_EFFECT_INSTANCE_H

namespace MyHost {

  // class definition
  class MyEffectInstance : public OFX::Host::ImageEffect::Instance {
  public:
    MyEffectInstance(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                     OFX::Host::ImageEffect::Descriptor& desc,
                     const std::string& context);

    // get a new clip instance
    OFX::Host::Clip::Instance* newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
                                               const std::string& name, 
                                               OFX::Host::Clip::Descriptor* descriptor);

    // make a param set
    OFX::Host::Param::SetInstance* newParamSetInstance(OFX::Host::ImageEffect::Instance* plugin, 
                                                       OFX::Host::Param::SetDescriptor& descriptor);

    // vmessage
    OfxStatus vmessage(const char* type,
      const char* id,
      const char* format,
      va_list args);
       
    // live parameters
    OfxStatus getProjectSize(double& xSize, double& ySize);   
    OfxStatus getProjectOffset(double& xOffset, double& yOffset); 
    OfxStatus getProjectExtent(double& xSize, double& ySize);
    OfxStatus getProjectPixelAspectRatio(double& par);
    OfxStatus getEffectDuration(double& duration);    
    OfxStatus getFrameRate(double& frameRate);
  };

}

#endif // HOST_DEMO_EFFECT_INSTANCE_H