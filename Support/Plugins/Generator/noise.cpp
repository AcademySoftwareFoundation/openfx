/*
  OFX Genereator example plugin, a plugin that illustrates the use of the OFX Support library.

  Copyright (C) 2004-2005 The Foundry Visionmongers Ltd
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
#include "ofxsMultiThread.H"

#include "../include/ofxsProcessing.H"

#include "randomGenerator.H"


////////////////////////////////////////////////////////////////////////////////
// base class for the noise

/** @brief  Base class used to blend two images together */
class NoiseGeneratorBase : public OFX::ImageProcessor {
protected :
    float       _noiseLevel;   // how much to blend
    uint32_t    _seed;    // base seed
public :
    /** @brief no arg ctor */
    NoiseGeneratorBase(OFX::ImageEffect &instance)
      : OFX::ImageProcessor(instance)
      , _noiseLevel(0.5f)
      , _seed(0)
    {        
    }

    /** @brief set the scale */
    void setNoiseLevel(float v) {_noiseLevel = v;}    

    /** @brief the seed to use */
    void setSeed(uint32_t v) {_seed = v;}
};

/** @brief templated class to blend between two images */
template <class PIX, int nComponents, int max>
class NoiseGenerator : public NoiseGeneratorBase {
public :
    // ctor
    NoiseGenerator(OFX::ImageEffect &instance) 
      : NoiseGeneratorBase(instance)
    {}

    // and do some processing
    void multiThreadProcessImages(OfxRectI procWindow)
    {
        float noiseLevel = _noiseLevel;
        
        // set up a random number generator and set the seed
        RandomGenerator randy(_seed + procWindow.y1);
        
        // push pixels
        for(int y = procWindow.y1; y < procWindow.y2; y++) {
            if(_effect.abort()) break;

            PIX *dstPix = (PIX *) _dstImg->getPixelAddress(procWindow.x1, y);

            for(int x = procWindow.x1; x < procWindow.x2; x++) {
                for(int c = 0; c < nComponents; c++) {
                    // get the random value out of it, scale up by the pixel max level and the noise level
                    double randValue = max * noiseLevel * randy.random();

                    if(max == 1) // implies floating point, so don't clamp
                        dstPix[c] = PIX(randValue);
                    else {  // integer base one, clamp it
                        dstPix[c] = randValue < 0 ? 0 : (randValue > max ? max : PIX(randValue));
                    }
                }
                dstPix += nComponents;
            }
        }
    }
    
};

////////////////////////////////////////////////////////////////////////////////
/** @brief The plugin that does our work */
class NoisePlugin : public OFX::ImageEffect {
protected :
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip *dstClip_;

    OFX::DoubleParam  *noise_;

public :
    /** @brief ctor */
    NoisePlugin(OfxImageEffectHandle handle)
      : ImageEffect(handle)
      , dstClip_(0)
      , noise_(0)
    {
        dstClip_ = fetchClip("Output");
        noise_   = fetchDoubleParam("Noise");
    }

    /* Override the render */
    virtual void render(const OFX::RenderArguments &args);

    /* Override the clip preferences, we need to say we are setting the frame varying flag */
    virtual void getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences);

    /* set up and run a processor */
    void setupAndProcess(NoiseGeneratorBase &, const OFX::RenderArguments &args);

    /** @brief The get RoD action.  We flag an infinite rod */
    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod);
};


////////////////////////////////////////////////////////////////////////////////
/** @brief render for the filter */

////////////////////////////////////////////////////////////////////////////////
// basic plugin render function, just a skelington to instantiate templates from


/* set up and run a processor */
void
NoisePlugin::setupAndProcess(NoiseGeneratorBase &processor, const OFX::RenderArguments &args)
{
    // get a dst image
    std::auto_ptr<OFX::Image>  dst(dstClip_->fetchImage(args.time));
    OFX::BitDepthEnum         dstBitDepth    = dst->getPixelDepth();
    OFX::PixelComponentEnum   dstComponents  = dst->getPixelComponents();
  
    // set the images
    processor.setDstImg(dst.get());

    // set the render window
    processor.setRenderWindow(args.renderWindow);

    // set the scales
    processor.setNoiseLevel(noise_->getValueAtTime(args.time));

    // set the seed based on the current time, and double it we get difference seeds on different fields
    processor.setSeed(uint32_t(args.time * 2.0f));

    // Call the base class process member, this will call the derived templated process code
    processor.process();
}

/* Override the clip preferences, we need to say we are setting the frame varying flag */
void 
NoisePlugin::getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences)
{
    clipPreferences.setOutputFrameVarying(true);
}

/** @brief The get RoD action.  We flag an infinite rod */
bool 
NoisePlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args, OfxRectD &rod)
{
    // we can generate noise anywhere on the image plan, so set our RoD to be infinite
    rod.x1 = rod.y1 = kOfxFlagInfiniteMin;
    rod.x2 = rod.y2 = kOfxFlagInfiniteMax;
    return true;
}

// the overridden render function
void
NoisePlugin::render(const OFX::RenderArguments &args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum       dstBitDepth    = dstClip_->getPixelDepth();
    OFX::PixelComponentEnum dstComponents  = dstClip_->getPixelComponents();

    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA) {
        switch(dstBitDepth) {
        case OFX::eBitDepthUByte : {      
            NoiseGenerator<unsigned char, 4, 255> fred(*this);
            setupAndProcess(fred, args);
        }
        break;

        case OFX::eBitDepthUShort : {
            NoiseGenerator<unsigned short, 4, 65535> fred(*this);
            setupAndProcess(fred, args);
        }                          
        break;

        case OFX::eBitDepthFloat : {
            NoiseGenerator<float, 4, 1> fred(*this);
            setupAndProcess(fred, args);
        }
        break;
        }
    }
    else {
        switch(dstBitDepth) {
        case OFX::eBitDepthUByte : {
            NoiseGenerator<unsigned char, 1, 255> fred(*this);
            setupAndProcess(fred, args);
        }
        break;

        case OFX::eBitDepthUShort : {
            NoiseGenerator<unsigned short, 1, 65536> fred(*this);
            setupAndProcess(fred, args);
        }                          
        break;

        case OFX::eBitDepthFloat : {
            NoiseGenerator<float, 1, 1> fred(*this);
            setupAndProcess(fred, args);
        }                          
        break;
        }
    } 
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
            id.pluginIdentifier = "net.sf.openfx:noisePlugin";
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
            desc.setLabels("Noise", "Noise", "Noise");
            desc.setPluginGrouping("OFX");

            // add the supported contexts, only filter at the moment
            desc.addSupportedContext(eContextGenerator);

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

            // we can render both fields simultaneously if there is no animation, flag this by setting setRenderTwiceAlways to be false
            desc.setRenderTwiceAlways(false);
        }        
        
        /** @brief The describe in context function, passed a plugin descriptor and a context */
        void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context) 
        {
            // create the mandated output clip
            ClipDescriptor *dstClip = desc.defineClip("Output");
            dstClip->addSupportedComponent(ePixelComponentRGBA);
            dstClip->addSupportedComponent(ePixelComponentAlpha);
            dstClip->setSupportsTiles(true);

            // we only fetch single fields from fielded imagery
            dstClip->setFieldExtraction(eFieldExtractSingle);
            
            // We are a general or filter context, define a speed param and a page of controls to put that in
            DoubleParamDescriptor *param = desc.defineDoubleParam("Noise");
            param->setLabels("noise", "noise", "noise");
            param->setScriptName("noise");
            param->setHint("How much noise to make.");
            param->setDefault(0.2);
            param->setRange(0, 10);
            param->setIncrement(0.1);
            param->setDislayRange(0, 1);
            param->setAnimates(true); // can animate
            param->setDoubleType(eDoubleTypeScale);

            
            // make a page to put it in
            PageParamDescriptor *page = desc.definePageParam("Controls");

            // add our speed param into it
            page->addChild(*param);
        }

        /** @brief The create instance function, the plugin must return an object derived from the \ref OFX::ImageEffect class */
        ImageEffect *createInstance(OfxImageEffectHandle handle, ContextEnum context)
        {
            return new NoisePlugin(handle);
        }
    };
};
