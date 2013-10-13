/*
OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
Copyright (C) 2004-2005 The Open Effects Association Ltd
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

#if defined(WIN32) || defined(WIN64)

/** @brief This file contains code that skins the ofx interact suite (for image effects) */


#include "ofxsSupportPrivate.h"
#include <algorithm> // for find



/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
*/
namespace OFX {

  /** @brief fetch a pixel scale out of the property set */
  static OfxRectI getWindowLocation(const PropertySet &props)
  {
    OfxRectI windowLoc;
    windowLoc.x1 = props.propGetInt(kOfxHWndInteractPropLocation, 0);
    windowLoc.y1 = props.propGetInt(kOfxHWndInteractPropLocation, 1);
    windowLoc.x2 = props.propGetInt(kOfxHWndInteractPropLocation, 2);
    windowLoc.y2 = props.propGetInt(kOfxHWndInteractPropLocation, 3);
    return windowLoc;
  }

  static HWND getParentWindow(const PropertySet &props)
  {
      HWND hwndParent;
      hwndParent = (HWND) props.propGetPointer(kOfxHWndInteractPropParent);
      return hwndParent;
  }

  /** @brief retrieves the image effect pointer from the interact handle */
  static ImageEffect *retrieveEffectFromInteractHandle(OfxInteractHandle handle)
  {
    // get the properties set on this handle
    OfxPropertySetHandle propHandle;
    OfxStatus stat = OFX::Private::gHWNDInteractSuite->interactGetPropertySet(handle, &propHandle);
    throwSuiteStatusException(stat);
    PropertySet interactProperties(propHandle);

    // get the effect handle from this handle
    OfxImageEffectHandle effectHandle = (OfxImageEffectHandle) interactProperties.propGetPointer(kOfxPropEffectInstance);

    // get the effect properties 
    return OFX::Private::retrieveImageEffectPointer(effectHandle);
  }

  /** @brief ctor */
  HWNDInteract::HWNDInteract(OfxInteractHandle handle)
    : _interactHandle(handle)
    , _effect(0)
  {
    // get the properties set on this handle
    OfxPropertySetHandle propHandle;
    OfxStatus stat = OFX::Private::gHWNDInteractSuite->interactGetPropertySet(handle, &propHandle);
    throwSuiteStatusException(stat);
    _interactProperties.propSetHandle(propHandle);

    // set othe instance data on the property handle to point to this interact
    _interactProperties.propSetPointer(kOfxPropInstanceData, (void *)this);

    // get the effect handle from this handle        
    _effect = retrieveEffectFromInteractHandle(handle);
  }

  /** @brief ctor */
  HWNDInteract::~HWNDInteract()
  {
  }

  /** @brief since windows handles it's own messages, need a way to signal back to the
      host when values need updating
  */
  void HWNDInteract::triggerUpdate()
  {
    OfxStatus stat = OFX::Private::gHWNDInteractSuite->interactUpdate(_interactHandle);
    throwSuiteStatusException(stat);
  }

  /** @brief the function called to draw in the interact */
  bool 
    HWNDInteract::createWindow(const CreateWindowArgs &args, PropertySet &outArgs)
  {
    return false;
  }

  /** @brief the function called to handle pen motion in the interact

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    HWNDInteract::disposeWindow(const HWNDInteractArgs &args)
  {
    return false;
  }

  /** @brief the function called to handle pen down events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    HWNDInteract::moveWindow(const MoveWindowArgs &args)
  {
    return false;
  }

  /** @brief the function called to handle pen up events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    HWNDInteract::showWindow(const HWNDInteractArgs &args)
  {
    return false;
  }

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief ctor */
  HWNDInteractArgs::HWNDInteractArgs(const PropertySet &props)
  {
  }

  /** @brief ctor */
  CreateWindowArgs::CreateWindowArgs(const PropertySet &props)
    : HWNDInteractArgs(props)
  {
      hwndParent = getParentWindow(props);
  }

  /** @brief ctor */
  MoveWindowArgs::MoveWindowArgs(const PropertySet &props)
    : HWNDInteractArgs(props)
  {
      location = getWindowLocation(props);
  }

  namespace Private {
    /** @brief fetches our pointer out of the props on the handle */
    HWNDInteract *retrieveHWNDInteractPointer(OfxInteractHandle handle) 
    {
      HWNDInteract *instance;

      // get the prop set on the handle
      OfxPropertySetHandle propHandle;
      OfxStatus stat = OFX::Private::gHWNDInteractSuite->interactGetPropertySet(handle, &propHandle);
      throwSuiteStatusException(stat);

      // make our wrapper object
      PropertySet props(propHandle);

      // fetch the instance data out of the properties
      instance = (HWNDInteract *) props.propGetPointer(kOfxPropInstanceData);

      OFX::Log::error(instance == 0, "Instance data handle in effect instance properties is NULL!");

      // need to throw something here

      // and dance to the music
      return instance;
    }

    /** @brief The common entry point used by all interacts */
    OfxStatus
        hwndInteractMainEntry(
            const std::string     &action,
            OfxInteractHandle      handle,
            PropertySet            inArgs,
            PropertySet            outArgs)
    {
      OfxStatus stat = kOfxStatReplyDefault;

      // get the interact pointer
      HWNDInteract *interact = retrieveHWNDInteractPointer(handle);

      // if one was not made, return and do nothing
      if(!interact)
        return stat;

      if(action == kOfxActionDestroyInstance) {
        delete interact;
        stat = kOfxStatOK;
      }
      else if(action ==   kOfxHWndInteractActionCreateWindow) {
        // make the draw args
        CreateWindowArgs createArgs(inArgs);
        if(interact->createWindow(createArgs, outArgs))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxHWndInteractActionMoveWindow) {

        // make the draw args
        MoveWindowArgs args(inArgs);
        if(interact->moveWindow(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxHWndInteractActionDisposeWindow) {
        // make the draw args
        HWNDInteractArgs args(inArgs);
        if(interact->disposeWindow(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxHWndInteractActionShowWindow) {
        // make the draw args
        HWNDInteractArgs args(inArgs);
        if(interact->showWindow(args))
          stat = kOfxStatOK;
      }

      return stat;
    }

    /** @brief The main entry for image effect overlays */
    OfxStatus hwndInteractMainEntry(
        const char             *actionRaw,
        const void             *handleRaw,
        OfxPropertySetHandle    inArgsRaw,
        OfxPropertySetHandle    outArgsRaw,
        InteractDescriptor2&    desc)
    {
      OFX::Log::print("********************************************************************************");
      OFX::Log::print("START hwndInteractMainEntry (%s)", actionRaw);
      OFX::Log::indent();
      OfxStatus stat = kOfxStatReplyDefault;

      try {
        // Cast the raw handle to be an image effect handle, because that is what it is
        OfxInteractHandle handle = (OfxInteractHandle) handleRaw;

        // Turn the arguments into wrapper objects to make our lives easier
        OFX::PropertySet inArgs(inArgsRaw);
        OFX::PropertySet outArgs(outArgsRaw);

        // turn the action into a std::string
        std::string action(actionRaw);

        // figure the actions
        if (action == kOfxActionDescribe) {
          OfxPropertySetHandle propHandle;
          OfxStatus stat = OFX::Private::gInteractSuite->interactGetPropertySet(handle, &propHandle);
          throwSuiteStatusException(stat);
          PropertySet interactProperties(propHandle);
          desc.setPropertySet(&interactProperties);
          desc.describe();
        }
        else if (action == kOfxActionCreateInstance) 
        {
          // fetch the image effect we are being made for out of the interact's property handle
          ImageEffect *effect = retrieveEffectFromInteractHandle(handle);
          OFX::HWNDInteract* interact = desc.createInstance(handle, effect);
          // and all was well
          stat = kOfxStatOK;
        }
        else {
          stat = hwndInteractMainEntry(action, handle, inArgs, outArgs);
        }

      }
      catch(...)
      {
        stat = kOfxStatFailed;
      }

      OFX::Log::outdent();
      OFX::Log::print("STOP hwndInteractMainEntry (%s)", actionRaw);
      return stat;
    }

  }; // end namespace private

}; // end of namespace

#endif // #if defined(WIN32) || defined(WIN64)
