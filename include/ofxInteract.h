#ifndef _ofxInteract_h_
#define _ofxInteract_h_

#include "ofxCore.h"

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


#ifdef __cplusplus
extern "C" {
#endif

/** @file ofxInteract.h
Contains the API for ofx plugin defined GUIs and interaction.
*/

#define kOfxInteractSuite "OfxInteractSuite"


/** @brief Blind declaration of an OFX interactive gui 
*/
typedef struct OfxInteract *OfxInteractHandle;

/**
   \addtogroup PropertiesAll
*/
/*@{*/
/**
   \defgroup PropertiesInteract Interact Property Definitions

These are the list of properties used by the Interact API documented in \ref CustomInteractionPage.
*/
/*@{*/
/** @brief Interact instance property, the set of parameters on which a value change will trigger a redraw for the interact

    - string X N

    If the interact is representing the state of some OFX parameter, then is will
    need to be redrawn if that parameter's value changes. This multi-dimensional property
    links such parameters to the interact.

    The interact can be slaved to multiple parameters (setting index 0, then index 1 etc...)
 */
#define kOfxInteractPropSlaveToParam "OfxInteractPropSlaveToParam"

/** @brief Interact argument property, size of a real screen pixel under the interact's cannonical projection.

    - double X 2
    Read only property of an interact instance
 */
#define kOfxInteractPropPixelScale "OfxInteractPropPixelScale"

/** @brief Interact argument property, size of the instance's openGL viewport

    - int X 2 (x, y)
    - Read only property on the inArgs handle passed to an interacts main function
    - Read only property of an interact instance handle
 */
#define kOfxInteractPropViewportSize "OfxInteractPropViewport"

/** @brief Interact argument property, the background colour of the application behind an interact instance
    - double X 3
    Read only property of an interact instance
 */
#define kOfxInteractPropBackgroundColour "OfxInteractPropBackgroundColour"

/** @brief Interact property, pen position in cannonical coordinates

    - double X 2 (x, y)
    - Read only property on the inArgs handle passed to an interacts main function
 */
#define kOfxInteractPropPenPosition "OfxInteractPropPenPosition"

/** @brief Interact property, pressure of the pen, from 0.0 to 1.0f

    - double X 1, from 0 to 1
    - Read only property on the inArgs handle passed to an interacts main function
 */
#define kOfxInteractPropPenPressure "OfxInteractPropPenPressure"

/** @brief Interact instance and descriptor property, bit depth of the interact's openGL frame buffer

    - int X 1
    - number of bits per component in the openGL frame buffer
 */
#define kOfxInteractPropBitDepth "OfxInteractPropBitDepth"

/** @brief Interact instance and descriptor property, whether the frame buffer includes and alpha component or not

    - int X 1, 0 or 1
    - 0 indicates no alpha component
    - 1 indicates an alpha component
 */
#define kOfxInteractPropHasAlpha "OfxInteractPropHasAlpha"

/*@}*/
/*@}*/

/**
   \addtogroup ActionsAll
*/
/*@{*/
/**
   \defgroup InteractActions Intereact Actions

These are the list of actions passed to an interacts' entry point function. For more details on how to deal with actions, see \ref InteractActions.
*/
/*@{*/

/** @brief Action passed to interacts telling it to redraw, see \ref InteractsActionDraw for more details */
#define kOfxInteractActionDraw "OfxInteractActionDraw"

/** @brief Action passed to interacts for an interact pen motion , see \ref InteractsActionPen for more details
 */
#define kOfxInteractActionPenMotion "OfxInteractActionPenMotion"

/**@brief Action passed to interacts for a pen down , see \ref InteractsActionPen for more details
 */
#define kOfxInteractActionPenDown   "OfxInteractActionPenDown"

/**@brief Action passed to interacts for a pen up, see \ref InteractsActionPen for more details
 */
#define kOfxInteractActionPenUp     "OfxInteractActionPenUp"

/**@brief Action passed to interacts for a key down, see \ref InteractsActionKey for more details
 */
#define kOfxInteractActionKeyDown   "OfxInteractActionKeyDown"

/**@brief Action passed to interacts for a key down, see \ref InteractsActionKey for more details

 */
#define kOfxInteractActionKeyUp     "OfxInteractActionKeyUp"

/**@brief Action passed to interacts for a key repeat , see \ref InteractsActionKey for more details

 */
#define kOfxInteractActionKeyRepeat     "OfxInteractActionKeyRepeat"

/**@brief Action passed to interacts for a gain of input focus, see \ref InteractsActionFocus for more details
 */
#define kOfxInteractActionGainFocus "OfxInteractActionGainFocus"

/**@brief Action passed to interacts for a loss of input focus, see \ref InteractsActionFocus for more details
 */
#define kOfxInteractActionLoseFocus "OfxInteractActionLoseFocus"

/*@}*/
/*@}*/

/** @brief API for an interact host */
typedef struct OfxInteractSuiteV1 {	
  /** @brief Requests an openGL buffer swap on the interact instance */
  OfxStatus (*interactSwapBuffers)(OfxInteractHandle interactInstance);

  /** @brief Requests a redraw of the interact instance */
  OfxStatus (*interactRedraw)(OfxInteractHandle interactInstance);

  /** @brief Gets the property set handle for this interact handle */
  OfxStatus (*interactGetPropertySet)(OfxInteractHandle interactInstance,
				      OfxPropertySetHandle *property);
} OfxInteractSuiteV1;

#ifdef __cplusplus
}
#endif

/** @page CustomInteractionPage OFX : Custom Interactions

In many situations plugins will want to draw their own GUI tools and react to pen and keyboard input. In OFX this is done via the OfxInteract API, which is found in the file ofxInteract.h.

OFX relies on openGL to perform all drawing for custom interacts, due to its portabilty, robustness and wide implementation.

Each object that can have plug-in defined custom interaction will have a property, which is a pointer to a ::OfxPluginEntryPoint. For example a parameter can have its default interface overriden by setting its ::kOfxParamPropInteractV1 property. This entry point is \em not the same as the one in the OfxPlugin struct, as it needs to respond to actions in a different way than the plugin itself.

Interacts exist separately to the plugin instance, they have their own descriptor and instance handles passed into their separate ::OfxPluginEntryPoint. 

The ::kOfxActionDescribe action is passed to an interact, \em after the effect hosting the interact has been described.

An interact instance cannot exist without a plugin instance, an interact's instance, once created, is bound to a single instance of a plugin. 

There will be no more than one interact instance per object per plugin instance. eg: if a plugin has an overlay and a custom parameter interact, it will have at most one instance of each of them.

All interacts of the same type share openGL display lists, even if they are in different openGL contexts.

All interacts of the same type will have the same pixel types (this is a side effect of the last point), this will always be double buffered with at least RGB components. Alpha and the exact bit depth is left to the implementation.

So for example, all image effect overlays share the same display lists and have the same pixel depth, and all custom parameter GUIs share the same display list and have the same pixel depth, but overlays and custom parameter GUIs do not necassarily share the same display list/pixel depths.

An interact instance may be used in more than one view. Consider an image effect overlay interact in a host that supports multiple viewers to an effect instance. The same interact instance will be used in all views, the relevant properties controlling the view being changed before any action is passed to the interact. In this example, the draw action would be called once for each view open on the instance, with the projection, viewport and pixel scale being set appropriately for the view before each action.  (Should these view varying things be put in inArgs rather than the instance?)

An interact descriptor has the following properties,
  - ::kOfxInteractPropHasAlpha         - is there an alpha component in the frame buffer
  - ::kOfxInteractPropBitDepth         - bit depth of each component in the frame buffer

All interact instance have the following properties...
  - ::kOfxPropEffectInstance           - the effect instance the interact is associated with,
  - ::kOfxPropInstanceData             - private data pointer that the interact can set on its data
  - ::kOfxInteractPropPixelScale       - size of a real screen pixel under the interact's cannonical projection,
  - ::kOfxInteractPropBackgroundColour - the colour of the application GUI behind the interact
  - ::kOfxInteractPropViewportSize     - the viewport for the interact
  - ::kOfxInteractPropHasAlpha         - is there an alpha component in the frame buffer
  - ::kOfxInteractPropBitDepth         - bit depth of each component in the frame buffer
  - ::kOfxInteractPropSlaveToParam     - the names of the parameters the interact is slaved to

@section InteractOpenGLStates Cannonical Projections and the Default Open GL States

Each interact object has its own cannonical projection, which is set by the host before the plugin's 'draw' action is called. The cannonical projection setup the projection and modelview matrix so that any coordinates issued will be in correct coordinate space. Exactly what this is will depend on how the specific use of that interact object, look at the relevant documentation in \ref ImageEffectOverlays and \ref ParametersInteracts.

Interacts may or may not need to draw their backgrounds and perform buffer swaps, again it depends on the actual interact.

All other openGL states will be in their defaults. The plugin must reset these states to their defaults upon completion of its draw routine.

The interact \em must \em not change the viewport that the host has established for it.

<HR>
@section InteractActions Actions Passed to an Interact

Interacts have the following set of actions passed to their ::OfxPluginEntryPoint function...
  - ::kOfxActionDescribe
  - ::kOfxActionCreateInstance
  - ::kOfxActionDestroyInstance  
  - ::kOfxInteractActionDraw
  - ::kOfxInteractActionPenMotion
  - ::kOfxInteractActionPenDown
  - ::kOfxInteractActionPenUp
  - ::kOfxInteractActionKeyDown
  - ::kOfxInteractActionKeyUp
  - ::kOfxInteractActionKeyRepeat
  - ::kOfxInteractActionGainFocus
  - ::kOfxInteractActionLoseFocus

 The \e handle parameter should be cast to a OfxInteract *.
<HR>

@subsection InteractActionsDescribe The Describe Action

::kOfxActionDescribe is the first action passed to an interact instance after its creation. It is there to allow the plugin to create any user data it may need.

The \e handle parameter is a description handle for the interact. Set various properties in this to describe the behaviour of the interact. 

The \e inArgs and \e outArgs parameters are redundant for interacts and set to NULL.

\pre
- ::kOfxActionDescribe has been passed to the plugin's main entry point

\return
- ::kOfxStatOK, the action was trapped and all was well

\par Default		
- If not trapped, the default action is for the host to carry on as normal

<HR>
@subsection InteractActionsCreateInstance The Create Instance Action

 ::kOfxActionCreateInstance is the first action passed to an interact instance after its creation. It is there to allow the plugin to create any user data it may need.

The \e handle parameter is the \em interact's instance handle (not the effect's). Do not use any openGL calls inside any of this action.

The \e inArgs and \e outArgs parameters are redundant for interacts and set to NULL.

\pre
- ::kOfxActionDescribe has been passed to the interact's entry point
- ::kOfxActionCreateInstance has been passed to the plugin instance's entry point

\return
- ::kOfxStatOK, the action was trapped and all was well

\par Default		
- If not trapped, the default action is for the host to carry on as normal

<HR>
@subsection InteractActionsDestroyInstance The Destroy Instance Action

The ::kOfxActionDestroyInstance is the last action passed to an interact before its destruction. It is there to allow the plugin to destroy any user data it may have created.

The \e handle parameter is the \em interact's instance handle (not the effect's). Do not use any openGL calls inside any of this action.

The \e inArgs parameter has the following properties,
- ::kOfxPropEffectInstance - a handle to the effect for which the interact was created.

The \e outArgs parameter is redundant for interacts and set to NULL.

\return
- ::kOfxStatOK, the action was trapped and all was well


\par Default		
- If not trapped, the default action is for the host to carry on as normal

<HR>
@subsection InteractsActionDraw The Draw Action

::kOfxInteractActionDraw is issued to an interact whenever the host needs the plugin to redraw the given interact. The interact should issue any openGL calls it needs at this point.

Note that the interact may (in the case of custom parameter GUIS) or may not (in the case of image effect overlays) be required to swap buffers, that is up to the kind of interact.

The \e handle parameter is the \em interact's instance handle (not the effect's). 

The \e inArgs parameter has the following properties,
- ::kOfxPropEffectInstance - a handle to the effect for which the interact has been,
- ::kOfxInteractPropViewportSize - the openGL viewport size for the instance
- ::kOfxInteractPropPixelScale - the scale factor to convert cannonical pixels to screen pixels
- ::kOfxInteractPropBackgroundColour - the background colour of the application behind the current view

If the plugin is an image effect, the inArgs also has the following properties
   - ::kOfxPropTime         - the effect time at which changed occured
   - ::kOfxImageEffectPropRenderScale     - the render scale applied to any image fetched

The \e outArgs parameter is redundant for interacts and set to NULL.

\pre
  - the openGL context for this interact has been set
  - the projection matrix will correspond to the interact's cannonical voew

\return

\par Default		
- If not trapped, the default action is for the host to carry on as normal


<HR>
@subsection InteractsActionPen The Pen Actions

Three actions are issued by a host in response to use of a pointing device by a user, these are...
  - ::kOfxInteractActionPenMotion - issued whenever the  interact is the pen's focus and the pen has changed position, whether the pointer is up or down,
  - ::kOfxInteractActionPenDown - issued whenever the interact is the pen's focus and the pen is activated,
  - ::kOfxInteractActionPenUp - issued whenever the interact is the pen's focus and the pen is deactivated.

The \e handle parameter is the \em interact's instance handle (not the effect's). Do not use any openGL calls inside any of these actions.

The \e inArgs parameter has the following properties,
- ::kOfxPropEffectInstance - a handle to the effect for which the interact has been,
- ::kOfxInteractPropViewportSize - the openGL viewport size for the instance
- ::kOfxInteractPropPixelScale - the scale factor to convert cannonical pixels to screen pixels
- ::kOfxInteractPropBackgroundColour - the background colour of the application behind the current view
- ::kOfxInteractPropPenPosition - postion of the pen in,
- ::kOfxInteractPropPenPressure - the pressure of the pen,

If the plugin is an image effect, the inArgs also has the following properties
   - ::kOfxPropTime         - the effect time at which changed occured
   - ::kOfxImageEffectPropRenderScale     - the render scale applied to any image fetched

The \e outArgs parameter is redundant for interacts and set to NULL.

\return
- ::kOfxStatOK, the action was trapped and the host should not pass the event to other objects it may own
- ::kOfxStatReplyDefault, the action was not trapped and the host can deal with it if it wants

\par Default		
- If not trapped, the default action is for the host to carry on as normal

<HR>
@subsection InteractsActionKey The Keyboard Action

Three actions are issued by a host in response to use of a keyboard device by the user, these are
  - ::kOfxInteractActionKeyDown - issued whenever a key is depressed, 
  - ::kOfxInteractActionKeyUp - issued whenever a key is released,
  - ::kOfxInteractActionKeyRepeat - issued whenever a continually depressed key issues an auto repeat,

If the plugin is an image effect, the inArgs also has the following properties
   - ::kOfxPropTime         - the effect time at which changed occured
   - ::kOfxImageEffectPropRenderScale     - the render scale applied to any image fetched

The \e handle parameter is the \em interact's instance handle (not the effect's). Do not use any openGL calls inside any of these actions.

The \e inArgs parameter has the following properties,
- ::kOfxPropEffectInstance - a handle to the effect for which the interact has been,
- ::kOfxPropKeySym    - single integer value representing the key that was manipulated, this may not have a UTF8 representation (eg: a return key)
- ::kOfxPropKeyString - UTF8 string representing a character key that was pressed, some keys have no UTF8 encoding, in which case this is ""

The \e outArgs parameter is redundant for interacts and set to NULL.

\return
- ::kOfxStatOK, the action was trapped and the host should not pass the event to other objects it may own
- ::kOfxStatReplyDefault, the action was not trapped and the host can deal with it if it wants

\par Default		
- If not trapped, the default action is for the host to carry on as normal

<HR>
@subsection InteractsActionFocus The Focus Actions

An interact is notified when it gains or loses input focus by the following two actions...
  - ::kOfxInteractActionGainFocus - when the interact gains the input focus,
  - ::kOfxInteractActionLoseFocus - when the interact loses the input focus

The \e handle parameter is the \em interact's instance handle (not the effect's). Do not use any openGL calls inside any of these actions.

The \e inArgs parameter has the following properties,
- ::kOfxPropEffectInstance - a handle to the effect for which the interact is being used on,
- ::kOfxInteractPropViewportSize - the openGL viewport size for the instance,
- ::kOfxInteractPropPixelScale - the scale factor to convert cannonical pixels to screen pixels,
- ::kOfxInteractPropBackgroundColour - the background colour of the application behind the current view

If the plugin is an image effect, the inArgs also has the following properties
   - ::kOfxPropTime         - the effect time at which changed occured
   - ::kOfxImageEffectPropRenderScale     - the render scale applied to any image fetched

The \e outArgs parameter is redundant for interacts and set to NULL.

\return
- ::kOfxStatOK, the action was trapped and all was well

\par Default		
- If not trapped, the default action is for the host to carry on as normal




*/


#endif
