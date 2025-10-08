// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

// Simple transition plugin based on official OpenFX crossfade example
// This plugin blends between two input clips

#include <algorithm>
#include <cmath>
#include <memory>

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsProcessing.h"

////////////////////////////////////////////////////////////////////////////////
// Base class for the transition processor
class TransitionBase : public OFX::ImageProcessor {
protected:
    const OFX::Image *_fromImg;
    const OFX::Image *_toImg;
    float _blend;

public:
    TransitionBase(OFX::ImageEffect &instance)
        : OFX::ImageProcessor(instance)
        , _fromImg(0)
        , _toImg(0)
        , _blend(0.5f)
    {
    }

    void setFromImg(const OFX::Image *v) { _fromImg = v; }
    void setToImg(const OFX::Image *v) { _toImg = v; }
    void setBlend(float v) { _blend = v; }
};

// Template to do the processing
template <class PIX, int nComponents, int maxValue>
class ImageTransition : public TransitionBase {
public:
    ImageTransition(OFX::ImageEffect &instance)
        : TransitionBase(instance)
    {}

    static PIX lerp(const PIX &v1, const PIX &v2, float blend)
    {
        return PIX((v2 - v1) * blend + v1);
    }

    void multiThreadProcessImages(OfxRectI procWindow)
    {
        float blend = _blend;

        for (int y = procWindow.y1; y < procWindow.y2; y++) {
            if (_effect.abort()) break;

            PIX *dstPix = (PIX *)_dstImg->getPixelAddress(procWindow.x1, y);

            for (int x = procWindow.x1; x < procWindow.x2; x++) {
                PIX *fromPix = (PIX *)(_fromImg ? _fromImg->getPixelAddress(x, y) : 0);
                PIX *toPix = (PIX *)(_toImg ? _toImg->getPixelAddress(x, y) : 0);

                if (fromPix && toPix) {
                    for (int c = 0; c < nComponents; ++c) {
                        dstPix[c] = lerp(fromPix[c], toPix[c], blend);
                    }
                } else if (fromPix && blend <= 0.5f) {
                    // Use from image when to image is missing and we're closer to from
                    for (int c = 0; c < nComponents; ++c) {
                        dstPix[c] = fromPix[c];
                    }
                } else if (toPix && blend > 0.5f) {
                    // Use to image when from image is missing and we're closer to to
                    for (int c = 0; c < nComponents; ++c) {
                        dstPix[c] = toPix[c];
                    }
                } else {
                    // No source, make output black
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

class TransitionPlugin : public OFX::ImageEffect {
protected:
    OFX::Clip *_dstClip;
    OFX::Clip *_fromClip;
    OFX::Clip *_toClip;
    OFX::DoubleParam *_transitionParam;

public:
    TransitionPlugin(OfxImageEffectHandle handle)
        : ImageEffect(handle)
        , _dstClip(0)
        , _fromClip(0)
        , _toClip(0)
        , _transitionParam(0)
    {
        _dstClip = fetchClip(kOfxImageEffectOutputClipName);
        _fromClip = fetchClip(kOfxImageEffectTransitionSourceFromClipName);
        _toClip = fetchClip(kOfxImageEffectTransitionSourceToClipName);
        _transitionParam = fetchDoubleParam("Transition");
    }

    virtual void render(const OFX::RenderArguments &args) override;
    virtual bool isIdentity(const OFX::IsIdentityArguments &args, OFX::Clip * &identityClip, double &identityTime) override;
    virtual bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod) override;
    virtual void getRegionsOfInterest(const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois) override;
    virtual void getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences) override;

private:
    template <int nComponents>
    void renderForComponents(const OFX::RenderArguments &args);

    template <class PIX, int nComponents, int maxValue>
    void setupAndProcess(ImageTransition<PIX, nComponents, maxValue> &processor, const OFX::RenderArguments &args);
};

template <class PIX, int nComponents, int maxValue>
void TransitionPlugin::setupAndProcess(ImageTransition<PIX, nComponents, maxValue> &processor, const OFX::RenderArguments &args)
{
    std::unique_ptr<OFX::Image> dst(_dstClip->fetchImage(args.time));
    if (!dst.get()) {
        OFX::throwSuiteStatusException(kOfxStatFailed);
    }

    std::unique_ptr<const OFX::Image> fromImg((_fromClip && _fromClip->isConnected()) ?
                                        _fromClip->fetchImage(args.time) : 0);
    std::unique_ptr<const OFX::Image> toImg((_toClip && _toClip->isConnected()) ?
                                        _toClip->fetchImage(args.time) : 0);

    processor.setDstImg(dst.get());
    processor.setFromImg(fromImg.get());
    processor.setToImg(toImg.get());
    processor.setRenderWindow(args.renderWindow);

    // Get the transition value (0.0 = fully from image, 1.0 = fully to image)
    float blend = (float)_transitionParam->getValueAtTime(args.time);
    processor.setBlend(blend);

    processor.process();
}

template <int nComponents>
void TransitionPlugin::renderForComponents(const OFX::RenderArguments &args)
{
    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();

    switch (dstBitDepth) {
    case OFX::eBitDepthUByte: {
        ImageTransition<unsigned char, nComponents, 255> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    case OFX::eBitDepthUShort: {
        ImageTransition<unsigned short, nComponents, 65535> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    case OFX::eBitDepthFloat: {
        ImageTransition<float, nComponents, 1> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    default:
        OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
    }
}

void TransitionPlugin::render(const OFX::RenderArguments &args)
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

bool TransitionPlugin::isIdentity(const OFX::IsIdentityArguments &args, OFX::Clip * &identityClip, double &identityTime)
{
    float blend = (float)_transitionParam->getValueAtTime(args.time);
    identityTime = args.time;

    // At the start? Use from clip
    if (blend <= 0.0) {
        identityClip = _fromClip;
        return true;
    }

    // At the end? Use to clip
    if (blend >= 1.0) {
        identityClip = _toClip;
        return true;
    }

    // Blending needed
    return false;
}

bool TransitionPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod)
{
    OfxRectD fromRoD = {0, 0, 0, 0};
    OfxRectD toRoD = {0, 0, 0, 0};

    if (_fromClip && _fromClip->isConnected()) {
        fromRoD = _fromClip->getRegionOfDefinition(args.time);
    }

    if (_toClip && _toClip->isConnected()) {
        toRoD = _toClip->getRegionOfDefinition(args.time);
    }

    // Union of both clips
    rod.x1 = std::min(fromRoD.x1, toRoD.x1);
    rod.y1 = std::min(fromRoD.y1, toRoD.y1);
    rod.x2 = std::max(fromRoD.x2, toRoD.x2);
    rod.y2 = std::max(fromRoD.y2, toRoD.y2);

    return true;
}

void TransitionPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois)
{
    rois.setRegionOfInterest(*_fromClip, args.regionOfInterest);
    rois.setRegionOfInterest(*_toClip, args.regionOfInterest);
}

void TransitionPlugin::getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences)
{
    // Don't change anything from the defaults
}

////////////////////////////////////////////////////////////////////////////////
// Plugin factory

mDeclarePluginFactory(TransitionPluginFactory, {}, {});

void TransitionPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    desc.setLabels("Transition", "Transition", "Transition");
    desc.setPluginGrouping("Transition");
    desc.addSupportedContext(OFX::eContextTransition);
    desc.addSupportedContext(OFX::eContextGeneral);
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

void TransitionPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum /*context*/)
{
    // Define the sourceFrom input clip
    OFX::ClipDescriptor *fromClip = desc.defineClip(kOfxImageEffectTransitionSourceFromClipName);
    fromClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    fromClip->addSupportedComponent(OFX::ePixelComponentRGB);
    fromClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    fromClip->setTemporalClipAccess(false);
    fromClip->setSupportsTiles(true);

    // Define the sourceTo input clip
    OFX::ClipDescriptor *toClip = desc.defineClip(kOfxImageEffectTransitionSourceToClipName);
    toClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    toClip->addSupportedComponent(OFX::ePixelComponentRGB);
    toClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    toClip->setTemporalClipAccess(false);
    toClip->setSupportsTiles(true);

    // Create the mandated output clip
    OFX::ClipDescriptor *dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(true);

    // Define the mandated "Transition" param
    OFX::DoubleParamDescriptor *param = desc.defineDoubleParam("Transition");
    param->setLabels("transition", "transition", "transition");
    param->setScriptName("transition");
    param->setHint("Transition between the two input clips (0.0 = first clip, 1.0 = second clip).");
    param->setDefault(0.5);
    param->setRange(0.0, 1.0);
    param->setDisplayRange(0.0, 1.0);
    param->setAnimates(true);
    param->setDoubleType(OFX::eDoubleTypeScale);

    OFX::PageParamDescriptor *page = desc.definePageParam("Controls");
    page->addChild(*param);
}

OFX::ImageEffect* TransitionPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::ContextEnum /*context*/)
{
    return new TransitionPlugin(handle);
}

namespace OFX
{
    namespace Plugin
    {
        void getPluginIDs(PluginFactoryArray &ids)
        {
            static TransitionPluginFactory p("com.example.transitionplugin", 1, 0);
            ids.push_back(&p);
        }
    }
}