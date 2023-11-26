#include "GainPlugin.h"

#include <stdio.h>

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsProcessing.h"
#include "ofxsLog.h"

#define kPluginName "Gain"
#define kPluginGrouping "OpenFX Sample"
#define kPluginDescription "Apply seperate RGB gain adjustments to each channels"
#define kPluginIdentifier "com.OpenFXSample.Gain"
#define kPluginVersionMajor 1
#define kPluginVersionMinor 0

#define kSupportsTiles false
#define kSupportsMultiResolution false
#define kSupportsMultipleClipPARs false

////////////////////////////////////////////////////////////////////////////////

class ImageScaler : public OFX::ImageProcessor
{
public:
    explicit ImageScaler(OFX::ImageEffect& p_Instance);

    virtual void processImagesCUDA();
    virtual void processImagesOpenCL();
    virtual void processImagesMetal();
    virtual void multiThreadProcessImages(OfxRectI p_ProcWindow);

    void setSrcImg(OFX::Image* p_SrcImg);
    void setScales(float p_ScaleR, float p_ScaleG, float p_ScaleB, float p_ScaleA);

private:
    OFX::Image* _srcImg;
    float _scales[4];
};

ImageScaler::ImageScaler(OFX::ImageEffect& p_Instance)
    : OFX::ImageProcessor(p_Instance)
{
}

#ifndef __APPLE__
extern void RunCudaKernel(void* p_Stream, int p_Width, int p_Height, float* p_Gain, const float* p_Input, float* p_Output);
#endif

void ImageScaler::processImagesCUDA()
{
#ifndef __APPLE__
    const OfxRectI& bounds = _srcImg->getBounds();
    const int width = bounds.x2 - bounds.x1;
    const int height = bounds.y2 - bounds.y1;

    float* input = static_cast<float*>(_srcImg->getPixelData());
    float* output = static_cast<float*>(_dstImg->getPixelData());

    RunCudaKernel(_pCudaStream, width, height, _scales, input, output);
#endif
}

#ifdef __APPLE__
extern void RunMetalKernel(void* p_CmdQ, int p_Width, int p_Height, float* p_Gain, const float* p_Input, float* p_Output);
#endif

void ImageScaler::processImagesMetal()
{
#ifdef __APPLE__
    const OfxRectI& bounds = _srcImg->getBounds();
    const int width = bounds.x2 - bounds.x1;
    const int height = bounds.y2 - bounds.y1;

    float* input = static_cast<float*>(_srcImg->getPixelData());
    float* output = static_cast<float*>(_dstImg->getPixelData());

    RunMetalKernel(_pMetalCmdQ, width, height, _scales, input, output);
#endif
}

extern void RunOpenCLKernel(void* p_CmdQ, int p_Width, int p_Height, float* p_Gain, const float* p_Input, float* p_Output);

void ImageScaler::processImagesOpenCL()
{
    const OfxRectI& bounds = _srcImg->getBounds();
    const int width = bounds.x2 - bounds.x1;
    const int height = bounds.y2 - bounds.y1;

    float* input = static_cast<float*>(_srcImg->getPixelData());
    float* output = static_cast<float*>(_dstImg->getPixelData());

    RunOpenCLKernel(_pOpenCLCmdQ, width, height, _scales, input, output);
}

void ImageScaler::multiThreadProcessImages(OfxRectI p_ProcWindow)
{
    for (int y = p_ProcWindow.y1; y < p_ProcWindow.y2; ++y)
    {
        if (_effect.abort()) break;

        float* dstPix = static_cast<float*>(_dstImg->getPixelAddress(p_ProcWindow.x1, y));

        for (int x = p_ProcWindow.x1; x < p_ProcWindow.x2; ++x)
        {
            float* srcPix = static_cast<float*>(_srcImg ? _srcImg->getPixelAddress(x, y) : 0);

            // do we have a source image to scale up
            if (srcPix)
            {
                for(int c = 0; c < 4; ++c)
                {
                    dstPix[c] = srcPix[c] * _scales[c];
                }
            }
            else
            {
                // no src pixel here, be black and transparent
                for (int c = 0; c < 4; ++c)
                {
                    dstPix[c] = 0;
                }
            }

            // increment the dst pixel
            dstPix += 4;
        }
    }
}

void ImageScaler::setSrcImg(OFX::Image* p_SrcImg)
{
    _srcImg = p_SrcImg;
}

void ImageScaler::setScales(float p_ScaleR, float p_ScaleG, float p_ScaleB, float p_ScaleA)
{
    _scales[0] = p_ScaleR;
    _scales[1] = p_ScaleG;
    _scales[2] = p_ScaleB;
    _scales[3] = p_ScaleA;
}

////////////////////////////////////////////////////////////////////////////////
/** @brief The plugin that does our work */
class GainPlugin : public OFX::ImageEffect
{
public:
    explicit GainPlugin(OfxImageEffectHandle p_Handle);

    /* Override the render */
    virtual void render(const OFX::RenderArguments& p_Args);

    /* Override is identity */
    virtual bool isIdentity(const OFX::IsIdentityArguments& p_Args, OFX::Clip*& p_IdentityClip, double& p_IdentityTime);

    /* Override changedParam */
    virtual void changedParam(const OFX::InstanceChangedArgs& p_Args, const std::string& p_ParamName);

    /* Override changed clip */
    virtual void changedClip(const OFX::InstanceChangedArgs& p_Args, const std::string& p_ClipName);

    /* Set the enabledness of the component scale params depending on the type of input image and the state of the scaleComponents param */
    void setEnabledness();

    /* Set up and run a processor */
    void setupAndProcess(ImageScaler &p_ImageScaler, const OFX::RenderArguments& p_Args);

private:
    // Does not own the following pointers
    OFX::Clip* m_DstClip;
    OFX::Clip* m_SrcClip;

    OFX::DoubleParam* m_Scale;
    OFX::DoubleParam* m_ScaleR;
    OFX::DoubleParam* m_ScaleG;
    OFX::DoubleParam* m_ScaleB;
    OFX::DoubleParam* m_ScaleA;
    OFX::BooleanParam* m_ComponentScalesEnabled;
};

GainPlugin::GainPlugin(OfxImageEffectHandle p_Handle)
    : ImageEffect(p_Handle)
{
    m_DstClip = fetchClip(kOfxImageEffectOutputClipName);
    m_SrcClip = fetchClip(kOfxImageEffectSimpleSourceClipName);

    m_Scale = fetchDoubleParam("scale");
    m_ScaleR = fetchDoubleParam("scaleR");
    m_ScaleG = fetchDoubleParam("scaleG");
    m_ScaleB = fetchDoubleParam("scaleB");
    m_ScaleA = fetchDoubleParam("scaleA");
    m_ComponentScalesEnabled = fetchBooleanParam("scaleComponents");

    // Set the enabledness of our RGBA sliders
    setEnabledness();
}

void GainPlugin::render(const OFX::RenderArguments& p_Args)
{
    if ((m_DstClip->getPixelDepth() == OFX::eBitDepthFloat) && (m_DstClip->getPixelComponents() == OFX::ePixelComponentRGBA))
    {
        ImageScaler imageScaler(*this);
        setupAndProcess(imageScaler, p_Args);
    }
    else
    {
        OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
    }
}

bool GainPlugin::isIdentity(const OFX::IsIdentityArguments& p_Args, OFX::Clip*& p_IdentityClip, double& p_IdentityTime)
{
    double rScale = 1.0, gScale = 1.0, bScale = 1.0, aScale = 1.0;

    if (m_ComponentScalesEnabled->getValueAtTime(p_Args.time))
    {
        rScale = m_ScaleR->getValueAtTime(p_Args.time);
        gScale = m_ScaleG->getValueAtTime(p_Args.time);
        bScale = m_ScaleB->getValueAtTime(p_Args.time);
        aScale = m_ScaleA->getValueAtTime(p_Args.time);
    }

    const double scale = m_Scale->getValueAtTime(p_Args.time);
    rScale *= scale;
    gScale *= scale;
    bScale *= scale;

    if ((rScale == 1.0) && (gScale == 1.0) && (bScale == 1.0) && (aScale == 1.0))
    {
        p_IdentityClip = m_SrcClip;
        p_IdentityTime = p_Args.time;
        return true;
    }

    return false;
}

void GainPlugin::changedParam(const OFX::InstanceChangedArgs& p_Args, const std::string& p_ParamName)
{
    if (p_ParamName == "scaleComponents")
    {
        setEnabledness();
    }
}

void GainPlugin::changedClip(const OFX::InstanceChangedArgs& p_Args, const std::string& p_ClipName)
{
    if (p_ClipName == kOfxImageEffectSimpleSourceClipName)
    {
        setEnabledness();
    }
}

void GainPlugin::setEnabledness()
{
    // the component enabledness depends on the clip being RGBA and the param being true
    const bool enable = (m_ComponentScalesEnabled->getValue() && (m_SrcClip->getPixelComponents() == OFX::ePixelComponentRGBA));

    m_ScaleR->setEnabled(enable);
    m_ScaleG->setEnabled(enable);
    m_ScaleB->setEnabled(enable);
    m_ScaleA->setEnabled(enable);
}

void GainPlugin::setupAndProcess(ImageScaler& p_ImageScaler, const OFX::RenderArguments& p_Args)
{
    // Get the dst image
    std::auto_ptr<OFX::Image> dst(m_DstClip->fetchImage(p_Args.time));
    OFX::BitDepthEnum dstBitDepth = dst->getPixelDepth();
    OFX::PixelComponentEnum dstComponents = dst->getPixelComponents();

    // Get the src image
    std::auto_ptr<OFX::Image> src(m_SrcClip->fetchImage(p_Args.time));
    OFX::BitDepthEnum srcBitDepth = src->getPixelDepth();
    OFX::PixelComponentEnum srcComponents = src->getPixelComponents();

    // Check to see if the bit depth and number of components are the same
    if ((srcBitDepth != dstBitDepth) || (srcComponents != dstComponents))
    {
        OFX::throwSuiteStatusException(kOfxStatErrValue);
    }

    double rScale = 1.0, gScale = 1.0, bScale = 1.0, aScale = 1.0;

    if (m_ComponentScalesEnabled->getValueAtTime(p_Args.time))
    {
        rScale = m_ScaleR->getValueAtTime(p_Args.time);
        gScale = m_ScaleG->getValueAtTime(p_Args.time);
        bScale = m_ScaleB->getValueAtTime(p_Args.time);
        aScale = m_ScaleA->getValueAtTime(p_Args.time);
    }

    const double scale = m_Scale->getValueAtTime(p_Args.time);
    rScale *= scale;
    gScale *= scale;
    bScale *= scale;

    // Set the images
    p_ImageScaler.setDstImg(dst.get());
    p_ImageScaler.setSrcImg(src.get());

    // Setup OpenCL and CUDA Render arguments
    p_ImageScaler.setGPURenderArgs(p_Args);

    // Set the render window
    p_ImageScaler.setRenderWindow(p_Args.renderWindow);

    // Set the scales
    p_ImageScaler.setScales(rScale, gScale, bScale, aScale);

    // Call the base class process member, this will call the derived templated process code
    p_ImageScaler.process();
}

////////////////////////////////////////////////////////////////////////////////

using namespace OFX;

GainPluginFactory::GainPluginFactory()
    : OFX::PluginFactoryHelper<GainPluginFactory>(kPluginIdentifier, kPluginVersionMajor, kPluginVersionMinor)
{
}

void GainPluginFactory::describe(OFX::ImageEffectDescriptor& p_Desc)
{
    // Basic labels
    p_Desc.setLabels(kPluginName, kPluginName, kPluginName);
    p_Desc.setPluginGrouping(kPluginGrouping);
    p_Desc.setPluginDescription(kPluginDescription);

    // Add the supported contexts, only filter at the moment
    p_Desc.addSupportedContext(eContextFilter);
    p_Desc.addSupportedContext(eContextGeneral);

    // Add supported pixel depths
    p_Desc.addSupportedBitDepth(eBitDepthFloat);

    // Set a few flags
    p_Desc.setSingleInstance(false);
    p_Desc.setHostFrameThreading(false);
    p_Desc.setSupportsMultiResolution(kSupportsMultiResolution);
    p_Desc.setSupportsTiles(kSupportsTiles);
    p_Desc.setTemporalClipAccess(false);
    p_Desc.setRenderTwiceAlways(false);
    p_Desc.setSupportsMultipleClipPARs(kSupportsMultipleClipPARs);

    // Setup OpenCL render capability flags
    p_Desc.setSupportsOpenCLRender(true);

    // Setup CUDA render capability flags on non-Apple system
#ifndef __APPLE__
    p_Desc.setSupportsCudaRender(true);
    p_Desc.setSupportsCudaStream(true);
#endif

    // Setup Metal render capability flags only on Apple system
#ifdef __APPLE__
    p_Desc.setSupportsMetalRender(true);
#endif

    // Indicates that the plugin output does not depend on location or neighbours of a given pixel.
    // Therefore, this plugin could be executed during LUT generation.
    p_Desc.setNoSpatialAwareness(true);
}

static DoubleParamDescriptor* defineScaleParam(OFX::ImageEffectDescriptor& p_Desc, const std::string& p_Name, const std::string& p_Label,
                                               const std::string& p_Hint, GroupParamDescriptor* p_Parent)
{
    DoubleParamDescriptor* param = p_Desc.defineDoubleParam(p_Name);
    param->setLabels(p_Label, p_Label, p_Label);
    param->setScriptName(p_Name);
    param->setHint(p_Hint);
    param->setDefault(1);
    param->setRange(0, 10);
    param->setIncrement(0.1);
    param->setDisplayRange(0, 10);
    param->setDoubleType(eDoubleTypeScale);

    if (p_Parent)
    {
        param->setParent(*p_Parent);
    }

    return param;
}


void GainPluginFactory::describeInContext(OFX::ImageEffectDescriptor& p_Desc, OFX::ContextEnum /*p_Context*/)
{
    // Source clip only in the filter context
    // Create the mandated source clip
    ClipDescriptor* srcClip = p_Desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(ePixelComponentRGBA);
    srcClip->setTemporalClipAccess(false);
    srcClip->setSupportsTiles(kSupportsTiles);
    srcClip->setIsMask(false);

    // Create the mandated output clip
    ClipDescriptor* dstClip = p_Desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(ePixelComponentRGBA);
    dstClip->addSupportedComponent(ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportsTiles);

    // Make some pages and to things in
    PageParamDescriptor* page = p_Desc.definePageParam("Controls");

    // Group param to group the scales
    GroupParamDescriptor* componentScalesGroup = p_Desc.defineGroupParam("componentScales");
    componentScalesGroup->setHint("Scales on the individual component");
    componentScalesGroup->setLabels("Components", "Components", "Components");

    // Make overall scale params
    DoubleParamDescriptor* param = defineScaleParam(p_Desc, "scale", "scale", "Scales all component in the image", 0);
    page->addChild(*param);

    // Add a boolean to enable the component scale
    BooleanParamDescriptor* boolParam = p_Desc.defineBooleanParam("scaleComponents");
    boolParam->setDefault(true);
    boolParam->setHint("Enables scales on individual components");
    boolParam->setLabels("Scale Components", "Scale Components", "Scale Components");
    boolParam->setParent(*componentScalesGroup);
    page->addChild(*boolParam);

    // Make the four component scale params
    param = defineScaleParam(p_Desc, "scaleR", "red", "Scales the red component of the image", componentScalesGroup);
    page->addChild(*param);

    param = defineScaleParam(p_Desc, "scaleG", "green", "Scales the green component of the image", componentScalesGroup);
    page->addChild(*param);

    param = defineScaleParam(p_Desc, "scaleB", "blue", "Scales the blue component of the image", componentScalesGroup);
    page->addChild(*param);

    param = defineScaleParam(p_Desc, "scaleA", "alpha", "Scales the alpha component of the image", componentScalesGroup);
    page->addChild(*param);
}

ImageEffect* GainPluginFactory::createInstance(OfxImageEffectHandle p_Handle, ContextEnum /*p_Context*/)
{
    return new GainPlugin(p_Handle);
}

void OFX::Plugin::getPluginIDs(PluginFactoryArray& p_FactoryArray)
{
    static GainPluginFactory gainPlugin;
    p_FactoryArray.push_back(&gainPlugin);
}
