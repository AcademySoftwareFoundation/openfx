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

/** @brief This file contains code that skins the ofx interact suite (for image effects) */


#include "ofxsSupportPrivate.H"

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
 */
namespace OFX {

    /** @brief fetch a pixel scale out of the property set */
    static OfxPointD getPixelScale(const PropertySet &props)
    {
        OfxPointD pixelScale;
        pixelScale.x = props.propGetDouble(kOfxInteractPropPixelScale, 0);
        pixelScale.y = props.propGetDouble(kOfxInteractPropPixelScale, 1);
        return pixelScale;
    }

    /** @brief fetch a render scale out of the property set */
    static OfxPointD getRenderScale(const PropertySet &props)
    {
        OfxPointD v;
        v.x = props.propGetDouble(kOfxImageEffectPropRenderScale, 0);
        v.y = props.propGetDouble(kOfxImageEffectPropRenderScale, 1);
        return v;
    }

    /** @brief fetch a background colour out of the property set */
    static OfxRGBColourD getBackgroundColour(const PropertySet &props)
    {
        OfxRGBColourD backGroundColour;
        backGroundColour.r = props.propGetDouble(kOfxInteractPropBackgroundColour, 0);
        backGroundColour.g = props.propGetDouble(kOfxInteractPropBackgroundColour, 1);
        backGroundColour.b = props.propGetDouble(kOfxInteractPropBackgroundColour, 2);
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
    
    /** @brief ctor */
    Interact::~Interact()
    {
    }

    /** @brief The bitdepth of each component in the openGL frame buffer */
    int 
    Interact::bitDepth(void) const
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
    Interact::Interact::pixelScale(void) const
    {
        OfxPointD v;
        v.x = _interactProperties.propGetDouble(kOfxInteractPropPixelScale, 0);
        v.y = _interactProperties.propGetDouble(kOfxInteractPropPixelScale, 1);
        return v;
    }

    /** @brief Request a redraw */
    void 
    Interact::Interact::requestRedraw(void) const
    {
        OfxStatus stat = OFX::Private::gInteractSuite->interactRedraw(_interactHandle);
        throwSuiteStatusException(stat);
    }

    /** @brief Swap a buffer in the case of a double bufferred interact, this is possibly a silly one */
    void 
    Interact::Interact::swapBuffers(void) const
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
        i = find(_slaveParams.begin(), _slaveParams.end(), p);
        if(i == _slaveParams.end()) {
            // we have a new one to add in here
            _slaveParams.push_back(p);
            
            // and set the property
            int n = _interactProperties.propGetDimension(kOfxInteractPropSlaveToParam);
            _interactProperties.propSetString(kOfxInteractPropSlaveToParam, p->name(), n);
        }
        
    }

    /** @brief Remova a param that the interact should be redrawn on if its value changes */
    void 
    Interact::removeParamToSlaveTo(Param *p)
    {
        // do we have it already ?
        std::list<Param *>::iterator i;
        i = find(_slaveParams.begin(), _slaveParams.end(), p);
        if(i != _slaveParams.end()) {
            // clobber it from the list
            _slaveParams.erase(i);

            // reset the property to remove our dead one
            _interactProperties.propReset(kOfxInteractPropSlaveToParam);

            // and add them all in again
            int n = 0;
            for(i = _slaveParams.begin(); i != _slaveParams.end(); ++i, ++n) {
                _interactProperties.propSetString(kOfxInteractPropSlaveToParam, (*i)->name(), n);
            }
        }
    }

    /** @brief the background colour */
    OfxRGBColourD Interact::backgroundColour(void) const
    {
        return getBackgroundColour(_interactProperties);
    }

    /** @brief the function called to draw in the interact */
    bool 
    Interact::draw(const DrawArgs &args)
    {
        return false;
    }

    /** @brief the function called to handle pen motion in the interact
        
    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    bool 
    Interact::penMotion(const PenArgs &args)
    {
        return false;
    }

    /** @brief the function called to handle pen down events in the interact 
        
    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    bool 
    Interact::penDown(const PenArgs &args)
    {
        return false;
    }

    /** @brief the function called to handle pen up events in the interact 
        
    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    bool 
    Interact::penUp(const PenArgs &args)
    {
        return false;
    }

    /** @brief the function called to handle key down events in the interact 
        
    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    bool 
    Interact::keyDown(const KeyArgs &args)
    {
        return false;
    }

    /** @brief the function called to handle key up events in the interact 
        
    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    bool 
    Interact::keyUp(const KeyArgs &args)
    {
        return false;
    }

    /** @brief the function called to handle key down repeat events in the interact 
        
    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    bool 
    Interact::keyRepeat(const KeyArgs &args)
    {
        return false;
    }

    /** @brief Called when the interact is given input focus */
    void 
    Interact::gainFocus(const FocusArgs &args)
    {
    }

    /** @brief Called when the interact is loses input focus */
    void 
    Interact::loseFocus(const FocusArgs &args)
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
    DrawArgs::DrawArgs(const PropertySet &props)
    {
        pixelScale       = getPixelScale(props);
        backGroundColour = getBackgroundColour(props);
        time             = props.propGetDouble(kOfxPropTime);
        renderScale      = getRenderScale(props);
    }

    /** @brief ctor */
    PenArgs::PenArgs(const PropertySet &props)
    {
        pixelScale    = getPixelScale(props);
        time          = props.propGetDouble(kOfxPropTime);
        renderScale   = getRenderScale(props);
        penPosition.x = props.propGetDouble(kOfxInteractPropPenPosition, 0);
        penPosition.y = props.propGetDouble(kOfxInteractPropPenPosition, 1);
        penPressure   = props.propGetDouble(kOfxInteractPropPenPressure);        
    }

    /** @brief ctor */
    KeyArgs::KeyArgs(const PropertySet &props)
    {
        time          = props.propGetDouble(kOfxPropTime);
        renderScale   = getRenderScale(props);
        keySymbol     = props.propGetInt(kOfxPropKeySym);
        keyString     = props.propGetString(kOfxPropKeyString);
    }

    /** @brief ctor */
    FocusArgs::FocusArgs(const PropertySet &props)
    {
        pixelScale       = getPixelScale(props);
        backGroundColour = getBackgroundColour(props);
        time             = props.propGetDouble(kOfxPropTime);
        renderScale      = getRenderScale(props);
    }

    namespace Private {
        /** @brief fetches our pointer out of the props on the handle */
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
      
            // and dance to the music
            return instance;
        }

        /** @brief The common entry point used by all interacts */
        OfxStatus
        interactMainEntry(const std::string     &action,
                          OfxInteractHandle      handle,
                          PropertySet	           inArgs,
                          PropertySet	           outArgs)
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
            else if(action ==	kOfxInteractActionDraw) {
                // make the draw args
                DrawArgs drawArgs(inArgs);
                if(interact->draw(drawArgs))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionPenMotion) {

                // make the draw args
                PenArgs args(inArgs);
                if(interact->penMotion(args))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionPenDown) {
                // make the draw args
                PenArgs args(inArgs);
                if(interact->penDown(args))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionPenUp) {
                // make the draw args
                PenArgs args(inArgs);
                if(interact->penUp(args))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionKeyDown) {
                // make the draw args
                KeyArgs args(inArgs);
                if(interact->keyDown(args))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionKeyUp) {
                // make the draw args
                KeyArgs args(inArgs);
                if(interact->keyUp(args))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionKeyRepeat) {
                // make the draw args
                KeyArgs args(inArgs);
                if(interact->keyRepeat(args))
                    stat = kOfxStatOK;
            }
            else if(action ==	kOfxInteractActionGainFocus) {
                // make the draw args
                FocusArgs args(inArgs);
                interact->gainFocus(args);
            }
            else if(action ==	kOfxInteractActionGainFocus) {
                // make the draw args
                FocusArgs args(inArgs);
                interact->loseFocus(args);
            }

            return stat;
        }


        /** @brief The main entry for image effect overlays */
        OfxStatus
        overlayInteractMainEntry(const char		*actionRaw,
                                 const void		*handleRaw,
                                 OfxPropertySetHandle	 inArgsRaw,
                                 OfxPropertySetHandle	 outArgsRaw)
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
                if (action == kOfxActionCreateInstance) {
                    // fetch the image effect we are being made for out of the interact's property handle
                    ImageEffect *effect = retrieveEffectFromInteractHandle(handle);

                    // ok make an interact
                    OverlayInteract *interact = effect->createOverlayInteract(handle);

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

}; // end of namespace
