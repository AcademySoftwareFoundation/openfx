/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  The Foundry Visionmongers Ltd
  35-36 Gt Marlborough St 
  London W1V 7FN
  England
*/

#include <stdio.h>
#include "ofxsImageEffect.H"

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


/** @brief OFX namespace */
namespace OFX {

  /** @brief plugin namespace */
  namespace Plugin {
    
    /** @brief identify the plug-in */
    void getPluginID(OFX::PluginID &id)
    {
      id.pluginIdentifier = "co.uk.thefoundry:propertyTester";
      id.pluginVersionMajor = 1;
      id.pluginVersionMinor = 0;      
    }

    /** @brief empty load function */
    void loadAction(void)
    {
      // do anything here that you need to do globally
      fprintf(stderr, "load called!\n");
    }

    /** brief empty unload function */
    void unloadAction(void)    {
      // do anything here that you need to undo globally
      fprintf(stderr, "unload called!\n");
    }

    /** @brief The basic describe function, passed a plugin descriptor */
    void describe(OFX::ImageEffectDescriptor &desc) 
    {
      // basic labels
      desc.setLabels("Prop Tester", "Prop Tester", "Property Tester");
      desc.setPluginGrouping("OFX Test");

      // add the supported contexts, only filter at the moment
      desc.addSupportedContext(eContextGenerator);
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

    /** @brief The describe in context function, passed a plugin descriptor and a context */
    void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context) 
    {
      // Source clip only in the filter context
      if(context == eContextFilter) {
	// create the mandated source clip
	ClipDescriptor *srcClip = desc.defineClip("Source");
	srcClip->addSupportedComponent(ePixelComponentRGBA);
	srcClip->setTemporalClipAccess(false);
	srcClip->setOptional(false);
	srcClip->setSupportsTiles(true);
	srcClip->setIsMask(false);
      }

      // create the mandated output clip
      ClipDescriptor *dstClip = desc.defineClip("Output");
      dstClip->addSupportedComponent(ePixelComponentRGBA);
      dstClip->setTemporalClipAccess(false);
      dstClip->setOptional(false);
      dstClip->setSupportsTiles(true);
      dstClip->setIsMask(false);

      // make an int param
      IntParamDescriptor *iParam = desc.defineIntParam("IntParam");
      iParam->setLabels("Int Param", "Int Param", "Integer Param");
      iParam->setScriptName("intParam");
      iParam->setHint("An integer parameter");
      iParam->setDefault(0);
      iParam->setRange(-100, 100);
      iParam->setDislayRange(-100, 100);
    }

    /** @brief The create instance function, the plugin must return an object derived from the \ref OFX::ImageEffect class */
    ImageEffect *createInstance(OfxImageEffectHandle handle, ContextEnum context)
    {
      if(context == eContextFilter) 
	return new FilterPlugin(handle);
      else if(context == eContextGenerator) 
	return new GeneratorPlugin(handle);
    }
  };
};


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
