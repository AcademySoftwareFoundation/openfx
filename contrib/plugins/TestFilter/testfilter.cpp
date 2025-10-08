// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

// Simple filter plugin based on official OpenFX Invert example
// This plugin inverts the input image

#include <algorithm>
#include <cmath>
#include <memory>

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsProcessing.h"

////////////////////////////////////////////////////////////////////////////////
// Base class for the filter processor
class FilterBase : public OFX::ImageProcessor {
protected:
    OFX::Image *_srcImg;

public:
    FilterBase(OFX::ImageEffect &instance)
        : OFX::ImageProcessor(instance)
        , _srcImg(0)
    {
    }

    void setSrcImg(OFX::Image *v) { _srcImg = v; }
};

// Template to do the processing
template <class PIX, int nComponents, int maxValue>
class ImageFilter : public FilterBase {
public:
    ImageFilter(OFX::ImageEffect &instance)
        : FilterBase(instance)
    {}

    void multiThreadProcessImages(OfxRectI procWindow)
    {
        for (int y = procWindow.y1; y < procWindow.y2; y++) {
            if (_effect.abort()) break;

            PIX *dstPix = (PIX *)_dstImg->getPixelAddress(procWindow.x1, y);

            for (int x = procWindow.x1; x < procWindow.x2; x++) {
                PIX *srcPix = (PIX *)(_srcImg ? _srcImg->getPixelAddress(x, y) : 0);

                if (srcPix) {
                    for (int c = 0; c < nComponents; ++c) {
                        if (nComponents != 4 || c != 3) { // Don't modify alpha channel
                            // Simple invert operation - modify this for your effect
                            if (maxValue == 1) { // float
                                dstPix[c] = maxValue - srcPix[c];
                            } else { // integer types
                                dstPix[c] = maxValue - srcPix[c];
                            }
                        } else {
                            dstPix[c] = srcPix[c]; // copy alpha
                        }
                    }
                } else {
                    // no source, make output black
                    for (int c = 0; c < nComponents; ++c) {
                        dstPix[c] = 0;
                    }
                }
                dstPix += nComponents;
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Plugin implementation

class FilterPlugin : public OFX::ImageEffect {
protected:
    OFX::Clip *_dstClip;
    OFX::Clip *_srcClip;

public:
    FilterPlugin(OfxImageEffectHandle handle)
        : ImageEffect(handle)
        , _dstClip(0)
        , _srcClip(0)
    {
        _dstClip = fetchClip(kOfxImageEffectOutputClipName);
        _srcClip = fetchClip(kOfxImageEffectSimpleSourceClipName);
    }

    virtual void render(const OFX::RenderArguments &args) override;
    virtual bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod) override;
    virtual void getRegionsOfInterest(const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois) override;
    virtual void getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences) override;

private:
    template <int nComponents>
    void renderForComponents(const OFX::RenderArguments &args);

    template <class PIX, int nComponents, int maxValue>
    void setupAndProcess(ImageFilter<PIX, nComponents, maxValue> &processor, const OFX::RenderArguments &args);
};

template <class PIX, int nComponents, int maxValue>
void FilterPlugin::setupAndProcess(ImageFilter<PIX, nComponents, maxValue> &processor, const OFX::RenderArguments &args)
{
    std::unique_ptr<OFX::Image> dst(_dstClip->fetchImage(args.time));
    if (!dst.get()) {
        OFX::throwSuiteStatusException(kOfxStatFailed);
    }

    std::unique_ptr<const OFX::Image> src((_srcClip && _srcClip->isConnected()) ?
                                        _srcClip->fetchImage(args.time) : 0);

    processor.setDstImg(dst.get());
    processor.setSrcImg(const_cast<OFX::Image *>(src.get()));
    processor.setRenderWindow(args.renderWindow);

    processor.process();
}

template <int nComponents>
void FilterPlugin::renderForComponents(const OFX::RenderArguments &args)
{
    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();

    switch (dstBitDepth) {
    case OFX::eBitDepthUByte: {
        ImageFilter<unsigned char, nComponents, 255> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    case OFX::eBitDepthUShort: {
        ImageFilter<unsigned short, nComponents, 65535> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    case OFX::eBitDepthFloat: {
        ImageFilter<float, nComponents, 1> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    default:
        OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
    }
}

void FilterPlugin::render(const OFX::RenderArguments &args)
{
    OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();

    switch (dstComponents) {
    case OFX::ePixelComponentRGBA:
        renderForComponents<4>(args);
        break;
    case OFX::ePixelComponentRGB:
        renderForComponents<3>(args);
        break;
    case OFX::ePixelComponentAlpha:
        renderForComponents<1>(args);
        break;
    default:
        OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
        break;
    }
}

bool FilterPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod)
{
    if (_srcClip && _srcClip->isConnected()) {
        rod = _srcClip->getRegionOfDefinition(args.time);
        return true;
    }
    return false;
}

void FilterPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois)
{
    rois.setRegionOfInterest(*_srcClip, args.regionOfInterest);
}

void FilterPlugin::getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences)
{
    // don't change anything from the defaults
}

////////////////////////////////////////////////////////////////////////////////
// Plugin factory

mDeclarePluginFactory(FilterPluginFactory, {}, {});

void FilterPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    desc.setLabels("Filter", "Filter", "Filter");
    desc.setPluginGrouping("Filter");
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);
    desc.setSingleInstance(false);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(true);
    desc.setSupportsTiles(true);
    desc.setTemporalClipAccess(false);
    desc.setRenderTwiceAlways(false);
    desc.setSupportsMultipleClipPARs(false);
}

void FilterPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context)
{
    OFX::ClipDescriptor *srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setTemporalClipAccess(false);
    srcClip->setSupportsTiles(true);
    srcClip->setIsMask(false);

    OFX::ClipDescriptor *dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(true);
}

OFX::ImageEffect* FilterPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::ContextEnum context)
{
    return new FilterPlugin(handle);
}

namespace OFX
{
    namespace Plugin
    {
        void getPluginIDs(PluginFactoryArray &ids)
        {
            static FilterPluginFactory p("com.example.filterplugin", 1, 0);
            ids.push_back(&p);
        }
    }
}