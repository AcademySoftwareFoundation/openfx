// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause


/*
  OpenFX Example plugin that exercises various forms of Choice Params

  - Regular Choice param
  - Choice param with order specified
  - StrChoice param

  The latter two are only available in hosts supporting OpenFX 1.5 or later.
 */
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <new>
#include <cstring>
#include <string>
#include "ofxImageEffect.h"
#include "ofxMemory.h"
#include "ofxMultiThread.h"

#include "../include/ofxUtilities.H" // example support utils

#if defined __APPLE__ || defined __linux__ || defined __FreeBSD__
#  define EXPORT __attribute__((visibility("default")))
#elif defined _WIN32
#  define EXPORT OfxExport
#else
#  error Not building on your operating system quite yet
#endif

// pointers64 to various bits of the host
OfxHost                 *gHost;
OfxImageEffectSuiteV1 *gEffectHost = 0;
OfxPropertySuiteV1    *gPropHost = 0;
OfxParameterSuiteV1   *gParamHost = 0;
OfxMemorySuiteV1      *gMemoryHost = 0;
OfxMultiThreadSuiteV1 *gThreadHost = 0;
OfxMessageSuiteV1     *gMessageSuite = 0;
OfxInteractSuiteV1    *gInteractHost = 0;

// some flags about the host's behaviour
std::string gHostName;
int gHostSupportsMultipleBitDepths = false;
int gHostSupportsStrChoice = false;
bool gHostSupportsChoiceOrder = true; // assume OK

// private instance data type
struct MyInstanceData {
  bool isGeneralEffect;

  // handles to the clips we deal with
  OfxImageClipHandle sourceClip;
  OfxImageClipHandle outputClip;

  // handles to a our parameters
  OfxParamHandle redChoiceParam;
  OfxParamHandle greenChoiceParam;
  OfxParamHandle blueChoiceParam;
};

/* mandatory function to set up the host structures */


// Convenience wrapper to get private data 
static MyInstanceData *
getMyInstanceData( OfxImageEffectHandle effect)
{
  // get the property handle for the plugin
  OfxPropertySetHandle effectProps;
  gEffectHost->getPropertySet(effect, &effectProps);

  // get my data pointer out of that
  MyInstanceData *myData = 0;
  gPropHost->propGetPointer(effectProps,  kOfxPropInstanceData, 0, 
			    (void **) &myData);
  return myData;
}

/** @brief Called at load */
static OfxStatus
onLoad(void)
{
  return kOfxStatOK;
}

/** @brief Called before unload */
static OfxStatus
onUnLoad(void)
{
  return kOfxStatOK;
}

//  instance construction
static OfxStatus
createInstance( OfxImageEffectHandle effect)
{
  // get a pointer to the effect properties
  OfxPropertySetHandle effectProps;
  gEffectHost->getPropertySet(effect, &effectProps);

  // get a pointer to the effect's parameter set
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(effect, &paramSet);

  // make private instance data
  MyInstanceData *myData = new MyInstanceData;
  char *context = 0;

  // is this instance a general effect?
  gPropHost->propGetString(effectProps, kOfxImageEffectPropContext, 0,  &context);
  myData->isGeneralEffect = context && (strcmp(context, kOfxImageEffectContextGeneral) == 0);

  // cache param handles
  gParamHost->paramGetHandle(paramSet, "red_choice", &myData->redChoiceParam, 0);
  gParamHost->paramGetHandle(paramSet, "green_choice", &myData->greenChoiceParam, 0);
  gParamHost->paramGetHandle(paramSet, "blue_choice", &myData->blueChoiceParam, 0);

  // cache clip handles
  gEffectHost->clipGetHandle(effect, kOfxImageEffectSimpleSourceClipName, &myData->sourceClip, 0);
  gEffectHost->clipGetHandle(effect, kOfxImageEffectOutputClipName, &myData->outputClip, 0);
  
  // set private instance data
  gPropHost->propSetPointer(effectProps, kOfxPropInstanceData, 0, (void *) myData);

  return kOfxStatOK;
}

// instance destruction
static OfxStatus
destroyInstance( OfxImageEffectHandle  effect)
{
  // get my instance data
  MyInstanceData *myData = getMyInstanceData(effect);

  // and delete it
  if(myData)
    delete myData;
  return kOfxStatOK;
}

// tells the host what region we are capable of filling
OfxStatus 
getSpatialRoD( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  // retrieve any instance data associated with this effect
  MyInstanceData *myData = getMyInstanceData(effect);

  OfxTime time;
  gPropHost->propGetDouble(inArgs, kOfxPropTime, 0, &time);
  
  // my RoD is the same as my input's
  OfxRectD rod;
  gEffectHost->clipGetRegionOfDefinition(myData->sourceClip, time, &rod);

  // set the rod in the out args
  gPropHost->propSetDoubleN(outArgs, kOfxImageEffectPropRegionOfDefinition, 4, &rod.x1);

  return kOfxStatOK;
}

// tells the host how much of the input we need to fill the given window
OfxStatus 
getSpatialRoI( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  // get the RoI the effect is interested in from inArgs
  OfxRectD roi;
  gPropHost->propGetDoubleN(inArgs, kOfxImageEffectPropRegionOfInterest, 4, &roi.x1);

  // the input needed is the same as the output, so set that on the source clip
  gPropHost->propSetDoubleN(outArgs, "OfxImageClipPropRoI_Source", 4, &roi.x1);

  // retrieve any instance data associated with this effect
  MyInstanceData *myData = getMyInstanceData(effect);

  return kOfxStatOK;
}

// Tells the host how many frames we can fill, only called in the general context.
// This is actually redundant as this is the default behaviour, but for illustrative
// purposes.
OfxStatus 
getTemporalDomain( OfxImageEffectHandle  effect,  OfxPropertySetHandle /*inArgs*/,  OfxPropertySetHandle outArgs)
{
  MyInstanceData *myData = getMyInstanceData(effect);

  double sourceRange[2];
  
  // get the frame range of the source clip
  OfxPropertySetHandle props; gEffectHost->clipGetPropertySet(myData->sourceClip, &props);
  gPropHost->propGetDoubleN(props, kOfxImageEffectPropFrameRange, 2, sourceRange);

  // set it on the out args
  gPropHost->propSetDoubleN(outArgs, kOfxImageEffectPropFrameRange, 2, sourceRange);
  
  return kOfxStatOK;
}


// Set our clip preferences 
static OfxStatus 
getClipPreferences( OfxImageEffectHandle  effect,  OfxPropertySetHandle /*inArgs*/,  OfxPropertySetHandle outArgs)
{
  // retrieve any instance data associated with this effect
  MyInstanceData *myData = getMyInstanceData(effect);
  
  // get the component type and bit depth of our main input
  int  bitDepth;
  bool isRGBA;
  ofxuClipGetFormat(myData->sourceClip, bitDepth, isRGBA, true); // get the unmapped clip component

  // get the strings used to label the various bit depths
  const char *bitDepthStr = bitDepth == 8 ? kOfxBitDepthByte : (bitDepth == 16 ? kOfxBitDepthShort : kOfxBitDepthFloat);

  // set out output to be the same same as the input bitdepth
  if(gHostSupportsMultipleBitDepths)
    gPropHost->propSetString(outArgs, "OfxImageClipPropDepth_Output", 0, bitDepthStr);

  return kOfxStatOK;
}

// are the settings of the effect performing an identity operation
static OfxStatus
isIdentity( OfxImageEffectHandle  effect,
	    OfxPropertySetHandle inArgs,
	    OfxPropertySetHandle outArgs)
{
  return kOfxStatReplyDefault;
}

////////////////////////////////////////////////////////////////////////////////
// function called when the instance has been changed by anything
static OfxStatus
instanceChanged( OfxImageEffectHandle  effect,
		 OfxPropertySetHandle inArgs,
		 OfxPropertySetHandle /*outArgs*/)
{
  // see why it changed
  char *changeReason;
  gPropHost->propGetString(inArgs, kOfxPropChangeReason, 0, &changeReason);

  // we are only interested in user edits
  if(strcmp(changeReason, kOfxChangeUserEdited) != 0) return kOfxStatReplyDefault;

  // fetch the type of the object that changed
  char *typeChanged;
  gPropHost->propGetString(inArgs, kOfxPropType, 0, &typeChanged);

  // was it a clip or a param?
  bool isClip = strcmp(typeChanged, kOfxTypeClip) == 0;
  bool isParam = strcmp(typeChanged, kOfxTypeParameter) == 0;

  // get the name of the thing that changed
  char *objChanged;
  gPropHost->propGetString(inArgs, kOfxPropName, 0, &objChanged);

  // don't trap any others
  return kOfxStatReplyDefault;
}

////////////////////////////////////////////////////////////////////////////////
// rendering routines
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
  if(x < rect.x1 || x >= rect.x2 || y < rect.y1 || y >= rect.y2 || !img)
    return 0;
  PIX *pix = (PIX *) (((char *) img) + (y - rect.y1) * bytesPerLine);
  pix += x - rect.x1;  
  return pix;
}

////////////////////////////////////////////////////////////////////////////////
// base class to process images with
class Processor {
 protected :
  OfxImageEffectHandle  instance;
  float         rScale, gScale, bScale, aScale;
  void *srcV, *dstV;
  OfxRectI srcRect, dstRect;
  int srcBytesPerLine, dstBytesPerLine;
  OfxRectI  window;

 public :
  Processor(OfxImageEffectHandle  inst,
            float rScal, float gScal, float bScal, float aScal,
            void *src, OfxRectI sRect, int sBytesPerLine,
            void *dst, OfxRectI dRect, int dBytesPerLine,
            OfxRectI  win)
    : instance(inst)
    , rScale(rScal)
    , gScale(gScal)
    , bScale(bScal)
    , aScale(aScal)
    , srcV(src)
    , dstV(dst)
    , srcRect(sRect)
    , dstRect(dRect)
    , srcBytesPerLine(sBytesPerLine)
    , dstBytesPerLine(dBytesPerLine)
    , window(win)
  {}  

  static void multiThreadProcessing(unsigned int threadId, unsigned int nThreads, void *arg);
  virtual void doProcessing(OfxRectI window) = 0;
  void process(void);
};


// function call once for each thread by the host
void
Processor::multiThreadProcessing(unsigned int threadId, unsigned int nThreads, void *arg)
{
  Processor *proc = (Processor *) arg;

  // slice the y range into the number of threads it has
  unsigned int dy = proc->window.y2 - proc->window.y1;
  
  unsigned int y1 = proc->window.y1 + threadId * dy/nThreads;
  unsigned int y2 = proc->window.y1 + std::min((threadId + 1) * dy/nThreads, dy);

  OfxRectI win = proc->window;
  win.y1 = y1; win.y2 = y2;

  // and render that thread on each
  proc->doProcessing(win);  
}

// function to kick off rendering across multiple CPUs
void
Processor::process(void)
{
  unsigned int nThreads;
  gThreadHost->multiThreadNumCPUs(&nThreads);
  gThreadHost->multiThread(multiThreadProcessing, nThreads, (void *) this);
}

// template to do the RGBA processing
template <class PIX, int max, int isFloat>
class ProcessRGBA : public Processor{
public :
  ProcessRGBA(OfxImageEffectHandle  instance,
	      float rScale, float gScale, float bScale, float aScale,
	      void *srcV, OfxRectI srcRect, int srcBytesPerLine,
	      void *dstV, OfxRectI dstRect, int dstBytesPerLine,
	      OfxRectI  window)
    : Processor(instance,
                rScale, gScale, bScale, aScale,
                srcV,  srcRect,  srcBytesPerLine,
                dstV,  dstRect,  dstBytesPerLine,
                window)
  {
  }

  void doProcessing(OfxRectI procWindow)
  {
    PIX *src = (PIX *) srcV;
    PIX *dst = (PIX *) dstV;

    for(int y = procWindow.y1; y < procWindow.y2; y++) {
      if(gEffectHost->abort(instance)) break;

      PIX *dstPix = pixelAddress(dst, dstRect, procWindow.x1, y, dstBytesPerLine);

      for(int x = procWindow.x1; x < procWindow.x2; x++) {
        
        PIX *srcPix = pixelAddress(src, srcRect, x, y, srcBytesPerLine);
        
        // figure the scale values per component
        float sR = 1.0 + (rScale - 1.0);
        float sG = 1.0 + (gScale - 1.0);
        float sB = 1.0 + (bScale - 1.0);
        float sA = 1.0 + (aScale - 1.0);

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

// the process code  that the host sees
static OfxStatus render( OfxImageEffectHandle  instance,
                         OfxPropertySetHandle inArgs,
                         OfxPropertySetHandle /*outArgs*/)
{
  // get the render window and the time from the inArgs
  OfxTime time;
  OfxRectI renderWindow;
  OfxStatus status = kOfxStatOK;

  gPropHost->propGetDouble(inArgs, kOfxPropTime, 0, &time);
  gPropHost->propGetIntN(inArgs, kOfxImageEffectPropRenderWindow, 4, &renderWindow.x1);

  // retrieve any instance data associated with this effect
  MyInstanceData *myData = getMyInstanceData(instance);

  // property handles and members of each image
  // in reality, we would put this in a struct as the C++ support layer does
  OfxPropertySetHandle sourceImg = NULL, outputImg = NULL;
  int srcRowBytes, srcBitDepth, dstRowBytes, dstBitDepth;
  OfxRectI dstRect, srcRect;
  void *src, *dst;

  try {
    // get the source image
    bool _isAlpha;
    sourceImg = ofxuGetImage(myData->sourceClip, time, srcRowBytes, srcBitDepth, _isAlpha, srcRect, src);
    if(sourceImg == NULL) throw OfxuNoImageException();

    // get the output image
    outputImg = ofxuGetImage(myData->outputClip, time, dstRowBytes, dstBitDepth, _isAlpha, dstRect, dst);
    if(outputImg == NULL) throw OfxuNoImageException();

    // see if they have the same depths and bytes and all
    if(srcBitDepth != dstBitDepth) {
      throw OfxuStatusException(kOfxStatErrImageFormat);
    }

    float rScale, gScale, bScale, aScale;

    int rChoice;
    int gChoice;
    gParamHost->paramGetValueAtTime(myData->redChoiceParam, time, &rChoice);
    gParamHost->paramGetValueAtTime(myData->greenChoiceParam, time, &gChoice);
    std::string blueChoice = "blue_1.0";
    if (gHostSupportsStrChoice) {
      char *bChoice;              // NOTE: string!
      gParamHost->paramGetValueAtTime(myData->blueChoiceParam, time, &bChoice);
      blueChoice = bChoice;
    }

    if (rChoice == 0)
      rScale = 0;
    if (rChoice == 1)
      rScale = 0.5;
    if (rChoice == 2)
      rScale = 1.0;

    if (gChoice == 0)
      gScale = 0;
    if (gChoice == 1)           // "lots"
      gScale = 1.0;
    if (gChoice == 2)           // "some"
      gScale = 0.5;
    if (gChoice == 3)           // should not happen
      gScale = 10;


    // string-valued param
    if (blueChoice == "blue_0.0")
      bScale = 0;
    if (blueChoice == "blue_0.5")
      bScale = 0.5;
    if (blueChoice == "blue_1.0")
      bScale = 1.0;

    // always
    aScale = 1.0;

    // do the rendering
    switch(dstBitDepth) {
    case 8 : {
      ProcessRGBA<OfxRGBAColourB, 255, 0> fred(instance, rScale, gScale, bScale, aScale,
                                               src, srcRect, srcRowBytes,
                                               dst, dstRect, dstRowBytes,
                                               renderWindow);
      fred.process();
    }
      break;

    case 16 : {
      ProcessRGBA<OfxRGBAColourS, 65535, 0> fred(instance, rScale, gScale, bScale, aScale,
                                                 src, srcRect, srcRowBytes,
                                                 dst, dstRect, dstRowBytes,
                                                 renderWindow);
      fred.process();
    }
      break;

    case 32 : {
      ProcessRGBA<OfxRGBAColourF, 1, 1> fred(instance, rScale, gScale, bScale, aScale,
                                             src, srcRect, srcRowBytes,
                                             dst, dstRect, dstRowBytes,
                                             renderWindow);
      fred.process();
      break;
    }
    }
  }
  catch(OfxuNoImageException &ex) {
    // if we were interrupted, the failed fetch is fine, just return kOfxStatOK
    // otherwise, something weird happened
    if(!gEffectHost->abort(instance)) {
      status = kOfxStatFailed;
    }
  }
  catch(OfxuStatusException &ex) {
    status = ex.status();
  }

  // release the data pointers
  if(sourceImg)
    gEffectHost->clipReleaseImage(sourceImg);
  if(outputImg)
    gEffectHost->clipReleaseImage(outputImg);
  
  return status;
}

//  describe the plugin in context
static OfxStatus
describeInContext( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs)
{
  // get the context from the inArgs handle
  char *context;
  gPropHost->propGetString(inArgs, kOfxImageEffectPropContext, 0, &context);
  bool isGeneralContext = strcmp(context, kOfxImageEffectContextGeneral) == 0;

  OfxPropertySetHandle props;
  // define the single output clip in both contexts
  gEffectHost->clipDefine(effect, kOfxImageEffectOutputClipName, &props);

  // set the component types we can handle on out output
  gPropHost->propSetString(props, kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA);

  // define the single source clip in both contexts
  gEffectHost->clipDefine(effect, kOfxImageEffectSimpleSourceClipName, &props);

  // set the component types we can handle on our main input
  gPropHost->propSetString(props, kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA);

  ////////////////////////////////////////////////////////////////////////////////
  // define the parameters for this context
  // fetch the parameter set from the effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(effect, &paramSet);

  // First choice param
  gParamHost->paramDefine(paramSet, kOfxParamTypeChoice, "red_choice", &props);
  gPropHost->propSetInt(props, kOfxParamPropDefault, 0, 0);
  gPropHost->propSetString(props, kOfxParamPropScriptName, 0, "red_choice");
  gPropHost->propSetString(props, kOfxPropLabel, 0, "Red Choice Param");
  gPropHost->propSetString(props, kOfxParamPropChoiceOption, 0, "Red: none");
  gPropHost->propSetString(props, kOfxParamPropChoiceOption, 1, "Red: some");
  gPropHost->propSetString(props, kOfxParamPropChoiceOption, 2, "Red: lots");


  // Second choice param: using order.
  // This will produce incorrect ordering in hosts that don't support order.
  // Note that index 1 here is "lots", index 2 is "some"
  gParamHost->paramDefine(paramSet, kOfxParamTypeChoice, "green_choice", &props);
  gPropHost->propSetInt(props, kOfxParamPropDefault, 0, 0);
  gPropHost->propSetString(props, kOfxParamPropScriptName, 0, "green_choice");
  gPropHost->propSetString(props, kOfxPropLabel, 0, "Green Choice Param");
  gPropHost->propSetString(props, kOfxParamPropChoiceOption, 0, "Green: none");
  gPropHost->propSetString(props, kOfxParamPropChoiceOption, 1, "Green: lots");
  gPropHost->propSetString(props, kOfxParamPropChoiceOption, 2, "Green: some");
  // Order sets the display order: choices will be displayed in this order
  auto stat = gPropHost->propSetInt(props, kOfxParamPropChoiceOrder, 0, 0); // first
  if (stat == kOfxStatOK) {
      gPropHost->propSetInt(props, kOfxParamPropChoiceOrder, 1, 2); // last
      gPropHost->propSetInt(props, kOfxParamPropChoiceOrder, 2, 1); // middle
  } else {
      gHostSupportsChoiceOrder = false;
  }

  // Third choice param: using StrChoice, string-valued choice param
  if (gHostSupportsStrChoice) {
    gParamHost->paramDefine(paramSet, kOfxParamTypeStrChoice, "blue_choice", &props);
    gPropHost->propSetString(props, kOfxParamPropDefault, 0, "blue_1.0");
    gPropHost->propSetString(props, kOfxParamPropScriptName, 0, "blue_choice");
    gPropHost->propSetString(props, kOfxPropLabel, 0, "Blue Choice Param");
    gPropHost->propSetString(props, kOfxParamPropChoiceOption, 0, "Blue: none");
    gPropHost->propSetString(props, kOfxParamPropChoiceOption, 1, "Blue: some");
    gPropHost->propSetString(props, kOfxParamPropChoiceOption, 2, "Blue: lots");
    // host will return and store these values
    gPropHost->propSetString(props, kOfxParamPropChoiceEnum, 0, "blue_0.0");
    gPropHost->propSetString(props, kOfxParamPropChoiceEnum, 1, "blue_0.5");
    gPropHost->propSetString(props, kOfxParamPropChoiceEnum, 2, "blue_1.0");

    gPropHost->propSetInt(props, kOfxParamPropChoiceOrder, 0, 0);
    gPropHost->propSetInt(props, kOfxParamPropChoiceOrder, 1, 1);
    gPropHost->propSetInt(props, kOfxParamPropChoiceOrder, 2, 2);
  }
  
  // make a page of controls and add my parameters to it
  gParamHost->paramDefine(paramSet, kOfxParamTypePage, "Main", &props);
  gPropHost->propSetString(props, kOfxParamPropPageChild, 0, "red_choice");
  gPropHost->propSetString(props, kOfxParamPropPageChild, 1, "green_choice");
  gPropHost->propSetString(props, kOfxParamPropPageChild, 2, "blue_choice");

  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// the plugin's description routine
static OfxStatus
describe(OfxImageEffectHandle  effect)
{
  // first fetch the host APIs, this cannot be done before this call
  OfxStatus stat;
  if((stat = ofxuFetchHostSuites()) != kOfxStatOK)
    return stat;

  // record a few host features
  gPropHost->propGetInt(gHost->host, kOfxImageEffectPropSupportsMultipleClipDepths, 0, &gHostSupportsMultipleBitDepths);
  gPropHost->propGetInt(gHost->host, kOfxParamHostPropSupportsStrChoice, 0, &gHostSupportsStrChoice);
  {
    char *hostName;
    gPropHost->propGetString(gHost->host, kOfxPropName, 0, &hostName);
    gHostName = hostName;
    if (gHostName == "DaVinciResolve")
      gHostSupportsStrChoice = true; // As of version 18.1, Resolve supports StrChoice but not yet the support prop
  }

  // get the property handle for the plugin
  OfxPropertySetHandle effectProps;
  gEffectHost->getPropertySet(effect, &effectProps);

  // We can render both fields in a fielded images in one hit if there is no animation
  // So set the flag that allows us to do this
  gPropHost->propSetInt(effectProps, kOfxImageEffectPluginPropFieldRenderTwiceAlways, 0, 0);

  // say we can support multiple pixel depths and let the clip preferences action deal with it all.
  gPropHost->propSetInt(effectProps, kOfxImageEffectPropSupportsMultipleClipDepths, 0, 1);
  
  // set the bit depths the plugin can handle
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 1, kOfxBitDepthShort);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 2, kOfxBitDepthFloat);

  // set some labels and the group it belongs to
  gPropHost->propSetString(effectProps, kOfxPropLabel, 0, "OFX Choice Param Example");
  gPropHost->propSetString(effectProps, kOfxImageEffectPluginPropGrouping, 0, "OFX Example");

  // define the contexts we can be used in
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextFilter);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedContexts, 1, kOfxImageEffectContextGeneral);
  
  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// The main function
static OfxStatus
pluginMain(const char *action,  const void *handle, OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  try {
  // cast to appropriate type
  OfxImageEffectHandle effect = (OfxImageEffectHandle) handle;

  if(strcmp(action, kOfxActionDescribe) == 0) {
    return describe(effect);
  }
  else if(strcmp(action, kOfxImageEffectActionDescribeInContext) == 0) {
    return describeInContext(effect, inArgs);
  }
  else if(strcmp(action, kOfxActionLoad) == 0) {
    return onLoad();
  }
  else if(strcmp(action, kOfxActionUnload) == 0) {
    return onUnLoad();
  }
  else if(strcmp(action, kOfxActionCreateInstance) == 0) {
    return createInstance(effect);
  } 
  else if(strcmp(action, kOfxActionDestroyInstance) == 0) {
    return destroyInstance(effect);
  } 
  else if(strcmp(action, kOfxImageEffectActionIsIdentity) == 0) {
    return isIdentity(effect, inArgs, outArgs);
  }    
  else if(strcmp(action, kOfxImageEffectActionRender) == 0) {
    return render(effect, inArgs, outArgs);
  }    
  else if(strcmp(action, kOfxImageEffectActionGetRegionOfDefinition) == 0) {
    return getSpatialRoD(effect, inArgs, outArgs);
  }  
  else if(strcmp(action, kOfxImageEffectActionGetRegionsOfInterest) == 0) {
    return getSpatialRoI(effect, inArgs, outArgs);
  }  
  else if(strcmp(action, kOfxImageEffectActionGetClipPreferences) == 0) {
    return getClipPreferences(effect, inArgs, outArgs);
  }  
  else if(strcmp(action, kOfxActionInstanceChanged) == 0) {
    return instanceChanged(effect, inArgs, outArgs);
  }  
  else if(strcmp(action, kOfxImageEffectActionGetTimeDomain) == 0) {
    return getTemporalDomain(effect, inArgs, outArgs);
  }  
  } catch (std::bad_alloc) {
    // catch memory
    //std::cout << "OFX Plugin Memory error." << std::endl;
    return kOfxStatErrMemory;
  } catch ( const std::exception& e ) {
    // standard exceptions
    //std::cout << "OFX Plugin error: " << e.what() << std::endl;
    return kOfxStatErrUnknown;
  } catch (int err) {
    // ho hum, gone wrong somehow
    return err;
  } catch ( ... ) {
    // everything else
    //std::cout << "OFX Plugin error" << std::endl;
    return kOfxStatErrUnknown;
  }

  // other actions to take the default value
  return kOfxStatReplyDefault;
}

// function to set the host structure
static void
setHostFunc(OfxHost *hostStruct)
{
  gHost         = hostStruct;
}

////////////////////////////////////////////////////////////////////////////////
// the plugin struct 
static OfxPlugin basicPlugin = 
{       
  kOfxImageEffectPluginApi,
  1,
  "org.openfx.ChoiceParamPlugin",
  1,
  0,
  setHostFunc,
  pluginMain
};
   
// the two mandated functions
EXPORT OfxPlugin *
OfxGetPlugin(int nth)
{
  if(nth == 0)
    return &basicPlugin;
  return 0;
}
 
EXPORT int
OfxGetNumberOfPlugins(void)
{       
  return 1;
}
