#include <iostream>
#include <fstream>
#include <time.h>

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

namespace MyHost {

  MyClipInstance::MyClipInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Clip::Descriptor& desc) 
    : _effect(effect), _name(name), OFX::Host::Clip::Instance(desc,effect)
  {}

  // live properties
  OfxStatus MyClipInstance::getPixelDepth(std::string &pixelDepth)
  {
    pixelDepth = kOfxBitDepthByte;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getComponents(std::string &components)
  {
    components = kOfxImageComponentRGBA;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedBitDepth(std::string &unmappedBitDepth)
  {
    unmappedBitDepth = kOfxBitDepthByte;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedComponents(std::string &unmappedComponents)
  {
    unmappedComponents = kOfxImageComponentRGBA;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getPremult(std::string &premult)
  {
    premult = kOfxImageOpaque;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getAspectRatio(double &aspectRatio)
  {
    aspectRatio = 4.0/3.0;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getFrameRate(double &frameRate)
  {
    frameRate = 25.0;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getFrameRange(double &startFrame, double &endFrame)
  {
    startFrame = 0.0;
    endFrame = 1.0;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getField(std::string &field)
  {
    field = kOfxImageFieldBoth;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getConnected(int &connected)
  {
    connected = true;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedFrameRate(double &unmappedFrameRate)
  {
    unmappedFrameRate = 25.0;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame)
  {
    unmappedStartFrame = 0.0;
    unmappedEndFrame = 1.0;
    return kOfxStatOK;
  }

  OfxStatus MyClipInstance::getContinuousSamples(int &continuousSamples)
  {
    continuousSamples = true;
    return kOfxStatOK;
  }

  // kOfxImagePropData
  class MyImage : public OFX::Host::Clip::Image {
    unsigned char* _data;
    bool           _delete;
  public:
    MyImage(OFX::Host::Clip::Instance& instance, unsigned char* data, std::string uid, bool del = true) : 
            OFX::Host::Clip::Image(instance,
                                   1.0,1.0,
                                   (void*)data,
                                   0,0,720,576,
                                   0,0,720,576,
                                   4*720,
                                   kOfxImageFieldBoth,
                                   uid), 
        _data(data),
        _delete(del)
  {
  }

    void colour(){
      unsigned char* img = _data;
      for(int y=0;y<576;y++){
        for(int x=0;x<720;x++){
          img[(x+(y*576))] = (char)(255.0f*float(x)/720.f);
          img[(x+(y*576))+1] = (char)(255.0f*float(y)/576.f);
          img[(x+(y*576))+2] = 255;
          img[(x+(y*576))+3] = 255;
        }
      }
    }

    ~MyImage() {
      if(_delete) delete _data;
    }
  };

  // get image
  OfxStatus MyClipInstance::getImage(OfxTime time, OfxRectD *h2, OFX::Host::Clip::Image*& image)
  {
    char uid[64];
    sprintf(uid,"%d",::time(0));

    MyImage* myImage = 0;

    if(getName()=="Output"){
      unsigned char* data = new unsigned char[720*576*4];
      _lastOutput = data;
      myImage = new MyImage(*this,data,uid,false);
    }
    else{
      myImage = new MyImage(*this,new unsigned char[720*576*4],uid);
      myImage->colour();
    }

    image = myImage;

    return kOfxStatOK;
  }

  // get clip rod
  OfxStatus MyClipInstance::getRegionOfDefinition(OfxRectD& rod)
  {
    rod.x1 = 0;
    rod.y1 = 0;
    rod.x2 = 720;
    rod.y2 = 576;
    return kOfxStatOK;
  }

} // MyHost