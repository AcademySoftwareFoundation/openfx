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
     - ::kOfxImageEffectPropColourManagementBasic - only basic colourspaces from ofxColourspaceList.h may be used
     - ::kOfxImageEffectPropColourManagementCore - only core colourspaces from ofxColourspaceList.h may be used
     - ::kOfxImageEffectPropColourManagementFull - any colourspace from ofxColourspaceList.h may be used
     - ::kOfxImageEffectPropColourManagementOCIO - any OCIO config may be used (implies use of the OCIO library)

Hosts should set this property if they will provide colourspace information 
to plug-ins.
Plug-ins should set this property if they can use host-provided colourspace 
information.
Collectively, the full, core and basic styles are referred to as native
colour management. OCIO is used as the reference for the colour management
API, but is not required to implement the native styles.

The colourspace strings used in the native styles are from
openfx-native-1.5.ocio, which is based on the OCIO ACES Studio built-in
config, studio-config-v2.1.0_aces-v1.3_ocio-v2.3, and stored for OFX purposes
in ofxColourspaceList.h. Additionally, there is a scheme for cross-referencing
between clips, and a set of "basic colourspaces", which are designed to be
generic names for a family of colourspaces. When a basic colourspace is used,
this means the host is free to choose any colourspace with the same encoding
and reference space (scene vs display).

The assumption is that OCIO > Full > Core > Basic, so the highest style
supported by both host and plug-in will be chosen.
*/
#define kOfxImageEffectPropColourManagementStyle "OfxImageEffectPropColourManagementStyle"

/* String used to indicate that no colour management is available. */
#define kOfxImageEffectPropColourManagementNone "OfxImageEffectPropColourManagementNone"
/* String used to indicate that basic colour management is available. */
#define kOfxImageEffectPropColourManagementBasic "OfxImageEffectPropColourManagementBasic"
/* String used to indicate that core colour management is available. */
#define kOfxImageEffectPropColourManagementCore "OfxImageEffectPropColourManagementCore"
/* String used to indicate that Full colour management is available. */
#define kOfxImageEffectPropColourManagementFull "OfxImageEffectPropColourManagementFull"
/* String used to indicate that OCIO colour management is available. */
#define kOfxImageEffectPropColourManagementOCIO "OfxImageEffectPropColourManagementOCIO"

/** @brief The path to the OCIO config used for this instance

   - Type - string X 1
   - Property Set - image effect instance (read only)
   - Valid Values - Filesystem path to the config or URI starting ocio://

A host must set this property on any effect instances where it has negotiated 
OCIO colour management (kOfxImageEffectPropColourManagementOCIO).
Use of URIs for built-in configs, such as ocio://default is permitted.

When the core or full styles are in use, a host must set this property to
point to an OCIO config which defines all the colourspaces required for that
style, which will allow a plug-in that uses OCIO to work directly with native
styles. The included config, openfx-native-v1.5_aces-v1.3_ocio-v2.3.ocio, is
suitable, but hosts might provide a different config which is compatible. If
hosts choose to use their own config, its definitions for native colourspaces
must exactly match those found in openfx-native-v1.5_aces-v1.3_ocio-v2.3.ocio.

As basic colourspaces are not defined in the OpenFX native config, hosts may
set this property as they wish when using the basic style. If hosts do provide
a config, an OCIO-based plug-in may attempt to use it to look up mappings for
basic colourspaces, but leaving those undefined must not be considered an
error.
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
vectors or similar images which should not be colour managed should use
kOfxColourspaceRaw,

Both host and plug-in should use the value of 
kOfxImageClipPropPreferredColourspace where reasonable.

Cross-referencing between clips is possible by setting this property to
"OfxColourspace_<clip>". For example a plug-in may set this property on its
output clip to "OfxColourspace_Source", telling the host that the colourspace
of the output matches the input. In the basic style, plug-ins are recommended
to use cross-referencing for their output clip unless kOfxColourspaceRaw is
required.

If a clip sets OfxImageClipPropIsMask or it only supports
OfxImageComponentAlpha, colour management is disabled and this property
must be unset.
*/
#define kOfxImageClipPropColourspace "OfxImageClipPropColourspace"

/** @brief The preferred colourspace for this clip

   - Type - string X N
   - Property Set - clip instance (read only) and ::kOfxImageEffectActionGetClipPreferences action out args property (read/write)
   - Valid Values - colourspace that is permitted under the style in use. 
                    For Basic, any colourspace from ofxColourspaceList.h where IsBasic is true.
                    For Core, any colourspace from ofxColourspaceList.h where IsCore is true.
                    For Full, any colourspace from ofxColourspaceList.h.
                    For OCIO, any string acceptable to Config::getColorSpace().

Plug-ins may set this property during kOfxImageEffectActionGetClipPreferences 
to request images in a colourspace which is convenient for them. The
property is an ordered set of colourspace identifiers, which may be any of the
names or aliases supported by the colour management style in use. If plug-ins
prefer more esoteric colourspaces, they are encouraged to also include basic
colourspacesas a fallback. For example a colour grading plug-in which supports
a specific camera and expects a log colourspace might list:

"arri_logc4", "arri_logc3_ei800", "ACEScct", "ofx_log"

The host is free to choose any colourspace from this list, but should favour
the first mutually agreeable colourspace, and set kOfxImageClipPropColourspace
to tell the plug-in which colourspace has been selected. A host does not need
to convert into the first choice just because it can, as this might be
inefficient, and should avoid scene-to-display or display-to-scene conversions
where possible.

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

Cross-referencing between clips is possible by setting this property to
"OfxColourspace_<clip>". For example a plug-in may set this property on a
second input clip to "OfxColourspace_Source" to tell the host it would like
both input clips to be in the same colourspace. A host might set the same
thing on the plug-in's output clip to request that the plug-in outputs the
same colourspace as the input.

If a plug-in has inputs which expect motion vectors, depth values or other
non-colour channels, it should set the preferred colourspace to
kOfxColourspaceRaw. Similarly, if a host requests outputs in a typical scene
colourspace, but the plug-in is producing motion vectors, it should ignore
the request and set kOfxImageClipPropColourspace to kOfxColourspaceRaw.
*/
#define kOfxImageClipPropPreferredColourspaces "OfxImageClipPropPreferredColourspaces"

/** @brief The display colourspace used in the plug-in's viewport

   - Type - string X 1
   - Property Set - image effect instance (read only)
   - Valid Values - any colourspace from ofxColourspaceList.h

Used with native colour management styles, this property is relevant 
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

/** @brief

 This action allows a host to ask an effect, given a list of preferred
 colourspaces, what colourspace will be used for its output clip. This should
 be called after first gathering the plug-ins preferred input colourspaces
 via OfxImageEffectActionGetClipPreferences. The host must set
 kOfxImageClipPropColourspace on the output clip to the chosen colourspace,
 or the default value of OfxColourspace_Source.

 @param  handle handle to the instance, cast to an \ref OfxImageEffectHandle
 @param  inArgs has the property
     - \ref kOfxImageClipPropPreferredColourspaces the list of preferred colourspaces

 @param  outArgs has the property
     - \ref kOfxImageClipPropColourspace the colourspace selected by the plug-in

 @returns
     -  \ref kOfxStatOK, the action was trapped and the colourspace was set in the outArgs property set
     -  \ref kOfxStatReplyDefault, the action was not trapped and the host should use the default value of OfxColourspace_Source
     -  \ref kOfxStatErrMemory, in which case the action may be called again after a memory purge
     -  \ref kOfxStatFailed, something wrong, but no error code appropriate, plugin to post message
     -  \ref kOfxStatErrFatal
 */
#define kOfxImageEffectActionGetOutputColourspace "OfxImageEffectActionGetOutputColourspace"

#ifdef __cplusplus
}
#endif

#endif
