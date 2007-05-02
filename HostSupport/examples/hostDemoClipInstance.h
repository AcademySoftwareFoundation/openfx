#ifndef HOST_DEMO_CLIP_INSTANCE_H
#define HOST_DEMO_CLIP_INSTANCE_H

namespace MyHost {

  class MyClipInstance : public OFX::Host::Clip::Instance {
  protected:
    MyEffectInstance* _effect;
    std::string       _name;
    unsigned char*    _lastOutput;
  public:
    MyClipInstance(MyEffectInstance* effect, const std::string& name, OFX::Host::Clip::Descriptor& desc);

    unsigned char* getLastOutput() { return _lastOutput; }

    // implemented abstract members of clip instance

    // live properties
    OfxStatus getPixelDepth(std::string &pixelDepth);
    OfxStatus getComponents(std::string &components);
    OfxStatus getUnmappedBitDepth(std::string &unmappedBitDepth);
    OfxStatus getUnmappedComponents(std::string &unmappedComponents);
    OfxStatus getPremult(std::string &premult);
    OfxStatus getAspectRatio(double &aspectRatio);
    OfxStatus getFrameRate(double &frameRate);
    OfxStatus getFrameRange(double &startFrame, double &endFrame);
    OfxStatus getField(std::string &field);
    OfxStatus getConnected(int &connected);
    OfxStatus getUnmappedFrameRate(double &unmappedFrameRate);
    OfxStatus getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame);
    OfxStatus getContinuousSamples(int &continuousSamples);

    // get image
    OfxStatus getImage(OfxTime time, OfxRectD *h2, OFX::Host::Clip::Image*& image);

    // get clip rod
    OfxStatus getRegionOfDefinition(OfxRectD& rod);
  };

}

#endif // HOST_DEMO_CLIP_INSTANCE_H
