// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <ctime>

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

// We are hard coding everything in our example, in a real host you
// need to enquire things from your host.
namespace MyHost {
  const double    kPalPixelAspect = double(768)/double(720);
  const int       kPalSizeXPixels = 720;
  const int       kPalSizeYPixels = 576;
  const OfxRectI  kPalRegionPixels = {0, 0, kPalSizeXPixels, kPalSizeYPixels};
  //const OfxRectD  kPalRegionCanon = {0,0, kPalSizeXPixels * kPalPixelAspect ,kPalSizeYPixels};

  // 5x3 bitmaps for digits 0..9 and period
  const char digits[11][5][3] = {
    { {0,1,0},
      {1,0,1},
      {1,0,1},
      {1,0,1},
      {0,1,0} },
    { {0,1,0},
      {1,1,0},
      {0,1,0},
      {0,1,0},
      {0,1,0} },    
    { {0,1,0},
      {1,0,1},
      {0,0,1},
      {0,1,0},
      {1,1,1} },    
    { {1,1,0},
      {0,0,1},
      {0,1,1},
      {0,0,1},
      {1,1,0} },      
    { {0,1,0},
      {1,0,0},
      {1,0,1},
      {1,1,1},
      {0,0,1} },    
    { {1,1,1},
      {1,0,0},
      {1,1,0},
      {0,0,1},
      {1,1,0} },  
    { {0,1,1},
      {1,0,0},
      {1,1,0},
      {1,0,1},
      {0,1,0} },      
    { {1,1,1},
      {0,0,1},
      {0,1,0},
      {0,1,1},
      {0,1,0} },      
    { {0,1,0},
      {1,0,1},
      {0,1,0},
      {1,0,1},
      {0,1,0} },  
    { {0,1,0},
      {1,0,1},
      {0,1,1},
      {0,0,1},
      {1,1,0} },
    { {0,0,0},
      {0,0,0},
      {0,0,0},
      {0,0,0},
      {0,1,0} }
  };

  // draw digit d at x,y int the width*height image pointed to by data
  static void drawDigit(OfxRGBAColourB* data, int width, int height, int d, int x, int y , int scale, OfxRGBAColourB color) {
    assert(0 <= x && x+3*scale < width);
    assert(0 <= y && y+5*scale < height);

    for (int j = 0; j < 5; ++j) {
      for (int i = 0; i < 3; ++i) {
        if (digits[d][j][i]) {
          for (int jj = 0; jj < scale; ++jj) {
            for (int ii = 0; ii < scale; ++ii) {
              int x1 = x + i*scale + ii;
              int y1 = y + j*scale + jj;
              data[x1+y1*width] = color;
            }
          }
        }
      }
    }
  }

  /// images are always SD PAL progressive full res images for the purpose of this example only
  MyImage::MyImage(MyClipInstance &clip, OfxTime time, int view)
    : OFX::Host::ImageEffect::Image(clip) /// this ctor will set basic props on the image
    , _data(NULL)
  {
    // make some memory
    _data = new OfxRGBAColourB[kPalSizeXPixels * kPalSizeYPixels] ; /// PAL SD RGBA
    
    int fillValue = (int)(floor(255.0 * (time/OFXHOSTDEMOCLIPLENGTH))) & 0xff;
    OfxRGBAColourB color;
    color.r = color.g = color.b = fillValue;
    color.a = 255;

    std::fill(_data, _data + kPalSizeXPixels * kPalSizeYPixels, color);
    // draw the time and the view number in reverse color
    const int scale = 5;
    const int charwidth = 4*scale;
    color.r = color.g = color.b = 255-fillValue;
    int xx = 50;
    int yy = 50;
    int d;
    d = (int(time)/10)%10;
    drawDigit(_data, kPalSizeXPixels, kPalSizeYPixels, d, xx, yy, scale, color);
    xx += charwidth;
    d = int(time)%10;
    drawDigit(_data, kPalSizeXPixels, kPalSizeYPixels, d, xx, yy, scale, color);
    xx += charwidth;
    d = 10;
    drawDigit(_data, kPalSizeXPixels, kPalSizeYPixels, d, xx, yy, scale, color);
    xx += charwidth;
    d = int(time*10)%10;
    drawDigit(_data, kPalSizeXPixels, kPalSizeYPixels, d, xx, yy, scale, color);
    xx = 50;
    yy += 8*scale;
    d = int(view)%10;
    drawDigit(_data, kPalSizeXPixels, kPalSizeYPixels, d, xx, yy, scale, color);

    // render scale x and y of 1.0
    setDoubleProperty(kOfxImageEffectPropRenderScale, 1.0, 0);
    setDoubleProperty(kOfxImageEffectPropRenderScale, 1.0, 1); 

    // data ptr
    setPointerProperty(kOfxImagePropData,_data);

    // bounds and rod
    setIntProperty(kOfxImagePropBounds, kPalRegionPixels.x1, 0);
    setIntProperty(kOfxImagePropBounds, kPalRegionPixels.y1, 1);
    setIntProperty(kOfxImagePropBounds, kPalRegionPixels.x2, 2);
    setIntProperty(kOfxImagePropBounds, kPalRegionPixels.y2, 3);
    
    setIntProperty(kOfxImagePropRegionOfDefinition, kPalRegionPixels.x1, 0);
    setIntProperty(kOfxImagePropRegionOfDefinition, kPalRegionPixels.y1, 1);
    setIntProperty(kOfxImagePropRegionOfDefinition, kPalRegionPixels.x2, 2);
    setIntProperty(kOfxImagePropRegionOfDefinition, kPalRegionPixels.y2, 3);        

    // row bytes
    setIntProperty(kOfxImagePropRowBytes, kPalSizeXPixels * sizeof(OfxRGBAColourB));
  }

  OfxRGBAColourB* MyImage::pixel(int x, int y) const
  {
    OfxRectI bounds = getBounds();
    if ((x >= bounds.x1) && ( x< bounds.x2) && ( y >= bounds.y1) && ( y < bounds.y2) )
    {
      int rowBytes = getIntProperty(kOfxImagePropRowBytes);
      int offset = (y - bounds.y1) * rowBytes + (x - bounds.x1) * sizeof(OfxRGBAColourB);
      return reinterpret_cast<OfxRGBAColourB*>(&(reinterpret_cast<char*>(_data)[offset]));
    }
    return 0;
  }

  MyImage::~MyImage() 
  {
    delete _data;
  }

  MyClipInstance::MyClipInstance(MyEffectInstance* effect, OFX::Host::ImageEffect::ClipDescriptor *desc)
    : OFX::Host::ImageEffect::ClipInstance(effect, *desc)
    , _effect(effect)
    , _name(desc->getName())
    , _outputImage(NULL)
  {
  }

  MyClipInstance::~MyClipInstance()
  {
    if(_outputImage)
      _outputImage->releaseReference();
  }
   
  /// Get the Raw Unmapped Pixel Depth from the host. We are always 8 bits in our example
  const std::string &MyClipInstance::getUnmappedBitDepth() const
  {
    static const std::string v(kOfxBitDepthByte);
    return v;
  }
    
  /// Get the Raw Unmapped Components from the host. In our example we are always RGBA
  const std::string &MyClipInstance::getUnmappedComponents() const
  {
    static const std::string v(kOfxImageComponentRGBA);
    return v;
  }

  // PreMultiplication -
  //
  //  kOfxImageOpaque - the image is opaque and so has no premultiplication state
  //  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
  //  kOfxImageUnPreMultiplied - the image is unpremultiplied
  const std::string &MyClipInstance::getPremult() const
  {
    static const std::string v(kOfxImageUnPreMultiplied);
    return v;
  }

  // Pixel Aspect Ratio -
  //
  //  The pixel aspect ratio of a clip or image.
  double MyClipInstance::getAspectRatio() const
  {
    /// our clip is pretending to be progressive PAL SD, so return 1.06666
    return kPalPixelAspect;
  }
  
  // Frame Rate -
  double MyClipInstance::getFrameRate() const
  {
    /// our clip is pretending to be progressive PAL SD, so return 25
    return 25.0;
  }
  
  // Frame Range (startFrame, endFrame) -
  //
  //  The frame range over which a clip has images.
  void MyClipInstance::getFrameRange(double &startFrame, double &endFrame) const
  {
    // pretend we have a second's worth of PAL SD
    startFrame = 0;
    endFrame = 25;
  }

  /// Field Order - Which spatial field occurs temporally first in a frame.
  /// \returns 
  ///  - kOfxImageFieldNone - the clip material is unfielded
  ///  - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occurring first in a frame
  ///  - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occurring first in a frame
  const std::string &MyClipInstance::getFieldOrder() const
  {
    /// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
    static const std::string v(kOfxImageFieldNone);
    return v;
  }
        
  // Connected -
  //
  //  Says whether the clip is actually connected at the moment.
  bool MyClipInstance::getConnected() const
  {
    return true;
  }
  
  // Unmapped Frame Rate -
  //
  //  The unmaped frame range over which an output clip has images.
  double MyClipInstance::getUnmappedFrameRate() const
  {
    /// our clip is pretending to be progressive PAL SD, so return 25
    return 25;
  }
  
  // Unmapped Frame Range -
  //
  //  The unmaped frame range over which an output clip has images.
  // this is applicable only to hosts and plugins that allow a plugin to change frame rates
  void MyClipInstance::getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) const
  {
    // pretend we have a second's worth of PAL SD
    unmappedStartFrame = 0;
    unmappedEndFrame = 25;
  }

  // Continuous Samples -
  //
  //  0 if the images can only be sampled at discrete times (eg: the clip is a sequence of frames),
  //  1 if the images can only be sampled continuously (eg: the clip is in fact an animating roto spline and can be rendered anywhen). 
  bool MyClipInstance::getContinuousSamples() const
  {
    return false;
  }


  /// override this to return the rod on the clip canonical coords!
  OfxRectD MyClipInstance::getRegionOfDefinition(OfxTime time) const
  {
    /// our clip is pretending to be progressive PAL SD, so return 0<=x<768, 0<=y<576 
    OfxRectD v;
    v.x1 = v.y1 = 0;
    v.x2 = 768;
    v.y2 = 576;
    return v;
  }
  
  /// override this to fill in the image at the given time.
  /// The bounds of the image on the image plane should be 
  /// 'appropriate', typically the value returned in getRegionsOfInterest
  /// on the effect instance. Outside a render call, the optionalBounds should
  /// be 'appropriate' for the.
  /// If bounds is not null, fetch the indicated section of the canonical image plane.
  OFX::Host::ImageEffect::Image* MyClipInstance::getImage(OfxTime time, const OfxRectD *optionalBounds)
  {
    if(_name == "Output") {
      if(!_outputImage) {
        // make a new ref counted image
        _outputImage = new MyImage(*this, 0);
      }
     
      // add another reference to the member image for this fetch
      // as we have a ref count of 1 due to construction, this will
      // cause the output image never to delete by the plugin
      // when it releases the image
      _outputImage->addReference();

      // return it
      return _outputImage;
    }
    else {
      // Fetch on demand for the input clip.
      // It does get deleted after the plugin is done with it as we
      // have not incremented the auto ref
      // 
      // You should do somewhat more sophisticated image management
      // than this.
      MyImage *image = new MyImage(*this, time);
      return image;
    }
  }

} // MyHost
