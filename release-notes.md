# OpenFX image processing plug-in API

This is the release 1.5.1 of OFX, the Open Effects image-processing plug-in standard API.

Documentation and more info can be found at:

* [The OpenFX website](http://openeffects.org)
* [OFX Programming Guide By Example](https://github.com/ofxa/openfx/tree/master/Guide)
* [OFX API v. 1.4 Reference](http://openeffects.org/documentation/api_doc)
* [OFX API Programming Guide](http://openeffects.org/documentation/guide)
* [OFX API Programming Reference](http://openeffects.org/documentation/reference)
* [OFX Discussion Google Group](https://groups.google.com/forum/#!forum/ofx-discussion)

# Release Notes - 1.5.1

This is version 1.5.1 of the OpenFX API, a minor release with bug fixes and improvements.

## Key Features of OpenFX Version 1.5.1:

- **CPU Render Support**: Added `kOfxImageEffectPropCPURenderSupported` property to allow plugins to declare CPU rendering capability (or rather lack thereof).
- **Thumbnail Rendering**: Added `kOfxImageEffectPropThumbnailRender` property to support efficient thumbnail generation.
- **Spatial Awareness**: Added `kOfxImageEffectPropNoSpatialAwareness`. Allows the host and plugin to coordinate a render that ensures no spatial changes to the image.


## Fixes in OpenFX Version 1.5.1:

- Fix LICENSE.md to conform to Github template for BSD 3-Clause (issue #194).
- Use predefined `_WIN32` macro everywhere instead of defining `WINDOWS`, improving Windows portability.
- Fix CI build problems with conan version, expat dependency, and compiler compatibility.
- Restore older CentOS 7 builds for vfx-2021 & 2022 compatibility.
- Clarify that Image Effect Hosts property `kOfxPropType` must have the value
  `kOfxTypeImageEffectHost`, and fix host support lib to implement that.

## Documentation Improvements

- Significantly cleaned up documentation for Interacts, including DrawSuite and Overlay Interacts interaction.
- Fixed documentation for `clipGetRegionOfDefinition`.
- Updated colourspace example for more complete coverage.
- Fixed numerous typos throughout Documentation, include, Support, and HostSupport directories (via codespell).
- Corrected copyright notice with explanation in CONTRIBUTING.md (addresses issues #110, #206).
- Updated documentation footer and build script to support `uv` package manager.
- Removed stray references to old OFX Association.

## Deprecations

None

## Detailed List of Changes

- Add `kOfxImageEffectPropCPURenderSupported` (issue #212) - Property to indicate CPU rendering support.
- Add `kOfxImageEffectPropThumbnailRender` (issue #193) - Property for thumbnail rendering mode.

# Release Notes - 1.5

This is version 1.5 of the OFX API.

## Key Features of OpenFX Version 1.5:
- Color Management: Enhanced color management APIs to ensure accurate and consistent color rendering across different source media, devices and platforms.
- DrawSuite: On-screen drawing capabilities to allow hosts and plugins to work together without requiring OpenGL.
- GPU Support: Comprehensive GPU support between hosts and plugins, including OpenCL, CUDA, and Metal, for accelerated performance and smoother rendering.
- Choice Params: Improved parameter handling with the addition of choice parameters, allowing for more intuitive and organized user interfaces.
- Binary Data: Support for binary data to facilitate the handling of plugin-specific data types and improve processing efficiency.
- Windows ARM64: Version 1.6 enables future compatibility with Windows ARM64, expanding the range of supported devices and platforms.
- Houdini Support: SideFX has announced support for OpenFX in their upcoming version 20.5 of Houdini, in the new Copernicus image-processing node.

## Deprecations

None

## Detailed List of Changes

- Add `OfxSetHost` call
- Add `OfxDrawSuite` for drawing image overlays without use of OpenGL
- Add OpenCL, CUDA and Metal support for direct GPU rendering
- Add :c:macro:`kOfxParamPropChoiceOrder` for reordering Choice Params
- Colour management is in [[ofxColour.h][include/ofxColour.h]]

# Release Notes - 1.4

This is version 1.4 of the OFX API.  Significant additions include a
Dialog Suite for plugins to request the host to allow them to put up a
modal dialog, a NativeOrigin host property, draft render quality
support, half-float format tag available for CPU rendering (not just
OpenGL), and a new internationalizable version of the progress suite.

A number of ambiguities in the spec have also been clarified in this
version, including allowing OpenGL processing and tiled rendering to
be enabled/disabled in Instance Changed events, and clarifying the
semantics of dialogs and the progress suite.  The old Analysis pass
action has also been removed.

## Deprecations

In this release, some old properties and tags which were deprecated in
previous releases have now been moved to a new header file, `include/oldOfx.h`.
To continue to support those old tags, whether as a plugin or a host,
you'll need to include oldOfx.h.

## Detailed List of Changes

* _removed_ = final stage of deprecation: moved to oldOfx.h
* _added_ = new property or tag
* _refined/modified_ = documentation of precise expectations (without touching definitions)
also:   some prototypes add a const

ofxImageEffects.h :

* removed -  kOfxImageEffectPropInAnalysis
* removed -  kOfxImageComponentYUVA
* added   -  kOfxImageEffectPropRenderQualityDraft
* refined -  kOfxImageEffectPropSupportsTiles
* added   -  kOfxImageEffectHostPropNativeOrigin
* refined -  kOfxImageEffectPropRenderScale (Interact action was missing in supported actions)
* refined -  prop kOfxBitDepthHalf (kOfxImageEffectPropPixelDepth)
* refined -  prop kOfxBitDepthHalf (kOfxImageClipPropUnmappedPixelDepth)
* refined -  prop kOfxBitDepthHalf (kOfxImageEffectPropSupportedPixelDepths)

ofxParam.h:

* removed - kOfxParamPropPluginMayWrite
* removed - kOfxParamDoubleTypeNormalisedX
* removed - kOfxParamDoubleTypeNormalisedY
* removed - kOfxParamDoubleTypeNormalisedXAbsolute
* removed - kOfxParamDoubleTypeNormalisedYAbsolute
* removed - kOfxParamDoubleTypeNormalisedXY
* removed - kOfxParamDoubleTypeNormalisedXYAbsolute
* modified - kOfxParamPropInteractMinimumSize is now a double (matches kOfxParamPropInteractSize)
* modified - changed comments with regards to OfxParameterSuiteV1 (i.e. no Analysis action anymore)
* refined - paramGetValue, paramSetValue: precision about when it's safe to use - i.e. Instance Changed domain only

ofxPixels.h:

* removed - OfxYUVAColourB;OfxYUVAColourS;OfxYUVAColourF // leftover from 1.2 deprecation
* modified - OfxRGBColourD should not have an "a" component

ofxProgress.h:

* refined - OfxProgressSuiteV1
* added - OfxProgressSuiteV2

ofxDialog.h: NEW

* added - OfxDialogSuiteV1

ofxOpenGLRender.h:

* refined - kOfxImageEffectPropOpenGLRenderSupported

ofxCore.h:

* modified - kOfxFlagInfiniteMax (INT_MAX)  (and Min)
* added - kOfxBitDepthHalf

ofxInteract.h:

* removed - kOfxInteractPropViewportSize

ofxOld.h:

* New file added to capture removals ("removed" above)

/Support folder:
* C++ wrapper adds support for v1.3 and v1.4
