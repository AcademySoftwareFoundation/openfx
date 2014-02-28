#ifndef _ofxsHWNDInteract_H_
#define _ofxsHWNDInteract_H_

#if defined(WIN32) || defined(WIN64)

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

/** @file This file contains core code that wraps OFX 'objects' with C++ classes.

This file only holds code that is visible to a plugin implementation, and so hides much
of the direct OFX objects and any library side only functions.
*/
#include "ofxsParam.h"

#include <Windows.h>

/** @brief Nasty macro used to define empty protected copy ctors and assign ops */
#define mDeclareProtectedAssignAndCC(CLASS) \
  CLASS &operator=(const CLASS &) {assert(false); return *this;}      \
  CLASS(const CLASS &) {assert(false); } 

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
*/
namespace OFX {

  /** @brief forward declaration */
  class ImageEffect;

  /// all image effect interacts have these argumens
  struct HWNDInteractArgs {
    /// ctor
    HWNDInteractArgs(const PropertySet &props);
  };

  /** @brief struct to pass arguments into OFX::Interact::createWindow */
  struct CreateWindowArgs : public HWNDInteractArgs {
    CreateWindowArgs(const PropertySet &props);

    HWND            hwndParent;
  };

  /** @brief POD  to pass arguments into OFX::Interact pen actions */
  struct MoveWindowArgs : public HWNDInteractArgs {
    MoveWindowArgs(const PropertySet &props);

    OfxRectI        location;          /**< @brief The rect to move the window to */
  };


  /** @brief Wraps up an OFX interact object for an Image Effect. It won't work for any other plug-in type at present (it would need to be broken into a hierarchy of classes).
  */
  class HWNDInteract {
  protected :
    OfxInteractHandle  _interactHandle;     /**< @brief The handle for this interact */
    PropertySet        _interactProperties; /**< @brief The property set on this interact */
    ImageEffect*       _effect;             /**< @brief The instance we are associated with */

  public : 
    /** @brief ctor */
    HWNDInteract(OfxInteractHandle handle);

    /** @brief virtual destructor */
    virtual ~HWNDInteract(); 

    PropertySet &getProperties() { return _interactProperties; }

    /** @brief since windows handles it's own messages, need a way to signal back to the
        host when values need updating
    */
    void triggerUpdate();

    ////////////////////////////////////////////////////////////////////////////////
    // override the below in derived classes to do something useful

    /** @brief the function called to draw in the interact */
    virtual bool createWindow(const CreateWindowArgs &args, PropertySet &outArgs);

    /** @brief the function called to handle pen motion in the interact

    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    virtual bool moveWindow(const MoveWindowArgs &args);

    /** @brief the function called to handle pen down events in the interact 

    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    virtual bool disposeWindow(const HWNDInteractArgs &args);

    /** @brief the function called to handle pen up events in the interact 

    returns true if the interact trapped the action in some sense. This will block the action being passed to 
    any other interact that may share the viewer.
    */
    virtual bool showWindow(const HWNDInteractArgs &args);

  };

  class InteractDescriptor2
  {
  public:
    InteractDescriptor2():_props(0) {}
    virtual ~InteractDescriptor2() {}
    void                         setPropertySet(OFX::PropertySet* props){ _props = props; }
    virtual HWNDInteract*        createInstance(OfxInteractHandle handle, ImageEffect *effect) = 0;
    virtual OfxPluginEntryPoint* getMainEntry() = 0;
    virtual void                 describe() {}
  protected:
    OFX::PropertySet* _props;
  };

  typedef InteractDescriptor2 HWNDInteractDescriptor;

  namespace Private
  {
    OfxStatus hwndInteractMainEntry(
        const char*             actionRaw,
        const void*             handleRaw,
        OfxPropertySetHandle    inArgsRaw,
        OfxPropertySetHandle    outArgsRaw,
        HWNDInteractDescriptor& desc);
  }

  template<class DESC>
  class HWNDInteractMainEntry
  {
  protected:
    static OfxStatus hwndInteractMainEntry(const char *action, const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out)
    {
      static DESC desc;
      return OFX::Private::hwndInteractMainEntry(action, handle, in, out, desc);
    }
  };

  template<class DESC, class INSTANCE>
  class DefaultEffectHWNDInteractDescriptor : public HWNDInteractDescriptor, public HWNDInteractMainEntry<DESC>
  {
  public:
    HWNDInteract* createInstance(OfxInteractHandle handle, ImageEffect *effect) { return new INSTANCE(handle, effect); }
    virtual OfxPluginEntryPoint* getMainEntry() { return HWNDInteractMainEntry<DESC>::hwndInteractMainEntry; }
  };

};


#undef mDeclareProtectedAssignAndCC

#endif // #if defined(WIN32) || defined(WIN64)

#endif // #ifndef _ofxsHWNDInteract_H_

