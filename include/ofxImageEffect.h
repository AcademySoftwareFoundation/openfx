#ifndef _ofxImageEffect_h_
#define _ofxImageEffect_h_

/*
Software License :

Copyright (c) 2003-2004, The Foundry Visionmongers Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Foundry Visionmongers Ltd, nor the names of its 
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
*/

#include "ofxCore.h"
#include "ofxParam.h"
#include "ofxInteract.h"
#include "ofxMessage.h"
#include "ofxMemory.h"
#include "ofxMultiThread.h"
#include "ofxInteract.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @brief String used to label OFX Image Effect Plug-ins

    Set the pluginApi member of the OfxPluginHeader inside any OfxImageEffectPluginStruct
    to be this so that the host knows the plugin is an image effect.    
 */
#define kOfxImageEffectPluginApi "OfxImageEffectPluginAPI"

/** @brief The current version of the Image Effect API
 */
#define kOfxImageEffectPluginApiVersion 1

/** @brief Blind declaration of an OFX image effect
*/
typedef struct OfxImageEffectStruct *OfxImageEffectHandle;

/** @brief Blind declaration of an OFX image effect
*/
typedef struct OfxImageClipStruct *OfxImageClipHandle;

/** @brief Blind declaration for an handle to image memory returned by the image memory management routines */
typedef struct OfxImageMemoryStruct *OfxImageMemoryHandle;

/** @brief String to label something with unset components */
#define kOfxImageComponentNone "OfxImageComponentNone"

/** @brief String to label images with RGBA components */
#define kOfxImageComponentRGBA "OfxImageComponentRGBA"

/** @brief String to label images with only Alpha components */
#define kOfxImageComponentAlpha "OfxImageComponentAlpha"

/** @brief String to label images with YUVA components */
#define kOfxImageComponentYUVA "OfxImageComponentYUVA"

/** @brief Use to define the generator image effect context. See \ref ImageEffectContexts
 */
#define kOfxImageEffectContextGenerator "OfxImageEffectContextGenerator"

/** @brief Use to define the filter effect image effect context See \ref ImageEffectContexts */
#define kOfxImageEffectContextFilter "OfxImageEffectContextFilter"

/** @brief Use to define the transition image effect context See \ref ImageEffectContexts */
#define kOfxImageEffectContextTransition "OfxImageEffectContextTransition"

/** @brief Use to define the paint image effect context  See \ref ImageEffectContexts */
#define kOfxImageEffectContextPaint "OfxImageEffectContextPaint"

/** @brief Use to define the general image effect context  See \ref ImageEffectContexts */
#define kOfxImageEffectContextGeneral "OfxImageEffectContextGeneral"

/** @brief Use to define the retimer effect context  See \ref ImageEffectContexts */
#define kOfxImageEffectContextRetimer "OfxImageEffectContextRetimer"

/** @brief Used as a value for ::kOfxPropType on image effect host handles */
#define kOfxTypeImageEffectHost "OfxTypeImageEffectHost"

/** @brief Used as a value for ::kOfxPropType on image effect plugin handles */
#define kOfxTypeImageEffect "OfxTypeImageEffect"

/** @brief Used as a value for ::kOfxPropType on image effect instance handles  */
#define kOfxTypeImageEffectInstance "OfxTypeImageEffectInstance"

/** @brief Used as a value for ::kOfxPropType on image effect clips */
#define kOfxTypeClip "OfxTypeClip"

/** @brief Used as a value for ::kOfxPropType on image effect images */
#define kOfxTypeImage "OfxTypeImage"

/**
   \addtogroup ActionsAll
*/
/*@{*/
/**
   \defgroup ImageEffectActions Image Effect Actions

These are the list of actions passed to an image effect plugin's main function. For more details on how to deal with actions, see \ref ImageEffectActions.
*/
/*@{*/

/** @brief Action called to get the region of definition of an effect */
#define kOfxImageEffectActionGetRegionOfDefinition        "OfxImageEffectActionGetRegionOfDefinition"

/** @brief Action called to get the regions of interest of an effect */
#define kOfxImageEffectActionGetRegionsOfInterest         "OfxImageEffectActionGetRegionsOfInterest"

/** @brief Action called to get the temporal domain of an effect */
#define kOfxImageEffectActionGetTimeDomain                "OfxImageEffectActionGetTimeDomain"

/** @brief Action called to get the frame ranged needed on an input clip to render a single frame */
#define kOfxImageEffectActionGetFramesNeeded              "OfxImageEffectActionGetFramesNeeded"

/** @brief Action called to have a plug-in define input an output clip preferences */
#define kOfxImageEffectActionGetClipPreferences       "OfxImageEffectActionGetClipPreferences"

/** @brief Action called to have a plug-in say if it is an identity transform */
#define kOfxImageEffectActionIsIdentity            "OfxImageEffectActionIsIdentity"

/** @brief Action to render an image */
#define kOfxImageEffectActionRender                "OfxImageEffectActionRender"

/** @brief Action indicating a series of render calls are about to happen */
#define kOfxImageEffectActionBeginSequenceRender   "OfxImageEffectActionBeginSequenceRender"

/** @brief Action to render a series of render calls have completed */
#define kOfxImageEffectActionEndSequenceRender      "OfxImageEffectActionEndSequenceRender"

/** @brief Action to describe an image effect in a specific context */
#define kOfxImageEffectActionDescribeInContext     "OfxImageEffectActionDescribeInContext"

/*@}*/
/*@}*/

/**
   \addtogroup PropertiesAll
*/
/*@{*/
/**
   \defgroup ImageEffectPropDefines Image Effect Property Definitions 

These are the list of properties used by the Image Effects API.
*/
/*@{*/
/** @brief Plugin property, sets the context a plugin can be used in
    - char * X N
    - must be one of kOfxImageEffectContex*
    - no default, it must be set in the define function
*/
#define kOfxImageEffectPropSupportedContexts "OfxImageEffectPropSupportedContexts"

/** @brief Instance property, the plugin handle passed to the initial 'describe' action. This value will be the same for all instances of a plugin.
    - pointer X 1
*/
#define kOfxImageEffectPropPluginHandle "OfxImageEffectPropPluginHandle"


/** @brief Plugin property, says whether more than one instance of a plugin can exist at the same time
- int X 1
- default 0

This must be either
- 0 - which means multiple instances can exist simultaneously,
- 1 - which means only one instance can exist at any one time.

  Some plugins, for whatever reason, may only be able to have a single instance in existance at any one time. This plugin property is used to indicate that.
*/
#define kOfxImageEffectPluginPropSingleInstance "OfxImageEffectPluginPropSingleInstance"

/** @brief Plugin property, says how many simultaneous renders the plugin can deal with
- string X 1
- default kOfxImageEffectRenderInstanceSafe

This must be either
- kOfxImageEffectRenderUnsafe - indicating that only a single 'render' call can be made at any time amoung all instances,
- kOfxImageEffectRenderInstanceSafe - indicating that any instance can have a single 'render' call at any one time,
- kOfxImageEffectRenderFullySafe - indicating that any instance of a plugin can have multiple renders running simultaneously
*/
#define kOfxImageEffectPluginRenderThreadSafety "OfxImageEffectPluginRenderThreadSafety"

/** @brief String used to label render threads as un thread safe, see, \ref ::kOfxImageEffectPluginRenderThreadSafety */
#define kOfxImageEffectRenderUnsafe "OfxImageEffectRenderUnsafe"
/** @brief String used to label render threads as instance thread safe, \ref ::kOfxImageEffectPluginRenderThreadSafety */
#define kOfxImageEffectRenderInstanceSafe "OfxImageEffectRenderInstanceSafe"
/** @brief String used to label render threads as fully thread safe, \ref ::kOfxImageEffectPluginRenderThreadSafety */
#define kOfxImageEffectRenderFullySafe "OfxImageEffectRenderFullySafe"

/** @brief Plugin property, says whether an effect lets the host perform per frame SMP threading

    - int X 1
    - default 1

    This must be either
     - 0 - which means that the plugin will perform any per frame SMP threading
     - 1 - which means the host can call an instance's render function simultaneously at the same frame, but with
           different windows to render.
*/
#define kOfxImageEffectPluginPropHostFrameThreading "OfxImageEffectPluginPropHostFrameThreading"

/** @brief Host and plugin, property, indicates whether a host or plugin can support clips of differing component depths going into/out of an effect

    - int X 1

This must be either
     - 0 - in which case the host or plugin does not support clips of multiple pixel depths,
     - 1 - which means a host or plugin is able to to deal with clips of multiple pixel depths,

If a host indicates that it can support multiple pixels depths, then it will allow the plugin to explicitly set 
the output clip's pixel depth in the ::kOfxImageEffectActionGetClipPreferences action. See \ref ImageEffectClipPreferences.

Defaults to 0 for a plugin.
*/
#define kOfxImageEffectPropSupportsMultipleClipDepths "OfxImageEffectPropMultipleClipDepths"

/** @brief Host and plugin, property, indicates whether a host or plugin can support clips of differing pixel aspect ratios going into/out of an effect

    - int X 1

This must be either
     - 0 - in which case the host or plugin does not support clips of multiple pixel aspect ratios
     - 1 - which means a host or plugin is able to to deal with clips of multiple pixel aspect ratios

If a host indicates that it can support multiple  pixel aspect ratios, then it will allow the plugin to explicitly set 
the output clip's aspect ratio in the ::kOfxImageEffectActionGetClipPreferences action. See \ref ImageEffectClipPreferences.

Defaults to 0 for a plugin.
*/
#define kOfxImageEffectPropSupportsMultipleClipPARs "OfxImageEffectPropSupportsMultipleClipPARs"


/** @brief Image effect property, the set of parameters on which a value change will trigger a change to clip preferences

    - string X N

    The plugin uses this to inform the host of the subset of parameters that affect the effect's clip preferences. A value
change in any one of these will trigger a call to the clip preferences action.

    The plugin can be slaved to multiple parameters (setting index 0, then index 1 etc...)
 */
#define kOfxImageEffectPropClipPreferencesSlaveParam "OfxImageEffectPropClipPreferencesSlaveParam"

/** Host property, indicates whether a plugin can set the frame rate of the output clip.

    - int X 1

This must be either
     - 0 - in which case the plugin may not change the frame rate of the output clip,
     - 1 - which means a plugin is able to change the output clip's frame rate in the ::kOfxImageEffectActionGetClipPreferences action.

See \ref ImageEffectClipPreferences.

If a clip can be continously sampled, the frame rate will be set to 0.
*/
#define kOfxImageEffectPropSetableFrameRate "OfxImageEffectPropSetableFrameRate"

/** @brief Host property, indicates whether a plugin can set the fielding of the output clip.

    - int X 1

This must be either
     - 0 - in which case the plugin may not change the fielding of the output clip,
     - 1 - which means a plugin is able to change the output clip's fielding in the ::kOfxImageEffectActionGetClipPreferences action.

See \ref ImageEffectClipPreferences.
*/
#define kOfxImageEffectPropSetableFielding "OfxImageEffectPropSetableFielding"

/** @brief Plugin property, says whether an effect needs to be renderred sequentially or not

    - int X 1
    - default 0
    This must be either
     - 0 - which means the host can render arbitrary frames of an instance over any number of CPUs and computers
     - 1 - which means the host must render all frames of an instance sequentially on the same computer

    If a host caches output, any frame renderred out of sequence in such an effect must be considerred invalid and cause a re-render
    of the sequence to correct that frame. This has implications for interactive sessions.
*/
#define kOfxImageEffectPluginPropSequentialRender "OfxImageEffectPluginPropSequentialRender"

/** @brief Plugin property, gets/sets the effect group for this plugin, this is purely a user interface issue for the host and might be ignored

    - char * X 1
    - UTF8 string
    - defaults to null and the plugin is ungrouped
*/
#define kOfxImageEffectPluginPropGrouping "OfxImageEffectPluginPropGrouping"

/** @brief ImageEffect Host Property, does the host support image effect \ref ImageEffectOverlays.

    - int X 1
    - 0 or 1
*/
#define kOfxImageEffectPropSupportsOverlays "OfxImageEffectPropSupportsOverlays"

/** @brief ImageEffect Plugin Property, sets the entry for an effect's overlay interaction

    - pointer X 1,
    - must point to an ::OfxPluginEntryPoint that handles the overlay interaction,
    - defaults to NULL for a plugin,

  See see \ref CustomInteractionPage and \ref ImageEffectOverlays.
*/
#define kOfxImageEffectPluginPropOverlayInteractV1 "OfxImageEffectPluginPropOverlayInteractV1"

/** @brief ImageEffect Plugin and Host Property, does the plugin or host support multiple resolution images  and images
with the origin not at 0,0

    - int X 1
    - 0 or 1
    - defaults to 1 for a plugin

    Multiple resolution images mean...
       - input and output images can be of any size
       - input and output images can be offset from the origin
*/
#define kOfxImageEffectPropSupportsMultiResolution "OfxImageEffectPropSupportsMultiResolution"

/** @brief ImageEffect Clip, Plugin  Host Property, does a clip, plugin or host support tiled images

    - int X 1
    - 0 or 1
    - defaults to 1 for a plugin

    tiled resolution images mean...
       - input and output images can contain pixel data that is only a subset of their full RoD
       - defaults to 1 for clips

    If a clip or plugin does not support tiled images, then the host should supply full RoD images to the effect whenever it fetches one.
*/
#define kOfxImageEffectPropSupportsTiles "OfxImageEffectPropSupportsTiles"

/** @brief ImageEffect instance property, indicating that an effect is performing an analysis pass

    - int X 1
    - 0 or 1
    - defaults to 0
*/
#define kOfxImageEffectPropInAnalysis "OfxImageEffectPropInAnalysis"

/** @brief ImageEffect Host, Plugin and clip Property, used to indicate temporal access support

    - int X 1
    - 0 or 1
    - defaults to 0 for plugins and clips

    - on a host, it indicates whether the host supports temporal access to images
    - on a plugin, indicates if the plugin needs temporal access to images
    - on a clip, it indicates that the clip needs temporal access to images
*/
#define kOfxImageEffectPropTemporalClipAccess "OfxImageEffectPropTemporalClipAccess"

/** @brief ImageEffect Instance Property used to enquire the context of the current instance
    - char * X 1
    It will be one of the kOfxImageEffectContext* # defines
 */
#define kOfxImageEffectPropContext "OfxImageEffectPropContext"

/** @brief ImageEffect clip and image property to indicate the current bit depth (after any mapping)
       - char * X 1, 

This is host set and will be one of...
     - kOfxBitDepthNone (implying a clip is unconnected)
     - kOfxBitDepthByte
     - kOfxBitDepthShort
     - kOfxBitDepthFloat
*/
#define kOfxImageEffectPropPixelDepth "OfxImageEffectPropPixelDepth"

/** @brief ImageEffect clip and image property to indicate the current component type (after any mapping)

       - char * X 1, 

    This is set by the host to one of the kOfxImageComponent* constants
      
   This is host set and will be one of...
     - kOfxImageComponentNone (implying a clip is unconnected)
     - kOfxImageComponentRGBA
     - kOfxImageComponentAlpha
*/
#define kOfxImageEffectPropComponents "OfxImageEffectPropComponents"

/** @brief ImageEffect image property which uniquely labels the image from all other images in the host appliction

       - char * X 1 

This is host set and allows a plug-in to differentiate between images. This is especially
useful if a plugin caches analysed information about the image (for example motion vectors). The plugin can label the
cached information with this identifier. If a user connects a different clip to the analysed input then the plugin can
detect this via an identifier change and re-evaluate the cached information.
*/
#define kOfxImagePropUniqueIdentifier "OfxImagePropUniqueIdentifier"

/** @brief ImageEffect clip and action argument property which indicates that the clip can be sampled continously

       - int * X 1 interpretted as a boolean

This is host set and will be one of...
     - 0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
     - 1 if the images can only be sampled continuously (eg: the clip is infact a roto spline).

If this is set to true, then the frame rate of the clip is effectively infinite, so to stop arithmetic
errors, it will be set to 0.

see \ref ImageEffectsActionGetClipPreferences
*/
#define kOfxImageClipPropContinuousSamples "OfxImageClipPropContinuousSamples"

/** @brief  ImageEffect clip property which indicates the current bit depth (before any mapping)
       - char * X 1, 

This is host set and will be one of...
     - kOfxBitDepthNone (implying a clip is unconnected)
     - kOfxBitDepthByte
     - kOfxBitDepthShort
     - kOfxBitDepthFloat
*/
#define kOfxImageClipPropUnmappedPixelDepth "OfxImageClipPropUnmappedPixelDepth"

/** ImageEffect clip property which indicates the current component type (before any host mapping)
       - char * X 1, 
    This is set by the host to one of the kOfxImageComponent* constants
      
   This is host set and will be one of...
     - kOfxImageComponentNone (implying a clip is unconnected)
     - kOfxImageComponentRGBA
     - kOfxImageComponentAlpha
*/
#define kOfxImageClipPropUnmappedComponents "OfxImageClipPropUnmappedComponents"

/** ImageEffect clip and image property to indicate their premultiplication state
- char * X 1,       

This is host set and will be one of...
- kOfxImageOpaque          - the image is opaque and so has no premultiplication state
- kOfxImagePreMultiplied   - the image is premultiplied by it's alpha
- kOfxImageUnPreMultiplied - the image is unpremultiplied

Also used as a setable \e outargs property in \ref ImageEffectsActionGetClipPreferences.
*/
#define kOfxImageEffectPropPreMultiplication "OfxImageEffectPropPreMultiplication"

/** Used to flag the alpha of an image as opaque */
#define kOfxImageOpaque  "OfxImageOpaque"

/** Used to flag an image as premultiplied */
#define kOfxImagePreMultiplied "OfxImageAlphaPremultiplied"

/** Used to flag an image as unpremultiplied */
#define kOfxImageUnPreMultiplied "OfxImageAlphaUnPremultiplied"


/** ImageEffect host and plugin property used to indicate bit depth support
    
    These must be one of the kOfxBitDepth* constants
    
    - for a plugin
       - char * X N, one for each depth supported
       - default for a plugin is to have none set, the plugin \em must define at least one in it's describe action
       
    - for a host property, this indicates what bit depths are supported by the host
       - char * X N, one for each depth supported
*/
#define kOfxImageEffectPropSupportedPixelDepths "OfxImageEffectPropSupportedPixelDepths"

/** ImageEffect host and clip property indicating the components supported,

    These must be one of the  kOfxImageComponent* constants
    
    - for a host property, this indicates what component types are supported by the host
       - char * X N, one for each component type supported
  
    - for a clip property, this indicates which component types are supported on that input
       - char * X N, one for each component type supported
       - default for a clip is to have none set, the plugin _must_ define at least one in it's define function
*/
#define kOfxImageEffectPropSupportedComponents "OfxImageEffectPropSupportedComponents"

/** ImageEffect clip property indicating if a clip is optional in the context it is declared in

    - int X 1
    - 0 or 1
    - defaults to false
*/
#define kOfxImageClipPropOptional "OfxImageClipPropOptional"

/** ImageEffect clip property indicating that a clip is intended to be a secondary 'mask' input which will only ever have a
single alpha channel.

    - int X 1
    - 0 or 1
    - defaults to false
    - This property act as hint to hosts, so they can use an appropriate interface for this input knowing that it will be
user as a grey scale mask. For example, it could have a rotospline tool attached in some nice manner.
*/
#define kOfxImageClipPropIsMask "OfxImageClipPropIsMask"


/** ImageEffect clip and image property, returns the pixel aspect ratio of the indicated input clip

    - double X 1
    - plugin read only as a clip and image property
    - used as a setable \e outargs property in \ref ImageEffectsActionGetClipPreferences.
*/
#define kOfxImagePropPixelAspectRatio "OfxImagePropPixelAspectRatio"

/** ImageEffect instance and clip property, returns the frame rate (frames/second)

    - double X 1
    - for an input clip this is the frame rate of the clip, plugin read only
    - for an instance, this is the frame rate of the project, plugin read only
    - used as a setable \e outargs property in \ref ImageEffectsActionGetClipPreferences.
*/
#define kOfxImageEffectPropFrameRate "OfxImageEffectPropFrameRate"

/** ImageEffect clip property, returns the original unmapped frame rate (frames/second) of a clip

    - double X 1
    - if a plugin changes the output frame rate in the pixel preferences action, this property allows a plugin
      to get to the original value.
*/
#define kOfxImageEffectPropUnmappedFrameRate "OfxImageEffectPropUnmappedFrameRate"

/** ImageEffect argument, returns the frame step for a sequence of renders

    - double X 1
    - generally 1 for frame based material, 0.5 for field based material
*/
#define kOfxImageEffectPropFrameStep "OfxImageEffectPropFrameStep"

/** ImageEffect clip property, returns the frame range over which a clip has images.

    - double X 2
      - dimension 0 is the first frame for which the image can produce valid data
      - dimension 1 is the last frame for which the image can produce valid data
    - plugin read only
*/
#define kOfxImageEffectPropFrameRange "OfxImageEffectPropFrameRange"


/** ImageEffect clip property, returns the original unmapped frame range over which a clip has images.

    - double X 2
      - dimension 0 is the first frame for which the image can produce valid data
      - dimension 1 is the last frame for which the image can produce valid data
    - plugin read only
    - if a plugin changes the output frame rate in the pixel preferences action, it will affect the frame range
      of the output clip, this property allows a plugin to get to the original value.
*/
#define kOfxImageEffectPropUnmappedFrameRange "OfxImageEffectPropUnmappedFrameRange"

/** ImageEffect clip property, Returns whether the clip input is actually connected at the moment
    - int X 1
    - 0 or 1
 */
#define kOfxImageClipPropConnected "OfxImageClipPropConnected"

/** ImageEffect action argument property, indicates whether the output image will vary from frame to frame, even if no parameters or
input image changes over that time.

   - int X 1
   - 0 or 1
   - defaults to 0

This property indicates whether a plugin will generate a different image from frame to frame, even if no parameters
or input image changes. For example a generater that creates random noise pixel at each frame.

This property is set by a plugin and can be changed in value changed actions. See \ref ImageEffectsActionGetClipPreferences and \ref ImageEffectClipPreferencesFrameVarying.

 */
#define kOfxImageEffectFrameVarying "OfxImageEffectFrameVarying"

/** ImageEffect argument property, gets the render scale to apply to spatial parameters, the 'x' value does not include any pixel aspect ratios.
    - double X 2
 */
#define kOfxImageEffectPropRenderScale "OfxImageEffectPropRenderScale"

/** ImageEffect Instance property, gets the extent of the project, see \ref CoordinateSystemsProjects for more on project extents.
    - double X 2

The extent is the section of the image plane that is coverred by the output image. So for a PAL D1 project you get an extent of 0,0 <-> 768,576. 

The extent is in \ref CannonicalCoordinates and only returns the top right position, as the extent is always rooted at 0,0.
 */
#define kOfxImageEffectPropProjectExtent "OfxImageEffectPropProjectExtent"

/** ImageEffect Instance property, gets the current size of the project, see \ref CoordinateSystemsProjects for more on project sizes.
    - double X 2

The size is in \ref CannonicalCoordinates.
 */
#define kOfxImageEffectPropProjectSize "OfxImageEffectPropProjectSize"

/** ImageEffect Instance property, gets the current project offset, see \ref CoordinateSystemsProjects for more on project offsets.
    - double X 2

The offset is in \ref CannonicalCoordinates.
 */
#define kOfxImageEffectPropProjectOffset "OfxImageEffectPropProjectOffset"


/** ImageEffect Instance property, gets the pixel aspect ratio of the project the instance is inside
    - double X 1
 */
#define kOfxImageEffectPropProjectPixelAspectRatio "OfxImageEffectPropPixelAspectRatio"

/** ImageEffect Instance property, gets the duration length of the effect
    - double X 1
 */
#define kOfxImageEffectInstancePropEffectDuration "OfxImageEffectInstancePropEffectDuration"

/** Clip property, indicating which spatial field comes first temporally in the footage
    - string X 1
   This will be one of
    - kOfxImageFieldNone  - the material is unfielded
    - kOfxImageFieldLower - the material is fielded, with scan line 0,2,4.... occuring first in a frame
    - kOfxImageFieldUpper - the material is fielded, with scan line 1,3,5.... occuring first in a frame
   This is host set.
 */
#define kOfxImageClipPropFieldOrder "OfxImageClipPropFieldOrder"

/** Image property, fetches the data pointer at which you can address pixels
    - pointer X 1
   This is host set.
 */
#define kOfxImagePropData "OfxImagePropData"

/** Image property, fetches the bounds of the image, in the Pixel Coordinate System
    - int X 4
    - order is x1, y1, x2, y2
   This is host set.
 */
#define kOfxImagePropBounds "OfxImagePropBounds"

/** Image property, fetches the number of bytes in a row of the image, note that this can be negative
     - int X 1
   This is host set.
 */
#define kOfxImagePropRowBytes "OfxImagePropRowBytes"


/** Image property, indicating which fields are present in the image
    - string X 1
his will be one of
    - ::kOfxImageFieldNone  - the image is an unfielded frame
    - ::kOfxImageFieldBoth  - the image is fielded and contains both interlaced fields 
    - ::kOfxImageFieldLower - the image is fielded and contains a single field, being the lower field (lines 0,2,4...)
    - ::kOfxImageFieldUpper - the image is fielded and contains a single field, being the upper field (lines 1,3,5...)
This is host set.
 */
#define kOfxImagePropField "OfxImagePropField"

/** @brief Plugin property, the plugin sets this to indicate how it renders fielded footage

    - int X 1
    - default is 1
   This will be one of
     - 0 - the plugin is to have it's render function called twice only if there is animation in any of it's parameters
     - 1 - the plugin is to have it's render function called twice always (default) 
 */
#define kOfxImageEffectPluginPropFieldRenderTwiceAlways "OfxImageEffectPluginPropFieldRenderTwiceAlways"

/** Clip property, the plugin sets this on a clip to indicate how it fetches fielded images

    - string X 1
    - default is kOfxImageFieldDoubled

   This will be one of
     - kOfxImageFieldBoth    - fetch a full frame interlaced image
     - kOfxImageFieldSingle  - fetch a single field, making a half height image
     - kOfxImageFieldDoubled - fetch a single field, but doubling each line and so making a full height image (default)
 */
#define kOfxImageClipPropFieldExtraction "OfxImageClipPropFieldExtraction"

/** Effect argument property, set by the host to indicate which field is being renderred
      - string X 1
 This will be one of,
      - kOfxImageFieldNone  - there are no fields to deal with, the image is full frame
      - kOfxImageFieldBoth  - the imagery is fielded and both scan lines should be renderred
      - kOfxImageFieldLower - the lower field is being rendered (lines 0,2,4...)
      - kOfxImageFieldUpper - the upper field is being rendered (lines 1,3,5...)
This is host set.
 */
#define kOfxImageEffectPropFieldToRender "OfxImageEffectPropFieldToRender"

/** Effect argument property, set by the plugin to indicate the region of definition of an effect
      - double X 4

This will be in \ref CannonicalCoordinates
 */
#define kOfxImageEffectPropRegionOfDefinition "OfxImageEffectPropRegionOfDefinition"

/** Effect argument property, set by the plugin to indicate the region of interest it wants from an input
      - double X 4

This will be in \ref CannonicalCoordinates.
 */
#define kOfxImageEffectPropRegionOfInterest "OfxImageEffectPropRegionOfInterest"

/** Effect argument property, set by the plugin to indicate the pixels that need to be renderred
      - int X 4
This will be in \ref PixelCoordinates.
 */
#define kOfxImageEffectPropRenderWindow "OfxImageEffectPropRenderWindow"


/** String used to label imagery as having no fields */
#define kOfxImageFieldNone "OfxFieldNone"
/** String used to label the lower field (scan lines 0,2,4...) of fielded imagery */
#define kOfxImageFieldLower "OfxFieldLower"
/** String used to label the upper field (scan lines 1,3,5...) of fielded imagery */
#define kOfxImageFieldUpper "OfxFieldUpper"
/** String used to label both fields of fielded imagery, indicating interlaced footage */
#define kOfxImageFieldBoth "OfxFieldBoth"
/** String used to label an image that consists of a single field, and so is half height */
#define kOfxImageFieldSingle "OfxFieldSingle"
/** String used to label an image that consists of a single field, but each scan line is double,
    and so is full height */
#define kOfxImageFieldDoubled "OfxFieldDoubled"

/*@}*/
/*@}*/


/** @brief the string that names image effect suites, passed to OfxHost::fetchSuite */
#define kOfxImageEffectSuite "OfxImageEffectSuite"

/** @brief The OFX Image Effects suite, version 1 */
typedef struct OfxImageEffectSuiteV1 {  
  /** @brief Retrieves the property set for the given image effect

  \arg \e imageEffect   image effect to get the property set for
  \arg \e propHandle    pointer to a the property set pointer, value is returned here

  The property handle is valid for the lifetime of the clip, which is generally the lifetime of the instance.

  returns
  - ::kOfxStatOK       - the property set was found and returned
  - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
  - ::kOfxStatErrUnknown    - if the type is unknown
  */
  OfxStatus (*getPropertySet)(OfxImageEffectHandle imageEffect,
			      OfxPropertySetHandle *propHandle);

  /** @brief Retrieves the parameter set for the given image effect

  \arg \e imageEffect   image effect to get the property set for
  \arg \e paramSet     pointer to a the parameter set, value is returned here

  The property handle is valid for the lifetime of the clip, which is generally the lifetime of the instance.

  returns
  - ::kOfxStatOK       - the property set was found and returned
  - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
  - ::kOfxStatErrUnknown    - if the type is unknown
  */
  OfxStatus (*getParamSet)(OfxImageEffectHandle imageEffect,
			   OfxParamSetHandle *paramSet);


  /** @brief Define a clip to the effect. 
      
   \arg \e pluginHandle - the handle passed into 'describeInContext' action
   \arg \e name - unique name of the clip to define
   \arg \e propertySet - a property handle for the clip descriptor will be returned here

   This function defines a clip to a host, the returned property set is used to describe
   various aspects of the clip to the host. Note that this does not create a clip instance.
   
\pre
 - we are inside the describe in context action.

\return
  */
  OfxStatus (*clipDefine)(OfxImageEffectHandle imageEffect,
			  const char *name,	 
			  OfxPropertySetHandle *propertySet);

  /** @brief Get the propery handle of the named input clip in the given instance 
   
   \arg \e imageEffect - an instance handle to the plugin
   \arg \e name        - name of the clip, previously used in a clip define call
   \arg \e clip        - where to return the clip
  \arg \e propertySet  if not null, the descriptor handle for a parameter's property set will be placed here.

  The propertySet will have the same value as would be returned by OfxImageEffectSuiteV1::clipGetPropertySet

      This return a clip handle for the given instance, note that this will \em not be the same as the
      clip handle returned by clipDefine and will be distanct to clip handles in any other instance
      of the plugin.

      Not a valid call in any of the describe actions.

\pre
 - create instance action called,
 - \e name passed to clipDefine for this context,
 - not inside describe or describe in context actions.
 
\post
 - handle will be valid for the life time of the instance.

  */
  OfxStatus (*clipGetHandle)(OfxImageEffectHandle imageEffect,
			     const char *name,
			     OfxImageClipHandle *clip,
			     OfxPropertySetHandle *propertySet);

  /** @brief Retrieves the property set for a given clip

  \arg \e clip          clip effect to get the property set for
  \arg \e propHandle    pointer to a the property set handle, value is returedn her

  The property handle is valid for the lifetime of the clip, which is generally the lifetime of the instance.

  returns
  - ::kOfxStatOK       - the property set was found and returned
  - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
  - ::kOfxStatErrUnknown    - if the type is unknown
  */
  OfxStatus (*clipGetPropertySet)(OfxImageClipHandle clip,
				  OfxPropertySetHandle *propHandle);

  /** @brief Get a handle for an image in a clip at the indicated time and indicated region

      \arg \e clip  - the clip to extract the image from
      \arg \e time        - time to fetch the image at
      \arg \e region      - region to fetch the image from (optional, set to NULL to get a 'default' region)
                            this is in the \ref CannonicalCoordinates. 
      \arg \e imageHandle - property set containing the image's data

  An image is fetched from a clip at the indicated time for the given region and returned in the imageHandle.

 If the \e region parameter is not set to NULL, then it will be clipped to the clip's Region of Definition for the given time. The returned image will be \em at \em least as big as this region. If the region parameter is not set, then the region fetched will be at least the Region of Interest the effect has previously specified, clipped the clip's Region of Definition.

If clipGetImage is called twice with the same parameters, then two separate image handles will be returned, each of which must be release. The underlying implementation could share image data pointers and use reference counting to maintain them.

\pre
 - clip was returned by clipGetHandle

\post
 - image handle is only valid for the duration of the action clipGetImage is called in
 - image handle to be disposed of by clipReleaseImage before the action returns

\return
- ::kOfxStatOK - the image was successfully fetched and returned in the handle,
- ::kOfxStatFailed - the image could not be fetched because it does not exist in the clip at the indicated time and/or region, the plugin
                     should continue operation, but assume the image was black and transparent.
- ::kOfxStatErrBadHandle - the clip handle was invalid,
- ::kOfxStatErrMemory - the host had not enough memory to complete the operation, plugin should abort whatever it was doing.

  */
  OfxStatus (*clipGetImage)(OfxImageClipHandle clip,
			    OfxTime       time,
			    OfxRectD     *region,
			    OfxPropertySetHandle   *imageHandle);
  
  /** @brief Releases the image handle previously returned by clipGetImage


\pre
 - imageHandle was returned by clipGetImage

\post
 - all operations on imageHandle will be invalid

\return
- ::kOfxStatOK - the image was successfully fetched and returned in the handle,
- ::kOfxStatErrBadHandle - the image handle was invalid,
 */
  OfxStatus (*clipReleaseImage)(OfxPropertySetHandle imageHandle);
  

  /** @brief Returns the spatial region of definition of the clip at the given time

      \arg \e clipHandle  - the clip to extract the image from
      \arg \e time        - time to fetch the image at
      \arg \e region      - region to fetch the image from (optional, set to NULL to get a 'default' region)
                            this is in the \ref CannonicalCoordinates. 
      \arg \e imageHandle - handle where the image is returned

  An image is fetched from a clip at the indicated time for the given region and returned in the imageHandle.

 If the \e region parameter is not set to NULL, then it will be clipped to the clip's Region of Definition for the given time. The returned image will be \em at \em least as big as this region. If the region parameter is not set, then the region fetched will be at least the Region of Interest the effect has previously specified, clipped the clip's Region of Definition.

\pre
 - clipHandle was returned by clipGetHandle

\post
 - bounds will be filled the RoD of the clip at the indicated time

\return
- ::kOfxStatOK - the image was successfully fetched and returned in the handle,
- ::kOfxStatFailed - the image could not be fetched because it does not exist in the clip at the indicated time, the plugin
                     should continue operation, but assume the image was black and transparent.
- ::kOfxStatErrBadHandle - the clip handle was invalid,
- ::kOfxStatErrMemory - the host had not enough memory to complete the operation, plugin should abort whatever it was doing.


  */
  OfxStatus (*clipGetRegionOfDefinition)(OfxImageClipHandle clip,
					 OfxTime time,
					 OfxRectD *bounds);

  /** @brief Returns whether to abort processing or not.

      \arg \e imageEffect  - instance of the image effect

  A host may want to signal to a plugin that it should stop whatever rendering it is doing and start again. 
  Generally this is done in interactive threads in response to users tweaking some parameter.

  This function indicates whether a plugin should stop whatever processing it is doing.
  
  \return
     - 0 if the effect should continue whatever processing it is doing
     - 1 if the effect should abort whatever processing it is doing  
 */
  int (*abort)(OfxImageEffectHandle imageEffect);

  /** @brief Allocate memory from the host's image memory pool
      
  \arg \e instanceHandle  - effect instance to associate with this memory allocation, may be NULL.
  \arg \e nBytes          - the number of bytes to allocate
  \arg \e memoryHandle    - pointer to the memory handle where a return value is placed

  Memory handles allocated by this should be freed by OfxImageEffectSuiteV1::imageMemoryFree. 
  To access the memory behind the handle you need to call  OfxImageEffectSuiteV1::imageMemoryLock.

  See \ref ImageEffectsMemoryAllocation.

  \returns 
  - kOfxStatOK if all went well, a valid memory handle is placed in \e memoryHandle
  - kOfxStatErrBadHandle if instanceHandle is not valid, memoryHandle is set to NULL
  - kOfxStatErrMemory if there was not enough memory to satisfy the call, memoryHandle is set to NULL
  */   
  OfxStatus (*imageMemoryAlloc)(OfxImageEffectHandle instanceHandle, 
				size_t nBytes,
				OfxImageMemoryHandle *memoryHandle);
	
  /** @brief Frees a memory handle and associated memory.
      
  \arg \e memoryHandle - memory handle returned by imageMemoryAlloc

  This function frees a memory handle and associated memory that was previously allocated via OfxImageEffectSuiteV1::imageMemoryAlloc

  If there are outstanding locks, these are ignored and the handle and memory are freed anyway.

  See \ref ImageEffectsMemoryAllocation.

  \returns
  - kOfxStatOK if the memory was cleanly deleted
  - kOfxStatErrBadHandle if the value of \e memoryHandle was not a valid pointer returned by OfxImageEffectSuiteV1::imageMemoryAlloc
  */   
  OfxStatus (*imageMemoryFree)(OfxImageMemoryHandle memoryHandle);

  /** @brief Lock the memory associated with a memory handle and make it available for use.

  \arg \e memoryHandle - memory handle returned by imageMemoryAlloc
  \arg \e returnedPtr - where to the pointer to the locked memory

  This function locks them memory associated with a memory handle and returns a pointer to it. The memory will be 16 byte aligned, to allow use of vector operations.
  
  Note that memory locks and unlocks nest.

  After the first lock call, the contents of the memory pointer to by \e returnedPtr is undefined. All subsequent calls to lock will return memory with the same contents as  the previous call.

  Also, if unlocked, then relocked, the memory associated with a memory handle may be at a different address.

  See also OfxImageEffectSuiteV1::imageMemoryUnlock and \ref ImageEffectsMemoryAllocation.
    
  \returns
  - kOfxStatOK if the memory was locked, a pointer is placed in \e returnedPtr
  - kOfxStatErrBadHandle if the value of \e memoryHandle was not a valid pointer returned by OfxImageEffectSuiteV1::imageMemoryAlloc, null is placed in \e *returnedPtr
  - kOfxStatErrMemory if there was not enough memory to satisfy the call, \e *returnedPtr is set to NULL
  */
  OfxStatus (*imageMemoryLock)(OfxImageMemoryHandle memoryHandle,
			       void **returnedPtr);

  /** @brief Unlock allocated image data

  \arg \e allocatedData - pointer to memory previously returned by OfxImageEffectSuiteV1::imageAlloc

  This function unlocks a previously locked memory handle. Once completely unlocked, memory associated with a memoryHandle is no longer available for use. Attempting to use it results in undefined behaviour.

  Note that locks and unlocks nest, and to fully unlock memory you need to match the count of locks placed upon it. 

  Also note, if you unlock a completely unlocked handle, it has no effect (ie: the lock count can't be negative).
    
  If unlocked, then relocked, the memory associated with a memory handle may be at a different address, however the contents will remain the same.

  See also OfxImageEffectSuiteV1::imageMemoryLock and \ref ImageEffectsMemoryAllocation.
  
  \returns
  - kOfxStatOK if the memory was unlocked cleanly,
  - kOfxStatErrBadHandle if the value of \e memoryHandle was not a valid pointer returned by OfxImageEffectSuiteV1::imageMemoryAlloc, null is placed in \e *returnedPtr
  */
  OfxStatus (*imageMemoryUnlock)(OfxImageMemoryHandle memoryHandle);

} OfxImageEffectSuiteV1;



/**
   \addtogroup StatusCodes
*/
/*@{*/
/**
   \defgroup StatusCodesImageEffect Image Effect API Status Codes 

These are status codes returned by functions in the OfxImageEffectSuite and Image Effect plugin functions.

They range from 1000 until 1999
*/
/*@{*/
/** @brief Error code for incorrect image formats */
#define kOfxStatErrImageFormat ((int) 1000)


/*@}*/
/*@}*/
#ifdef __cplusplus
}
#endif

/*------------------------------------------------------------------------------
          No source code is below, it is purely Doxygen documentation
  ------------------------------------------------------------------------------*/

/** @page ImageEffectsPage OFX : Image Effects
    
@section ImageEffectIntro Introduction

In general, image effects plugins take zero or more input clips and produce an output clip. So far so simple, however there are many devils hiding in the details. Several supporting suites are required from the host and the plugin needs to respond to a range of actions to work correctly. How an effect is intended to be used also complicates the issue, forcing sets of behaviours depending on the context of an effect.

@section ImageEffectsTheAPI The Image Effect API 

Plugins that implement the image effect API set the pluginApi member of the OfxPlugin to be ::kOfxImageEffectPluginApi, the current version of the API is 1. This is enough to label the plugin as a image effect plugin.

The suites that an Image Effects host \em must provide are....
     - OfxImageEffectSuiteV1
     - OfxPropertySuiteV1
     - OfxParameterSuiteV1
     - OfxInteractSuiteV1
     - OfxMemorySuiteV1
     - OfxMultiThreadSuiteV1
     - OfxMessageSuiteV1

The plug-in will have the following actions passed to it's entry point...
- ::kOfxActionLoad
- ::kOfxActionUnload
- ::kOfxActionDescribe
- ::kOfxImageEffectActionDescribeInContext
- ::kOfxActionCreateInstance
- ::kOfxActionDestroyInstance
- ::kOfxActionFlatten
- ::kOfxActionPurgeCaches
- ::kOfxActionBeginInstanceEdit
- ::kOfxActionEndInstanceEdit
- ::kOfxActionBeginInstanceChanged
- ::kOfxActionInstanceChanged
- ::kOfxActionEndInstanceChanged
- ::kOfxImageEffectActionIsIdentity
- ::kOfxImageEffectActionBeginSequenceRender
- ::kOfxImageEffectActionRender
- ::kOfxImageEffectActionEndSequenceRender
- ::kOfxImageEffectActionGetClipPreferences
- ::kOfxImageEffectActionGetFramesNeeded
- ::kOfxImageEffectActionGetRegionOfDefinition
- ::kOfxImageEffectActionGetRegionsOfInterest
- ::kOfxImageEffectActionGetTimeDomain

For a description of the image processing architecture that underlies OFX, see \ref ImageEffectsProcessingArchitecture.

@section ImageEffectDefinition Defining an Image Effect Plugin

Image Effects describe themselves to a host by responding to two actions, ::kOfxActionDescribe and ::kOfxImageEffectActionDescribeInContext. For more information on contexts see \ref ImageEffectContexts.

In the ::kOfxActionDescribe action a plugin describes its basic behaviour to a host by setting various properties on the plugin descriptor handle. This is where it says what bit depths it supports, what contexts it operates in, whether it supports tiling and so on. It does \em not define parameters or input clips in this action.

The ::kOfxImageEffectActionDescribeInContext action is called once per context and is where the plugin defines its context dependant behaviour and needs. This is where clips and parameters are defined, note that objects mandated for a context must be defined here.

*/


/** @page ImageEffectsThreadSafety OFX : Image Effect Thread and Recursion Safety

A host will generally be multi-threaded. Any host with a GUI will most likely have an interactive thread and a rendering thread, while any host running on a multi-CPU machine may have a render thread per CPU. Also, a host may batch effects off to a render farm, where the same effect has separate frames renderred on completely different machines. OFX needs to address all these situations.

Threads can be broken into two types, main theaads, where any action may be called, and render threads, where only a subset of actions may be called. For a given plug-in instance, there can be only one main thread and zero or more render threads. A plugin must be able to handle simultaneous actions called on the main and zero or more render thread. A plugin can control the number of simultaneous render threads via the ::kOfxImageEffectPluginRenderThreadSafety property.

The only actions that can be called on a render thread are...
  - ::kOfxImageEffectActionBeginSequenceRender
  - ::kOfxImageEffectActionRender
  - ::kOfxImageEffectActionEndSequenceRender
  - ::kOfxImageEffectActionIsIdentity
  - ::kOfxImageEffectActionGetFramesNeeded
  - ::kOfxImageEffectActionGetRegionOfDefinition
  - ::kOfxImageEffectActionGetRegionsOfInterest

If a plugin cannot support this multi-threading behaviour, it will need to perform explicit locking itself, using the locking mechanisms in the suites defined in ofxMultiThread.h

This will also mean that the host may need to perform locking on the various function calls over the API. For example, a main and render thread may both simultaneously attempt to access a parameter from a single plugin instance. The locking should,
- block write/read access
- not block on read/read access
- be fine grained at the level of individual function calls,
- be transparent to the plugin, so it will block until the call succeeds.

For example, a render thread will only cause a parameter to lock out writes only for the duration of the call that reads the parameter, not for the duration of the whole render action. This will allow a main thread to continue writing to the parameter during a render. This is especially important if you have a custom interactive GUI that you want to keep working during a render call.

Note that a main thread should generally issue an abort to any linked render thread when a parameter or other value affecting the effect (eg: time) has been changed by the user. A re-render should then be issued so that a correct frame is created.

How an effect handles simulanteous calls to render is dealt with in \ref ImageEffectsMultiThreadingRendering.

<hr>

@section ImageEffectsThreadSafetyRecursiveActions Recursive Actions

When running on a main thread, some actions may end up being called recursively. For example consider the following sequence of events in a plugin...

-# user sets parameter A in a GUI
-# host issues ::kOfxActionInstanceChanged action
-# plugin traps that and sets parameter B 
   -# host issues a new ::kOfxActionInstanceChanged action for parameter B
   -# plugin traps that and changes some internal private state and requests the overlay redraw itself
       -# ::kOfxInteractActionDraw issued to the effect's overlay
       -# plugin draws overlay
       -# ::kOfxInteractActionDraw returns
   -# ::kOfxActionInstanceChanged action for parameter B returns
-# ::kOfxActionInstanceChanged action returns

Here we have a set of actions being called recursively. A plugin must be able to deal with this.

The image effect actions which may trigger a recursive action call on a single instance are...
- ::kOfxActionBeginInstanceChanged
- ::kOfxActionInstanceChanged
- ::kOfxActionEndInstanceChanged

The interact actions which may trigger a recursive action to be called on the associated plugin instance are...
- ::kOfxInteractActionGainFocus
- ::kOfxInteractActionKeyDown
- ::kOfxInteractActionKeyRepeat
- ::kOfxInteractActionKeyUp
- ::kOfxInteractActionLoseFocus
- ::kOfxInteractActionPenDown
- ::kOfxInteractActionPenMotion
- ::kOfxInteractActionPenUp

The image effect actions which may be called recursively are...
- ::kOfxActionBeginInstanceChanged
- ::kOfxActionInstanceChanged
- ::kOfxActionEndInstanceChanged
- ::kOfxImageEffectActionGetClipPreferences

The interact actions which may be called recursively are...
- ::kOfxInteractActionDraw

*/

/** @page ImageEffectsImagesAndClips OFX : Images, Clips and Clip Preferences

@section ImageEffectsImagesAndClipsIntro What Is An Image?

Image Effects process images (funny that), this page describes images and clips of images, how they behave and how to deal with them. You should read \ref ImageEffectsCoordinates and \ref ImageEffectsImagePlane for details about the spatial coordinates that OFX uses.

Firstly some definitions...
- an \em image is a rectangular array of addressable pixels,
- a \em clip is a contiguous sequence of images that vary over time.

Images and clips contain pixels, these pixels can currently be of the following types...
- a colour pixel with red, green, blue, alpha components
- a colour pixel with YUV and alpha components
- single component 'alpha' images

The components of the pixels can be of the following types...
- 8 bit unsigned byte, with the nominal black and white points at 0 and 255 respectively,
- 16 bit unsigned short, with the nominal black and white points at 0 and 65535 respectively,
- 32 bit float, with the nominal black and white points at 0.0f and 1.0f respectively, component values are \em not clipped to 0.0f and 1.0f.

Components are packed per pixel in the following manner...
- RGBA pixels as R, G, B, A
- YUVA pixels as Y, U, V, A.

There are several structs for pixel types in ofxCore.h that can be used for raw pixels in OFX.

Images are always left to right, bottom to top, with the pixel data pointer being at the bottom left of the image. The pixels in a scan line are contiguously packed.

Scanlines need \em not be contiguously packed. The number of \em bytes between between a pixel in the same column, but separated by a scan line is known as the \b rowbytes of an image. Rowbytes can be negative, allowing for compositing systems with a native top to bottom scanline order to trivially support bottom to top images.

Clips and images also have a <B>pixel aspect ratio</B>, this is how much an actual addressable pixel must be stretched by in X to be square. For example PAL D1 images have a pixel aspect ratio of 1.06666. 

Images are rectangular, whose integral bounds are in \ref PixelCoordinates, with the image being X1 <= X < X2 and Y1 <= Y < Y2, ie: exclusive on the top and right. The bounds represent the amount of data present in the image, which may be larger, smaller or equal to the Region of Definition of the clip, depending on the architecture supported by the plugin.

Clips have a frame rate, which is the number of frames per second they are to be displayed out, see \ref ImageEffectsImagesAndTime for more details on how time and frame rates work in OFX. Some clips may be continously samplable (for example, if they are connected to animating geometry that can be renderred at arbitrary times), if this is so, the frame rate for these clips is set to 0.

Images may be composed of full frames, two fields or a single field, depending on its source and how the effect requests the image be processed. Clips are either full frame sequences or fielded sequences.

Images and clips also have a premultiplication state, this represents how the alpha component and the RGB/YUV components may have interacted.

Images and clips are manipulated via property handles, for the list of properties on clip and images see the page \ref ImageEffectsProperties.

<HR>

@section ImageEffectsImagesAndClipsDefiningClips Defining Clips

During an the effect's describe in context action an effect \em must define the clips mandated for that context, it can also define extra  clips that it may need for that context. It does this using the OfxImageEffectSuiteV1::clipDefine function, the property handle returned by this function is purely for definition purposes only. It has not persistance outside the describe in context action and is distinct to the clip property handles used by instances. The \e name parameter is how you can later access that clip in a plugin instance via the OfxImageEffectSuiteV1::clipGetHandle function. 

During the describe in context action, the plugin sets properties on a clip to control its use. The properties that can be set during a describe in context call are...
- ::kOfxPropLabel - to give a user readable name to the clip (the host need not use this, for example in a transition it is redundant),
- ::kOfxImageEffectPropSupportedComponents - to specify which components it is willing to accept on that clip,
- ::kOfxImageClipPropOptional - to specify if the clip is optional,
- ::kOfxImageClipPropFieldExtraction - specifies how to extract fielded images from a clip, see \ref ImageEffectsFieldRendering for more details on field and field rendering,
- ::kOfxImageEffectPropTemporalClipAccess - whether the effect wants to access images from the clip at times other that the frame being renderred.

Plugins \em must indicate which pixel depths they can process by setting the ::kOfxImageEffectPropSupportedPixelDepths on the plugin handle during the describe action.

Pixel Aspect Ratios, frame rates, fielding, components and pixel depths are constant for the duration of a clip, they cannot changed from frame to frame.

\b Note:
- it is an error not to set the ::kOfxImageEffectPropSupportedPixelDepths plugin property during its describe action
- it is an error not to define a mandated input clip during the describe in context action
- it is an error not to set the ::kOfxImageEffectPropSupportedComponents on an input clip during describe in context

\note The handle parameter passed to describe in context may not be the same as passed to the describe call.

<HR>

@section  ImageEffectsImagesAndClipsUsingClips Getting Images From Clips

Clips in instances are retrieved via the OfxImageEffectSuiteV1::clipGetHandle function. This returns a property handle for the clip in a specific instance. This handle is valid for the duration of the instance.

Images are fetched from a clip via the OfxImageEffectSuiteV1::clipGetImage function. This takes a time and an optional region to extract an image at from a given clip. This returns, in a property handle, an image fetched from the clip at a specfic time. The handle contains all the information relevant to dealing with that image.

Once fetched, an image must be released via the OfxImageEffectSuiteV1:::clipReleaseImage function. All images must be released within the action they were fetched in. You cannot retain an image after an action has returned.

Images may be fetched from an attached clip in the following situations...
- in the ::kOfxImageEffectActionRender action
- in the ::kOfxActionInstanceChanged and ::kOfxActionEndInstanceChanged actions with a ::kOfxPropChangeReason of ::kOfxChangeUserEdited

A host may not be able to support random temporal access, it flags its ability to do so via the ::kOfxImageEffectPropTemporalClipAccess property. A plugin that wishes to perform random temporal access must set a property of that name on the plugin handle and the clip it wishes to perform random access from.

\b Note:
- it is an error for a plugin to attempt random temporal image access if the host does not support it
- it is an error for a plugin to attempt random temporal image access if it has not flagged that it wishes to do so and the clip it wishes to do so from.

<HR>

@section ImageEffectsPremultiplication Premultiplication And Alpha

All images and clips have a premultiplication state. This is used to indicate how the image should interpret RGB (or YUV) pixels, with respect to alpha. The premultiplication state can be...
- ::kOfxImageOpaque          - the image is opaque and so has no premultiplication state, but the alpha component in all pixels is set to the white point,
- ::kOfxImagePreMultiplied   - the image is premultiplied by it's alpha,
- ::kOfxImageUnPreMultiplied - the image is unpremultiplied.

This document won't go into the details of premultiplication, but will simply state that OFX takes notice of it and flags images and clips accordingly.

The premultiplication state of a clip is constant over the entire duration of that clip.

<HR>

@section ImageEffectsPixelAspectRatios Clips and Pixel Aspect Ratios

All clips and images have a pixel aspect ratio, this is how much a 'real' pixel must be stretched by in X to be square. For example PAL D1 images have a pixel aspect ratio of 1.06666. See \ref ImageEffectsCoordinates for more information on how to map real pixels to the image plane and back.

The property ::kOfxImageEffectPropSupportsMultiPixelAspectRatios is used to control how a plugin deals with pixel aspect ratios. This is both a host and plugin property. For a host it can be set to...
  - 0 - the host only supports a single pixel aspect ratio for all clips, input or output, to an effect,
  - 1 - the host can support differing pixel aspect ratios for inputs and outputs

For a plugin it can be set to...
  - 0 - the plugin expects all pixel aspect ratios to be the same on all clips, input or output
  - 1 - the plugin will accept clips of differing pixel aspect ratio.

If a plugin does not accept clips of differing PARs, then the host must resample all images fed to that effect to agree with the output's PAR.

If a plugin does accept clips of differing PARs, it will need to specify the output clip's PAR in the ::kOfxImageEffectActionGetClipPreferences action.

<HR>

@section ImageEffectClipPreferences Clip Preferences

The ::kOfxImageEffectActionGetClipPreferences action is passed to an effect to allow a plugin to specify how it wishes to deal with it's input clips and to set properties in it's output clip. This is especially important when there are multiple inputs of which my have differing properties, eg; pixel depth.

More specifically, there are six properties that can be set, some on the input clip, some on the output clip, some on both. These are...
  - the depth of a clip's pixels, input or output clip,
  - the components of a clip's pixels, input or output clip,
  - the pixel aspect ratio of a clip, input or output clip,
  - the frame rate of the output clip, 
  - the fielding of the output clip,
  - the premultiplication state of the output clip,
  - whether the output clip varys from frame to frame, even if no paramerters or input images change over time,
  - whether the output clip can be sampled at sub-frame times and produce different images.

The behaviour specified by OFX means that a host may need to cast images from their native data format into one suitable for the plugin. It is better that the host do any of this pixel shuffling because,
- the behaviour is orthogonal for all plugins on that host,
- the code is not replicated in all plugins,
- the host can optimise the pixel shuffling in one pass with any other data grooming it may need to do.

A plugin gets to assert its clip preferences in several situations. Firstly whenever a clip is attached to a plugin, secondly whenever one of the parameters in the plugin property ::kOfxImageEffectPropClipPreferencesSlaveParam has its value changed. The clip preferences action is never called until all non optional clips have been attached to the plugin.

\b Note:
- these properties \em cannot animate over the duration of an effect,
- that the ability to set input and output clip preferences is restricted by the context of an effect,
- optional input clips do not have any context specific restrictions on plugin set preferences.

@subsection ImageEffectClipPreferencesFrameVarying Frame Varying Effects

Some plugins can generate differing output frames at different times, even if no parameters animate or no input images change. The ::kOfxImageEffectFrameVarying property set in the clip preferences action is used to flag this.

A counter example is a solid colour generator. If it has no animating parameters, the image generated at frame 0 will be the same as the image generated at any other frame. Intellegent hosts can render a single frame and cache that for use at all other times. 

On the other hand, a plugin that generates random noise at each frame, and seeds its random number generator with the render time, will create different images at different times. The host cannot render a single frame and cache that for use at subsequent times.

To differentiate between these two cases the ::kOfxImageEffectFrameVarying is used. If set to 1, it indicates that the effect will need to be rendered at each frame, even if no input images or parameters are varying. If set to 0, then a single frame can be rendered and used for all times if no input images or parameters vary. The default value is 0.

@subsection ImageEffectClipPreferencesContinousSamples Continuously Sampled Effects

Some effects can generate images at non frame boundaries, even if the inputs to the effect are frame based and there is no animation.

For example a fractal cloud generator whose pattern evolves with a speed parameter can be rendered at arbitrary times, not just on frame boundaries. Hosts that are interested in sub frame rendering can determine this by behaviour by examining the ::kOfxImageClipPropContinuousSamples property set in the clip preferences action. By default this is false.

\note Implicitly, all retimers effects can be continuously sampled.

@subsection ImageEffectClipPreferencesPixelDepths Specifying Pixel Depths

Hosts and plugins flag whether whether they can deal with input/output clips of differing pixel depths via the ::kOfxImageEffectPropSupportsMultipleClipDepths property.

If the host sets this to 0, then all effect's input and output clips will always have the same component depth, and the plugin may not remap them.

If the plugin sets this to 0, then the host will transparently map all of an effect's input and output clips to a single depth, even if the actual clips are of differing depths.

In the above two cases, the common component depth chosen will be the deepest depth of any input clip mapped to a depth the plugin supports that loses the least precision. eg: if a plugin supported 8 bit and float images, but the deepest clip attached to it was 16 bit, the host would transparently map all clips to float. 

If \em both the plugin and host set this to 1, then the plugin can, during the ::kOfxImageEffectActionGetClipPreferences, specify how the host is to map each clip, including the output clip. Note that this is the only case where a plugin may set the output depth.

@subsection ImageEffectClipPreferencesPixelComponents Specifying Pixel Components

A plugin specifies what components it is willing to accept on a clip via the ::kOfxImageEffectPropSupportedComponents on the clip's descriptor during the ::kOfxImageEffectActionDescribeInContext. This is one or more of RGBA, YUVA and A.

If an effect has multiple inputs, and each can be a range of component types, the effect may end up with component types that are incompatible for its purposes. In this case the effect will want to have the host remap the components of the inputs and to specify the components in the output.

For example, an general effect that blends two images will have have two inputs, each of which may be RGBA or A. In operation, if presented with RGBA on one and A on the other, it will most likely request that the A clip be mapped to RGBA by the host and the output be RGBA as well.

In all contexts, except for the general context, mandated input clips cannot have their component types remapped, nor can the output. Optional input clips can always have their component types remapped.

In the general context, all input clips may be remapped, as can the output clip. The output clip has it's default components set to be,
  - RGBA if any of the inputs is RGBA,
  - otherwise YUVA if any of the inputs is YUVA,
  - otherwise A if the effect has any inputs,
  - otherwise RGBA if there are no inputs.

\b Note:
It a host implementation detail as to how a host actually attaches real clips to a plugin. For instance, a host may allow a YUVA clip to be wired to an input that asks for RGBA only. However it must map the clip to RGBA in a manner that is transparent to the plugin. Similarly for any other component types that the plugin does not support on an input.

@subsection ImageEffectClipPreferencesPixelAspectRatios Specifying Pixel Aspect Ratios

Hosts and plugins flag whether whether they can deal with input/output clips of differing pixel aspect ratios via the ::kOfxImageEffectPropSupportsMultipleClipPARs property.

If the host sets this to 0, then all effect's input and output clips will always have the same pixel aspect ratio, and the plugin may not remap them.

If the plugin sets this to 0, then the host will transparently map all of an effect's input and output clips to a single pixel aspect ratio, even if the actual clips are of differring PARs.

In the above two cases, the common pixel aspect ratio chosen will be the smallest on all the inputs, as this preserves image data.

If \em both the plugin and host set this to 1, then the plugin can, during ::kOfxImageEffectActionGetClipPreferences, specify how the host is to map each clip, including the output clip. 

@subsection ImageEffectClipPreferencesFielding Specifying Fielding 

The ::kOfxImageEffectPropSetableFielding host property indicates if a plugin is able to change the fielding of the output clip from the default.

The default value of the output clip's fielding is host dependant, but in general,
- if any of the input clips are fielded, so will the output clip
- the output clip may be fielded irregardless of the input clips (for example, in a fielded project).

If the host allows a plugin to specify the fielding of the output clip, then a plugin may do so during the ::kOfxImageEffectActionGetClipPreferences. For example a defielding plugin will want to indicate that the output is frame based rather than fielded.

@subsection ImageEffectClipPreferencesFrameRates Specifying Frame Rates

The ::kOfxImageEffectPropSetableFrameRate host property indicates if a plugin is able to change the frame rate of the output clip from the default.

The default value of the output clip's frame rate is host dependant, but in general, it will be based on the input clips frame rates.

If the host allows a plugin to specify the frame rate of the output clip, then a plugin may do so during the ::kOfxImageEffectActionGetClipPreferences. For example a deinterlace plugin that separates both fields from fielded footage will want to do double the frame rate of the output clip.

If a plugin changes the frame rate, it is effectively changing the number of frames in the output clip. If our hypothetical deinterlace plugin doubles the frame rate of the output clip, it will be doubling the number of frames in that clip. The timing diagram below should help, showing how our fielded input has been turned into twice the number of frames on output.

\verbatim

   FIELDED SOURCE      0.0 0.5 1.0 1.5 2.0 2.5 3.0 3.5 4.0 4.5 ....
   DEINTELACED OUTPUT  0   1   2   3   4   5   6   7   8   9

\endverbatim

The maping of the number of output frames is...

    nFrames' = nFrames * FPS' / FPS

where
  - nFrames is the default number of frames,
  - nFrames' is the new number of output frames,
  - FPS is the default frame rate,
  - FPS' is the new frame rate specified by a plugin.
     

\note Not sure if this is quite right, need to cogitate slightly maybe.

@subsection ImageEffectClipPreferencesPremultiplication Specifying Premultiplication

All clips have a premultiplication state (see http://www.teamten.com/lawrence/graphics/premultiplication/ for a nice explanation). An effect cannot map the premultiplication state of the input clips, but it can specify the premultiplication state of the output clip.

The output's default premultiplication state is...
  - premultiplied if any of the inputs are premultiplied,
  - otherwise unpremultiplied if any of the inputs are unpremultiplied,
  - otherwise opaque.

<HR>

@section ImageEffectsMemoryAllocation Allocating Your Own Images

Under OFX, the  images you fetch from the host have already had their memory allocated. If a plug-in needs to define its owns temporary images buffers during processing, or to cache images between actions, then the plug-in should use the image memory allocation routines declared in OfxImageEffectSuiteV1. The reason for this is that many host have special purpose memory pools they manage to optimise memory usage as images can chew up memory very rapidly (eg: a 2K RGBA floating point film plate is 48 MBytes).

For general purpose (as in less than a megabyte) memory allocation, you should use the memory suite in ofxMemory.h

OFX provides four functions to deal with image memory. These are,
  - OfxImageEffectSuiteV1::imageMemoryAlloc
  - OfxImageEffectSuiteV1::imageMemoryFree
  - OfxImageEffectSuiteV1::imageMemoryLock
  - OfxImageEffectSuiteV1::imageMemoryUnlock

A host needs to be able defragment its image memory pool, potentially moving the contents of the memory you have allocated to another address, even saving it to disk under its own virtual memory caching scheme. Because of this when you request a block of memory, you are actually returned a handle to the memory, not the memory itself. To use the memory you must first lock the memory via the imageMemoryLock call, which will then return a pointer to the locked block of memory.

During an single action, there is generally no need to lock/unlock any temporary buffers you may have allocated via this mechanism. However image memory that is cached between actions should always be unlocked while it is not actually being used. This allows a host to do what it needs to do to optimise memory usage.

Note that locks and unlocks nest. This implies that there is a lock count kept on the memory handle, also not that this lock count cannot be negative. So unlocking a completely unlocked handle has no effect.

Note that the address returned by OfxImageEffectSuiteV1::imageMemoryLock is not invariant if the handle has been completely unlocked.

An example is below....

@verbatim
  // get a memory handle
  OfxImageMemoryHandle memHandle;
  gEffectSuite->imageMemoryAlloc(0, imageSize, &memHandle);

  // lock the handle and get a pointer
  void *memPtr;
  gEffectSuite->imageMemoryLock(memHandle, &memPtr);
  
  ... // do stuff with our pointer

  // now unlock it
  gEffectSuite->imageMemoryUnlock(memHandle);

  
  // lock it again, note that this may give a completely different address to the last lock
  gEffectSuite->imageMemoryLock(memHandle, &memPtr);
  
  ... // do more stuff

  // unlock it again
  gEffectSuite->imageMemoryUnlock(memHandle);

  // delete it all
  gEffectSuite->imageMemoryFree(memHandle);
@endverbatim


*/

/** @page ImageEffectsImagesAndTime OFX : Image Effects and Time

Within OFX Image Effects, there is only one temporal coordinate system, this is in output frames referenced to the start of the effect (so the first affected frame = 0). All times within the API are in that coordinate system.

All clips have a property that indicates the frames for which they can generate data. This is ::kOfxImageClipPropFrameRange, a 2D double parameter, with the first dimension being the first, and the second being last the time at which the clip will generate data.

Consider the example below, it is showing an effect of 10 frames duration applied to a clip lasting 20 frames. The first frame of the effect is infact the 5th frame of the clip. Both the input and output have the same frame rate.

@verbatim
Effect               0  1  2  3  4  5  6  7  8  9
Source   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
@endverbatim

In this example, if the effect asks for the source image at time '4', the host will actually return the 9th image of that clip. 

When queried the output and source clips would report the following.

@verbatim
           range[0]     range[1]    FPS
Output     0            9           25
Source    -4            15          25
@endverbatim

Consider the slightly more complex example below, where the output has a frame rate twice the input's

@verbatim
Effect         0  1  2  3  4  5  6  7  8  9
Source   0     1     2     3     4     5     6     7
@endverbatim

When  queried the output and source clips would report the following.

@verbatim
           range[0]     range[1]    FPS
Output     0            9           25
Source    -2            12          25
@endverbatim

Using simple arithmetic, any effect that needs to access a specific frame of an input, can do so with the formula...

  - f' = (f - range[0]) * srcFPS/outFPS

*/

/** @page ImageEffectsActions OFX : Image Effect Actions

This page lists the actions that will be passed to an image effect, and what the arguments to the OfxPlugin::main function are.

@section ImageEffectsActionDescribeInContext The Describe In Context Action

The ::kOfxImageEffectActionDescribeInContext action is called after the intial describe action, once for each context the effect is defined in. This action is there to allow the plugin to define context varying aspects of the plugin, for example clip properties, parameters etc...

The \e handle parameter is the handle to to the context, this may or may not be the same as passed to the describe call.

The \e inArgs parameter contains the following read only properties...
- ::kOfxImageEffectPropContext the context being described.

The \e outArgs handle is redundant and is set to null.

\pre
- ::kOfxActionDescribe has been called on the instance

\return
- ::kOfxStatReplyDefault, the default action should be taken
- ::kOfxStatOK, the action was trapped and appropriate action taken
- ::kOfxStatErrMissingHostFeature, in which the plugin will be unloaded and ignored, plugin may post message
- ::kOfxStatFailed, the  action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
- the default action is to do nothing

<HR>

@section ImageEffectsActionRender The Render Action

::kOfxImageEffectActionRender is passed to an image effect when the host needs the effect to render a frame.

The \e handle parameter is the instance of the image effect that is to be renderred.

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropTime - the time to render at
- ::kOfxImageEffectPropRenderWindow - the window (in Pixel Coordinates) to render
- ::kOfxImageEffectPropFieldToRender - the field that will be renderred
- ::kOfxImageEffectPropRenderScale - the scale factor being applied to the images being renderred

The \e outArgs parameter is redundant and is NULL.

\pre
- ::kOfxActionCreateInstance has been called on the instance
- the ::kOfxImageEffectActionBeginSequenceRender action has been called on the instance

\post
- the ::kOfxImageEffectActionEndSequenceRender action will be called on the instance

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatFailed, the render action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the render ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
- The render action must be trapped, it cannot return ::kOfxStatReplyDefault

<HR>

@section ImageEffectsActionBeginSequenceRender The Begin Sequence Render Action

::kOfxImageEffectActionBeginSequenceRender is passed to an image effect before it renders a range of frames. It is there to allow an effect to set things up for a long sequence of frames. Note that this is still called, even if only a single frame is being renderred in an interactive environment.

The \e handle parameter is the instance of the image effect that is to be renderred.

The \e inArgs parameter contains the following read only properties...
- ::kOfxImageEffectPropFrameRange - the range of frames (inclusive) that will be renderred,
- ::kOfxImageEffectPropFrameStep  - what is the step between frames, generally set to 1 (for full frame renders) or 0.5 (for fielded renders),
- ::kOfxPropIsInteractive  - is this a single frame render due to user interaction in a GUI, or a proper full sequence render.

The \e outArgs parameter is redundant and is NULL.

\pre
- ::kOfxActionCreateInstance has been called on the instance

\post
- the ::kOfxImageEffectActionEndSequenceRender action will be called on the instance

\return
- ::kOfxStatReplyDefault, do the default,
- ::kOfxStatOK, the action was trapped and all was well,
- ::kOfxStatFailed, the render action failed for some reason not documentable, the plugin should post a message,
- ::kOfxStatErrFatal, in which case we the program will be forced to quit,
- ::kOfxStatErrMemory, the render ran out of memory, in which case the host may call the render action again after a memory purge.

\par Default
- The default action is to continue on and render the sequence,

<HR>

@section ImageEffectsActionEndSequenceRender The End Sequence Render Action

::kOfxImageEffectActionEndSequenceRender is passed to an image effect after it has renderred a range of frames.

The \e handle parameter is the instance of the image effect that has just been renderred.

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropIsInteractive  - is this a single frame render due to user interaction in a GUI, or a proper full sequence render.

The \e outArgs parameter is redundant and is NULL.

\pre
- ::kOfxActionCreateInstance has been called on the instance

\post
- the ::kOfxImageEffectActionBeginSequenceRender action will be called on the instance

\return
- ::kOfxStatReplyDefault, do the default,
- ::kOfxStatOK, the action was trapped and all was well,
- ::kOfxStatFailed, the render action failed for some reason not documentable, the plugin should post a message,
- ::kOfxStatErrFatal, in which case we the program will be forced to quit,
- ::kOfxStatErrMemory, the render ran out of memory, in which case the host may call the render action again after a memory purge.

\par Default
- The default action is to continue on and render the sequence,

<HR>

@section ImageEffectsActionIsIdentity The Is Identity Action

::kOfxImageEffectActionIsIdentity is passed to an image effect to see if it can avoid processing it's inputs, for example, a blur with blur radius of 0. The time and window to render 
are in the \e inArgs parameter.

If the effect can pass an input through unprocessed, it should set the ::kOfxPropName property on the \e outArgs parameter
to be the name of the clip that is to be passed through. 

If the effect needs to process it's inputs, it should leave the ::kOfxPropName property in the \e outArgs parameter to it's default value of an empty string, or return ::kOfxStatReplyDefault.

The \e handle parameter is the instance of the image effect that is to be renderred.

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropTime - the time to test for identity at
- ::kOfxImageEffectPropFieldToRender - the field to test for identity
- ::kOfxImageEffectPropRenderWindow - the window (in \ref PixelCoordinates) to test for identity under
- ::kOfxImageEffectPropRenderScale - the scale factor being applied to the images being renderred

The \e outArgs handle has the following properties that can be set...
- ::kOfxPropName this to the name of the clip that should be used if the effect is an identity transform, defaults to the empty string
- ::kOfxPropTime the time to use from the indicated source clip as an identity image (allowing time slips to happen), defaults to the time on inArgs

\pre
- ::kOfxActionCreateInstance has been called on the instance

\return
- ::kOfxStatReplyDefault, the default action should be taken
- ::kOfxStatOK, the action was trapped and the outArgs contains the relevant values
- ::kOfxStatFailed, the is identity action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the identity action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
- The default is to have the effect render at the time in the given window

<HR>

@section ImageEffectsActionGetRegionOfDefinition The Get Region Of Definition Action

The ::kOfxImageEffectActionGetRegionOfDefinition action asks the effect to set its Region of Definition, in the Cannonical Coordinate system. The Region Of
Definition is the maximum region of the image plane that the effect can fill.

The \e handle parameter is an instance of the image effect,

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropTime - the time to ask for a RoD

The \e outArgs handle has the following properties that can be set...
- ::kOfxImageEffectPropRegionOfDefinition, the region of definition of the plugin, this is initially set by the host to the default RoD, explained below

\pre
- ::kOfxActionCreateInstance has been called on the instance

\return
- ::kOfxStatReplyDefault,  the default action should be taken
- ::kOfxStatOK, the action was trapped and the RoD was set
- ::kOfxStatFailed, the  action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
The default will depend on the context of the effect, see \ref ImageEffectContexts.
- The default RoD for a generator is the project window 
- The default RoD for a filter effect and retimer effect is the RoD of its 'Source' input
- The default RoD for a transition effect is the union of the RoDs of the 'SourceFrom' and 'SourceTo' inputs
- The default RoD for a paint effect is the RoD of its 'Source' input
- The default RoD for a general effect is the union of the RoDs of all the effect's non optional inputs, if none exist, then it is the project window

<HR>

@section ImageEffectsActionGetRegionsOfInterest The Get Region Of Interest Action

The ::kOfxImageEffectActionGetRegionsOfInterest action asks the effect what regions of each input image is required to process a given window at a given time.

For example a simple per pixel image processing effect would simply want the same window from its inputs, while a blur effect would want a window that was grown by the blur's radius.

Note that if a plugin sets its ::kOfxImageEffectPropSupportsTiles to false, then this action is really redundant, as the effect is asking that to be passed the entire Region of Definition of each input.

The \e outArgs param has a set of properties, one for each input clip, named "OfxImageClipPropRoI_" with the name of the clip post-pended. For example "OfxImageClipPropRoI_Source". All these properties are 4 dimensional doubles. For example...

\verbatim
  OfxRectD sourcRoI, maskRoI;
  ...
  gPropHost->propSetDoubleN(outArgs, "OfxImageClipPropRoI_Source", 4, &sourcRoI.x1);
  gPropHost->propSetDoubleN(outArgs, "OfxImageClipPropRoI_Mask", 4, &maskRoI.x1);

\endverbatim

The \e handle parameter is an instance of the image effect,

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropTime - the time to ask for a RoI
- ::kOfxImageEffectPropRegionOfInterest - the window (in Cannonical Coordinates) to get the RoI for each input

The \e outArgs handle has the following properties that are to be set...
- one double X 4 property for each input clip currently attached, labelled with "OfxImageClipPropRoI_" post pended with the clip's name.
  These will all be set by the host to have a default value of the RoI passed in

\pre
- ::kOfxActionCreateInstance has been called on the instance

\return
- ::kOfxStatReplyDefault, the default action should be taken
- ::kOfxStatOK, the action was trapped and the RoI were set
- ::kOfxStatFailed, the  action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
- The default action is to set the RoI on each input to be the window passed in on the inArgs parameter

<HR>

@section ImageEffectsActionGetTimeDomain The Get Time Domain Action

The ::kOfxImageEffectActionGetTimeDomain action asks an effect how many frames it is defined for, depending on the effect, this will generally depend upon the state of it's parameters and the temporal offset and duration of any input clips. 

This action is only called in the general context, in all other contexts the host controls an effect's frame range.

The \e handle parameter is an instance of the image effect,

The \e inArgs parameter is redunant and is set to null.

The \e outArgs handle has the following properties that can be set...
- ::kOfxImageEffectPropFrameRange - the frame range an effect can produce images for

\pre
- ::kOfxActionCreateInstance has been called on the instance
- the effect has been created in the general effect context

\return
- ::kOfxStatReplyDefault,  the default action should be taken
- ::kOfxStatOK, the action was trapped and the RoD was set
- ::kOfxStatFailed, the  action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
The default is,
  - the union of all the frame ranges of the non optional input clips,
  - infinite if there are no non optional input clips.

<HR>

@section ImageEffectsActionGetFramesNeeded The Get Frames Needed Action

The ::kOfxImageEffectActionGetFramesNeeded action asks the effect what frames are needed from each input clip to process a given frame.

For example a temporal based degrainer may need several frames around the frame to render to do its work.

This action need only ever be called if the plugin sets the ::kOfxImageEffectPropTemporalClipAccess property on the plugin handle to be true. Otherwise it is assumed that the only frame needed from the inputs is the current one.

The \e outArgs param has a set of properties, one for each input clip, named "OfxImageClipPropFrameRange_" with the name of the clip post-pended. For example "OfxImageClipPropFrameRange_Source". All these properties are 2 dimensional doubles. For example...

\verbatim
  double rangeSource[2], rangeMask[2];
  ...
  gPropHost->propSetDoubleN(outArgs, "OfxImageClipPropFrameRange_Source", 2, rangeSource);
  gPropHost->propSetDoubleN(outArgs, "OfxImageClipPropFrameRange_Mask", 2, rangeMask);

\endverbatim

The \e handle parameter is an instance of the image effect,

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropTime - the time at which a frame will be needed to be renderred

The \e outArgs handle has the following properties that are to be set...
- one double X 2 property for each input clip currently attached, labelled with "OfxImageClipPropFrameRange_" post pended with the clip's name. The firsy dimension is the first frame needed, the second dimension is the last frame needed.  These will all be set by the host to have the default frame range.

\pre
- ::kOfxActionCreateInstance has been called on the instance,
- ::kOfxImageEffectPropTemporalClipAccess has been set on the plugin's handle.

\return
- ::kOfxStatReplyDefault, the default action should be taken
- ::kOfxStatOK, the action was trapped and the RoI were set
- ::kOfxStatFailed, the  action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
- The default action is to set the frame range to be a single frame, which is the one passed in on the inArgs parameter.

<HR>

@section ImageEffectsActionGetClipPreferences The Get Clip Preferences Action

This action allows a plugin to dynamically specify its preferences for input and output clips. Please see \ref ImageEffectClipPreferences for more details on the behaviour.

\b Note:
- the pixel preferences are constant for the duration of an effect, so this action need only be called once per clip, not once per frame.

The \e handle parameter is an instance of the image effect,

The \e inArgs parameter is redundant an is the NULL pointer.

The \e outArgs handle has the following properties that are to be set...
- a set of char * X 1 properties, one for each of the input clips currently attached and the output clip, labelled with "OfxImageClipPropComponents_" post pended with the clip's name. This must be set to one of the component types which the host supports and the effect stated it can accept on that input,
- a set of char * X 1 properties, one for each of the input clips currently attached and the output clip, labelled with "OfxImageClipPropDepth_" post pended with the clip's name. This must be set to one of the pixel depths both the host and plugin supports,
- a set of double X 1 properties, one for each of the input clips currently attached and the output clip, labelled with "OfxImageClipPropPAR_" post pended with the clip's name. This is the pixel aspect ratio of the input and output clips. This must be set to a positive non zero double value,
- ::kOfxImageEffectPropFrameRate , the frame rate of the output clip, this must be set to a positive non zero double value,
- ::kOfxImagePropPixelAspectRatio , the fielding of the output clip,
- ::kOfxImageEffectPropPreMultiplication , the premultiplication of the output clip,
- ::kOfxImageClipPropContinuousSamples, whether the output clip can produce different images at non-frame intervals, defaults to false,
- ::kOfxImageEffectFrameVarying, whether the output clip can produces different images at different times, even if all parameters and inputs are constant, defaults to false.

All the properties on outargs are set to their default values when the action is called, these are specified in \ref ImageEffectClipPreferences.

\pre
- ::kOfxActionCreateInstance has been called on the instance

\return
- ::kOfxStatReplyDefault, the default action should be taken
- ::kOfxStatOK, the action was trapped and the pixel preferences were set
- ::kOfxStatFailed, the  action failed for some reason not documentable, the plugin should post a message
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, the action ran out of memory, in which case the host may call the render action again after a memory purge

\par Default
- The default action is the same as if ::kOfxImageEffectPropClipMapping was set to ::kOfxImageClipMappingDefault.

 */

/** @page ImageEffectsRendering OFX : Rendering Image Effects

@section ImageEffectsRenderingIntro Introduction

The ::kOfxImageEffectActionRender action is passed to plugins, when the host requires them to render an output frame. For more information on exactly what the arguments to the render action are, see \ref ImageEffectsActionRender. Briefly, the 'handle' is an handle to the instance, the inArgs contain the time to render at and the window to render, while the outArgs is redundant.

@section ImageEffectsRenderingSequences Rendering Sequences

All calls to the ::kOfxImageEffectActionRender are bracketed by a pair of ::kOfxImageEffectActionBeginSequenceRender and ::kOfxImageEffectActionEndSequenceRender actions. This is to allow plugins to prepare themselves for rendering long sequences by setting up any tables etc.. it may need to.

The ::kOfxImageEffectActionBeginSequenceRender will indicate the frame range that is to be renderred, and whether this is purely a single frame render due to interactive feedback from a user in a GUI.

<HR>

@section ImageEffectsRenderingIsIdentity Identity Effects

If an effect does nothing to its inputs (for example a blur with blur size set to '0') it can indicate that it is an identity function via the ::kOfxImageEffectActionIsIdentity action. The plugin indicates which input the host should use for the region in question. This allows a host to short circuit all sorts of processing.

A host must call this action prior to rendering a region, and if a plugin indicates that it is an identity, then the host must not call the plugin's render action for that region.

<HR>


@section ImageEffectsRenderingRegions Rendering and The Region Actions

Many hosts attempt to minimise the areas that they render by using regions of interest and regions of definition, while some of the simpler hosts do not attempt to do so. In general the order of actions, per frame renderred, is something along the lines of....
  -# ask the effect for it's region of definition,
  -# ask the effect for the regions of interest of each of it's inputs,
  -# clip those regions of interest against the region of definition of each of those inputs,
  -# rendering and caching each of those inputs,
  -# get the effect to render it's region of definition, passing in the cached inputs as images.

A host can ask an effect to render an arbitrary window of pixels, generally these should be clipped to an effect's region of definition, however, depending on the host, they may not be. The actual region to render is indicated by the ::kOfxImageEffectPropRenderWindow.

If an effect is asked to render outside of its region of definition, it should fill those pixels in with black transparent pixels.

Depending on the host, an effect may be passed input images that are different to the regions of interest is specified, effects should be prepared for this. Any pixels outside of an input's region of definition will be set to be black and transparent.

Note thate OfxImageEffectSuiteV1::clipGetImage function takes an optional \e region parameter. This is a region, in \ref CannonicalCoordinates that the effect would like on that input clip. If not used in a render action, then the image returned will be based on the previous get region of interest action. If used, then the image returned will be based on this (usually be clipped to the input's region of definition). Generally a plugin should not use the \e region parameter in the render action, but should leave it to the 'default' region.

<HR>

@section ImageEffectsMultiThreadingRendering Multi-threaded Rendering

Multiple render actions may be passed to an effect at the same time. A plug-in states it's level of render thread safety by setting the ::kOfxImageEffectPluginRenderThreadSafety string property. This can be set to one of three states....
   - ::kOfxImageEffectRenderUnsafe       - indicating that only a single 'render' action can be made at any time amoung all instances,
   - ::kOfxImageEffectRenderInstanceSafe - indicating that any instance can have a single 'render' action at any one time,
   - ::kOfxImageEffectRenderFullySafe    - indicating that any instance of a plugin can have multiple renders running simultaneously

@subsection ImageEffectsSMPRendering Rendering in a Symetric Multi Processing Enviroment

When rendering on computers that have more that once CPU (or this new-fangled hyperthreading), hosts and effects will want to take advantage of all that extra CPU goodness to speed up rendering. This means multi-threading of the render function in some way. 

If the plugin has set ::kOfxImageEffectPluginRenderThreadSafety to ::kOfxImageEffectRenderFullySafe, the host may choose to render a single frame across multiple CPUs by having each CPU render a different window. However, the plugin may wish to remain in charge of multithreading a single frame. The plugin set property ::kOfxImageEffectPluginPropHostFrameThreading (int X 1, 0 or 1) informs the host as to whether the host should perform SMP on the effect. It can be set to either...
     - 1, in which case the host will attempt to multithread an effect instance by calling it's render function called simultaneously, each call will be with a different renderWindow, but be at the same frame
     - 0, in which case the host only ever calls the render function once per frame. If the effect wants to multithread it must use the OfxMultiThreadSuite API.

A host may have a render farm of computers. Depending exactly how the host works with it's render farm, it may have multiple copies on an instance spread over the farm rendering separate frame ranges, 1-100 on station A, 101 to 200 on station B and so on...

@subsection ImageEffectsSequentialRendering Rendering Sequential Effects

Some plugin's need to cache interframe behaviour to work correctly. Eg: a particle system, an image stabilisation system. In such cases a plugin instance will need to be rendered sequentially from frame 0 on a single computer (though it might be able to SMP a single frame over several CPUs). Such plugins should set the ::kOfxImageEffectPluginPropSequentialRender (int X 1, 0 or 1). This property can be set to...
     - 0, in which case the host can render an instance over arbitrary frame ranges on an arbitrary number of computers without any problem (default),
     - 1, in which case the host must render an instance on a single computer over it's entire frame range, from first to last.

Hosts may still render sequential effects with random frame access, typically in interactive sessions. In such cases, the plugin can detect that the instance is being interactively manipulated and hack an approximation together for UI purposes. When eventually rendering the sequence, the host \em must ignore all frames rendered out of order and not cache them for use in the final result.

<HR>

@section ImageEffectsFieldRendering OFX : Fields and Field Rendering

Fields are evil, but until the world decides to adopt sensible video standard and casts the current ones into the same pit as 2 inch video tape, we are stuck with them.

Before we start, some nomenclature. The Y-Axis is considerred to be up, so in a fielded image,
  - even scan lines 0,2,4,6,... are collectively referred to as the lower field,
  - odd scan lines 1,3,5,7... are collective referred to as the upper field.

We don't call them odd and even, so as to avoid confusion with video standard, which have scanline 0 at the top, and so have the opposite sense of our 'odd' and 'even'.

Clips and images from those clips are flagged as to whether they are fielded or not, and if so what is the spatial/temporal ordering of the fields in that image. The ::kOfxImageClipPropFieldOrder (string X 1) property can be...
  - ::kOfxImageFieldNone  - the material is unfielded
  - ::kOfxImageFieldLower - the material is fielded, with scan line 0,2,4.... occuring first in a frame
  - ::kOfxImageFieldUpper - the material is fielded, with scan line 1,3,5.... occuring first in a frame

Images extracted from a clip flag what their fieldedness is with the property ::kOfxImagePropField (string X 1), this can be....
  - ::kOfxImageFieldNone  - the image is an unfielded frame
  - ::kOfxImageFieldBoth  - the image is fielded and contains both interlaced fields 
  - ::kOfxImageFieldLower - the image is fielded and contains a single field, being the lower field (lines 0,2,4...)
  - ::kOfxImageFieldUpper - the image is fielded and contains a single field, being the upper field (lines 1,3,5...)

When rendering unfielded footage, the host will only ever call the effect's render function once per frame, with the time being integers, 0.0, 1.0, 2.0 ....

The plugin specifies how it deals with fielded imagery by setting the ::kOfxImageEffectPluginPropFieldRendeTrwiceAlways (int X 1) property. This can be,
      - 0 - the plugin is to have it's render function called twice only if there is animation in any of it's parameters
      - 1 - the plugin is to have it's render function called twice always (default) 

The reason for this is an optimisation. Imagine a text generator with no animation being asked to render into a fielded output clip, it can treat an interlaced fielded image as an unfielded frame. So the host can get the effect to render both fields in one hit and save on the overhead required to do the rendering in two passes.

If called twice per frame, the time passed to the render function will be f and f+0.5. So 0.0 0.5 1.0 1.5 etc...

During the render call, the host set instance property ::kOfxImageEffectInstancePropFieldToRender (string X 1) tells the effect which field it should render, this can be one of...
      - ::kOfxImageFieldNone  - there are no fields to deal with, the image is full frame
      - ::kOfxImageFieldBoth  - the imagery is fielded and both scan lines should be renderred
      - ::kOfxImageFieldLower - the lower field is being rendered (lines 0,2,4...)
      - ::kOfxImageFieldUpper - the upper field is being rendered (lines 1,3,5...)

\b Note: 
::kOfxImageFieldBoth will only be set if ::kOfxImageEffectPluginPropFieldRenderTwiceAlways is set to 0 on the plugin,

When fetching an image from a fielded clip the host will do one of the following, depending on the plugin set clip property ::kOfxImageClipPropFieldExtraction (string X 1)
     - ::kOfxImageFieldBoth    - fetch a full frame interlaced image
     - ::kOfxImageFieldSingle  - fetch a single field, making a half height image
     - ::kOfxImageFieldDoubled - fetch a single field, but doubling each line and so making a full height image (default)

If fetching a single field, the actual field fetched from the source frame is...
  - the first temporal field if the time passed to clipGetImage has a fractional part of 0.0 <= f < 0.5
  - the second temporal field otherwise,

To illustrate this last behaviour, the two examples below show an output with twice the frame rate of the input and how clipGetImage maps to the input. The .0 and .5 mean first and second temporal fields.

@verbatim
Behaviour with unfielded footage

output 0       1       2       3
source 0       0       1       1
@endverbatim

@verbatim
Behaviour with fielded footage

output 0.0 0.5 1.0 1.5 2.0 2.5 3.0 3.5            
source 0.0 0.0 0.5 0.5 1.0 1.0 1.5 1.5
@endverbatim

\b Note:
- while some rarely used video standards can have odd number of scan-lines, under OFX, both fields \em always consist of the same number of lines. Pad with black where needed.
- host developers, for single field extracted images, you don't need to do any buffer copies, you just need to set the row bytes property of the returned image to twice the normal value, and maybe tweak the start address by a scanline.

<HR>

@section ImageEffectsRenderingAnalysis Analysing Sequences 

Some effects are multipass, in that they need to analyse an entire sequence before they can render the final output. For example an effect that performs wire removal may need to track the wire across the sequence first, have the user fix up any glitches in the tracking, then render the final output. Typically such effects write information back into a sub set of parameters during the analysis phase, which they then later use during the rendering.

The sequence of events used to perform an analysis on most hosts is...
-# set the effect up for analysis,
-# analyse entire sequence of input frames writing values into parameters as we go,
-# user interaction of verify analysis, the last two steps may be repeated several times,
-# set up effect for rendering,
-# finished frames are rendered.

Within OFX this is done by having...
-# the user changing some parameter to indicate the effect will be performing an analysis, this parameter has a call back that then sets the instance property ::kOfxImageEffectPropInAnalysis to true,
-# the user rendering the sequence of frames to be analysed, 
  - the normal OFX begin sequence/render/actions are called,
  - the effect analyses input and writes various parameters and updates various GUIS,
  - the effect should render something into the output clip for the analysis, even if it is just a copy of the input,
-# user verifying the analysis by looking at the parameter values and the analysis output clip,
-# user turning off the 'analysis' parameter, the call back of which sets the instance property ::kOfxImageEffectPropInAnalysis to be false,
-# the user triggering the final render of the sequence, the plugin uses the analysis values to do the final render.

If flagged as analysing, hosts should not attempt to cache frames based on parameter values, as the analysis could get you into a silly recursive loop of some form. The host attempting to re-render a frame because it's parameter value has changed, which causes the parameter value to change, so it re-renders etc... That sort of thing.

QUESTION
 - should OFX allow random frame access from the user interface actions? If so, an effect could define an 'analyse sequence' push button, the user simply presses that and the instance changed action would do a for loop over the input frames analysing them. This would do away with the analysis mode completely. How many hosts support this sort of thing?

<HR>

@section ImageEffectsRenderingAndGUIS Rendering In An Interactive Environment

Any host with an interface will most likely have an interactive thread and a rendering thread. This allows an effect to be manipulated while having renders batched off to a background thread. This will mean that some degree of locking will go on to prevent simultaneous read/writes occuring, see \ref ImageEffectsThreadSafety for more on thread safety.

A host may need to abort a backgrounded render, typically in response to a user changing a parameter value. An effect should occasionally poll the OfxImageEffectSuiteV1::abort function to see if it should give up on rendering.

*/

/** @page ImageEffectContexts OFX : Image Effect Effect Contexts

    How an image effect is intended to be used by an end user affects how it should interact with a host application. For example an effect that is to be used as a transition between two clips works differently to an effect that is a simple filter. One must have two inputs and know how much to mix between the two input clips, the other has fewer constraints on it. Within OFX we have standardised several different uses and have called them \b contexts. 

    An immage effect plugins can also be used in several different contexts, for example, this allows the same plugin to be used as a no input image generator, a single input 'filter' effect and a multiple input tree effect. 

    More specifically, a context mandates certain behaviours from an effect when it is described or instantiated in that context. The major issue is the number of input clips it takes, and how it can interact with those input clips.

The current contexts supported are,
    - generator, 
       - no compulsory input clips,       
       - used by a host to create imagery from scratch, eg: a noise generator,
    - a filter
       - a single compulsory input clip,
       - a traditional 'filter effect', similar to an After Effects plugin,
    - a transition
       - two compulsory input clips and a compulsory 'Transition' double parameter,
       - used by a host to perform transitions between clips, eg: a dissolve,
    - paint effect
       - two compulsory input clips, one image to paint onto, the other a mask to control where the effect happens,
       - used by hosts to use an effect in a paint brush,
    - retimer
       - single compulsory input clip, and a compulsory 'SourceTime' double parameter,
       - used by a host to change the speed of a clip,
    - general effect
        - arbitrary number of inputs
	- generally used in a 'tree' compositing environment
	- catch all context.

A plugin tells the host what contexts it can work in during it's describe action. This is done via the plugin property ::kOfxImageEffectPropSupportedContexts. A plugin can set from 1 to 6 contexts it is willing to work in, these are defined via the ::kOfxImageEffectContext* constants.

Plugins set their per context behaviour via the  ::kOfxImageEffectActionDescribeInContext action. The host then calls this once per context, labelling which context it is being called in. Any context dependant aspects of the plugin are declare in here, and is where all clips and parameters \em must be defined, both the mandated and non-mandated ones.

A plugin instance is created in a specific context. That context will not changed over the lifetime of that instance. The context can be retrieved from the instance via the ::kOfxImageEffectPropContext property handle.

Each context mandates certain behaviours for an instance operating in that context. This covers the inputs to the effect and the manner in which certain actions can behave.

<HR>

@section ImageEffectContextGenerator Generator Context

A generator context is for cases where a plugin can create images without any input clips, eg: a colour grad effect. 

In this context, a plugin has the following mandated clips,
     - an output clip named 'Output'

Any input clips that are specified must be optional.

A host is responsible for setting the initial preferences of the output clip, it must do this in a manner that is transparent to the plugin. So the pixel depths, components, fielding, frame rate and pixel aspect ratio are under the control of the host. How it arrives at these is a matter for the host, but as a plugin specifies what components it can produce on output, as well as the pixel depths it supports, the host must choose one of these. 

Generators still have Regions of Definition. This should generally be,
- based on the project size eg: an effect that renders a 3D sky simulation,
- based on parameter settings eg: an effect that renders a circle in an arbitrary location,
- infinite, eg: a noise generator.

The host should take into consideration the RoD of the effect when rendering the output clip.

The pixel preferences action is constrained in this context by the following,
  - a plugin cannot change the component type of the 'Output' clip,

<HR>

@section ImageEffectContextClipFilter Filter Effect Context

A filter effect is the ordinary way most effects are used on a single input (think 'After Effects' plugin).

In this context, a plugin has the following mandated objects...
     - an input clip named 'Source' 
     - an output clip named 'Output'

Any other input clips that are specified must be optional.

The pixel preferences action is constrained in this context by the following,
  - a plugin cannot change the component type of the 'Output' clip, it will always be the same as the 'Source' clip,

<HR>

@section ImageEffectContextTransition Transition Context

Transitions are effects that blend from one clip to another over time, eg: a wipe or a cross dissolve.

In this context, a plugin has the following mandated objects...
     - an input clip names 'SourceFrom' 
     - an input clip names 'SourceTo' 
     - an output clip named 'Output'
     - a single double parameter called 'Transition'

Any other input clips that are specified must be optional.

The 'Transition' parameter cannot be labelled, positioned or controlled by the plug-in in anyway, it can only have it's value read, which will have a number returned between the value of 0 and 1.

The pixel preferences action is constrained in this context by the following,
  - the component types of the "SourceFrom", "SourceTo" and "Output" clips will always be the same,
  - the pixel depths of the "SourceFrom", "SourceTo" and "Output" clips will always be the same,
  - a plugin cannot change any of the pixel preferences of any of the clips.

<HR>

@section ImageEffectContextPaint Paint Context

Paint effects are effects used inside digital painting system, where the effect is limitted to a small area of the source image via  a masking image.

In this context, a plugin has the following mandated objects...
     - an input clip names 'Source' ,
     - an input clip names 'Brush', the only component type it supports is 'alpha',
     - an output clip named 'Output'.

Any other input clips that are specified must be optional.

The masking images consists of pixels from 0 to the white point of the pixel depth. Where the mask is zero the effect should not occur, where the effect is 1 the effect should be 'full on', where it is grey the effect should blend with the source in some manner.

The masking image may be smaller than the source image, even if the effect states that it cannot support multi-resolution images.

The pixel preferences action is constrained in this context by the following,
  - the pixel depths of the "Source", "Brush" and "Output" clips will always be the same,
  - the component type of "Source" and "Output" will always be the same,
  - a plugin cannot change any of the pixel preferences of any of the clips.

<HR>

@section ImageEffectContextRetimer Retimer Context

The retimer context is for effects that change the length of a clip by interpolating frames from the source clip to create an inbetween output frame.

This context has the following mandated objects... 
     - an input clip names 'Source' 
     - an output clip named 'Output'
     - a 1D double parameter named 'SourceTime"

Any other input clips that are specified must be optional.

The 'SourceTime' parameter cannot be labelled, positioned or controlled by the plug-in in anyway, it can only have it's value read. Its value is the source time to map to the output time. So if the output time is '3' and the 'SourceTime' parameter returns 8.5 at this time, the resulting image should be an interpolated between source frames 8 and 9.

The pixel preferences action is constrained in this context by the following,
  - the pixel depths of the "Source" and "Output" clips will always be the same,
  - the component types of the "Source" and "Output" clips will always be the same,
  - a plugin cannot change any of the pixel preferences of any of the clips.

<HR>

@section ImageEffectContextGeneral General Context

The general context is to some extent a catch all context, but is generally how a 'tree' effect should be instantiated. It has no constraints on the clips it needs on input, nor on the pixel preferences actions.

In this context, has the following mandated objects...
     - an output clip named "Output"


*/

/** @page ImageEffectGUIs OFX : Image Effect User Interfaces

Effects have user interfaces where their parameters and other properties are presented to users. The inteface to an effect's parameters is described in \ref ParametersInterfaces. 

@section ImageEffectOverlays Overlay Interacts

Hosts will display images (both input and output) in user their interfaces. A plugin can draw tools in this display by setting the ::kOfxImageEffectPluginPropOverlayInteractV1 property to point to a OfxInteractClientStructV1. For more details on interacts, see \ref CustomInteractionPage. This interact is meant to 

The viewport for such interacts will depend completely on the host.
 
The GL_PROJECTION matrix will be set up so that it maps openGL coordinates into the image plane (see \ref ImageEffectsImagePlane).

The GL_MODELVIEW matrix will be the identity matrix.

Other events that an overlay interact will be passed are...
   - ::kOfxImageEffectEventChangedClip if the currently viewed clip has been wired to a new clip

Other properties that on an overlay interact instance are...
   - ::kOfxImageEffectOverlayPropClipViewed for the clip that is currently being viewed in the interact

*/

/** @page ImageEffectsProcessingArchitecture OFX : Image Processing Architectures

@section ImageEffectsProcessingArchitectureIntro Introduction

OFX supports a range of image processing architectures. The simpler ones being special cases of the most complex one. Levels of support, in both plug-in and host, are signalled by setting appropriate properties in the plugin and host.

@subsection ImageEffectsImagePlane The Image Plane

At it's most generalised, OFX allows for a complex imaging architecture based around an infinite 2D plane on which we are filling in pixels.

Firstly, there is some subsection of this infinite plane that the user wants to be the end result of their work, call this the project extent. The project extent is always rooted, on its bottom left, at the origin of the image plane. The project extent defines the upper right hand corner of the project window. For example a PAL sized project spans (0, 0) to (768, 576) on the image plane. 

The project extent is retrieved via an image effect's instance's ::kOfxImageEffectPropProjectExtent property. For more on project sizes see \ref CoordinateSystemsProjects.

We define an image effect as something that can fill in a rectangle of pixels in this infinite plane.

An effect takes an arbitrary number of inputs (0->N), some of these may be optional. The inputs to an effect may be the outputs from other effects, or direct from some source imagery.

@subsection ImageEffectsRoD Regions Of Definition

An effect has a "Region of Definition" (RoD), this is is the maximum area of the plane that the effect can fill in. for example: a 'read source media' effect would only be able to fill an area as big as it's source media. An effect's RoD may need to be based on the RoD of any inputs, for example: the RoD of a contrast/brightness colour corrector would generally be the RoD of it's input, while the RoD of a rotation effect would be bigger than that of it's input.

Infinite RoDs are valid, eg: a no-input noise generator could generate random colours anywhere it was asked to.

An effect's RoD is constant for a given set of inputs and plugin parameters.

An infinite RoD is flagged by setting the minimums to be ::kOfxFlagInfiniteMin and the maxmimums to be ::kOfxFlagInfiniteMax, for both double and integer rects.

@subsection ImageEffectsRoI Regions Of Interest and Rendering

An effect will be asked to fill in some region of this infinite plane. The section it is being asked to fill in is called the "Region of Interest" (RoI). 

Before an effect has been asked to process a given RoI, it will be asked to specify the area of each input it will need to process that RoI. For example: a Colour Correction effect only needs as much input as it does output, while a blur will need an area that is larger than the specified RoI by a border of the same width as the blur radius.

When actually asked to render, the image data supplied for the effect's inputs may either not exist, or be a subset of the regions previously requested by the effect (because they may have been clipped to the inputs' own RoDs).

The image data supplied to the effect from it's inputs will be some contiguous chunk of the 2D plane. These may be bigger and or small than the RoI requested from the inputs. Also, they may be a sub-section of the inputs' own RoDs, due to the host attempting to tile rendering to reduce memory and CPU demands.

The image data supplied to the effect to be filled in will be some contiguous chunk of the 2D plane. This may be bigger (but never smaller) than the RoI that the effect was requested to render.

@subsection ImageEffectsSimplerArchitectures Simpler Architectures

The above architecture is quite complex, as the inputs supplied can lie anywhere on the image plane, as can the output, an they can be subsections of the complete image. Not all hosts work in this way, generally it is only the more advance compositing systems working on large resolution images. An example is Apple's Shake compositing system.

Some other systems allow for images to be anywhere on the image plane, but always pass around full RoD images, never sub tiles. An example is Softimage's 'Eddie' compositor.

The simplest systems, don't have any of of the above complexity. The RoDs, RoIs, images and project sizes in such systems are exactly the same, always. An example is Discreet's FLAME compositor (prior to version 8).

Similarly, some plugin effects cannot handle sub RoD images, or even images not rooted at the origin.

The OFX architecture is meant to support all of them. 

Assuming a plugin supports the most general architecture, it will trivially run on hosts with simpler architectures.

However, if a plugin does not support sub-RoD images, or arbitarily positioned images, they may not run cleanly on hosts that expect them to do so.

To this end, two properties are provided that flag the capabilities of a plugin or host .....
   - ::kOfxImageEffectPropSupportsMultiResolution - which indicates support for images of differing sizes not centred on the origin.
   - ::kOfxImageEffectPropSupportsTiles - which indicates support for images that contain less than full frame pixel data

A plug-in should flag these appropriately, so that hosts know how to deal with the effect. A host can either choose to refuse to load the plugin, or, preferentially, pad images with an appropriate amount of black/transparent pixels.

The ::kOfxImageEffectActionGetRegionsOfInterest is redundant for plugins that do not support tiles, as the plugin is asking that it be given the full Region of Definition of all its inputs. A host may have difficulty doing this (for example with an input that is attached to an effect that can create infinite images such as a random noise generator), if so, it should clamp images to some a size in some manner.
*/

/** @page ImageEffectsCoordinates OFX : Coordinate Systems

As stated in \ref ImageEffectsImagePlane, images are simply some rectangle in a potentially infinite plane of pixels. However real pixels have to take into account pixel aspect ratios and proxy render scales, and will \em not be in the same space as the image plane.

@section CannonicalCoordinates Cannonical Coordinates
The image plane is always in a coordinate system of square unscaled pixels. For example a PAL D1 frame occupies (0,0) <-> (768,576). We call this the <B>Cannonical Coordinate System</B>.

@section PixelCoordinates Pixel Coordinates
'Real' images, where we have to deal with addressable pixels in memory, are in a coordinate system of non-square proxy scaled integer values. So a PAL D1 image, being renderred as a half resolution proxy would be (0,0) <-> (360, 288), which takes into account both the pixel aspect ratio of 1.067 and a scale factor of 0.5f. We call this the <B> Pixel Coordinate System</B>.

@section InfiniteRegions Infinite Regions
Sometimes a plug-in needs to specify an infinite region (eg: a noise generator flagging the fact that it has an infinite RoD). Such infinite regions, in pixel or cannonical coordinates are flagged by setting the minimums to be ::kOfxFlagInfiniteMin and the maxmimums to be ::kOfxFlagInfiniteMax, for both double and integer rects. Eg...

\verbatim
     outputRoD.x1 = kOfxFlagInfiniteMin;
     outputRoD.y1 = kOfxFlagInfiniteMin;
     outputRoD.x2 = kOfxFlagInfiniteMax;
     outputRoD.y2 = kOfxFlagInfiniteMax;
\endverbatim


@section MappingCoordinates Mapping Between Coordinate Systems
To map between the two the pixel aspect ratio and the render scale need to be known, and it is a simple case of multiplication and rounding. More specifically,  given...
- pixel aspect ratio, \e PAR, found on the image property ::kOfxImagePropPixelAspectRatio
- render scale in X \e SX, found on the first dimension of the effect property ::kOfxImageEffectPropRenderScale
- render scale in Y \e SY, found on the second dimension of the effect property ::kOfxImageEffectPropRenderScale
- field scale in Y \e FS, this is 
    - 0.5 if the image property ::kOfxImagePropField is ::kOfxImageFieldLower or ::kOfxImageFieldUpper
    - 1.0 otherwise.

To map an X coordinate from Pixel coordinates to Cannonical coordinates, we perform the following multiplication
  - X' = (X * PAR)/SX

To map a Y coordinate from Pixel coordinates to Cannonical coordinates, we perform the following multiplication
  - Y' = Y/(SY * FS)

To map from Cannonical to Pixel Coordinates you invert the operations above and truncate any fractional parts.

The Cannonical coordinate system is always referenced by floating point values.

The Pixel coordinate system is always referenced by integer values, generally via a OfxRectI structure. It is used when refering to operations on actual pixels, and so is what is passed to the OfxImageEffectPluginStructV1::render function, and what is returned by the OfxImageEffectSuiteV1::clipGetImage function.

@section CoordinateSystemsProjects Project Extents, Sizes and Offsets

As described in \ref ImageEffectsImagePlane, the project extent is the section of the image plane that is coverred by an image that is the desired output of the project, so for a PAL D1 project you get an extent of 0,0 <-> 768,576. The extent is always rooted at the origin, so the effect instance property ::kOfxImageEffectPropProjectExtent, only has two doubles in it, being the upper right hand corner of the extent, in \ref CannonicalCoordinates.

Project sizes and project offsets need a bit of ASCII art to explain (must make some TIFFs eventually). 

Consider a project that is going to be output as PAL D1 imagery, the extent will be 0,0<->768,576. However our example is a letter box 16:9 project, which leaves a strip of black at bottom and top. The size of the letter box is 768 by 432, while the bottom left of the letter box is offset from the origin by 0,77. The ASCII art below shows the details.....

\verbatim
                                                (768,576) 
             ---------------------------------------
             |                                     |
             |                BLACK                |
             |.....................................| (768, 504)
             |                                     |
             |                                     |
             |        LETTER BOXED IMAGERY         |
             |                                     |
             |                                     |
      (0,72) |.....................................|
             |                                     |
             |                BLACK                |
             |                                     |
             ---------------------------------------
	   (0,0)
             

\endverbatim

So in this example...
- the \b extent of the project is the full size of the output image, which is 768x576,
- the \b size of the project is the size of the letter box section, which is 768x432,
- the \b offset of the project is the bottom left corner of the project window, which is 0,72.

The following properties on an effect instance handle allow you to fetch these values...
- ::kOfxImageEffectPropProjectExtent for the extent of the current project,
- ::kOfxImageEffectPropProjectSize for the size of the current project,
- ::kOfxImageEffectPropProjectOffset for the offset of the current project.

These are useful when you have a parameter you want to be interpreted spatially, especially if the project containing the parameter is transformed to another type.

For example, an operator is working on a standard definition 4:3 project, saved out the project and passed it over
to another operator working on a 16:9 HD version of the project (not stretched to fit). It would look like a bit
like the ASCII art below,

\verbatim
                          ---------------------------------
                         |      :                   :      |
                         |      :                   :      |
      -------------      |      :                   :      |
     |             |     |   B  :                   :  B   |
     |             |     |   L  :                   :  L   |
     |  IMAGERY    | <-> |   A  :      IMAGERY      :  A   |
     |             |     |   C  :                   :  C   |
     | +           |     |   K  :                   :  K   |
      -------------      |      :  +                :      |
                         |      :                   :      |
                          ---------------------------------
\endverbatim

Our hypothetical effect draws a circle of a certain radius, the centre of which is
a 2D positional parameter, marked by the '+' in the ASCII art above. 

If raw parameter values from the SD project are used in the HD project, then the circle would be too small and in the wrong position.

Such spatially dependant parameters need to know about the project in question to behave nicely.

We use the ::kOfxParamPropDoubleType property to allow us to deal with this situation. It can be set to one of the following....
      - ::kOfxParamDoubleTypeNormalisedX - normalised size wrt to the project's X dimension (1D only),
      - ::kOfxParamDoubleTypeNormalisedXAbsolute - normalised absolute position on the X axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedY - normalised size wrt to the project's Y dimension(1D only),
      - ::kOfxParamDoubleTypeNormalisedYAbsolute - normalised absolute position on the Y axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedXY - normalised to the project's X and Y size (2D only),
      - ::kOfxParamDoubleTypeNormalisedXYAbsolute - normalised to the projects X and Y size, and is an absolute position on the image plane.

In our example we would set the radius to be ::kOfxParamDoubleTypeNormalisedX, fetch the value and scale that by the project size before we render the circle. The host should present such normalised parameters to the user in a 'sensible' range. So for a PAL project, it would be from 0..768, where the plug-in sees 0..1. 

The position can be handled by the ::kOfxParamDoubleTypeNormalisedXYAbsolute case. In which case the plugin must scale the parameter's value by the project size and add in the project offset. This will allow the positional parameter to be moved between projects transparently.  

\b Note:
- HACK! will nee a project stretch value for stretched projects, blergh!

 */

/** @page ImageEffectsProperties OFX : Image Effects Properties

This pages lists the properties on each objects used by the OFX Image Effects API, version 1.0

@section ImageEffectsPropertiesHost Host Properties

An image effect host has the following read only properties...
- ::kOfxPropType 
- ::kOfxPropName
- ::kOfxPropLabel
- ::kOfxImageEffectPropSupportsOverlays
- ::kOfxImageEffectPropSupportsMultiResolution
- ::kOfxImageEffectPropSupportsTiles
- ::kOfxImageEffectPropTemporalClipAccess
- ::kOfxImageEffectPropSupportedComponents
- ::kOfxImageEffectPropSupportedContexts
- ::kOfxImageEffectPropSupportsMultipleClipDepths
- ::kOfxImageEffectPropSupportsMultipleClipPARs
- ::kOfxImageEffectPropSetableFrameRate
- ::kOfxImageEffectPropSetableFielding

@section ImageEffectsPropertiesPlugin Plugin Properties

An image effect plugin has the following properties, these can only be set inside the 'describe' actions ...
- ::kOfxPropType 
- ::kOfxPropLabel
- ::kOfxPropShortLabel 
- ::kOfxPropLongLabel 
- ::kOfxImageEffectPropSupportedContexts
- ::kOfxImageEffectPluginPropGrouping
- ::kOfxImageEffectPluginPropSingleInstance
- ::kOfxImageEffectPluginRenderThreadSafety
- ::kOfxImageEffectPluginPropHostFrameThreading
- ::kOfxImageEffectPluginPropSequentialRender
- ::kOfxImageEffectPluginPropOverlayInteractV1
- ::kOfxImageEffectPropSupportsMultiResolution
- ::kOfxImageEffectPropSupportsTiles
- ::kOfxImageEffectPropTemporalClipAccess
- ::kOfxImageEffectPropSupportedPixelDepths
- ::kOfxImageEffectPluginPropFieldRenderTwiceAlways
- ::kOfxImageEffectPropSupportsMultipleClipDepths
- ::kOfxImageEffectPropSupportsMultipleClipPARs

@section ImageEffectsPropertiesInstance Instance Properties

An image effect instance has the following properties, all but kOfxPropInstanceData are read only...
- ::kOfxPropType 
- ::kOfxImageEffectPropContext *
- ::kOfxPropInstanceData *
- ::kOfxImageEffectPropProjectSize *
- ::kOfxImageEffectPropProjectOffset *
- ::kOfxImageEffectPropProjectExtent *
- ::kOfxImageEffectPropProjectPixelAspectRatio *
- ::kOfxImageEffectInstancePropEffectDuration *
- ::kOfxImageEffectPropFrameRate *
- ::kOfxPropIsInteractive *
- ::kOfxImageEffectFrameVarying * 

@section ImageEffectsPropertiesClip Clip Properties

An image effect clip has the following properties...
- ::kOfxPropType 
- ::kOfxPropName
- ::kOfxPropLabel
- ::kOfxPropShortLabel 
- ::kOfxPropLongLabel 
- ::kOfxImageEffectPropSupportedComponents
- ::kOfxImageEffectPropTemporalClipAccess
- ::kOfxImageClipPropOptional
- ::kOfxImageClipPropFieldExtraction
- ::kOfxImageClipPropIsMask 
- ::kOfxImageEffectPropSupportsTiles 
- ::kOfxImageEffectPropPixelDepth *
- ::kOfxImageEffectPropComponents *
- ::kOfxImageClipPropUnmappedPixelDepth *
- ::kOfxImageClipPropUnmappedComponents *
- ::kOfxImageEffectPropPreMultiplication *
- ::kOfxImagePropPixelAspectRatio *
- ::kOfxImageEffectPropFrameRate *
- ::kOfxImageEffectPropFrameRange *
- ::kOfxImageClipPropFieldOrder *
- ::kOfxImageClipPropConnected *
- ::kOfxImageEffectPropUnmappedFrameRange*
- ::kOfxImageEffectPropUnmappedFrameRate*
- ::kOfxImageClipPropContinuousSamples*

@section ImageEffectsPropertiesImage Image Properties

An image effect Image has the following read only properties...
- ::kOfxPropType  *
- ::kOfxImageEffectPropPixelDepth *
- ::kOfxImageEffectPropComponents *
- ::kOfxImageEffectPropPreMultiplication *
- ::kOfxImageEffectPropRenderScale *
- ::kOfxImagePropPixelAspectRatio *
- ::kOfxImagePropData *
- ::kOfxImagePropBounds *
- ::kOfxImagePropRowBytes *
- ::kOfxImagePropField *
- ::kOfxImagePropUniqueIdentifier*

@section ImageEffectsPropertiesInArgs Properties Used By the 'inArgs' Parameter 
- UNFINISHED
- ::kOfxPropTime

@section ImageEffectsPropertiesOutArgs Properties Used By the 'outArgs' Parameter 
- UNFINISHED

\b Note:
- the properties marked * can only be accessed outside the effect's describe action on instances or objects fetched from instances, they have undefined values in the describe actions.

 */

/** @page ImageEffectsOptionalBeetsPage OFX : Image Effects Mandatory and Optional Support

@section ImageEffectsOptionalBeetsIntro Introduction

This page documents the sections of OFX that are optional or mandatory for either host or plugin. If not documented here, a behaviour is considerred to be mandatory.

All optional behaviours are flagged via properties on plugins and hosts. These flags exist on the host's handle or the plugin's handle (not an instance handle). Plugins need to flag any optional behaviour in their 'describe' function, so hosts can deal with them. 

If a plugin needs a feature to work, it is recommended that the plugin enquires if that feature exists in it's describe function, if the host does not support the feature, the plugin should post an error message and return the ::kOfxStatErrMissingHostFeature error message. Hosts should then unload the plugin and ignore it.

If a plugin sets an unsupported feature in its describe function, a host should not this, let the describe function complete, post an error message and then unload and ignore the plugin.

@section ImageEffectsOptionalBeetsParameter Parameters

Animation on custom parameters is optional, a host flags support via ::kOfxParamHostPropSupportsCustomAnimation host property.

Custom interacts overriding the default interface for an individual parameters is optional, a host flags this with the ::kOfxParamHostPropSupportsCustomInteract host property.


@section ImageEffectsOptionalBeetsParameterPixelTypes Pixel Types 

All hosts and plugins must support a pixel depth of ::kOfxBitDepthByte . Support for other bit depths is optional. Hosts indicate which bitdepths they support via the ::kOfxImageEffectPropSupportedPixelDepths on the host handle. Plugins indicate which bitdepths they support by setting this same property on the plugin handle.

All hosts and plugins must support the ::kOfxImageComponentRGBA and ::kOfxImageComponentAlpha component types. If a host does not natively support the single channel option, it must behave to a plugin as if it does, extracting channels from it's native image format.


@section ImageEffectsOptionalBeetsParameterOverlay Image Effect Overlays

The support of image effect \ref ImageEffectOverlays is optional. A host flags its support for this via the ::kOfxImageEffectPropSupportsOverlays host property.

@section ImageEffectsOptionalBeetsParameterPixelPushing Image Effect Plugin Options

Plugins flag their support of tiled rendering via the ::kOfxImageEffectPropSupportsTiles plugin property.

Plugins flag their support of multi-resolution rendering via the ::kOfxImageEffectPropSupportsMultiResolution plugin property.

Plugins flag whether they will let hosts perform per frame SMP threading via the ::kOfxImageEffectPluginPropHostFrameThreading plugin property.

Plugins flag whether they need to be sequentially renderred via the ::kOfxImageEffectPluginPropSequentialRender plugin property.

Plugins flag whether only one instance of the plugin can exist at any one time via the ::kOfxImageEffectPluginPropSingleInstance plugin property.


\em UNFINISHED
 */

/** @page ImageEffectsExtendingBeetsPage OFX : Extending The API

@section ImageEffectsPageExtendingBeets Introduction

OFX is designed to be extended on both an individual host application level as well as the OFX level. This can be done via various mechanisms, all revolving around properties and suites.

All host extended properties \em must be labelled in the following manner,
@verbatim
   reverse domain name of manufacturer ":" hostSystem ":" property label
@endverbatim

eg:  "uk.co.thefoundry:TestHost:paramPropertyLayoutPosition"

Yes this is verbose, but it is unambiguous, so other host systems can identify and ignore any optional properties. 

@section ImageEffectsPageExtendingBeetsUI Host Specific Interface Tweaks

Firstly, hosts can freely extended the set of properties on OFX objects that relate to interface layout and behaviour. For example, a host system may add a property that indicates the position a parameter should appear in on a paged layout. 

However if the plugin does not set a host specific property, the host \em must still load and run the plugin as best it can.

\em UNFINISHED
 */

/** @page ImageEffectsKnownProblems OFX : Known Problems

@section ImageEffectsKnownProblemsIntro Introduction

This section covers known problems and ambiguities that currently exist in OFX.


@section ImageEffectsKnownProblemsRegionsOfInterestInTemporalAccessEffects Random Temporal Access and Regions of Interest

Effects that perform random temporal access on input clips have no way of indicating the regions of interest to the host outside the render action. 

Currently the regions of interest action assumes that an effect only wants input frames at a single time, that being the output time.

\em UNFINISHED
 */



#endif

