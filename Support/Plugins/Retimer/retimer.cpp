/*
  OFX retimer example plugin, a plugin that illustrates the use of the OFX Support library.

  This will not work very well on fielded imagery.

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

#include <math.h> // for floor
#include <float.h> // for FLT_MAX

#include <stdio.h>
#include "ofxsImageEffect.H"
#include "ofxsMultiThread.H"

#include "../include/ofxsProcessing.H"
#include "../include/ofxsImageBlender.H"

////////////////////////////////////////////////////////////////////////////////
/** @brief The plugin that does our work */
class RetimerPlugin : public OFX::ImageEffect {
protected :
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip *dstClip_;            /**< @brief Mandated output clips */
    OFX::Clip *srcClip_;            /**< @brief Mandated input clips */

    OFX::DoubleParam  *sourceTime_; /**< @brief mandated parameter, only used in the retimer context. */
    OFX::DoubleParam  *speed_;      /**< @brief only used in the filter context. */
    OFX::DoubleParam  *duration_;   /**< @brief how long the output should be as a proportion of input. General context only  */

public :
    /** @brief ctor */
    RetimerPlugin(OfxImageEffectHandle handle)
      : ImageEffect(handle)
      , dstClip_(0)
      , srcClip_(0)
      , sourceTime_(0)
      , duration_(0)
      , speed_(0)
    {
        dstClip_ = fetchClip("Output");
        srcClip_ = fetchClip("Source");

        // What parameters we instantiate depend on the context
        if(getContext() == OFX::eContextRetimer)
            // fetch the mandated parameter which the host uses to pass us the frame to retime to
            sourceTime_   = fetchDoubleParam("SourceTime");
        else // context == OFX::eContextFilter || context == OFX::eContextGeneral
            // filter context means we are in charge of how to retime, and our example is using a speed curve to do that
            speed_   = fetchDoubleParam("Speed");

        // fetch duration param for general context
        if(getContext() == OFX::eContextGeneral)
            duration_ = fetchDoubleParam("Duration");
    }

    /* Override the render */
    virtual void render(const OFX::RenderArguments &args);

    /* override the time domain action, only for the general context */
    virtual bool getTimeDomain(OfxRangeD &range);
    
    /* set up and run a processor */
    void
    setupAndProcess(OFX::ImageBlenderBase &, const OFX::RenderArguments &args);
};


////////////////////////////////////////////////////////////////////////////////
/** @brief render for the filter */

////////////////////////////////////////////////////////////////////////////////
// basic plugin render function, just a skelington to instantiate templates from

// make sure components are sane
static void
checkComponents(const OFX::Image &src,
                OFX::BitDepthEnum dstBitDepth,
                OFX::PixelComponentEnum dstComponents)
{
    OFX::BitDepthEnum      srcBitDepth     = src.getPixelDepth();
    OFX::PixelComponentEnum srcComponents  = src.getPixelComponents();
        
    // see if they have the same depths and bytes and all
    if(srcBitDepth != dstBitDepth || srcComponents != dstComponents)
        throw int(1); // HACK!! need to throw an sensible exception here!        
}

/* set up and run a processor */
void
RetimerPlugin::setupAndProcess(OFX::ImageBlenderBase &processor, const OFX::RenderArguments &args)
{
    // get a dst image
    std::auto_ptr<OFX::Image>  dst(dstClip_->fetchImage(args.time));
    OFX::BitDepthEnum          dstBitDepth    = dst->getPixelDepth();
    OFX::PixelComponentEnum    dstComponents  = dst->getPixelComponents();
  
    // figure the frame we should be retiming from
    double sourceTime;
    
    if(getContext() == OFX::eContextRetimer) {
        // the host is specifying it, so fetch it from the "sourceTime" pseudo-param
        sourceTime = sourceTime_->getValueAtTime(args.time);
    }
    else {
        // we have our own param, which is a speed, so we integrate it to get the time we want
        sourceTime = speed_->integrate(0, args.time);
    }

    // figure the two images we are blending between
    double fromTime, toTime;
    double blend;

    if(args.fieldToRender == OFX::eFieldNone) {
        // unfielded, easy peasy
        fromTime = floor(sourceTime);
        toTime = fromTime + 1;
        blend = sourceTime - fromTime;
    }
    else {
        // Fielded clips, pook. We are rendering field doubled images,
        // and so need to blend between fields, not frames.
        double frac = sourceTime - floor(sourceTime);
        if(frac < 0.5) {
            // need to go between the first and second fields of this frame
            fromTime = floor(sourceTime); // this will get the first field
            toTime   = fromTime + 0.5;    // this will get the second field of the same frame
            blend    = frac * 2.0;        // and the blend is between those two
        }
        else { // frac > 0.5
            fromTime = floor(sourceTime) + 0.5; // this will get the second field of this frame
            toTime   = floor(sourceTime) + 1.0; // this will get the first field of the next frame
            blend    = (frac - 0.5) * 2.0;
        }
    }

    // fetch the two source images
    std::auto_ptr<OFX::Image> fromImg(srcClip_->fetchImage(fromTime));
    std::auto_ptr<OFX::Image> toImg(srcClip_->fetchImage(toTime));

    // make sure bit depths are sane
    if(fromImg.get()) checkComponents(*fromImg, dstBitDepth, dstComponents);
    if(toImg.get()) checkComponents(*toImg, dstBitDepth, dstComponents);

    // set the images
    processor.setDstImg(dst.get());
    processor.setFromImg(fromImg.get());
    processor.setToImg(toImg.get());

    // set the render window
    processor.setRenderWindow(args.renderWindow);

    // set the blend between
    processor.setBlend(blend);

    // Call the base class process member, this will call the derived templated process code
    processor.process();
}

/* override the time domain action, only for the general context */
bool 
RetimerPlugin::getTimeDomain(OfxRangeD &range)
{
    // this should only be called in the general context, ever!
    if(getContext() == OFX::eContextGeneral) {
        // If we are a general context, we can changed the duration of the effect, so have a param to do that
        // We need a separate param as it is impossible to derive this from a speed param and the input clip
        // duration (the speed may be animating or wired to an expression).
        double duration = duration_->getValue(); //don't animate
        
        // how many frames on the input clip
        OfxRangeD srcRange = srcClip_->getFrameRange();
        
        range.min = 0;
        range.max = srcRange.max * duration;
        return true;
    }

    return false;
}

// the overridden render function
void
RetimerPlugin::render(const OFX::RenderArguments &args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum       dstBitDepth    = dstClip_->getPixelDepth();
    OFX::PixelComponentEnum dstComponents  = dstClip_->getPixelComponents();

    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA) {
        switch(dstBitDepth) {
        case OFX::eBitDepthUByte : {      
            OFX::ImageBlender<unsigned char, 4> fred(*this);
            setupAndProcess(fred, args);
        }
        break;

        case OFX::eBitDepthUShort : {
            OFX::ImageBlender<unsigned short, 4> fred(*this);
            setupAndProcess(fred, args);
        }                          
        break;

        case OFX::eBitDepthFloat : {
            OFX::ImageBlender<float, 4> fred(*this);
            setupAndProcess(fred, args);
        }
        break;
        }
    }
    else {
        switch(dstBitDepth) {
        case OFX::eBitDepthUByte : {
            OFX::ImageBlender<unsigned char, 1> fred(*this);
            setupAndProcess(fred, args);
        }
        break;

        case OFX::eBitDepthUShort : {
            OFX::ImageBlender<unsigned short, 1> fred(*this);
            setupAndProcess(fred, args);
        }                          
        break;

        case OFX::eBitDepthFloat : {
            OFX::ImageBlender<float, 1> fred(*this);
            setupAndProcess(fred, args);
        }                          
        break;
        }
    } // switch
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
            id.pluginIdentifier = "net.sf.openfx:Retimer";
            id.pluginVersionMajor = 1;
            id.pluginVersionMinor = 0;      
        }

        /** @brief empty load function */
        void loadAction(void)
        {
            // we can't be used on hosts that don't perfrom temporal clip access
            if(!gHostDescription->temporalClipAccess) {
                throw OFX::Exception::HostInadequate("Need random temporal image access to work");
            }
        }

        /** brief empty unload function */
        void unloadAction(void)    
        {
        }

        /** @brief The basic describe function, passed a plugin descriptor */
        void describe(OFX::ImageEffectDescriptor &desc) 
        {
            // basic labels
            desc.setLabels("Retimer", "Retimer", "Retimer");
            desc.setPluginGrouping("OFX");

            // Say we are a transition context
            desc.addSupportedContext(OFX::eContextRetimer);
            desc.addSupportedContext(OFX::eContextFilter);
            desc.addSupportedContext(OFX::eContextGeneral);

            // Add supported pixel depths
            desc.addSupportedBitDepth(eBitDepthUByte);
            desc.addSupportedBitDepth(eBitDepthUShort);
            desc.addSupportedBitDepth(eBitDepthFloat);

            // set a few flags
            desc.setSingleInstance(false);
            desc.setHostFrameThreading(false);
            desc.setSupportsMultiResolution(true);
            desc.setSupportsTiles(true);
            desc.setTemporalClipAccess(true); // say we will be doing random time access on clips
            desc.setRenderTwiceAlways(false);
            desc.setSupportsMultipleClipPARs(false);
        }        
        
        /** @brief The describe in context function, passed a plugin descriptor and a context */
        void describeInContext(OFX::ImageEffectDescriptor &desc, ContextEnum context) 
        {
            // we are a transition, so define the sourceTo input clip
            ClipDescriptor *toClip = desc.defineClip("Source");
            toClip->addSupportedComponent(ePixelComponentRGBA);
            toClip->addSupportedComponent(ePixelComponentAlpha);
            toClip->setTemporalClipAccess(true); // say we will be doing random time access on this clip
            toClip->setSupportsTiles(true);
            toClip->setFieldExtraction(eFieldExtractDoubled); // which is the default anyway

            // create the mandated output clip
            ClipDescriptor *dstClip = desc.defineClip("Output");
            dstClip->addSupportedComponent(ePixelComponentRGBA);
            dstClip->addSupportedComponent(ePixelComponentAlpha);
            toClip->setFieldExtraction(eFieldExtractDoubled); // which is the default anyway
            dstClip->setSupportsTiles(true);

            // what param we have is dependant on the host
            if(context == OFX::eContextRetimer) {
                // Define the mandated "SourceTime" param, note that we don't do anything with this other than.
                // describe it. It is not a true param but how the host indicates to the plug-in which frame
                // it wants you to retime to. It appears on no plug-in side UI, it is purely the host's to manage.
                DoubleParamDescriptor *param = desc.defineDoubleParam("SourceTime");
            }
            else {
                // We are a general or filter context, define a speed param and a page of controls to put that in
                DoubleParamDescriptor *param = desc.defineDoubleParam("Speed");
                param->setLabels("speed", "speed", "speed");
                param->setScriptName("speed");
                param->setHint("How much to changed the speed of the input clip");
                param->setDefault(1);
                param->setRange(-FLT_MAX, FLT_MAX);
                param->setIncrement(0.05);
                param->setDislayRange(0, 1);
                param->setAnimates(true); // can animate
                param->setDoubleType(eDoubleTypeScale);

                // make a page to put it in
                PageParamDescriptor *page = desc.definePageParam("Controls");

                // add our speed param into it
                page->addChild(*param);

                // If we are a general context, we can changed the duration of the effect, so have a param to do that
                // We need a separate param as it is impossible to derive this from a speed param and the input clip
                // duration (the speed may be animating or wired to an expression).
                if(context == OFX::eContextGeneral) {
                    // We are a general or filter context, define a speed param and a page of controls to put that in
                    DoubleParamDescriptor *param = desc.defineDoubleParam("Duraction");
                    param->setLabels("duration", "duraction", "duration");
                    param->setScriptName("duration");
                    param->setHint("How long the output clip should be, as a proportion of the input clip's length.");
                    param->setDefault(1);
                    param->setRange(0, 10);
                    param->setIncrement(0.1);
                    param->setDislayRange(0, 10);
                    param->setAnimates(false); // no animation here!
                    param->setDoubleType(eDoubleTypeScale);

                    // add param to page
                    page->addChild(*param);
                }
            }
        }

        /** @brief The create instance function, the plugin must return an object derived from the \ref OFX::ImageEffect class */
        ImageEffect *createInstance(OfxImageEffectHandle handle, ContextEnum context)
        {
            return new RetimerPlugin(handle);
        }
    };
};
