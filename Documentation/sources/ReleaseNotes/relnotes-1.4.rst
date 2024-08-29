.. SPDX-License-Identifier: CC-BY-4.0
OpenFX Release Notes for V1.4
=============================

Release Date: Sept 1, 2015

This is version 1.4 of the OpenFX API.  Significant additions include a
Dialog Suite for plugins to request the host to allow them to put up a
modal dialog, a NativeOrigin host property, draft render quality
support, half-float format tag available for CPU rendering (not just
OpenGL), and a new internationalizable version of the progress suite.

A number of ambiguities in the spec have also been clarified in this
version, including allowing OpenGL processing and tiled rendering to
be enabled/disabled in Instance Changed events, and clarifying the
semantics of dialogs and the progress suite.  The old Analysis pass
action has also been removed.


New Suites
----------
- :c:struct:`OfxDialogSuiteV1`

New Suite Versions
------------------
- :c:struct:`OfxProgressSuiteV2`

New Properties
--------------

- :c:macro:`kOfxImageEffectPropRenderQualityDraft`
- :c:macro:`kOfxImageEffectHostPropNativeOrigin`

Deprecations
------------

None

Removals
--------

- Analysis pass action
- :c:macro:`kOfxImageComponentYUVA` and related: YUVA pixel formats
- :c:macro:`kOfxInteractPropViewportSize`
- :c:macro:`kOfxParamPropPluginMayWrite`
- :c:macro:`kOfxImageEffectPropInAnalysis`
- :c:macro:`kOfxParamDoubleTypeNormalised` - removed in favor of :c:macro:`kOfxParamDoubleType`

