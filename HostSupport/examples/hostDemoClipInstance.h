// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef HOST_DEMO_CLIP_INSTANCE_H
#define HOST_DEMO_CLIP_INSTANCE_H

#define OFXHOSTDEMOCLIPLENGTH 1.0

namespace MyHost {

  // forward
  class MyClipInstance;

  /// make an image up
  class MyImage : public OFX::Host::ImageEffect::Image 
  {
  protected :
    OfxRGBAColourB   *_data; // where we are keeping our image data
  public :
    explicit MyImage(MyClipInstance &clip, OfxTime t, int view = 0);
    OfxRGBAColourB* pixel(int x, int y) const;
    ~MyImage();
  };

  class MyClipInstance : public OFX::Host::ImageEffect::ClipInstance {
  protected:
    MyEffectInstance *_effect;
    std::string       _name;
    MyImage          *_outputImage; ///< only set for output clips

  public:
    MyClipInstance(MyEffectInstance* effect, OFX::Host::ImageEffect::ClipDescriptor* desc);

    virtual ~MyClipInstance();
    MyImage* getOutputImage() { return _outputImage; }

    /// Get the Raw Unmapped Pixel Depth from the host
    ///
    /// \returns
    ///    - kOfxBitDepthNone (implying a clip is unconnected image)
    ///    - kOfxBitDepthByte
    ///    - kOfxBitDepthShort
    ///    - kOfxBitDepthHalf
    ///    - kOfxBitDepthFloat
    const std::string &getUnmappedBitDepth() const;
    
    /// Get the Raw Unmapped Components from the host
    ///
    /// \returns
    ///     - kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
    ///     - kOfxImageComponentRGBA
    ///     - kOfxImageComponentAlpha
    virtual const std::string &getUnmappedComponents() const;

    // PreMultiplication -
    //
    //  kOfxImageOpaque - the image is opaque and so has no premultiplication state
    //  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
    //  kOfxImageUnPreMultiplied - the image is unpremultiplied
    virtual const std::string &getPremult() const;

    // Pixel Aspect Ratio -
    //
    //  The pixel aspect ratio of a clip or image.
    virtual double getAspectRatio() const;

    // Frame Rate -
    //
    //  The frame rate of a clip or instance's project.
    virtual double getFrameRate() const;

    // Frame Range (startFrame, endFrame) -
    //
    //  The frame range over which a clip has images.
    virtual void getFrameRange(double &startFrame, double &endFrame) const ;

    /// Field Order - Which spatial field occurs temporally first in a frame.
    /// \returns 
    ///  - kOfxImageFieldNone - the clip material is unfielded
    ///  - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occurring first in a frame
    ///  - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occurring first in a frame
    virtual const std::string &getFieldOrder() const;
        
    // Connected -
    //
    //  Says whether the clip is actually connected at the moment.
    virtual bool getConnected() const;

    // Unmapped Frame Rate -
    //
    //  The unmaped frame range over which an output clip has images.
    virtual double getUnmappedFrameRate() const;

    // Unmapped Frame Range -
    //
    //  The unmaped frame range over which an output clip has images.
    virtual void getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) const;

    // Continuous Samples -
    //
    //  0 if the images can only be sampled at discrete times (eg: the clip is a sequence of frames),
    //  1 if the images can only be sampled continuously (eg: the clip is in fact an animating roto spline and can be rendered anywhen). 
    virtual bool getContinuousSamples() const;

    /// override this to fill in the image at the given time.
    /// The bounds of the image on the image plane should be 
    /// 'appropriate', typically the value returned in getRegionsOfInterest
    /// on the effect instance. Outside a render call, the optionalBounds should
    /// be 'appropriate' for the.
    /// If bounds is not null, fetch the indicated section of the canonical image plane.
    virtual OFX::Host::ImageEffect::Image* getImage(OfxTime time, const OfxRectD *optionalBounds);

#ifdef OFX_SUPPORTS_OPENGLRENDER
    /// override this to fill in the OpenGL texture at the given time.
    /// The bounds of the image on the image plane should be
    /// 'appropriate', typically the value returned in getRegionsOfInterest
    /// on the effect instance. Outside a render call, the optionalBounds should
    /// be 'appropriate' for the.
    /// If bounds is not null, fetch the indicated section of the canonical image plane.
    virtual OFX::Host::ImageEffect::Texture* loadTexture(OfxTime time, const char *format, const OfxRectD *optionalBounds) { return NULL; };
#endif

    /// override this to return the rod on the clip
    virtual OfxRectD getRegionOfDefinition(OfxTime time) const;

  };

}

#endif // HOST_DEMO_CLIP_INSTANCE_H
