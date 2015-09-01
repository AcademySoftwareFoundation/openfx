# OpenFX image processing plug-in API

This is the latest release of OFX, the Open Effects image-processing plug-in standard API.

Documentation and more info can be found at:

* [The OFX Association website](http://openeffects.org)
* [OFX Programming Guide By Example](https://github.com/ofxa/openfx/tree/master/Guide)
* [OFX API v. 1.4 Reference](http://openeffects.org/documentation/api_doc)
* [OFX API Programming Guide](http://openeffects.org/documentation/guide)
* [OFX API Programming Reference](http://openeffects.org/documentation/reference)
* [OFX Discussion Google Group](https://groups.google.com/forum/#!forum/ofx-discussion)

# Release Notes

This is version 1.4 of the OFX API.  Significant additions include a
Dialog Suite for plugins to request the host to allow them to put up a
modal dialog, a NativeOrigin host property, draft render quality
support, half-float format tag for OpenGL rendering, and a new
internationalizable version of the progress suite.

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

# Detailed List of Changes

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
* refined - paramGetValue, paramSetValue: precision about when it's safe to use - i.e. Instance Changed domain only)

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
