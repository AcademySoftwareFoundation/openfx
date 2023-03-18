// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause


#include <iostream>
#include <fstream>
#include <cassert>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"
#include "ofxPixels.h"

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
   
////////////////////////////////////////////////////////////////////////////////
// This example code can only work with the example 'invert' plugin built
// with the OFX C++ support library. With some programming you may be able to
// get it to work with another plug-in.
//
//
// It's job is purely to show you what functions you need to fill in to provide
// an OFX image effect plugin host. It is (as we say in England) a 'noddy' example.
// 
// Run this in a debugger and step through to see what it does. Also be sure to
// build the invert example and set OFX_PLUGIN_PATH so that it can see it.
//
// It works by hard coding progressive PAL SD imagery to input and output clips,
// the images are black going in (and should be white coming out of the plugin).
//
// There is no file io to work with this.

void exportToPPM(const std::string& fname, MyHost::MyImage* im)
{
  std::ofstream op(fname.c_str());
  OfxRectI rod = im->getROD();
  op << "P3" << "\t# FORMAT" << std::endl;
  op << rod.x2 - rod.x1 << "\t#WIDTH" << std::endl;
  op << rod.y2 - rod.y1 << "\t#HEIGHT" <<std::endl;
  //This assumes 8-bit.
  op << "255" << std::endl;
  for (int y = rod.y1; y< rod.y2; ++y)
  {
    for (int x = rod.x1; x < rod.x2; ++x)
    {
      OfxRGBAColourB* pix = im->pixel(x,y);
      if(pix)
        op << (int)pix->r << " " << (int)pix->g << " " << (int)pix->b << " " << std::endl;
      else
        op << "0 0 0" << std::endl;
    }
  }
}

int main(int argc, char **argv) 
{
  //_CrtSetBreakAlloc(3168);
#ifdef _WIN32
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
  // set the version label in the global cache
  OFX::Host::PluginCache::getPluginCache()->setCacheVersion("hostDemoV1");

  // create our derived image effect host which provides
  // a factory to make plugin instances and acts
  // as a description of the host application
  MyHost::Host myHost;

  // make an image effect plugin cache. This is what knows about
  // all the plugins.
  OFX::Host::ImageEffect::PluginCache imageEffectPluginCache(myHost);

  // register the image effect cache with the global plugin cache
  imageEffectPluginCache.registerInCache(*OFX::Host::PluginCache::getPluginCache());

  // try to read an old cache
  std::ifstream ifs("hostDemoPluginCache.xml");
  OFX::Host::PluginCache::getPluginCache()->readCache(ifs);
  OFX::Host::PluginCache::getPluginCache()->scanPluginFiles();
  ifs.close();

  /// flush out the current cache
  std::ofstream of("hostDemoPluginCache.xml");
  OFX::Host::PluginCache::getPluginCache()->writePluginCache(of);
  of.close();

  // get the invert example plugin which uses the OFX C++ support code
  OFX::Host::ImageEffect::ImageEffectPlugin* plugin = imageEffectPluginCache.getPluginById("net.sf.openfx:invertPlugin");

  imageEffectPluginCache.dumpToStdOut();

  if(plugin) {
    // create an instance of it as a filter
    // the first arg is the context, the second is client data we are allowed to pass down the call chain

    std::unique_ptr<OFX::Host::ImageEffect::Instance> instance(plugin->createInstance(kOfxImageEffectContextFilter, NULL));

    if(instance)
    {
        OfxStatus stat;

      // now we need to call the create instance action. Only call this once you have initialised all the params
      // and clips to their correct values. So if you are loading a saved plugin state, set up your params from
      // that state, _then_ call create instance.
      stat = instance->createInstanceAction();
      assert(stat == kOfxStatOK || stat == kOfxStatReplyDefault);

      // now we need to to call getClipPreferences on the instance so that it does the clip component/depth
      // logic and caches away the components and depth on each clip.
      bool ok = instance->getClipPreferences();
      assert(ok);
      
      // current render scale of 1
      OfxPointD renderScale;
      renderScale.x = renderScale.y = 1.0;

      // The render window is in pixel coordinates
      // ie: render scale and a PAR of not 1
      OfxRectI  renderWindow;
      renderWindow.x1 = renderWindow.y1 = 0;
      renderWindow.x2 = 720;
      renderWindow.y2 = 576;

      /// RoI is in canonical coords, 
      OfxRectD  regionOfInterest;
      regionOfInterest.x1 = regionOfInterest.y1 = 0;
      regionOfInterest.x2 = renderWindow.x2 * instance->getProjectPixelAspectRatio();
      regionOfInterest.y2 = 576;
      
      int numFramesToRender = OFXHOSTDEMOCLIPLENGTH;

      // say we are about to render a bunch of frames 
      stat = instance->beginRenderAction(0, numFramesToRender, 1.0, false, renderScale, /*sequential=*/true, /*interactive=*/false
                                         );
      assert(stat == kOfxStatOK || stat == kOfxStatReplyDefault);

      // get the output clip
      MyHost::MyClipInstance* outputClip = dynamic_cast<MyHost::MyClipInstance*>(instance->getClip("Output"));
      assert(outputClip);

      for(int t = 0; t <= numFramesToRender; ++t) 
      {
        // call get region of interest on each of the inputs
        OfxTime frame = t;

        // get the RoI for each input clip
        // the regions of interest for each input clip are returned in a std::map
        // on a real host, these will be the regions of each input clip that the
        // effect needs to render a given frame (clipped to the RoD).
        //
        // In our example we are doing full frame fetches regardless.
        std::map<OFX::Host::ImageEffect::ClipInstance *, OfxRectD> rois;
        stat = instance->getRegionOfInterestAction(frame, renderScale,
                                                   regionOfInterest, rois);
        assert(stat == kOfxStatOK || stat == kOfxStatReplyDefault);

        // render a frame
        stat = instance->renderAction(t,kOfxImageFieldBoth,renderWindow, renderScale, /*sequential=*/true, /*interactive=*/false, /*draft=*/false);
        assert(stat == kOfxStatOK);

        // get the output image buffer
        MyHost::MyImage *outputImage = outputClip->getOutputImage();

        std::ostringstream ss;
        ss << "Output." << t << ".ppm";
        exportToPPM(ss.str(), outputImage);
      }

      instance->endRenderAction(0, numFramesToRender, 1.0, false, renderScale, /*sequential=*/true, /*interactive=*/false
                                );
    }
  }
  OFX::Host::PluginCache::clearPluginCache();
  return 0;
}
