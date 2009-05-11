/*
OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
Copyright (C) 2004 The Open Effects Association Ltd
Author Bruno Nicoletti bruno@thefoundry.co.uk

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
* Neither the name The Open Effects Association Ltd, nor the names of its 
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The Open Effects Association Ltd
1 Wardour St
London W1D 6PA
England


*/
#ifdef _WINDOWS
#include <windows.h>
#endif

#ifdef __APPLE__
#include <AGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stdio.h>
#include "ofxsImageEffect.h"

class ColourInteract : public OFX::ParamInteract
{
protected:
  enum State
  {
    eDefault,
    ePoised,
    eDragging
  };
  State _state;
  double _radius;
    
public:
  ColourInteract(OfxInteractHandle handle, OFX::ImageEffect* effect, const std::string& paramName):
      OFX::ParamInteract(handle, effect), _state(eDefault), _radius(5.0)
  {
     _param = effect->fetchRGBParam(paramName);
  }
  virtual bool draw(const OFX::DrawArgs &args)
  {
    OfxPointI size = getInteractSize();
    glBegin (GL_POLYGON);
    
    glColor3f (0.0, 0.0, 0.0);
    glVertex2f (-0.5, -0.5);

    glColor3f (1.0, 0.0, 0.0);
    glVertex2f (-0.5f, size.y-0.5f);

    glColor3f (0.0, 1.0, 0.0);
    glVertex2f (size.x - 0.5f, size.y - 0.5f);
    
    glColor3f (0.0, 0.0, 1.0);
    glVertex2f (size.x - 0.5f, -0.5f);
    
    glEnd();

    double r,g,b,x,y;
    _param->getValueAtTime(args.time, r, g, b);
    positionFromColour(r, g, b, x, y);
    glColor3f(1.0f, 1.0f, 1.0f);
    if(_state==ePoised)
      glColor3f(0.5f, 0.5f, 0.5f);

    glBegin(GL_POLYGON);
    glVertex2d(x - _radius, y - _radius);
    glVertex2d(x - _radius, y + _radius);
    glVertex2d(x + _radius, y + _radius);
    glVertex2d(x + _radius, y - _radius);
    glEnd();

    return true;
  }
  bool hitTest(double posx, double posy, double time) const
  {
    double r,g,b,x,y;
    _param->getValueAtTime(time, r, g, b);
    positionFromColour(r, g, b, x, y);
    if(posx > x - _radius &&
       posy > y - _radius &&
       posx < x + _radius &&
       posy < y + _radius)
       return true;
    return false;
  }
  void positionFromColour(double r, double g, double b, double& x, double& y) const
  {
    x = (b/(g+b)) * 100.0 - 0.5;
    y = (r/(g+r)) * 100.0 - 0.5;
  }
  void colourFromPosition(double x, double y, double& r, double& g, double& b) const
  {
    x += 0.5;
    y += 0.5;
    x = x*0.01;
    y = y*0.01;
    r = y * ( 1.0 - x);
    b = x * ( 1.0 - y);
    g = (1.0 - x) * (1.0 - y);
  }
  virtual bool penMotion(const OFX::PenArgs &args)
  {
    if(_state != eDragging)
    {
      if(hitTest(args.penPosition.x, args.penPosition.y, args.time))
      {
        _state = ePoised;
        requestRedraw();
        return true;
      }
    }
    else
    {
      double r,g,b;
      colourFromPosition(args.penPosition.x, args.penPosition.y, r, g, b);
      _param->setValueAtTime(args.time, r, g, b);
      requestRedraw();
      return true;
    }
    _state = eDefault;
    requestRedraw();
    return false;
  }
  virtual bool penDown(const OFX::PenArgs &args)
  {
    if(hitTest(args.penPosition.x, args.penPosition.y, args.time))
    {
      _state = eDragging;
      return true;
    }
    return false;
  }
  virtual bool penUp(const OFX::PenArgs &args)
  {
    _state = ePoised;
    requestRedraw();
    return true;
  }
  virtual ~ColourInteract(){}
protected:
  OFX::RGBParam* _param;
};

//Need an instance count as a template parameter in order to generate a different mainEntry point for each instance.
// Hopefully this will be fixed in the next iteration of the OFX standard.
template<int INSTANCECOUNT>
class ColourInteractDescriptor : public OFX::DefaultParamInteractDescriptor<ColourInteractDescriptor<INSTANCECOUNT>, ColourInteract>
{
public:
  using OFX::DefaultParamInteractDescriptor<ColourInteractDescriptor<INSTANCECOUNT>,ColourInteract>::setInteractSizeAspect;
  using OFX::DefaultParamInteractDescriptor<ColourInteractDescriptor<INSTANCECOUNT>,ColourInteract>::setInteractMinimumSize;
  using OFX::DefaultParamInteractDescriptor<ColourInteractDescriptor<INSTANCECOUNT>,ColourInteract>::setInteractPreferredSize;
  virtual void describe()
  {
    setInteractSizeAspect(1.0);
    setInteractMinimumSize(50, 50);
    setInteractPreferredSize(100, 100);
  }
};


////////////////////////////////////////////////////////////////////////////////
/** @brief base class of the plugin */
class BasePlugin : public OFX::ImageEffect {
protected :
  // do not need to delete this, the ImageEffect is managing them for us
  OFX::Clip *dstClip_;

public :
  /** @brief ctor */
  BasePlugin(OfxImageEffectHandle handle)
    : ImageEffect(handle)
    , dstClip_(0)
  {
    dstClip_ = fetchClip("Output");
  }

};

////////////////////////////////////////////////////////////////////////////////
/** @brief generator effect version of the plugin */
class GeneratorPlugin : public BasePlugin {
public :
  /** @brief ctor */
  GeneratorPlugin(OfxImageEffectHandle handle)
    : BasePlugin(handle)
  {}

  /** @brief client render function, this is one of the few that must be set */
  virtual void render(const OFX::RenderArguments &args);
};

////////////////////////////////////////////////////////////////////////////////
/** @brief filter version of the plugin */
class FilterPlugin : public BasePlugin {
protected :
  // do not need to delete this, the ImageEffect is managing them for us
  OFX::Clip *srcClip_;

public :
  /** @brief ctor */
  FilterPlugin(OfxImageEffectHandle handle)
    : BasePlugin(handle)
    , srcClip_(0)
  {
    srcClip_ = fetchClip("Source");
  }

  /** @brief client render function, this is one of the few that must be set */
  virtual void render(const OFX::RenderArguments &args);
};


////////////////////////////////////////////////////////////////////////////////
/** @brief filter version of the plugin */
class GeneralPlugin : public FilterPlugin {
protected :
  // do not need to delete this, the ImageEffect is managing them for us
  OFX::Clip *extraClip_;

public :
  /** @brief ctor */
  GeneralPlugin(OfxImageEffectHandle handle)
    : FilterPlugin(handle)
    , extraClip_(0)
  {
    extraClip_ = fetchClip("Extra");
  }
};

using namespace OFX;

class PropTesterPluginFactory : public OFX::PluginFactoryHelper<PropTesterPluginFactory>
{
public:
  PropTesterPluginFactory():OFX::PluginFactoryHelper<PropTesterPluginFactory>("net.sf.openfx:propertyTester", 1, 0){}
  virtual void describe(OFX::ImageEffectDescriptor &desc);
  virtual void describeInContext(OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context);
  virtual OFX::ImageEffect* createInstance(OfxImageEffectHandle handle, OFX::ContextEnum context);
};

namespace OFX 
{
  namespace Plugin 
  {
    void getPluginIDs(OFX::PluginFactoryArray &ids)
    {
      static PropTesterPluginFactory p;
      ids.push_back(&p);
    }
  }
}

void PropTesterPluginFactory::describe(OFX::ImageEffectDescriptor &desc) 
{
  // basic labels
  desc.setLabels("Prop Tester", "Prop Tester", "Property Tester");
  desc.setPluginGrouping("OFX Test");

  // add the supported contexts, only filter at the moment
  desc.addSupportedContext(eContextGenerator);
  desc.addSupportedContext(eContextFilter);
  desc.addSupportedContext(eContextGeneral);


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

/** @brief describe a string param with the given name and type */
void describeStringParam(OFX::ImageEffectDescriptor &desc, const std::string &name, StringTypeEnum strType, PageParamDescriptor *page)
{
  StringParamDescriptor *param = desc.defineStringParam(name);
  param->setDefault(name);
  param->setScriptName(name);
  param->setHint("A string parameter");
  param->setLabels(name, name, name);
  param->setStringType(strType);
  page->addChild(*param);
}

/** @brief describe a double param */
void describeDoubleParam(OFX::ImageEffectDescriptor &desc, const std::string &name, DoubleTypeEnum doubleType,
                         double min, double max, PageParamDescriptor *page)
{
  DoubleParamDescriptor *param = desc.defineDoubleParam(name);
  param->setLabels(name, name, name);
  param->setScriptName(name);
  param->setHint("A double parameter");
  param->setDefault(0);
  param->setRange(min, max);
  param->setDisplayRange(min, max);
  param->setDoubleType(doubleType);
  page->addChild(*param);
}

/** @brief describe a double param */
void describe2DDoubleParam(OFX::ImageEffectDescriptor &desc, const std::string &name, DoubleTypeEnum doubleType,
                           double min, double max, PageParamDescriptor *page)
{
  Double2DParamDescriptor *param = desc.defineDouble2DParam(name);
  param->setLabels(name, name, name);
  param->setScriptName(name);
  param->setHint("A 2D double parameter");
  param->setDefault(0, 0);
  param->setRange(min, min, max, max);
  param->setDisplayRange(min, min, max, max);
  param->setDoubleType(doubleType);
  page->addChild(*param);
}

/** @brief describe a double param */
void describe3DDoubleParam(OFX::ImageEffectDescriptor &desc, const std::string &name, DoubleTypeEnum doubleType,
                           double min, double max, PageParamDescriptor *page)
{
  Double3DParamDescriptor *param = desc.defineDouble3DParam(name);
  param->setLabels(name, name, name);
  param->setScriptName(name);
  param->setHint("A 3D double parameter");
  param->setDefault(0, 0, 0);
  param->setRange(min, min, min, max, max, max);
  param->setDisplayRange(min, min, min, max, max, max);
  param->setDoubleType(doubleType);
  page->addChild(*param);
}

/** @brief The describe in context function, passed a plugin descriptor and a context */
void PropTesterPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context) 
{
  // Source clip only in the filter context
  if(context == eContextGeneral) {
    // create the mandated source clip
    ClipDescriptor *srcClip = desc.defineClip("Extra");
    srcClip->addSupportedComponent(ePixelComponentRGBA);
    srcClip->setTemporalClipAccess(false);
    srcClip->setOptional(false);
    srcClip->setSupportsTiles(true);
    srcClip->setIsMask(false);
  }

  // Source clip only in the filter context
  if(context == eContextFilter || context == eContextGeneral) {
    // create the mandated source clip
    ClipDescriptor *srcClip = desc.defineClip("Source");
    srcClip->addSupportedComponent(ePixelComponentRGBA);
    srcClip->setTemporalClipAccess(false);
    //srcClip->setOptional(false);
    srcClip->setSupportsTiles(true);
    srcClip->setIsMask(false);
  }

  // create the mandated output clip
  ClipDescriptor *dstClip = desc.defineClip("Output");
  dstClip->addSupportedComponent(ePixelComponentRGBA);
  dstClip->setTemporalClipAccess(false);
  //dstClip->setOptional(false);
  dstClip->setSupportsTiles(true);
  dstClip->setIsMask(false);


  // make some pages and to things in 
  PageParamDescriptor *page1 = desc.definePageParam("page1");
  PageParamDescriptor *page2 = desc.definePageParam("page2");
  PageParamDescriptor *page3 = desc.definePageParam("page3");

  // make an int param
  IntParamDescriptor *iParam = desc.defineIntParam("Int");
  iParam->setLabels("Int", "Int", "Int");
  iParam->setScriptName("int");
  iParam->setHint("An integer parameter");
  iParam->setDefault(0);
  iParam->setRange(-100, 100);
  iParam->setDisplayRange(-100, 100);

  page1->addChild(*iParam);

  // make a 2D int param
  Int2DParamDescriptor *i2DParam = desc.defineInt2DParam("Int2D");
  i2DParam->setLabels("Int2D", "Int2D", "Int2D");
  i2DParam->setScriptName("int2D");
  i2DParam->setHint("A 2D integer parameter");
  i2DParam->setDefault(0, 0);
  i2DParam->setRange(-100, -100, 100, 100);
  i2DParam->setDisplayRange(-100, -100, 100, 100);

  page1->addChild(*i2DParam);

  // make a 3D int param
  Int3DParamDescriptor *i3DParam = desc.defineInt3DParam("Int3D");
  i3DParam->setLabels("Int3D", "Int3D", "Int2D");
  i3DParam->setScriptName("int3D");
  i3DParam->setHint("A 3D integer parameter");
  i3DParam->setDefault(0, 0, 0);
  i3DParam->setRange(-100, -100, -100, 100, 100, 100);
  i3DParam->setDisplayRange(-100, -100, -100, 100, 100, 100);

  page1->addChild(*i3DParam);

  page1->addChild(PageParamDescriptor::gSkipColumn);

  // boolean
  BooleanParamDescriptor *boolean = desc.defineBooleanParam("bool");
  boolean->setLabels("bool", "bool", "bool");
  boolean->setDefault(false);

  page1->addChild(*boolean);

  // choice 
  ChoiceParamDescriptor *choice = desc.defineChoiceParam("choice");
  choice->setLabels("choice", "choice", "choice");
  choice->appendOption("This");
  choice->appendOption("That");
  choice->appendOption("The Other");
  choice->resetOptions();
  choice->appendOption("Tom");
  choice->appendOption("Dick");
  choice->appendOption("Harry");
  choice->setDefault(0);

  page1->addChild(*choice);

  page1->addChild(PageParamDescriptor::gSkipColumn);

  // push button
  PushButtonParamDescriptor *push = desc.definePushButtonParam("push");
  push->setLabels("push me", "push me", "push me Big Nose");
  page1->addChild(*push);

  // make a custom param
  CustomParamDescriptor *custom = desc.defineCustomParam("custom");
  custom->setLabels("custom", "custom", "custom");
  custom->setDefault("wibble");

  // rgba colours
  RGBAParamDescriptor *rgba = desc.defineRGBAParam("rgba");
  rgba->setLabels("rgba", "rgba", "rgba");
  rgba->setDefault(0, 0, 0, 1);

  page1->addChild(*rgba);

  RGBParamDescriptor *rgba2 = desc.defineRGBParam("rgbaCustom");
  rgba2->setLabels("RGB Custom", "RGB Custom", "RGB Custom");
  rgba2->setDefault(0, 1, 1);
  rgba2->setInteractDescriptor(new ColourInteractDescriptor<0>);
  page1->addChild(*rgba2);

  RGBParamDescriptor *rgba3 = desc.defineRGBParam("rgbaCustom2");
  rgba3->setLabels("RGB Custom 2", "RGB Custom 2", "RGB Custom 2");
  rgba3->setDefault(1, 0, 1);
  rgba3->setInteractDescriptor(new ColourInteractDescriptor<1>);
  page1->addChild(*rgba3);

  page1->addChild(PageParamDescriptor::gSkipRow);

  // rgb colour
  RGBParamDescriptor *rgb = desc.defineRGBParam("rgb");
  rgb->setLabels("rgb", "rgb", "rgb");
  rgb->setDefault(0, 0, 0);
  page1->addChild(*rgb);

  // make a 1D double parameter of each type
  describeDoubleParam(desc, "double", eDoubleTypePlain, -100, 100, page2);
  describeDoubleParam(desc, "angle", eDoubleTypeAngle, -100, 100, page2);
  describeDoubleParam(desc, "scale", eDoubleTypeScale, -1, 1, page2);
  describeDoubleParam(desc, "time", eDoubleTypeTime, -100, 100, page2);
  describeDoubleParam(desc, "absoluteTime", eDoubleTypeAbsoluteTime, 0, 1000, page2);
  describeDoubleParam(desc, "X_Value", eDoubleTypeNormalisedX, -1, 1, page2);
  describeDoubleParam(desc, "Y_Value", eDoubleTypeNormalisedY, -1, 1, page2);
  describeDoubleParam(desc, "X_Position", eDoubleTypeNormalisedXAbsolute, -1, 1, page2);
  describeDoubleParam(desc, "Y_Position", eDoubleTypeNormalisedYAbsolute, -1, 1, page2);

  page2->addChild(PageParamDescriptor::gSkipColumn);

  // make a 2D double parameter of each type
  describe2DDoubleParam(desc, "double2D", eDoubleTypePlain, -100, 100, page2);
  describe2DDoubleParam(desc, "angle2D", eDoubleTypeAngle, -100, 100, page2);
  describe2DDoubleParam(desc, "scale2D", eDoubleTypeScale, -1, 1, page2);
  describe2DDoubleParam(desc, "XY_Value", eDoubleTypeNormalisedXY, -1, 1, page2);
  describe2DDoubleParam(desc, "XY_Position", eDoubleTypeNormalisedXYAbsolute, -1, 1, page2);

  page2->addChild(PageParamDescriptor::gSkipColumn);

  // make a 3D double parameter of each type
  describe3DDoubleParam(desc, "double3D", eDoubleTypePlain, -100, 100, page2);
  describe3DDoubleParam(desc, "angle3D", eDoubleTypeAngle, -100, 100, page2);
  describe3DDoubleParam(desc, "scale3D", eDoubleTypeScale, -1, 1, page2);

  // make a string param param of each type
  describeStringParam(desc, "singleLine", eStringTypeSingleLine, page3);
  describeStringParam(desc, "multiLine", eStringTypeMultiLine, page3);
  describeStringParam(desc, "filePath", eStringTypeFilePath, page3);
  describeStringParam(desc, "dirPath", eStringTypeDirectoryPath, page3);
  describeStringParam(desc, "label", eStringTypeLabel, page3);

}

/** @brief The create instance function, the plugin must return an object derived from the \ref OFX::ImageEffect class */
ImageEffect* PropTesterPluginFactory::createInstance(OfxImageEffectHandle handle, ContextEnum context)
{
  if(context == eContextFilter) 
    return new FilterPlugin(handle);
  else if(context == eContextGenerator) 
    return new GeneratorPlugin(handle);
  else if(context == eContextGeneral) 
    return new GeneralPlugin(handle);


  // HACK!!! Throw something here!
  return NULL; // to shut the warning up
}



////////////////////////////////////////////////////////////////////////////////
/** @brief render for the generator */
void
GeneratorPlugin::render(const OFX::RenderArguments &args)
{
  OFX::Image *dst = 0;

  try {
    // get a dst image
    dst = dstClip_->fetchImage(args.time);

    // push some pixels
    // blah;
    // blah;
    // blah;
  }

  catch(...) {
    delete dst;
    throw;
  }

  // delete them
  delete dst;
}

////////////////////////////////////////////////////////////////////////////////
/** @brief render for the filter */
void
FilterPlugin::render(const OFX::RenderArguments &args)
{
  OFX::Image *src = 0, *dst = 0;

  try {
    // get a src image
    src = srcClip_->fetchImage(args.time);

    // get a dst image
    dst = dstClip_->fetchImage(args.time);

    // push some pixels
    // blah;
    // blah;
    // blah;
  }

  catch(...) {
    delete src;
    delete dst;
    throw;
  }

  // delete them
  delete src;
  delete dst;
}
