// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

/*
  Ofx Example plugin that shows how a custom parameters are managed.

  It is meant to illustrate certain features of the API, as opposed to being a perfectly
  crafted piece of image processing software.
 */

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif 

#include <cstring>
#include <cstdio>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <cmath>
#include <stdexcept>
#include <limits>
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

#define kPointParam   "point"
#define kDataParam    "data"
#define kBigDataParam "big_data"
#define kButtonParam  "button"
#define kTestBigDataParam  "test_data"

// pointers to various bits of the host
OfxHost                 *gHost;
OfxImageEffectSuiteV1 *gEffectHost = 0;
OfxPropertySuiteV1    *gPropHost = 0;
OfxParameterSuiteV1   *gParamHost = 0;
OfxMemorySuiteV1      *gMemoryHost = 0;
OfxMultiThreadSuiteV1 *gThreadHost = 0;
OfxMessageSuiteV1     *gMessageSuite = 0;
OfxInteractSuiteV1    *gInteractHost = 0;

// decode a custom param from a string
static OfxStatus
parseCustomParam(char *str, double &x, double &y)
{
  if(str && *str) {
    int nRead = sscanf(str, "%lg %lg", &x, &y);
    if(nRead != 2) 
      return kOfxStatErrFormat; // HACK need better error codes
  }
  else
    return kOfxStatErrFormat; // HACK need better error codes
  return kOfxStatOK;
} 

// encode a custom param into a string
void
writeCustomParam(char *str, int strlen, double x, double y)
{
#ifdef _WIN32
  _snprintf(str, strlen, "%lg %lg", x, y);
#else
  snprintf(str, strlen, "%lg %lg", x, y);
#endif
}

// get the current value of my custom parameter
static OfxStatus
getCustomParam(OfxImageEffectHandle pluginInstance, double &x, double &y)
{
  // get the parameter set from the effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(pluginInstance, &paramSet);
  
  // get the parameter from the parameter set
  OfxParamHandle param;
  gParamHost->paramGetHandle(paramSet, kPointParam, &param, NULL);

  // get my custom param's raw value
  char *str = 0;
  gParamHost->paramGetValue(param, &str);

  return parseCustomParam(str, x, y);
}

// set the current value of my custom parameter
static OfxStatus
setCustomParam(OfxImageEffectHandle pluginInstance, double x, double y)
{
  // encode the custom parameter
  char str[1024];
  writeCustomParam(str, 1024, x, y);

  // get the parameter set from the effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(pluginInstance, &paramSet);
  
  // get the parameter from the parameter set
  OfxParamHandle param;
  gParamHost->paramGetHandle(paramSet, kPointParam, &param, NULL);

  // set it's value
  return gParamHost->paramSetValue(param, str);
}

struct MyData
{
  int version = 1;
  int index = 0;

  void getColor(float color[3]) const
  {
    int r = (index + 0) % 3;
    int g = (index + 1) % 3;
    int b = (index + 2) % 3;
    color[r] = 1.0f;
    color[g] = 0.0f;
    color[b] = 0.0f;
  }
};

struct MyBigData
{
  static constexpr const int DataSize = 10 * 1024 * 1024; // 10MB
  unsigned char data[DataSize];

  void init()
  {
    // store increasing bytes
    for (auto i = 0; i < DataSize; i++)
      data[i] = i % 256;
  }
  bool test() const
  {
    for (auto i = 0; i < DataSize; i++)
	{
      if (data[i] != i % 256)
        return false;
	}
    return true;
  }
};

// get the current value of my data parameter
static OfxStatus
getBytesParam(OfxImageEffectHandle pluginInstance, const char *name, OfxBytes &bytes, double time = std::numeric_limits<double>::max())
{
  // get the parameter set from the effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(pluginInstance, &paramSet);
  
  // get the parameter from the parameter set
  OfxParamHandle param;
  OfxPropertySetHandle props;
  if (gParamHost->paramGetHandle(paramSet, name, &param, &props) == kOfxStatOK)
  {
    // get bytes value
    if (time != std::numeric_limits<double>::max())    
      gParamHost->paramGetValueAtTime(param, time, &bytes);
    else
      gParamHost->paramGetValue(param, &bytes);
    return kOfxStatOK;
  }
  return kOfxStatErrUnknown;
}

// set the current value of my custom parameter
static OfxStatus
setBytesParam(OfxImageEffectHandle pluginInstance, const char *name, const OfxBytes &bytes, double time = std::numeric_limits<double>::max())
{
  // get the parameter set from the effect
  OfxParamSetHandle paramSet;
  gEffectHost->getParamSet(pluginInstance, &paramSet);
  
  // get the parameter from the parameter set
  OfxParamHandle param;
  OfxPropertySetHandle props;
  if (gParamHost->paramGetHandle(paramSet, name, &param, &props) == kOfxStatOK)
  {
    if (time != std::numeric_limits<double>::max())    
      return gParamHost->paramSetValueAtTime(param, time, &bytes);
    return gParamHost->paramSetValue(param, &bytes);
  }
  return kOfxStatErrUnknown;
}

// The callback passed across the API do do custom parameter animation
static OfxStatus 
customParamInterpFunction(OfxImageEffectHandle /*instance*/,
			     OfxPropertySetHandle inArgs,
			     OfxPropertySetHandle outArgs)
{
  double x0, y0, x1, y1;
  OfxStatus err;

  char *fromValue, *toValue;

  // parse the custom param at the from value
  gPropHost->propGetString(inArgs, kOfxParamPropCustomValue, 0, &fromValue);
  if((err = parseCustomParam(fromValue, x0, y0)) != kOfxStatOK)
    return err;

  // parse the custom param at the to value
  gPropHost->propGetString(inArgs, kOfxParamPropCustomValue, 1, &toValue);
  if((err = parseCustomParam(toValue, x1, y1)) != kOfxStatOK)
    return err;

  // get the interp amount
  double interp;
  gPropHost->propGetDouble(inArgs, kOfxParamPropInterpolationAmount, 0, &interp);

  // and lerp the values
  double x = x0 + (x1 - x0) * interp;
  double y = y0 + (y1 - y0) * interp;

  // now encode the value and set it 
  char str[1024];
  writeCustomParam(str, 1024, x, y);
  gPropHost->propSetString(outArgs, kOfxParamPropCustomValue, 0, str);

  return kOfxStatOK;
}

// plugin instance construction
static OfxStatus
createInstance(OfxImageEffectHandle  /*instance*/)
{
  return kOfxStatOK;
}

// plugin instance destruction
static OfxStatus
destroyInstance(OfxImageEffectHandle  /*instance*/)
{
  return kOfxStatOK;
}

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

  // get the name of the thing that changed
  char *objChanged;
  gPropHost->propGetString(inArgs, kOfxPropName, 0, &objChanged);

  // if the button was pressed, modify the Bytes parameters
  if(strcmp(objChanged, kButtonParam) == 0)
  {
    double time;
    gPropHost->propGetDouble(inArgs, kOfxPropTime, 0, &time);

    // cycle the contents of the data param
    OfxBytes bytes;
    if (getBytesParam(effect, kDataParam, bytes, time) == kOfxStatOK)
    {
      MyData data;
      if (bytes.length == sizeof(MyData))
        data = *(MyData *)(bytes.data);
      data.index++;
		  bytes = { reinterpret_cast<const unsigned char *>(&data), sizeof(data) };
		  setBytesParam(effect, kDataParam, bytes, time);
    }
    return kOfxStatOK;
  }
  else if(strcmp(objChanged, kTestBigDataParam) == 0)
  {
    // check for existing big data
    OfxBytes bytes;    
    if (getBytesParam(effect, kBigDataParam, bytes) == kOfxStatOK)
    {
      if (bytes.length == MyBigData::DataSize)
      {
        auto data = reinterpret_cast<const MyBigData *>(bytes.data);
        if (data->test())
        {
          gMessageSuite->message(effect, kOfxMessageMessage, nullptr, "Big data supported");
        }
        else
        {
          gMessageSuite->message(effect, kOfxMessageMessage, nullptr, "Error in big data");
          bytes = { nullptr, 0 };
          setBytesParam(effect, kBigDataParam, bytes);
        }
      }
      else
      {
        // set some big data
        MyBigData *bigData = new MyBigData;
        bigData->init();
        bytes = { reinterpret_cast<const unsigned char *>(bigData), MyBigData::DataSize };

        setBytesParam(effect, kBigDataParam, bytes);
        delete bigData;
        gMessageSuite->message(effect, kOfxMessageMessage, nullptr, "Big data set");
      }
    }
    return kOfxStatOK;
  }

  // don't trap any others
  return kOfxStatReplyDefault;
}

static OfxStatus
isIdentity(OfxImageEffectHandle /*pluginInstance*/,
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
			OfxPropertySetHandle /*outArg*/)
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
interactDestroyInstance(OfxImageEffectHandle /*pluginInstance*/,
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
interactDraw(OfxImageEffectHandle pluginInstance, 
	     OfxInteractHandle interactInstance,
	     OfxPropertySetHandle drawArgs)
{
  OfxStatus err;
  // get my private interact data
  MyInteractData *data = getInteractData(interactInstance);

  double time;
  gPropHost->propGetDouble(drawArgs, kOfxPropTime, 0, &time);

  // get the project size
  OfxPointD projSize, projOffset;
  ofxuGetProjectSetup(pluginInstance, projSize, projOffset);

  // get the size of a pixel in the current projection
  double pixelScale[2];
  ofxuGetInteractPixelScale(drawArgs, pixelScale);

  // get my param's value
  double x, y;
  if((err = getCustomParam(pluginInstance, x, y)) != kOfxStatOK)
    return err;

  // get the opaque data if it exists
  MyData binary_data;
  {
    OfxBytes bytes;
    if (getBytesParam(pluginInstance, kDataParam, bytes, time) == kOfxStatOK)
    {
      if (bytes.length == sizeof(MyData))
        binary_data = *reinterpret_cast<const MyData *>(bytes.data);
    }
  }
  // scale it up to the project size as it is normalised
  x = projOffset.x + x * projSize.x;
  y = projOffset.y + y * projSize.y;

  // make the xhair a constant size on screen by scaling by the pixel scale
  float dx = kXHairSize * pixelScale[0];
  float dy = kXHairSize * pixelScale[1];

  // if the we have selected the Xhair, draw it highlit
  if(data->selected)
    glColor3f(1, 1, 1);
  else
  {
    // draw using the color defined in the custom data
    float color[3];
    binary_data.getColor(color);
    glColor3fv(color);
  }

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
interactPenMotion(OfxImageEffectHandle pluginInstance, 
		  OfxInteractHandle interactInstance, 
		  OfxPropertySetHandle inArgs)
{
  // get my data handle
  MyInteractData *data = getInteractData(interactInstance);

  // Have we grabbed on a pen down already?
  if(data->selected) {
    // get the project size as we are normalising to this
    OfxPointD projSize, projOffset;
    ofxuGetProjectSetup(pluginInstance, projSize, projOffset);

    // get the pen position and normalise that
    OfxPointD penPos;
    gPropHost->propGetDoubleN(inArgs, kOfxInteractPropPenPosition, 2, &penPos.x);
    penPos.x = (penPos.x - projOffset.x)/projSize.x;
    penPos.y = (penPos.y - projOffset.y)/projSize.y;

    // set the value of the 'point' param
    setCustomParam(pluginInstance, penPos.x, penPos.y);
    return kOfxStatOK;
  }
  return kOfxStatReplyDefault;
}

static OfxStatus
interactPenDown(OfxImageEffectHandle pluginInstance, 
		OfxInteractHandle interactInstance, 
		OfxPropertySetHandle inArgs)
{
  // get my data handle
  MyInteractData *data = getInteractData(interactInstance);

  // get the project size as we are normalising to this
  OfxPointD projSize, projOffset;
  ofxuGetProjectSetup(pluginInstance, projSize, projOffset);

  // get the point param's value
  double x, y;
  OfxStatus stat = getCustomParam(pluginInstance, x, y);
  if (stat != kOfxStatOK) {
    return stat;
  }

  // scale it up to the project size as it is a normalised spatial parameter
  x = projOffset.x + x * projSize.x;
  y = projOffset.y + y * projSize.y;

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
interactPenUp(OfxImageEffectHandle /*pluginInstance*/,
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
    // in which case handle is the interact instance

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
  
  // see if the host supports overlays and custom params and custom param animation
  int supportsOverlays;
  gPropHost->propGetInt(gHost->host, kOfxImageEffectPropSupportsOverlays, 0, &supportsOverlays);
  if(supportsOverlays == 0)
    return kOfxStatErrMissingHostFeature;

  // get the property handle for the plugin
  OfxPropertySetHandle effectProps;
  gEffectHost->getPropertySet(effect, &effectProps);

  // set the bit depths the plugin can handle
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 1, kOfxBitDepthShort);
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, 2, kOfxBitDepthFloat);

  // define the plugin to the host
  gPropHost->propSetString(effectProps, kOfxPropLabel, 0, "OFX Custom Param Example");
  gPropHost->propSetString(effectProps, kOfxImageEffectPluginPropGrouping, 0, "OFX Example");

  // define the contexts we can be used in
  gPropHost->propSetString(effectProps, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextFilter);
    
  // set the property that is the overlay's main entry point for the plugin
  gPropHost->propSetPointer(effectProps, kOfxImageEffectPluginPropOverlayInteractV1, 0,  (void *) overlayMain);
  
  return kOfxStatOK;
}

static OfxStatus
describeInContext(OfxImageEffectHandle effect, OfxPropertySetHandle /*inArgs*/)
{
  // define the mandated single source clip
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

  // Make a custom parameter
  gParamHost->paramDefine(paramSet, kOfxParamTypeCustom, kPointParam, &props);
  gPropHost->propSetString(props, kOfxParamPropDefault, 0, "0.5 0.5");
  gPropHost->propSetString(props, kOfxParamPropScriptName, 0, "point");
  gPropHost->propSetString(props, kOfxParamPropHint, 0, "Custom point attached to overlay crosshair");
  gPropHost->propSetString(props, kOfxPropLabel, 0, "Point");

  // set the custom anim callback of the host supports custom animation
  int supportsCustomAnim;
  gPropHost->propGetInt(gHost->host, kOfxParamHostPropSupportsCustomAnimation, 0, &supportsCustomAnim);
  if(supportsCustomAnim) {
    gPropHost->propSetInt(props, kOfxParamPropAnimates, 0, true);
    gPropHost->propSetPointer(props, kOfxParamPropCustomInterpCallbackV1, 0, (void *) customParamInterpFunction);
  }
  else {
    gPropHost->propSetInt(props, kOfxParamPropAnimates, 0, false);
  }

  // Make Bytes parameters to store the crosshair color and some large data
  if (gParamHost->paramDefine(paramSet, kOfxParamTypeBytes, kDataParam, &props) == kOfxStatOK)
  {
    gPropHost->propSetString(props, kOfxParamPropHint, 0, "Opaque data describing crosshair color");
    gPropHost->propSetInt(props, kOfxParamPropAnimates, 0, 1);
    gPropHost->propSetInt(props, kOfxParamPropIsAutoKeying, 0, 1);  

    // non-animating bytes
    gParamHost->paramDefine(paramSet, kOfxParamTypeBytes, kBigDataParam, &props);
    gPropHost->propSetString(props, kOfxParamPropHint, 0, "Large block of opaque data");

    // Make a button parameter to change the Bytes parameters
    gParamHost->paramDefine(paramSet, kOfxParamTypePushButton, kButtonParam, &props);
    gPropHost->propSetString(props, kOfxPropLabel, 0, "Change crosshair color");
 
    // Make a button parameter to test big data
    gParamHost->paramDefine(paramSet, kOfxParamTypePushButton, kTestBigDataParam, &props);
    gPropHost->propSetString(props, kOfxPropLabel, 0, "Test big data");
  }

  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// The main function
static OfxStatus
pluginMain(const char *action,  const void *handle,  OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs)
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
  else if(strcmp(action, kOfxActionCreateInstance) == 0) {
    return createInstance(effect);
  } 
  else if(strcmp(action, kOfxActionDestroyInstance) == 0) {
    return destroyInstance(effect);
  } 
  else if(strcmp(action, kOfxImageEffectActionIsIdentity) == 0) {
    return isIdentity(effect, inArgs, outArgs);
  }    
  else if(strcmp(action, kOfxActionInstanceChanged) == 0) {
    return instanceChanged(effect, inArgs, outArgs);
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
  "uk.co.thefoundry.CustomParamPlugin",
  2,
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
