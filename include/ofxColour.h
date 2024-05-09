#ifndef _ofxColour_h_
#define _ofxColour_h_

// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#ifdef __cplusplus
extern "C" {
#endif

/** @file ofxColour.h
Contains the API for colourspace data exchange.
*/

/** @brief What style of colour management does the host or plug-in offer?

   - Type - string X 1
   - Property Set - host descriptor (read only), plugin descriptor (read/write)
   - Default - kOfxImageEffectPropColourManagementStyleNone
   - Valid Values - This must be one of
     - ::kOfxImageEffectPropColourManagementNone - no colour management
     - ::kOfxImageEffectPropColourManagementNative - colourspaces from the OCIO built-in ACES Studio config are available (see ofxColourspaceList.h)
     - ::kOfxImageEffectPropColourManagementOCIO - any OCIO config may be used (implies use of the OCIO library)

Hosts should set this property if they will provide colourspace information 
to plug-ins.
Plug-ins should set this property if they can use host-provided colourspace 
information. OCIO is used as the reference for the colour management API, but 
is not required to implement the Native style.

The colourspace strings used in the Native style are from:
https://github.com/AcademySoftwareFoundation/OpenColorIO-Config-ACES/releases/download/v1.0.0/studio-config-v1.0.0_aces-v1.3_ocio-v2.1.ocio
and stored for OFX purposes in ofxColourspaceList.h.

The assumption is that OCIO > Native so the highest style supported by 
both host and plug-in will be chosen.
*/
#define kOfxImageEffectPropColourManagementStyle "OfxImageEffectPropColourManagementStyle"

/* String used to indicate that no colour management is available. */
#define kOfxImageEffectPropColourManagementNone "OfxImageEffectPropColourManagementNone"
/* String used to indicate that Native colour management is available. */
#define kOfxImageEffectPropColourManagementNative "OfxImageEffectPropColourManagementNative"
/* String used to indicate that OCIO colour management is available. */
#define kOfxImageEffectPropColourManagementOCIO "OfxImageEffectPropColourManagementOCIO"

/** @brief The path to the OCIO config used for this instance

   - Type - string X 1
   - Property Set - image effect instance (read only)
   - Valid Values - Filesystem path to the config or URI starting ocio://

A host must set this property on any effect instances where it has negotiated 
OCIO colour management (kOfxImageEffectPropColourManagementOCIO).
Use of URIs for built-in configs, such as ocio://default is permitted.

When native colour management is in use, a host must set this property to point
to the OCIO config used to define strings for the version of OFX it was built
against. This will allow a plug-in which uses OCIO to work directly with native
mode.
*/
#define kOfxImageEffectPropOCIOConfig "OfxImageEffectPropOCIOConfig"

/** @brief The colourspace used for this clip

   - Type - string X 1
   - Property Set - clip instance (read/write)
   - Valid Values - colourspace that is permitted under the style in use

Hosts should set this property to the colourspace of the input clip. Typically 
it will be set to the working colourspace of the host but could be any valid 
colourspace.
Plug-ins may set this property on an output clip. Plug-ins which output motion 
vectors or similar images which should not be colour managed can use the data 
colourspace which is present in the built-in OCIO configs.
Both host and plug-in should use the value of 
kOfxImageClipPropPreferredColourspace where reasonable.
If a clip sets OfxImageClipPropIsMask or it only supports
OfxImageComponentAlpha, colour management is disabled and this property
must be unset.
*/
#define kOfxImageClipPropColourspace "OfxImageClipPropColourspace"

/** @brief The preferred colourspace for this clip

   - Type - string X N
   - Property Set - clip instance (read only) and ::kOfxImageEffectActionGetClipPreferences action out args property (read/write)
   - Valid Values - colourspace that is permitted under the style in use. 
                    For Native, this any scene colourspace or role from ofxColourspaceList.h.
                    For OCIO, this could be any string acceptable to Config::getColorSpace().

Plug-ins may set this property during kOfxImageEffectActionGetClipPreferences 
to request images in a colourspace which is convenient for them. The
property is an ordered set of colourspace identifiers, which may be any of the
names or aliases supported by the colour management style in use. If plug-ins
prefer more esoteric colourspaces, they are encouraged to also include generic
roles as a fallback. For example a colour grading plug-in which supports a
specific camera and expects a log colourspace might list:

"arri_logc4", "arri_logc3_ei800", "ACEScct", "color_timing"

The host is free to choose any colourspace from this list, but should favour
the first mutually agreeable colourspace, and set kOfxImageClipPropColourspace
to tell the plug-in which colourspace has been selected. A host does not need
to convert into the first choice just because it can, as this might be
inefficient, and should scene-to-display or display-to-scene conversions where
possible.

In the event that the host cannot supply images in a requested colourspace,
it may supply images in any valid colourspace. Plug-ins must check
kOfxImageClipPropColourspace to see if their request was satisfied.

Hosts may set this on an output clip, which could be helpful in a generator
context, and plug-ins should follow the same logic as hosts when deciding
which colourspace to use.

It might be much less costly for a host to perform a conversion 
than a plug-in, so in the example of a plug-in which performs all internal 
processing in scene linear, it is sensible for the plug-in to universally 
assert that preference and the host to honour it if possible. However, if a 
plug-in is capable of adapting to any input colourspace, it should not set 
this preference.

If a plug-in has inputs which contain motion vectors, depth values or other
non-colour channels, it should set the preferred colourspace to
kOfxColourspaceRaw. Similarly, if a host requests outputs in a typical scene
colourspace, but the plug-in is producing motion vectors, it should ignore
the request and set kOfxImageClipPropColourspace to kOfxColourspaceRaw.
*/
#define kOfxImageClipPropPreferredColourspaces "OfxImageClipPropPreferredColourspaces"

/** @brief The display colourspace used in the plug-in's viewport

   - Type - string X 1
   - Property Set - image effect instance (read only)
   - Valid Values - colourspace from the Display Colorspaces section of ofxColourspaceList.h

Used with the Native colour management style, this property is relevant 
for plug-ins which have their own viewport in a custom window. Plug-ins should 
not expect this to be available during a render event.
Hosts should set this property to a display colourspace which matches that 
used in its own viewport. For a multi-display system, choose the colourspace 
for the display device where a native window would appear by default.
A host which supports OCIO should use the OCIO-specific display and view
properties instead.
 
*/
#define kOfxImageEffectPropDisplayColourspace "OfxImageEffectPropDisplayColourspace"

/** @brief The OCIO display to be used in the plug-in's viewport

   - Type - string X 1
   - Property Set - image effect instance (read only)
   - Valid Values - OCIO display that is present in the config

This OCIO-specific property allows the host to specify which OCIO display should be used. 
If not defined, the default rules for choosing a display will be followed.
*/
#define kOfxImageEffectPropOCIODisplay "OfxImageEffectPropOCIODisplay"

/** @brief The OCIO view to be used in the plug-in's viewport

   - Type - string X 1
   - Property Set - image effect instance (read only)
   - Valid Values - OCIO view for the display specified by kOfxImageEffectPropOCIODisplay

This OCIO-specific property allows the host to specify which OCIO view should be used.
If not defined, the default rules for choosing a view will be followed.
*/
#define kOfxImageEffectPropOCIOView "OfxImageEffectPropOCIOView"

#ifdef __cplusplus
}
#endif

#endif
