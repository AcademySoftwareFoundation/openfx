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

/** @brief This file contains code that skins the ofx interact suite (for image effects) */


#include "ofxsSupportPrivate.h"
#include <algorithm> // for find

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
*/
namespace OFX {

  /** @brief fetch a pixel scale out of the property set */
  static OfxPointD getPixelScale(const PropertySet &props)
  {
    OfxPointD pixelScale;
    props.propGetDoubleN(kOfxInteractPropPixelScale, &pixelScale.x, 2);

    return pixelScale;
  }

  /** @brief fetch a render scale out of the property set */
  static OfxPointD getRenderScale(const PropertySet &props)
  {
    OfxPointD v;
    props.propGetDoubleN(kOfxInteractPropPixelScale, &v.x, 2);

    return v;
  }

  /** @brief fetch a background colour out of the property set */
  static OfxRGBColourD getBackgroundColour(const PropertySet &props)
  {
    OfxRGBColourD backGroundColour;
    props.propGetDoubleN(kOfxInteractPropBackgroundColour, &backGroundColour.r, 3);

    return backGroundColour;
  }

  /** @brief retrieves the image effect pointer from the interact handle */
  static ImageEffect *retrieveEffectFromInteractHandle(OfxInteractHandle handle)
  {
    // get the properties set on this handle
    OfxPropertySetHandle propHandle;
    OfxStatus stat = OFX::Private::gInteractSuite->interactGetPropertySet(handle, &propHandle);
    throwSuiteStatusException(stat);
    PropertySet interactProperties(propHandle);

    // get the effect handle from this handle
    OfxImageEffectHandle effectHandle = (OfxImageEffectHandle) interactProperties.propGetPointer(kOfxPropEffectInstance);

    // get the effect properties 
    return OFX::Private::retrieveImageEffectPointer(effectHandle);
  }

  /** @brief ctor */
  Interact::Interact(OfxInteractHandle handle)
    : _interactHandle(handle)
    , _effect(0)
  {
    // get the properties set on this handle
    OfxPropertySetHandle propHandle;
    OfxStatus stat = OFX::Private::gInteractSuite->interactGetPropertySet(handle, &propHandle);
    throwSuiteStatusException(stat);
    _interactProperties.propSetHandle(propHandle);

    // set othe instance data on the property handle to point to this interact
    _interactProperties.propSetPointer(kOfxPropInstanceData, (void *)this);

    // get the effect handle from this handle        
    _effect = retrieveEffectFromInteractHandle(handle);
  }

  /** @brief virtual destructor */
  Interact::~Interact()
  {
    // fetched clips and params are owned and deleted by the ImageEffect and its ParamSet
  }

  /** @brief The bitdepth of each component in the openGL frame buffer */
  int 
    Interact::getBitDepth(void) const
  {
    return _interactProperties.propGetInt(kOfxInteractPropBitDepth);
  }

  /** @brief Does the openGL frame buffer have an alpha */
  bool
    Interact::hasAlpha(void) const
  {
    return _interactProperties.propGetInt(kOfxInteractPropHasAlpha) != 0;
  }

  /** @brief Returns the size of a real screen pixel under the interact's cannonical projection */
  OfxPointD 
    Interact::getPixelScale(void) const
  {
    return ::OFX::getPixelScale(_interactProperties);
  }

  /** @brief The suggested colour to draw a widget in an interact */
  bool
    Interact::getSuggestedColour(OfxRGBColourD &c) const
  {
    // OFX 1.2/1.3 specs say that the host should return kOfxStatReplyDefault if there is no suggested color
    OfxStatus stat = OFX::Private::gPropSuite->propGetDoubleN(_interactProperties.propSetHandle(), kOfxInteractPropSuggestedColour, 3, &c.r);
#ifdef OFX_EXTENSIONS_NUKE
    if (stat != kOfxStatOK && stat != kOfxStatReplyDefault) {
      stat = OFX::Private::gPropSuite->propGetDoubleN(_interactProperties.propSetHandle(), kOfxPropOverlayColour, 3, &c.r);
    }
#endif
    if (stat != kOfxStatOK) {
      return false; // host gave no suggestion (replied kOfxStatReplyDefault or property is unknown to host)
    }
    return true;
  }

#ifdef OFX_EXTENSIONS_NATRON
  /** @brief Sets wether the interact uses an additional colour picking value in the inArgs of its actions. */
  void
    Interact::setColourPicking(bool useColourPicking)
  {
    _interactProperties.propSetInt(kNatronOfxInteractColourPicking, (int)useColourPicking, 0, false);
  }
#endif

  /** @brief Request a redraw */
  void 
    Interact::requestRedraw(void) const
  {
    OfxStatus stat = OFX::Private::gInteractSuite->interactRedraw(_interactHandle);
    throwSuiteStatusException(stat);
  }

  /** @brief Swap a buffer in the case of a double bufferred interact, this is possibly a silly one */
  void 
    Interact::swapBuffers(void) const
  {
    OfxStatus stat = OFX::Private::gInteractSuite->interactSwapBuffers(_interactHandle);
    throwSuiteStatusException(stat);
  }

  /** @brief Set a param that the interact should be redrawn on if its value changes */
  void 
    Interact::addParamToSlaveTo(Param *p)
  {
    // do we have it already ?
    std::list<Param *>::iterator i;
    i = std::find(_slaveParams.begin(), _slaveParams.end(), p);
    if(i == _slaveParams.end()) {
      // we have a new one to add in here
      _slaveParams.push_back(p);

      // and set the property
      int n = _interactProperties.propGetDimension(kOfxInteractPropSlaveToParam);
      _interactProperties.propSetString(kOfxInteractPropSlaveToParam, p->getName(), n);
    }

  }

  /** @brief Remova a param that the interact should be redrawn on if its value changes */
  void 
    Interact::removeParamToSlaveTo(Param *p)
  {
    // do we have it already ?
    std::list<Param *>::iterator i;
    i = std::find(_slaveParams.begin(), _slaveParams.end(), p);
    if(i != _slaveParams.end()) {
      // clobber it from the list
      _slaveParams.erase(i);

      // reset the property to remove our dead one
      _interactProperties.propReset(kOfxInteractPropSlaveToParam);

      // and add them all in again
      int n = 0;
      for(i = _slaveParams.begin(); i != _slaveParams.end(); ++i, ++n) {
        _interactProperties.propSetString(kOfxInteractPropSlaveToParam, (*i)->getName(), n);
      }
    }
  }

  /** @brief the background colour */
  OfxRGBColourD Interact::getBackgroundColour(void) const
  {
    return OFX::getBackgroundColour(_interactProperties);
  }

  /** @brief the function called to draw in the interact */
  bool 
    Interact::draw(const DrawArgs &/*args*/)
  {
    return false;
  }

  /** @brief the function called to handle pen motion in the interact

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    Interact::penMotion(const PenArgs &/*args*/)
  {
    return false;
  }

  /** @brief the function called to handle pen down events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    Interact::penDown(const PenArgs &/*args*/)
  {
    return false;
  }

  /** @brief the function called to handle pen up events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    Interact::penUp(const PenArgs &/*args*/)
  {
    return false;
  }

  /** @brief the function called to handle key down events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    Interact::keyDown(const KeyArgs &/*args*/)
  {
    return false;
  }

  /** @brief the function called to handle key up events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    Interact::keyUp(const KeyArgs &/*args*/)
  {
    return false;
  }

  /** @brief the function called to handle key down repeat events in the interact 

  returns true if the interact trapped the action in some sense. This will block the action being passed to 
  any other interact that may share the viewer.
  */
  bool 
    Interact::keyRepeat(const KeyArgs &/*args*/)
  {
    return false;
  }

  /** @brief Called when the interact is given input focus */
  void 
    Interact::gainFocus(const FocusArgs &/*args*/)
  {
  }

  /** @brief Called when the interact is loses input focus */
  void 
    Interact::loseFocus(const FocusArgs &/*args*/)
  {
  }

  ////////////////////////////////////////////////////////////////////////////////
  // overlay interact guff

  /** @brief ctor */
  OverlayInteract::OverlayInteract(OfxInteractHandle handle)
    : Interact(handle)
  {
    // add this interact into the list of overlays that the effect knows about
    if(_effect)
      _effect->addOverlayInteract(this);
  }

  /** @brief ctor */
  OverlayInteract::~OverlayInteract()
  {
    // add this interact into the list of overlays that the effect knows about
    if(_effect)
      _effect->removeOverlayInteract(this);
  }

  ////////////////////////////////////////////////////////////////////////////////
  /** @brief ctor */
  InteractArgs::InteractArgs(const PropertySet &props)
  {
    time          = props.propGetDouble(kOfxPropTime);
    renderScale   = getRenderScale(props);
#ifdef OFX_EXTENSIONS_NUKE
    view          = props.propGetInt(kFnOfxImageEffectPropView, 0, false);
#endif
#ifdef OFX_EXTENSIONS_NATRON
    if ( !props.propExists(kNatronOfxPropPickerColour) ) {
      pickerColour.r = pickerColour.g = pickerColour.b = pickerColour.a = -1.;
      hasPickerColour = false;
    } else {
      props.propGetDoubleN(kNatronOfxPropPickerColour, &pickerColour.r, 4);
      hasPickerColour = (pickerColour.r != -1. || pickerColour.g != -1. || pickerColour.b != -1. || pickerColour.a != -1.);
    }
#endif
  }

  /** @brief ctor */
  DrawArgs::DrawArgs(const PropertySet &props)
    : InteractArgs(props)
  {
#ifdef kOfxInteractPropViewportSize // removed in OFX 1.4
    viewportSize.x = viewportSize.y = 0.;
    props.propGetDoubleN(kOfxInteractPropViewportSize, &viewportSize.x, 2, false);
#endif
    backGroundColour = getBackgroundColour(props);
    pixelScale       = getPixelScale(props);
  }

  /** @brief ctor */
  PenArgs::PenArgs(const PropertySet &props)
    : InteractArgs(props)
  {
#ifdef kOfxInteractPropViewportSize // removed in OFX 1.4
    viewportSize.x = viewportSize.y = 0.;
    props.propGetDoubleN(kOfxInteractPropViewportSize, &viewportSize.x, 2, false);
#endif
    pixelScale    = getPixelScale(props);
    backGroundColour = getBackgroundColour(props);
    props.propGetDoubleN(kOfxInteractPropPenPosition, &penPosition.x, 2);
    // Introduced in OFX 1.2. Return (-1,-1) if not available
    OfxPointI pen = {-1, -1};
    props.propGetIntN(kOfxInteractPropPenViewportPosition, &pen.x, 2);
    penViewportPosition.x = pen.x;
    penViewportPosition.y = pen.y;
    penPressure   = props.propGetDouble(kOfxInteractPropPenPressure);
  }

  /** @brief ctor */
  KeyArgs::KeyArgs(const PropertySet &props)
    : InteractArgs(props)
  {
    time          = props.propGetDouble(kOfxPropTime);
    renderScale   = getRenderScale(props);
    keyString     = props.propGetString(kOfxPropKeyString);
    keySymbol     = props.propGetInt(kOfxPropKeySym);
  }

  /** @brief ctor */
  FocusArgs::FocusArgs(const PropertySet &props)
    : InteractArgs(props)
  {
#ifdef kOfxInteractPropViewportSize // removed in OFX 1.4
    viewportSize.x = viewportSize.y = 0.;
    props.propGetDoubleN(kOfxInteractPropViewportSize, &viewportSize.x, 2, false);
#endif
    pixelScale       = getPixelScale(props);
    backGroundColour = getBackgroundColour(props);
  }

  bool InteractDescriptor::getHasAlpha() const
  {
    return _props->propGetInt(kOfxInteractPropHasAlpha, 0, false);
  }

  int InteractDescriptor::getBitDepth() const
  {
    return _props->propGetInt(kOfxInteractPropBitDepth, 0, false);
  }

#ifdef OFX_EXTENSIONS_NATRON
  /** @brief Sets wether the interact uses an additional colour picking value in the inArgs of its actions. */
  void InteractDescriptor::setColourPicking(bool useColourPicking)
  {
    _props->propSetInt(kNatronOfxInteractColourPicking, (int)useColourPicking, 0, false);
  }
#endif

  void ParamInteractDescriptor::setInteractSizeAspect(double asp)
  {
    _props->propSetDouble(kOfxParamPropInteractSizeAspect , asp);
  }

  void ParamInteractDescriptor::setInteractMinimumSize(int x, int y)
  {
    double s[2] = {x, y};
    _props->propSetDoubleN(kOfxParamPropInteractMinimumSize, s, 2);
  }

  void ParamInteractDescriptor::setInteractPreferredSize(int x, int y)
  {
    int s[2] = {x, y};
    _props->propSetIntN(kOfxParamPropInteractPreferedSize, s, 2);
  }

  ParamInteract::ParamInteract(OfxInteractHandle handle, ImageEffect* effect):Interact(handle), _effect(effect)
  {}

  OfxPointI ParamInteract::getInteractSize() const
  {
    OfxPointI ret;
    _interactProperties.propGetIntN(kOfxParamPropInteractSize, &ret.x, 2);

    return ret;
  }

  namespace Private {
    /** @brief fetches our pointer out of the props on the handle */
    static
    Interact *retrieveInteractPointer(OfxInteractHandle handle)
    {
      Interact *instance;

      // get the prop set on the handle
      OfxPropertySetHandle propHandle;
      OfxStatus stat = OFX::Private::gInteractSuite->interactGetPropertySet(handle, &propHandle);
      throwSuiteStatusException(stat);

      // make our wrapper object
      PropertySet props(propHandle);

      // fetch the instance data out of the properties
      instance = (Interact *) props.propGetPointer(kOfxPropInstanceData);

      OFX::Log::error(instance == 0, "Instance data handle in effect instance properties is NULL!");

      // need to throw something here
      if (!instance) {
        throwSuiteStatusException(kOfxStatErrBadHandle);
      }

      // and dance to the music
      return instance;
    }

    /** @brief The common entry point used by all interacts */
    static
    OfxStatus
      interactMainEntry(const std::string     &action,
      OfxInteractHandle      handle,
      PropertySet              inArgs,
      PropertySet              /*outArgs*/)
    {
      OfxStatus stat = kOfxStatReplyDefault;

      // get the interact pointer
      Interact *interact = retrieveInteractPointer(handle);

      // if one was not made, return and do nothing
      if(!interact)
        return stat;

      if(action == kOfxActionDestroyInstance) {
        delete interact;
        stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionDraw) {
        // make the draw args
        DrawArgs drawArgs(inArgs);
        if(interact->draw(drawArgs))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionPenMotion) {

        // make the draw args
        PenArgs args(inArgs);
        if(interact->penMotion(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionPenDown) {
        // make the draw args
        PenArgs args(inArgs);
        if(interact->penDown(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionPenUp) {
        // make the draw args
        PenArgs args(inArgs);
        if(interact->penUp(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionKeyDown) {
        // make the draw args
        KeyArgs args(inArgs);
        if(interact->keyDown(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionKeyUp) {
        // make the draw args
        KeyArgs args(inArgs);
        if(interact->keyUp(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionKeyRepeat) {
        // make the draw args
        KeyArgs args(inArgs);
        if(interact->keyRepeat(args))
          stat = kOfxStatOK;
      }
      else if(action ==   kOfxInteractActionGainFocus) {
        // make the draw args
        FocusArgs args(inArgs);
        interact->gainFocus(args);
      }
      else if(action ==   kOfxInteractActionLoseFocus) {
        // make the draw args
        FocusArgs args(inArgs);
        interact->loseFocus(args);
      }

      return stat;
    }

    /** @brief The main entry for image effect overlays */
    OfxStatus interactMainEntry(const char             *actionRaw,
      const void             *handleRaw,
      OfxPropertySetHandle    inArgsRaw,
      OfxPropertySetHandle    outArgsRaw,
      InteractDescriptor& desc)
    {
      OFX::Log::print("********************************************************************************");
      OFX::Log::print("START overlayInteractMainEntry (%s)", actionRaw);
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
          OFX::Interact* interact = desc.createInstance(handle, effect);
          (void)interact;
          // and all was well
          stat = kOfxStatOK;
        }
        else {
          stat = interactMainEntry(action, handle, inArgs, outArgs);
        }

      }
      catch(...)
      {
        stat = kOfxStatFailed;
      }

      OFX::Log::outdent();
      OFX::Log::print("STOP overlayInteractMainEntry (%s)", actionRaw);
      return stat;
    }

  }; // end namespace private

  // methods in ImageEffect which depends on ofxsInteract

  /** @brief force all overlays on this interact to be redrawn */
  void ImageEffect::redrawOverlays(void)
  {
    // find it
    std::list<OverlayInteract *>::iterator i;
    for(i = _overlayInteracts.begin(); i != _overlayInteracts.end(); ++i) {
      (*i)->requestRedraw();
    }
  }

}; // end of namespace
