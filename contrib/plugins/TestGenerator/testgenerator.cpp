// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

// Simple generator plugin based on official OpenFX noise example
// This plugin generates noise patterns

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsProcessing.h"

////////////////////////////////////////////////////////////////////////////////
// Base class for the generator processor
class GeneratorBase : public OFX::ImageProcessor {
protected:
    float _noiseLevel;
    uint32_t _seed;

public:
    GeneratorBase(OFX::ImageEffect &instance)
        : OFX::ImageProcessor(instance)
        , _noiseLevel(0.5f)
        , _seed(0)
    {
    }

    void setNoiseLevel(float v) { _noiseLevel = v; }
    void setSeed(uint32_t v) { _seed = v; }
};

// Template to do the processing
template <class PIX, int nComponents, int maxValue>
class ImageGenerator : public GeneratorBase {
public:
    ImageGenerator(OFX::ImageEffect &instance)
        : GeneratorBase(instance)
    {}

    void multiThreadProcessImages(OfxRectI procWindow)
    {
        float noiseLevel = _noiseLevel;

        // set up a random number generator
        std::random_device rd;
        std::mt19937_64 mt(rd());
        mt.seed(_seed + procWindow.y1);
        std::uniform_real_distribution<double> dist(0.0, maxValue * noiseLevel);

        // push pixels
        for (int y = procWindow.y1; y < procWindow.y2; y++) {
            if (_effect.abort()) break;

            PIX *dstPix = (PIX *)_dstImg->getPixelAddress(procWindow.x1, y);

            for (int x = procWindow.x1; x < procWindow.x2; x++) {
                for (int c = 0; c < nComponents; c++) {
                    double randValue = dist(mt);

                    if (maxValue == 1) { // implies floating point, so don't clamp
                        dstPix[c] = PIX(randValue);
                    } else { // integer base one, clamp it
                        dstPix[c] = randValue < 0 ? 0 : (randValue > maxValue ? maxValue : PIX(randValue));
                    }
                }
                dstPix += nComponents;
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Plugin implementation

class GeneratorPlugin : public OFX::ImageEffect {
protected:
    OFX::Clip *_dstClip;
    OFX::DoubleParam *_noiseParam;

public:
    GeneratorPlugin(OfxImageEffectHandle handle)
        : ImageEffect(handle)
        , _dstClip(0)
        , _noiseParam(0)
    {
        _dstClip = fetchClip(kOfxImageEffectOutputClipName);
        _noiseParam = fetchDoubleParam("Noise");
    }

    virtual void render(const OFX::RenderArguments &args) override;
    virtual bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod) override;
    virtual void getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences) override;

private:
    template <int nComponents>
    void renderForComponents(const OFX::RenderArguments &args);

    template <class PIX, int nComponents, int maxValue>
    void setupAndProcess(ImageGenerator<PIX, nComponents, maxValue> &processor, const OFX::RenderArguments &args);
};

template <class PIX, int nComponents, int maxValue>
void GeneratorPlugin::setupAndProcess(ImageGenerator<PIX, nComponents, maxValue> &processor, const OFX::RenderArguments &args)
{
    std::unique_ptr<OFX::Image> dst(_dstClip->fetchImage(args.time));
    if (!dst.get()) {
        OFX::throwSuiteStatusException(kOfxStatFailed);
    }

    processor.setDstImg(dst.get());
    processor.setRenderWindow(args.renderWindow);

    // set the noise level
    processor.setNoiseLevel((float)_noiseParam->getValueAtTime(args.time));

    // set the seed based on the current time
    processor.setSeed(uint32_t(args.time * 2.0f + 2000.0f));

    processor.process();
}

template <int nComponents>
void GeneratorPlugin::renderForComponents(const OFX::RenderArguments &args)
{
    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();

    switch (dstBitDepth) {
    case OFX::eBitDepthUByte: {
        ImageGenerator<unsigned char, nComponents, 255> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    case OFX::eBitDepthUShort: {
        ImageGenerator<unsigned short, nComponents, 65535> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    case OFX::eBitDepthFloat: {
        ImageGenerator<float, nComponents, 1> processor(*this);
        setupAndProcess(processor, args);
        break;
    }
    default:
        OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
    }
}

void GeneratorPlugin::render(const OFX::RenderArguments &args)
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

bool GeneratorPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &/*args*/, OfxRectD &rod)
{
    // we can generate noise anywhere on the image plane, so set our RoD to be infinite
    rod.x1 = rod.y1 = kOfxFlagInfiniteMin;
    rod.x2 = rod.y2 = kOfxFlagInfiniteMax;
    return true;
}

void GeneratorPlugin::getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences)
{
    clipPreferences.setOutputFrameVarying(true);
}

////////////////////////////////////////////////////////////////////////////////
// Plugin factory

mDeclarePluginFactory(GeneratorPluginFactory, {}, {});

void GeneratorPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    desc.setLabels("Generator", "Generator", "Generator");
    desc.setPluginGrouping("Generator");
    desc.addSupportedContext(OFX::eContextGenerator);
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

void GeneratorPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum /*context*/)
{
    // there has to be an input clip, even for generators
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(true);
    srcClip->setOptional(true);

    OFX::ClipDescriptor *dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(true);
    dstClip->setFieldExtraction(OFX::eFieldExtractSingle);

    OFX::DoubleParamDescriptor *param = desc.defineDoubleParam("Noise");
    param->setLabels("noise", "noise", "noise");
    param->setScriptName("noise");
    param->setHint("How much noise to make.");
    param->setDefault(0.2);
    param->setRange(0, 10);
    param->setIncrement(0.1);
    param->setDisplayRange(0, 1);
    param->setAnimates(true);
    param->setDoubleType(OFX::eDoubleTypeScale);

    OFX::PageParamDescriptor *page = desc.definePageParam("Controls");
    page->addChild(*param);
}

OFX::ImageEffect* GeneratorPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::ContextEnum /*context*/)
{
    return new GeneratorPlugin(handle);
}

namespace OFX
{
    namespace Plugin
    {
        void getPluginIDs(PluginFactoryArray &ids)
        {
            static GeneratorPluginFactory p("com.example.generatorplugin", 1, 0);
            ids.push_back(&p);
        }
    }
}