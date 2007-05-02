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
  OFX::Host::ImageEffect::ImageEffectPlugin* plugin = ia.getPluginById("uk.co.thefoundry:BasicGainPlugin");

  // create an instance
  OFX::Host::ImageEffect::Instance* tileFilterInstance = plugin->createInstance(kOfxImageEffectContextFilter);

  if(tileFilterInstance){
    // begin instance
    tileFilterInstance->beginInstanceChangedAction(kOfxChangeUserEdited);

    // instance change
    tileFilterInstance->paramInstanceChangedAction("scaleR",kOfxChangeUserEdited,0.0,1.0,1.0);

    // end instance
    tileFilterInstance->endInstanceChangedAction(kOfxChangeUserEdited);

    // purge your caches
    tileFilterInstance->purgeCachesAction();

    // sync your private data
    tileFilterInstance->syncPrivateDataAction();

    // begin instance
    tileFilterInstance->beginInstanceEditAction();

    // end instance
    tileFilterInstance->endInstanceEditAction();

    double   x1 = 0;
    double   y1 = 0;
    double   x2 = 720;
    double   y2 = 576;

    std::map<std::string,OfxRectD> rois;

    tileFilterInstance->getRegionOfInterestAction(0.0,1.0,1.0,x1,y1,x2,y2,rois);

    // render
    tileFilterInstance->beginRenderAction(0.0,1.0,1.0/25.0,true,1.0,1.0);

    for(double t=0.0;t<1.0;t=t+1.0/25.0){
      tileFilterInstance->renderAction(t,kOfxImageFieldBoth,x1,y1,x2,y2,1.0,1.0);
      
      MyHost::MyClipInstance* outputClip = (MyHost::MyClipInstance*)tileFilterInstance->getClip("Output");
      
      unsigned char* output = outputClip->getLastOutput();

      // do something with the output
      delete [] output;
    }

    tileFilterInstance->endRenderAction(0.0,1.0,1.0/25.0,true,1.0,1.0);

    // delete the instance
    delete tileFilterInstance;
  }

  return 0;
}
