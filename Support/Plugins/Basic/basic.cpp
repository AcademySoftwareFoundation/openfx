
#include <stdio.h>
#include "ofxsImageEffect.H"
#include "ofxsMultiThread.H"

////////////////////////////////////////////////////////////////////////////////
/** @brief base class of the plugin */
class BasicPlugin : public OFX::ImageEffect {
protected :
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip *dstClip_;
    OFX::Clip *srcClip_;

    OFX::DoubleParam  *scale_;
    OFX::DoubleParam  *rScale_;
    OFX::DoubleParam  *gScale_;
    OFX::DoubleParam  *bScale_;
    OFX::DoubleParam  *aScale_;
    OFX::BooleanParam *componentScalesEnabled_;

public :
    /** @brief ctor */
    BasicPlugin(OfxImageEffectHandle handle)
      : ImageEffect(handle)
      , dstClip_(0)
      , srcClip_(0)
      , scale_(0)
      , rScale_(0)
      , gScale_(0)
      , bScale_(0)
      , aScale_(0)
      , componentScalesEnabled_(0)
    {
        dstClip_ = fetchClip("Output");
        srcClip_ = fetchClip("Source");
        scale_   = fetchDoubleParam("scale");
        rScale_  = fetchDoubleParam("scaleR");
        gScale_  = fetchDoubleParam("scaleG");
        bScale_  = fetchDoubleParam("scaleB");
        aScale_  = fetchDoubleParam("scaleA");
        componentScalesEnabled_ = fetchBooleanParam("scaleComponents");
    }

    /* sets the enabledness of the component scale params depending on the type of input image and the state of the scaleComponents param */
    bool setEnabledness(double time);

    /* Override the render */
    virtual void render(const OFX::RenderArguments &args);

    /* override is identity */
    virtual bool isIdentity(const OFX::RenderArguments &args, OFX::Clip * &identityClip, double &identityTime);
    
    /* override changedParam */
    virtual void changedParam(const OFX::InstanceChangedArgs &args, const std::string &paramName);

    /* override changed clip */
    virtual void changedClip(const OFX::InstanceChangedArgs &args, const std::string &clipName);
};


////////////////////////////////////////////////////////////////////////////////
/** @brief render for the filter */

////////////////////////////////////////////////////////////////////////////////
// rendering routines

template <class T> inline T
Minimum(T a, T b) {    return (a < b) ? a : b;}

template <class T> inline T 
Clamp(T v, int min, int max)
{
    if(v < T(min)) return T(min);
    if(v > T(max)) return T(max);
    return v;
}

// look up a pixel in the image, does bounds checking to see if it is in the image rectangle
template <class PIX> inline PIX *
pixelAddress(PIX *img, OfxRectI rect, int x, int y, int bytesPerLine)
{  
    if(x < rect.x1 || x >= rect.x2 || y < rect.y1 || y > rect.y2)
        return 0;
    PIX *pix = (PIX *) (((char *) img) + (y - rect.y1) * bytesPerLine);
    pix += x - rect.x1;  
    return pix;
}

////////////////////////////////////////////////////////////////////////////////
// base class to process images with
class Processor : public OFX::MultiThread::Processor {
protected :
    OFX::ImageEffect *  instance;
    float         rScale, gScale, bScale, aScale;
    void *srcV, *dstV, *maskV; 
    OfxRectI srcRect, dstRect, maskRect;
    int srcBytesPerLine, dstBytesPerLine, maskBytesPerLine;
    OfxRectI  window;

public :
    Processor(OFX::ImageEffect *  inst,
              float rScal, float gScal, float bScal, float aScal,
              void *src, OfxRectI sRect, int sBytesPerLine,
              void *dst, OfxRectI dRect, int dBytesPerLine,
              void *mask, OfxRectI mRect, int mBytesPerLine,
              OfxRectI  win)
      : instance(inst)
      , rScale(rScal)
      , gScale(gScal)
      , bScale(bScal)
      , aScale(aScal)
      , srcV(src)
      , dstV(dst)
      , maskV(mask)
      , srcRect(sRect)
      , dstRect(dRect)
      , maskRect(mRect)
      , srcBytesPerLine(sBytesPerLine)
      , dstBytesPerLine(dBytesPerLine)
      , maskBytesPerLine(mBytesPerLine)
      , window(win)
    {}  

    // @brief this function is called once per thread by the base calls
    void multiThreadFunction(unsigned int threadId, unsigned int nThreads);

    // this is called by  multiThreadFunction to actually process images
    virtual void processImages(OfxRectI window) = 0;
};


// function call once for each thread by the host
void
Processor::multiThreadFunction(unsigned int threadId, unsigned int nThreads)
{
    // slice the y range into the number of threads it has
    unsigned int dy = window.y2 - window.y1;
  
    unsigned int y1 = window.y1 + threadId * dy/nThreads;
    unsigned int y2 = window.y1 + Minimum((threadId + 1) * dy/nThreads, dy);

    OfxRectI win = window;
    win.y1 = y1; win.y2 = y2;

    // and render that thread on each
    processImages(win);  
}

// template to do the RGBA processing
template <class PIX, class MASK, int max, int isFloat>
class ProcessRGBA : public Processor {
public :
    ProcessRGBA(OFX::ImageEffect *  instance,
                float rScale, float gScale, float bScale, float aScale,
                void *srcV, OfxRectI srcRect, int srcBytesPerLine,
                void *dstV, OfxRectI dstRect, int dstBytesPerLine,
                void *maskV, OfxRectI maskRect, int maskBytesPerLine,
                OfxRectI  window)
      : Processor(instance,
                  rScale, gScale, bScale, aScale,
                  srcV,  srcRect,  srcBytesPerLine,
                  dstV,  dstRect,  dstBytesPerLine,
                  maskV,  maskRect, maskBytesPerLine,
                  window)
    {
    }

    void processImages(OfxRectI procWindow)
    {
        PIX *src = (PIX *) srcV;
        PIX *dst = (PIX *) dstV;
        MASK *mask = (MASK *) maskV;

        for(int y = procWindow.y1; y < procWindow.y2; y++) {
            if(instance->abort()) break;

            PIX *dstPix = pixelAddress(dst, dstRect, procWindow.x1, y, dstBytesPerLine);

            for(int x = procWindow.x1; x < procWindow.x2; x++) {
        
                PIX *srcPix = pixelAddress(src, srcRect, x, y, srcBytesPerLine);
        
        
                // do any pixel masking?
                float maskV = 1.0f;
                if(mask) {
                    MASK *maskPix = pixelAddress(mask, maskRect, x, y, maskBytesPerLine);
                    if(maskPix) {
                        maskV = float(*maskPix)/float(max);
                    }
                    maskPix++;
                }

                // figure the scale values per component
                float sR = 1.0 + (rScale - 1.0) * maskV;
                float sG = 1.0 + (gScale - 1.0) * maskV;
                float sB = 1.0 + (bScale - 1.0) * maskV;
                float sA = 1.0 + (aScale - 1.0) * maskV;

                if(srcPix) {
                    // switch will be compiled out
                    if(isFloat) {
                        dstPix->r = srcPix->r * sR;
                        dstPix->g = srcPix->g * sG;
                        dstPix->b = srcPix->b * sB;
                        dstPix->a = srcPix->a * sA;
                    }
                    else {
                        dstPix->r = Clamp(int(srcPix->r * sR), 0, max);
                        dstPix->g = Clamp(int(srcPix->g * sG), 0, max);
                        dstPix->b = Clamp(int(srcPix->b * sB), 0, max);
                        dstPix->a = Clamp(int(srcPix->a * sA), 0, max);
                    }
                    srcPix++;
                }
                else {
                    dstPix->r = dstPix->g = dstPix->b = dstPix->a= 0;
                }
                dstPix++;
            }
        }
    }
};

// template to do the Alpha processing
template <class PIX, class MASK, int max, int isFloat>
class ProcessAlpha : public Processor {
public :
    ProcessAlpha( OFX::ImageEffect *instance,
                  float scale,
                  void *srcV, OfxRectI srcRect, int srcBytesPerLine,
                  void *dstV, OfxRectI dstRect, int dstBytesPerLine,
                  void *maskV, OfxRectI maskRect, int maskBytesPerLine,
                  OfxRectI  window)
      : Processor(instance,
                  scale, scale, scale, scale,
                  srcV,  srcRect,  srcBytesPerLine,
                  dstV,  dstRect,  dstBytesPerLine,
                  maskV,  maskRect, maskBytesPerLine,
                  window)
    {
    }

    void processImages(OfxRectI procWindow)
    {
        PIX *src = (PIX *) srcV;
        PIX *dst = (PIX *) dstV;
        MASK *mask = (MASK *) maskV;

        for(int y = procWindow.y1; y < procWindow.y2; y++) {
            if(instance->abort()) break;

            PIX *dstPix = pixelAddress(dst, dstRect, procWindow.x1, y, dstBytesPerLine);

            for(int x = procWindow.x1; x < procWindow.x2; x++) {
        
                PIX *srcPix = pixelAddress(src, srcRect, x, y, srcBytesPerLine);

                // do any pixel masking?
                float maskV = 1.0f;
                if(mask) {
                    MASK *maskPix = pixelAddress(mask, maskRect, x, y, maskBytesPerLine);
                    if(maskPix) {
                        maskV = float(*maskPix)/float(max);
                    }
                }

                // figure the scale values per component
                float theScale = 1.0 + (rScale - 1.0) * maskV;

                if(srcPix) {
                    // switch will be compiled out
                    if(isFloat) {
                        *dstPix = *srcPix * theScale;
                    }
                    else {
                        *dstPix = Clamp(int(*srcPix * theScale), 0, max);
                    }
                    srcPix++;
                }
                else {
                    *dstPix = 0;
                }
                dstPix++;
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// basic plugin render function
void
BasicPlugin::render(const OFX::RenderArguments &args)
{
    OFX::Image *src = 0, *dst = 0;

    try {
        // get a dst image
        dst = dstClip_->fetchImage(args.time);
        int dstRowBytes  =  dst->rowBytes();
        int dstBitDepth  =  dst->pixelDepth();
        bool dstIsAlpha  =  dst->pixelComponents() == OFX::ePixelComponentAlpha;
        OfxRectI dstRect =  dst->bounds();
        void *dstData    =  dst->pixelData();
  
        // fetch main input guff
        src = srcClip_->fetchImage(args.time);
        int srcRowBytes  =  src->rowBytes();
        int srcBitDepth  =  src->pixelDepth();
        bool srcIsAlpha  =  src->pixelComponents() ==  OFX::ePixelComponentAlpha;
        OfxRectI srcRect =  src->bounds();
        void *srcData    =  src->pixelData();
        
        // optional mask in the general context
        void     *maskData = 0;
        OfxRectI  maskRect;
        int       maskRowBytes;
        int       maskBitDepth = 8;
        bool      maskIsAlpha;

        // see if they have the same depths and bytes and all
        if(srcBitDepth != dstBitDepth || srcIsAlpha != dstIsAlpha)
            throw int(1); // HACK!! need to throw an error here!

        // get the scale parameters
        double scale = scale_->getValueAtTime(args.time);
        double rScale = 1, gScale = 1, bScale = 1, aScale = 1;
        if(componentScalesEnabled_->getValueAtTime(args.time)) {
            rScale = rScale_->getValueAtTime(args.time);
            gScale = gScale_->getValueAtTime(args.time);
            bScale = bScale_->getValueAtTime(args.time);
            aScale = aScale_->getValueAtTime(args.time);
        }
        rScale *= scale; gScale *= scale; bScale *= scale;

        // do the rendering
        if(!dstIsAlpha) {
            switch(dstBitDepth) {
            case 8 : {      
                ProcessRGBA<OfxRGBAColourB, unsigned char, 255, 0> fred(this, rScale, gScale, bScale, aScale,
                                                                        srcData, srcRect, srcRowBytes,
                                                                        dstData, dstRect, dstRowBytes,
                                                                        maskData, maskRect, maskRowBytes,
                                                                        args.renderWindow);
                fred.multiThread();                                          
            }
                break;

            case 16 : {
                ProcessRGBA<OfxRGBAColourS, unsigned short, 65535, 0> fred(this, rScale, gScale, bScale, aScale,
                                                                           srcData, srcRect, srcRowBytes,
                                                                           dstData, dstRect, dstRowBytes,
                                                                           maskData, maskRect, maskRowBytes,
                                                                           args.renderWindow);
                fred.multiThread();           
            }                          
                break;

            case 32 : {
                ProcessRGBA<OfxRGBAColourF, float, 1, 1> fred(this, rScale, gScale, bScale, aScale,
                                                              srcData, srcRect, srcRowBytes,
                                                              dstData, dstRect, dstRowBytes,
                                                              maskData, maskRect, maskRowBytes,
                                                              args.renderWindow);
                fred.multiThread();                                          
                break;
            }
            }
        }
        else {
            switch(dstBitDepth) {
            case 8 : {
                ProcessAlpha<unsigned char, unsigned char, 255, 0> fred(this, scale, 
                                                                        srcData, srcRect, srcRowBytes,
                                                                        dstData, dstRect, dstRowBytes,
                                                                        maskData, maskRect, maskRowBytes,
                                                                        args.renderWindow);
                fred.multiThread();                                                                                  
            }
                break;

            case 16 : {
                ProcessAlpha<unsigned short, unsigned short, 65535, 0> fred(this, scale, 
                                                                            srcData, srcRect, srcRowBytes,
                                                                            dstData, dstRect, dstRowBytes,
                                                                            maskData, maskRect, maskRowBytes,
                                                                            args.renderWindow);
                fred.multiThread();           
            }                          
                break;

            case 32 : {
                ProcessAlpha<float, float, 1, 1> fred(this, scale, 
                                                      srcData, srcRect, srcRowBytes,
                                                      dstData, dstRect, dstRowBytes,
                                                      maskData, maskRect, maskRowBytes,
                                                      args.renderWindow);
                fred.multiThread();           
            }                          
                break;
            }
        } // switch

    } // try
  
    catch(...) {
        delete src;
        delete dst;
        throw;
    }

    // delete them
    delete src;
    delete dst;
}

// overridden is identity
bool
BasicPlugin:: isIdentity(const OFX::RenderArguments &args, OFX::Clip * &identityClip, double &identityTime)
{
    // get the scale parameters
    double scale = scale_->getValueAtTime(args.time);
    double rScale = 1, gScale = 1, bScale = 1, aScale = 1;
    if(componentScalesEnabled_->getValueAtTime(args.time)) {
        rScale = rScale_->getValueAtTime(args.time);
        gScale = gScale_->getValueAtTime(args.time);
        bScale = bScale_->getValueAtTime(args.time);
        aScale = aScale_->getValueAtTime(args.time);
    }
    rScale *= scale; gScale *= scale; bScale *= scale;
    
    // do we do any scaling ?
    if(rScale == 1 && gScale == 1 && bScale == 1 && aScale == 1) {
        identityClip = srcClip_;
        identityTime = args.time;
        return true;
    }

    // nope, idenity we is
    return false;
}

// set the enabledness of the individual component scales
bool
BasicPlugin::setEnabledness(double time)
{
    // the componet enabledness depends on the clip being RGBA and the param being true
    bool v = componentScalesEnabled_->getValueAtTime(time) && srcClip_->pixelComponents() == OFX::ePixelComponentRGBA;

    // enable them
    rScale_->setEnabled(v);
    gScale_->setEnabled(v);
    bScale_->setEnabled(v);
    aScale_->setEnabled(v);
}

// we have changed a param
void
BasicPlugin::changedParam(const OFX::InstanceChangedArgs &args, const std::string &paramName)
{
    if(paramName == "scaleComponents")  setEnabledness(args.time); 
}

// we have changed a param
void
BasicPlugin::changedClip(const OFX::InstanceChangedArgs &args, const std::string &clipName)
{
    if(clipName == "Source")  setEnabledness(args.time);
}


////////////////////////////////////////////////////////////////////////////////
// code below is description code and instance creation code

/** @brief OFX namespace */
namespace OFX {

    /** @brief plugin namespace */
    namespace Plugin {
    
        /** @brief identify the plug-in */
        void getPluginID(OFX::PluginID &id)
        {
            id.pluginIdentifier = "net.sf.openfx:basicPlugin";
            id.pluginVersionMajor = 1;
            id.pluginVersionMinor = 0;      
        }

        /** @brief empty load function */
        void loadAction(void)
        {
        }

        /** brief empty unload function */
        void unloadAction(void)    
        {
        }

        /** @brief The basic describe function, passed a plugin descriptor */
        void describe(OFX::ImageEffectDescriptor &desc) 
        {
            // basic labels
            desc.setLabels("Basic Plugin", "Basic Plugin", "Basic Plugin");
            desc.setPluginGrouping("OFX");

            // add the supported contexts, only filter at the moment
            desc.addSupportedContext(eContextFilter);

            // add supported pixel depths
            desc.addSupportedBitDepth(eBitDepthUByte);
            desc.addSupportedBitDepth(eBitDepthUShort);
            desc.addSupportedBitDepth(eBitDepthFloat);

            // set a few flags
            desc.setSingleInstance(false);
            desc.setHostFrameThreading(false);
            desc.setSupportsMultiResolution(true);
            desc.setSupportsTiles(true);
            desc.setTemporalClipAccess(false);
            desc.setRenderTwiceAlways(false);
            desc.setSupportsMultipleClipPARs(false);
        }        
        
        // make a double scale param
        DoubleParamDescriptor *defineScaleParam(OFX::ImageEffectDescriptor &desc, 
                                                const std::string &name, const std::string &label, const std::string &hint,
                                                GroupParamDescriptor *parent)
        {
            DoubleParamDescriptor *param = desc.defineDoubleParam(name);
            param->setLabels(label, label, label);
            param->setScriptName(name);
            param->setHint(hint);
            param->setDefault(1);
            param->setRange(0, 10);
            param->setDislayRange(0, 10);
            param->setDoubleType(eDoubleTypeScale);
            if(parent) param->setParent(*parent);
            return param;
        }
        

        /** @brief The describe in context function, passed a plugin descriptor and a context */
        void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context) 
        {
            // Source clip only in the filter context
            // create the mandated source clip
            ClipDescriptor *srcClip = desc.defineClip("Source");
            srcClip->addSupportedComponent(ePixelComponentRGBA);
            srcClip->setTemporalClipAccess(false);
            srcClip->setOptional(false);
            srcClip->setSupportsTiles(true);
            srcClip->setIsMask(false);

            // create the mandated output clip
            ClipDescriptor *dstClip = desc.defineClip("Output");
            dstClip->addSupportedComponent(ePixelComponentRGBA);
            dstClip->setTemporalClipAccess(false);
            dstClip->setSupportsTiles(true);
            dstClip->setIsMask(false);

            // make some pages and to things in 
            PageParamDescriptor *page = desc.definePageParam("Controls");

            // group param to group the scales
            GroupParamDescriptor *componentScalesGroup = desc.defineGroupParam("componentScales");
            componentScalesGroup->setHint("Scales on the individual component");
            componentScalesGroup->setLabels("Components", "Components", "Components");            

            // make overall scale params 
            DoubleParamDescriptor *param = defineScaleParam(desc, "scale", "scale", "Scales all component in the image", 0);
            page->addChild(*param);

            // add a boolean to enable the component scale
            BooleanParamDescriptor *boolP = desc.defineBooleanParam("scaleComponents");
            boolP->setDefault(true);
            boolP->setHint("Enables scales on individual components");
            boolP->setLabels("Scale Components", "Scale Components", "Scale Components");
            boolP->setParent(*componentScalesGroup);
            page->addChild(*boolP);
            
            // make the four component scale params 
            param = defineScaleParam(desc, "scaleR", "red", "Scales the red component of the image", componentScalesGroup);
            page->addChild(*param);

            param = defineScaleParam(desc, "scaleG", "green", "Scales the green component of the image", componentScalesGroup);
            page->addChild(*param);

            param = defineScaleParam(desc, "scaleB", "blue", "Scales the blue component of the image", componentScalesGroup);
            page->addChild(*param);

            param = defineScaleParam(desc, "scaleA", "alpha", "Scales the alpha component of the image", componentScalesGroup);
            page->addChild(*param);
        }

        /** @brief The create instance function, the plugin must return an object derived from the \ref OFX::ImageEffect class */
        ImageEffect *createInstance(OfxImageEffectHandle handle, ContextEnum context)
        {
            return new BasicPlugin(handle);
        }
    };
};
