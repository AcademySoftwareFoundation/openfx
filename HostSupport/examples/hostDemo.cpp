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


// ofx host entry point
namespace OFX {

  namespace Host {

    namespace ImageEffect {

      void* gNewInstancePtr;

      ImageEffect::Instance* newInstance(void* ptr,
                                         ImageEffectPlugin* plugin,
                                         Descriptor& desc,
                                         const std::string& context)
      {
        return new MyHost::MyEffectInstance(plugin,desc,context);
      }

    } // ImageEffect

  } // Host

} // OFX


int main(int argc, char **argv) 
{
  OFX::Host::ImageEffect::gNewInstancePtr = 0;

  OFX::Host::ImageEffect::PluginCache ia(&MyHost::gOfxHost);
  ia.registerInCache(OFX::Host::gPluginCache);

  std::ifstream ifs("cache");
  OFX::Host::gPluginCache.readCache(ifs);
  OFX::Host::gPluginCache.scanPluginFiles();
  ifs.close();

  std::ofstream of("cache2");
  OFX::Host::gPluginCache.writePluginCache(of);
  of.close();

  // get a plugin
  OFX::Host::ImageEffect::ImageEffectPlugin* plugin = ia.getPluginById("uk.co.thefoundry.tinder.t_tile");

  // create an instance
  OFX::Host::ImageEffect::Instance* tileFilterInstance = plugin->createInstance(kOfxImageEffectContextFilter);

  return 0;
}
