// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

/*
  Ofx Example plugin that shows how an overlay is drawn and interacted with.

  It is meant to illustrate certain features of the API, as opposed to being a perfectly
  crafted piece of image processing software.

  Note, that the default bitdepths and components are specified for the source (RGBA + A, 8 + 16 + 32) and that
  this plugin does absolutely no image processing, it assumes the isIdentity action will be called before a render
  and so do nothing.
 */

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
 
#include <cstring>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <cmath>
#include <stdexcept>
#include <new>
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

#define kPointParam "point"

// pointers to various bits of the host
OfxHost               *gHost;
OfxImageEffectSuiteV1 *gEffectHost = 0;
OfxPropertySuiteV1    *gPropHost = 0;
OfxParameterSuiteV1   *gParamHost = 0;
OfxMemorySuiteV1      *gMemoryHost = 0;
OfxMultiThreadSuiteV1 *gThreadHost = 0;
OfxMessageSuiteV1     *gMessageSuite = 0;
OfxInteractSuiteV1    *gInteractHost = 0;


// we are always identity as we are just a hack example plugin
static OfxStatus
isIdentity(OfxImageEffectHandle  /*pluginInstance*/,
	   OfxPropertySetHandle /*inArgs*/,
	   OfxPropertySetHandle outArgs)
{
  // set the property in the out args indicating which is the identity clip
  gPropHost->propSetString(outArgs, kOfxPropName, 0, kOfxImageEffectSimpleSourceClipName);
  return kOfxStatOK;
}

// the process code  that the host sees
static OfxStatus render(OfxImageEffectHandle  /*instance*/,
			OfxPropertySetHandle /*inArgs*/,
			OfxPropertySetHandle /*outArgs*/)
{
  // do nothing as this should never be called as isIdentity should always be trapped
  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// the interaction routines
struct MyInteractData {
  bool selected;
  OfxParamHandle pointParam;

  explicit MyInteractData(OfxParamHandle pParam)
    : selected(false)
    , pointParam(pParam)
  {
  }
};

// get the interact data from an interact instance
static MyInteractData *
getInteractData(OfxInteractHandle interactInstance)
{    
  void *dataV = ofxuGetInteractInstanceData(interactInstance);
  return (MyInteractData *) dataV;
}

// creation of an interact instance
static OfxStatus 
interactDescribe(OfxInteractHandle /*interactDescriptor*/)
{

  // and we are good
  return kOfxStatOK;
}

// creation of an interact instance
static OfxStatus 
interactCreateInstance(OfxImageEffectHandle pluginInstance,
		       OfxInteractHandle interactInstance)
{
  // get the parameter set for this effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(pluginInstance, &paramSet);
  
  // fetch a handle to the point param from the parameter set
  OfxParamHandle pointParam;
  gParamHost->paramGetHandle(paramSet, kPointParam, &pointParam, 0);

  // make my interact's instance data
  MyInteractData *data = new MyInteractData(pointParam);

  // and set the interact's data pointer
  ofxuSetInteractInstanceData(interactInstance, (void *) data);

  OfxPropertySetHandle interactProps;
  gInteractHost->interactGetPropertySet(interactInstance, &interactProps);

  // slave this interact to the point param so redraws are triggered cleanly
  gPropHost->propSetString(interactProps, kOfxInteractPropSlaveToParam, 0, kPointParam);

  return kOfxStatOK;
}

// destruction of an interact instance
static OfxStatus 
interactDestroyInstance(OfxImageEffectHandle  /*pluginInstance*/,
			OfxInteractHandle interactInstance)
{
  MyInteractData *data = getInteractData(interactInstance);
  delete data;
  return kOfxStatOK;
}

// size of the cross hair in screen pixels
#define kXHairSize 10

// draw an interact instance
static OfxStatus 
interactDraw(OfxImageEffectHandle  pluginInstance,
	     OfxInteractHandle interactInstance,
	     OfxPropertySetHandle drawArgs)
{
  // get my private interact data
  MyInteractData *data = getInteractData(interactInstance);

  // get the size of a pixel in the current projection
  double pixelScale[2];
  ofxuGetInteractPixelScale(drawArgs, pixelScale);

  // get my param's value
  double x, y;
  gParamHost->paramGetValue(data->pointParam, &x, &y);

  // make the xhair a constant size on screen by scaling by the pixel scale
  float dx = kXHairSize * pixelScale[0];
  float dy = kXHairSize * pixelScale[1];

  // if the we have selected the Xhair, draw it highlit
  if(data->selected)
    glColor3f(1, 1, 1);
  else
    glColor3f(1, 0, 0);

  // Draw a cross hair, the current coordinate system aligns with the image plane.
  glPushMatrix();
  
  glTranslated(x, y, 0);

  glBegin(GL_LINES);

  glVertex2f(-dx, 0);
  glVertex2f(dx, 0);

  glVertex2f(0, -dy);
  glVertex2f(0, dy);

  glEnd();
  

  glPopMatrix();

  return kOfxStatOK;
}

// function reacting to pen motion
static OfxStatus
interactPenMotion(OfxImageEffectHandle  pluginInstance,
		  OfxInteractHandle interactInstance,	
		  OfxPropertySetHandle inArgs)
{
  // get my data handle
  MyInteractData *data = getInteractData(interactInstance);

  // Have we grabbed on a pen down already?
  if(data->selected) {
    // get the pen position
    OfxPointD penPos;
    gPropHost->propGetDoubleN(inArgs, kOfxInteractPropPenPosition, 2, &penPos.x);

    // set the value of the 'point' param
    gParamHost->paramSetValue(data->pointParam, penPos.x, penPos.y);
    return kOfxStatOK;
  }
  return kOfxStatReplyDefault;
}

static OfxStatus
interactPenDown(OfxImageEffectHandle  pluginInstance, 
		OfxInteractHandle interactInstance,	
		OfxPropertySetHandle inArgs)
{
  // get my data handle
  MyInteractData *data = getInteractData(interactInstance);

  // get the point param's value
  double x, y;
  gParamHost->paramGetValue(data->pointParam, &x, &y);

  // get the size of a pixel on screen
  double pixelScale[2];
  ofxuGetInteractPixelScale(inArgs, pixelScale);

  // see if the pen is within 5 screen pixels of the point, in which case, select it
  double penPos[2];
  gPropHost->propGetDoubleN(inArgs, kOfxInteractPropPenPosition, 2, penPos);
  if(fabs(x - penPos[0]) < 5 * pixelScale[0] && fabs(y - penPos[1]) < 5 * pixelScale[1]) {
    data->selected = true;   
    return kOfxStatOK;
  }
  return kOfxStatReplyDefault;
}

static OfxStatus
interactPenUp(OfxImageEffectHandle  /*pluginInstance*/,
		OfxInteractHandle interactInstance,
		OfxPropertySetHandle /*inArgs*/)
{
  // get my data handle
  MyInteractData *data = getInteractData(interactInstance);
  
  if(data->selected) {
    // pen's gone up, deselect
    data->selected = false;
    return kOfxStatOK;
  }
  return kOfxStatReplyDefault;
}

////////////////////////////////////////////////////////////////////////////////
// the entry point for the overlay
static OfxStatus
overlayMain(const char *action,  const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle /*outArgs*/)
{
  OfxInteractHandle interact = (OfxInteractHandle ) handle;
  
  OfxPropertySetHandle props;
  gInteractHost->interactGetPropertySet(interact, &props);

  if(strcmp(action, kOfxActionDescribe) == 0) {
    return interactDescribe(interact);
  }
  else {
    // fetch the effect instance from the interact
    OfxImageEffectHandle pluginInstance;
    gPropHost->propGetPointer(props, kOfxPropEffectInstance, 0, (void **) &pluginInstance); 
    
    if(strcmp(action, kOfxActionCreateInstance) == 0) {
      return interactCreateInstance(pluginInstance, interact);
    }
    else if(strcmp(action, kOfxActionDestroyInstance) == 0) {
      return interactDestroyInstance(pluginInstance, interact);
    }
    else if(strcmp(action, kOfxInteractActionDraw) == 0) {
      return interactDraw(pluginInstance, interact, inArgs);
    }
    else if(strcmp(action, kOfxInteractActionPenMotion) == 0) {
      return interactPenMotion(pluginInstance, interact, inArgs);
    }
    else if(strcmp(action, kOfxInteractActionPenDown) == 0) {
      return interactPenDown(pluginInstance, interact, inArgs);
    }
    else if(strcmp(action, kOfxInteractActionPenUp) == 0) {
      return interactPenUp(pluginInstance, interact, inArgs);
    }
    return kOfxStatReplyDefault;
  }
  return kOfxStatReplyDefault;
}

////////////////////////////////////////////////////////////////////////////////
// the plugin's description routine
static OfxStatus
describe(OfxImageEffectHandle effect)
{
  // fetch the host APIs
  OfxStatus stat;
  if((stat = ofxuFetchHostSuites()) != kOfxStatOK)
    return stat;

  // see if the host supports overlays
  int supportsOverlays;
  gPropHost->propGetInt(gHost->host, kOfxImageEffectPropSupportsOverlays, 0, &supportsOverlays);
  if(supportsOverlays == 0)
    return kOfxStatErrMissingHostFeature;

  // get the property handle for the plugin
  OfxPropertySetHandle effectProps;
  gEffectHost->getPropertySet(effect, &effectProps);

  // define the plugin to the host
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 1, kOfxBitDepthShort);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 2, kOfxBitDepthFloat);

  // set the bit depths the plugin can handle
  gPropHost->propSetString(effectProps, kOfxPropLabel, 0, "OFX Overlay Example");
  gPropHost->propSetString(effectProps, kOfxImageEffectPluginPropGrouping, 0, "OFX Example");

  // define the contexts we can be used in
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextFilter);
    
  // set the property that is the overlay's main entry point for the plugin
  gPropHost->propSetPointer(effectProps, kOfxImageEffectPluginPropOverlayInteractV1, 0,  (void *) overlayMain);
  
  return kOfxStatOK;
}

static OfxStatus
describeInContext(OfxImageEffectHandle  effect, OfxPropertySetHandle /*inArgs*/)
{
  // define the single source clip
  OfxPropertySetHandle props;
  gEffectHost->clipDefine(effect, kOfxImageEffectSimpleSourceClipName, &props);
  gPropHost->propSetString(props, kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA);
  gPropHost->propSetString(props, kOfxImageEffectPropSupportedComponents, 1, kOfxImageComponentAlpha);

  // define the output clip
  gEffectHost->clipDefine(effect, kOfxImageEffectOutputClipName, &props);
  gPropHost->propSetString(props, kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA);
  gPropHost->propSetString(props, kOfxImageEffectPropSupportedComponents, 1, kOfxImageComponentAlpha);
  
  // fetch the parameter set from the effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(effect, &paramSet);

  // define the 2D point we are going to draw an overlay for
  gParamHost->paramDefine(paramSet, kOfxParamTypeDouble2D, kPointParam, &props);
  gPropHost->propSetString(props, kOfxParamPropDoubleType, 0, kOfxParamDoubleTypeXYAbsolute);
  gPropHost->propSetString(props, kOfxParamPropDefaultCoordinateSystem, 0, kOfxParamCoordinatesNormalised);
  gPropHost->propSetDouble(props, kOfxParamPropDefault, 0, 0.5);
  gPropHost->propSetDouble(props, kOfxParamPropDefault, 1, 0.5);
  gPropHost->propSetString(props, kOfxParamPropHint, 0, "Point attached to overlay crosshair");
  gPropHost->propSetString(props, kOfxParamPropScriptName, 0, "point");
  gPropHost->propSetString(props, kOfxPropLabel, 0, "Point");

  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// The main function
static OfxStatus
pluginMain(const char *action, const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs)
{
  try {
  // cast to appropriate type
  OfxImageEffectHandle effect = (OfxImageEffectHandle ) handle;

  if(strcmp(action, kOfxActionDescribe) == 0) {
    return describe(effect);
  }
  else if(strcmp(action, kOfxImageEffectActionDescribeInContext) == 0) {
    return describeInContext(effect, inArgs);
  }
  else if(strcmp(action, kOfxImageEffectActionIsIdentity) == 0) {
    return isIdentity(effect, inArgs, outArgs);
  }    
  else if(strcmp(action, kOfxImageEffectActionRender) == 0) {
    return render(effect, inArgs, outArgs);
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
  "uk.co.thefoundry.BasicOverlayPlugin",
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


