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

// my host support code
namespace MyHost {

  MyEffectInstance::MyEffectInstance(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                     OFX::Host::ImageEffect::Descriptor& desc,
                                     const std::string& context) 
                                     : OFX::Host::ImageEffect::Instance(plugin,desc,context)
  {
  }

  // class member function implementation

  // get a new clip instance
  OFX::Host::Clip::Instance* MyEffectInstance::newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
    const std::string& name, 
    OFX::Host::Clip::Descriptor* descriptor)
  {
    return new MyClipInstance(this,name,*descriptor);
  }

  // make a param set
  OFX::Host::Param::SetInstance* MyEffectInstance::newParamSetInstance(OFX::Host::ImageEffect::Instance* plugin, 
                                                                       OFX::Host::Param::SetDescriptor& descriptor)
  {
    return new MyParamSetInstance(this,descriptor);
  }

  // vmessage
  OfxStatus MyEffectInstance::vmessage(const char* type,
                                       const char* id,
                                       const char* format,
                                       va_list args)
  {
    printf("%s %s ",type,id);
    vprintf(format,args);
    return kOfxStatOK;
  }

}