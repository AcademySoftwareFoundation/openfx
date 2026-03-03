.. _propertySetReferenceGenerated:
Property Sets Reference (Generated)
==================================

This reference is auto-generated from property set definitions in the OpenFX source code.
It provides an overview of property sets and their associated properties.
For each property, a link to its detailed description in the :doc:`Properties Reference (Generated) <ofxPropertiesReferenceGenerated>` is provided.

Regular Property Sets
--------------------

These property sets represent collections of properties associated with various OpenFX objects.

**Property Sets Quick Reference**

* :ref:`ClipDescriptor <propset_ClipDescriptor>`
* :ref:`ClipInstance <propset_ClipInstance>`
* :ref:`EffectDescriptor <propset_EffectDescriptor>`
* :ref:`EffectInstance <propset_EffectInstance>`
* :ref:`Image <propset_Image>`
* :ref:`ImageEffectHost <propset_ImageEffectHost>`
* :ref:`InteractDescriptor <propset_InteractDescriptor>`
* :ref:`InteractInstance <propset_InteractInstance>`
* :ref:`ParamDouble1D <propset_ParamDouble1D>`
* :ref:`ParameterSet <propset_ParameterSet>`
* :ref:`ParamsByte <propset_ParamsByte>`
* :ref:`ParamsChoice <propset_ParamsChoice>`
* :ref:`ParamsCustom <propset_ParamsCustom>`
* :ref:`ParamsDouble2D3D <propset_ParamsDouble2D3D>`
* :ref:`ParamsGroup <propset_ParamsGroup>`
* :ref:`ParamsInt2D3D <propset_ParamsInt2D3D>`
* :ref:`ParamsNormalizedSpatial <propset_ParamsNormalizedSpatial>`
* :ref:`ParamsPage <propset_ParamsPage>`
* :ref:`ParamsParametric <propset_ParamsParametric>`
* :ref:`ParamsStrChoice <propset_ParamsStrChoice>`
* :ref:`ParamsString <propset_ParamsString>`

.. _propset_ClipDescriptor:

**ClipDescriptor**
^^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxImageClipPropFieldExtraction <prop_OfxImageClipPropFieldExtraction>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropFieldExtraction`)
- :ref:`OfxImageClipPropIsMask <prop_OfxImageClipPropIsMask>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropIsMask`)
- :ref:`OfxImageClipPropOptional <prop_OfxImageClipPropOptional>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropOptional`)
- :ref:`OfxImageEffectPropSupportedComponents <prop_OfxImageEffectPropSupportedComponents>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropSupportedComponents`)
- :ref:`OfxImageEffectPropSupportsTiles <prop_OfxImageEffectPropSupportsTiles>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsTiles`)
- :ref:`OfxImageEffectPropTemporalClipAccess <prop_OfxImageEffectPropTemporalClipAccess>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropTemporalClipAccess`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)

.. _propset_ClipInstance:

**ClipInstance**
^^^^^^^^^^^^

- **Write Access**: host

**Properties**

- :ref:`OfxImageClipPropColourspace <prop_OfxImageClipPropColourspace>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropColourspace`)
- :ref:`OfxImageClipPropConnected <prop_OfxImageClipPropConnected>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropConnected`)
- :ref:`OfxImageClipPropContinuousSamples <prop_OfxImageClipPropContinuousSamples>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropContinuousSamples`)
- :ref:`OfxImageClipPropFieldExtraction <prop_OfxImageClipPropFieldExtraction>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropFieldExtraction`)
- :ref:`OfxImageClipPropFieldOrder <prop_OfxImageClipPropFieldOrder>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropFieldOrder`)
- :ref:`OfxImageClipPropIsMask <prop_OfxImageClipPropIsMask>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropIsMask`)
- :ref:`OfxImageClipPropOptional <prop_OfxImageClipPropOptional>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropOptional`)
- :ref:`OfxImageClipPropPreferredColourspaces <prop_OfxImageClipPropPreferredColourspaces>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxImageClipPropPreferredColourspaces`)
- :ref:`OfxImageClipPropUnmappedComponents <prop_OfxImageClipPropUnmappedComponents>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropUnmappedComponents`)
- :ref:`OfxImageClipPropUnmappedPixelDepth <prop_OfxImageClipPropUnmappedPixelDepth>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropUnmappedPixelDepth`)
- :ref:`OfxImageEffectPropComponents <prop_OfxImageEffectPropComponents>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropComponents`)
- :ref:`OfxImageEffectPropFrameRange <prop_OfxImageEffectPropFrameRange>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropFrameRange`)
- :ref:`OfxImageEffectPropFrameRate <prop_OfxImageEffectPropFrameRate>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropFrameRate`)
- :ref:`OfxImageEffectPropPixelDepth <prop_OfxImageEffectPropPixelDepth>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropPixelDepth`)
- :ref:`OfxImageEffectPropPreMultiplication <prop_OfxImageEffectPropPreMultiplication>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropPreMultiplication`)
- :ref:`OfxImageEffectPropSupportedComponents <prop_OfxImageEffectPropSupportedComponents>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropSupportedComponents`)
- :ref:`OfxImageEffectPropSupportsTiles <prop_OfxImageEffectPropSupportsTiles>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsTiles`)
- :ref:`OfxImageEffectPropTemporalClipAccess <prop_OfxImageEffectPropTemporalClipAccess>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropTemporalClipAccess`)
- :ref:`OfxImageEffectPropUnmappedFrameRange <prop_OfxImageEffectPropUnmappedFrameRange>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropUnmappedFrameRange`)
- :ref:`OfxImageEffectPropUnmappedFrameRate <prop_OfxImageEffectPropUnmappedFrameRate>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropUnmappedFrameRate`)
- :ref:`OfxImagePropPixelAspectRatio <prop_OfxImagePropPixelAspectRatio>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImagePropPixelAspectRatio`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)

.. _propset_EffectDescriptor:

**EffectDescriptor**
^^^^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxImageEffectPluginPropFieldRenderTwiceAlways <prop_OfxImageEffectPluginPropFieldRenderTwiceAlways>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginPropFieldRenderTwiceAlways`)
- :ref:`OfxImageEffectPluginPropGrouping <prop_OfxImageEffectPluginPropGrouping>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginPropGrouping`)
- :ref:`OfxImageEffectPluginPropHostFrameThreading <prop_OfxImageEffectPluginPropHostFrameThreading>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginPropHostFrameThreading`)
- :ref:`OfxImageEffectPluginPropOverlayInteractV1 <prop_OfxImageEffectPluginPropOverlayInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginPropOverlayInteractV1`)
- :ref:`OfxImageEffectPluginPropOverlayInteractV2 <prop_OfxImageEffectPluginPropOverlayInteractV2>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginPropOverlayInteractV2`)
- :ref:`OfxImageEffectPluginPropSingleInstance <prop_OfxImageEffectPluginPropSingleInstance>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginPropSingleInstance`)
- :ref:`OfxImageEffectPluginRenderThreadSafety <prop_OfxImageEffectPluginRenderThreadSafety>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginRenderThreadSafety`)
- :ref:`OfxImageEffectPluginRenderThreadSafety <prop_OfxImageEffectPluginRenderThreadSafety>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPluginRenderThreadSafety`)
- :ref:`OfxImageEffectPropClipPreferencesSlaveParam <prop_OfxImageEffectPropClipPreferencesSlaveParam>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropClipPreferencesSlaveParam`)
- :ref:`OfxImageEffectPropColourManagementAvailableConfigs <prop_OfxImageEffectPropColourManagementAvailableConfigs>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropColourManagementAvailableConfigs`)
- :ref:`OfxImageEffectPropColourManagementStyle <prop_OfxImageEffectPropColourManagementStyle>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropColourManagementStyle`)
- :ref:`OfxImageEffectPropMultipleClipDepths <prop_OfxImageEffectPropMultipleClipDepths>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsMultipleClipDepths`)
- :ref:`OfxImageEffectPropOpenCLSupported <prop_OfxImageEffectPropOpenCLSupported>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOpenCLSupported`)
- :ref:`OfxImageEffectPropOpenGLRenderSupported <prop_OfxImageEffectPropOpenGLRenderSupported>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOpenGLRenderSupported`)
- :ref:`OfxImageEffectPropSupportedContexts <prop_OfxImageEffectPropSupportedContexts>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropSupportedContexts`)
- :ref:`OfxImageEffectPropSupportedPixelDepths <prop_OfxImageEffectPropSupportedPixelDepths>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropSupportedPixelDepths`)
- :ref:`OfxImageEffectPropSupportsMultiResolution <prop_OfxImageEffectPropSupportsMultiResolution>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsMultiResolution`)
- :ref:`OfxImageEffectPropSupportsMultipleClipPARs <prop_OfxImageEffectPropSupportsMultipleClipPARs>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsMultipleClipPARs`)
- :ref:`OfxImageEffectPropSupportsTiles <prop_OfxImageEffectPropSupportsTiles>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsTiles`)
- :ref:`OfxImageEffectPropTemporalClipAccess <prop_OfxImageEffectPropTemporalClipAccess>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropTemporalClipAccess`)
- :ref:`OfxOpenGLPropPixelDepth <prop_OfxOpenGLPropPixelDepth>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxOpenGLPropPixelDepth`)
- :ref:`OfxPluginPropFilePath <prop_OfxPluginPropFilePath>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxPluginPropFilePath`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropPluginDescription <prop_OfxPropPluginDescription>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropPluginDescription`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`OfxPropVersion <prop_OfxPropVersion>` - Type: int, Dimension: Variable (doc: :c:macro:`kOfxPropVersion`)
- :ref:`OfxPropVersionLabel <prop_OfxPropVersionLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropVersionLabel`)

.. _propset_EffectInstance:

**EffectInstance**
^^^^^^^^^^^^^^

- **Write Access**: host

**Properties**

- :ref:`OfxImageEffectInstancePropEffectDuration <prop_OfxImageEffectInstancePropEffectDuration>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImageEffectInstancePropEffectDuration`)
- :ref:`OfxImageEffectInstancePropSequentialRender <prop_OfxImageEffectInstancePropSequentialRender>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectInstancePropSequentialRender`)
- :ref:`OfxImageEffectPropColourManagementConfig <prop_OfxImageEffectPropColourManagementConfig>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropColourManagementConfig`)
- :ref:`OfxImageEffectPropColourManagementStyle <prop_OfxImageEffectPropColourManagementStyle>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropColourManagementStyle`)
- :ref:`OfxImageEffectPropContext <prop_OfxImageEffectPropContext>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropContext`)
- :ref:`OfxImageEffectPropDisplayColourspace <prop_OfxImageEffectPropDisplayColourspace>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropDisplayColourspace`)
- :ref:`OfxImageEffectPropFrameRate <prop_OfxImageEffectPropFrameRate>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropFrameRate`)
- :ref:`OfxImageEffectPropOCIOConfig <prop_OfxImageEffectPropOCIOConfig>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOCIOConfig`)
- :ref:`OfxImageEffectPropOCIODisplay <prop_OfxImageEffectPropOCIODisplay>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOCIODisplay`)
- :ref:`OfxImageEffectPropOCIOView <prop_OfxImageEffectPropOCIOView>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOCIOView`)
- :ref:`OfxImageEffectPropOpenGLRenderSupported <prop_OfxImageEffectPropOpenGLRenderSupported>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOpenGLRenderSupported`)
- :ref:`OfxImageEffectPropPixelAspectRatio <prop_OfxImageEffectPropPixelAspectRatio>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropProjectPixelAspectRatio`)
- :ref:`OfxImageEffectPropPluginHandle <prop_OfxImageEffectPropPluginHandle>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropPluginHandle`)
- :ref:`OfxImageEffectPropProjectExtent <prop_OfxImageEffectPropProjectExtent>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropProjectExtent`)
- :ref:`OfxImageEffectPropProjectOffset <prop_OfxImageEffectPropProjectOffset>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropProjectOffset`)
- :ref:`OfxImageEffectPropProjectSize <prop_OfxImageEffectPropProjectSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropProjectSize`)
- :ref:`OfxImageEffectPropSupportsTiles <prop_OfxImageEffectPropSupportsTiles>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsTiles`)
- :ref:`OfxPropInstanceData <prop_OfxPropInstanceData>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxPropInstanceData`)
- :ref:`OfxPropIsInteractive <prop_OfxPropIsInteractive>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxPropIsInteractive`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)

.. _propset_Image:

**Image**
^^^^^

- **Write Access**: host

**Properties**

- :ref:`OfxImageEffectPropComponents <prop_OfxImageEffectPropComponents>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropComponents`)
- :ref:`OfxImageEffectPropPixelDepth <prop_OfxImageEffectPropPixelDepth>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropPixelDepth`)
- :ref:`OfxImageEffectPropPreMultiplication <prop_OfxImageEffectPropPreMultiplication>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropPreMultiplication`)
- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropRenderScale`)
- :ref:`OfxImagePropBounds <prop_OfxImagePropBounds>` - Type: int, Dimension: 4 (doc: :c:macro:`kOfxImagePropBounds`)
- :ref:`OfxImagePropData <prop_OfxImagePropData>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxImagePropData`)
- :ref:`OfxImagePropField <prop_OfxImagePropField>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImagePropField`)
- :ref:`OfxImagePropPixelAspectRatio <prop_OfxImagePropPixelAspectRatio>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImagePropPixelAspectRatio`)
- :ref:`OfxImagePropRegionOfDefinition <prop_OfxImagePropRegionOfDefinition>` - Type: int, Dimension: 4 (doc: :c:macro:`kOfxImagePropRegionOfDefinition`)
- :ref:`OfxImagePropRowBytes <prop_OfxImagePropRowBytes>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxImagePropRowBytes`)
- :ref:`OfxImagePropUniqueIdentifier <prop_OfxImagePropUniqueIdentifier>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImagePropUniqueIdentifier`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)

.. _propset_ImageEffectHost:

**ImageEffectHost**
^^^^^^^^^^^^^^^

- **Write Access**: host

**Properties**

- :ref:`OfxImageEffectHostPropIsBackground <prop_OfxImageEffectHostPropIsBackground>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectHostPropIsBackground`)
- :ref:`OfxImageEffectHostPropNativeOrigin <prop_OfxImageEffectHostPropNativeOrigin>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectHostPropNativeOrigin`)
- :ref:`OfxImageEffectInstancePropSequentialRender <prop_OfxImageEffectInstancePropSequentialRender>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectInstancePropSequentialRender`)
- :ref:`OfxImageEffectPropColourManagementAvailableConfigs <prop_OfxImageEffectPropColourManagementAvailableConfigs>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropColourManagementAvailableConfigs`)
- :ref:`OfxImageEffectPropColourManagementStyle <prop_OfxImageEffectPropColourManagementStyle>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropColourManagementStyle`)
- :ref:`OfxImageEffectPropMultipleClipDepths <prop_OfxImageEffectPropMultipleClipDepths>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsMultipleClipDepths`)
- :ref:`OfxImageEffectPropOpenCLSupported <prop_OfxImageEffectPropOpenCLSupported>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOpenCLSupported`)
- :ref:`OfxImageEffectPropOpenGLRenderSupported <prop_OfxImageEffectPropOpenGLRenderSupported>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropOpenGLRenderSupported`)
- :ref:`OfxImageEffectPropRenderQualityDraft <prop_OfxImageEffectPropRenderQualityDraft>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropRenderQualityDraft`)
- :ref:`OfxImageEffectPropSetableFielding <prop_OfxImageEffectPropSetableFielding>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSetableFielding`)
- :ref:`OfxImageEffectPropSetableFrameRate <prop_OfxImageEffectPropSetableFrameRate>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSetableFrameRate`)
- :ref:`OfxImageEffectPropSupportedComponents <prop_OfxImageEffectPropSupportedComponents>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropSupportedComponents`)
- :ref:`OfxImageEffectPropSupportedContexts <prop_OfxImageEffectPropSupportedContexts>` - Type: enum, Dimension: Variable (doc: :c:macro:`kOfxImageEffectPropSupportedContexts`)
- :ref:`OfxImageEffectPropSupportsMultiResolution <prop_OfxImageEffectPropSupportsMultiResolution>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsMultiResolution`)
- :ref:`OfxImageEffectPropSupportsMultipleClipPARs <prop_OfxImageEffectPropSupportsMultipleClipPARs>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsMultipleClipPARs`)
- :ref:`OfxImageEffectPropSupportsOverlays <prop_OfxImageEffectPropSupportsOverlays>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsOverlays`)
- :ref:`OfxImageEffectPropSupportsTiles <prop_OfxImageEffectPropSupportsTiles>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropSupportsTiles`)
- :ref:`OfxImageEffectPropTemporalClipAccess <prop_OfxImageEffectPropTemporalClipAccess>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropTemporalClipAccess`)
- :ref:`OfxParamHostPropMaxPages <prop_OfxParamHostPropMaxPages>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropMaxPages`)
- :ref:`OfxParamHostPropMaxParameters <prop_OfxParamHostPropMaxParameters>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropMaxParameters`)
- :ref:`OfxParamHostPropPageRowColumnCount <prop_OfxParamHostPropPageRowColumnCount>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamHostPropPageRowColumnCount`)
- :ref:`OfxParamHostPropSupportsBooleanAnimation <prop_OfxParamHostPropSupportsBooleanAnimation>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsBooleanAnimation`)
- :ref:`OfxParamHostPropSupportsChoiceAnimation <prop_OfxParamHostPropSupportsChoiceAnimation>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsChoiceAnimation`)
- :ref:`OfxParamHostPropSupportsCustomAnimation <prop_OfxParamHostPropSupportsCustomAnimation>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsCustomAnimation`)
- :ref:`OfxParamHostPropSupportsCustomInteract <prop_OfxParamHostPropSupportsCustomInteract>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsCustomInteract`)
- :ref:`OfxParamHostPropSupportsParametricAnimation <prop_OfxParamHostPropSupportsParametricAnimation>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsParametricAnimation`)
- :ref:`OfxParamHostPropSupportsStrChoice <prop_OfxParamHostPropSupportsStrChoice>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsStrChoice`)
- :ref:`OfxParamHostPropSupportsStrChoiceAnimation <prop_OfxParamHostPropSupportsStrChoiceAnimation>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsStrChoiceAnimation`)
- :ref:`OfxParamHostPropSupportsStringAnimation <prop_OfxParamHostPropSupportsStringAnimation>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamHostPropSupportsStringAnimation`)
- :ref:`OfxPropAPIVersion <prop_OfxPropAPIVersion>` - Type: int, Dimension: Variable (doc: :c:macro:`kOfxPropAPIVersion`)
- :ref:`OfxPropHostOSHandle <prop_OfxPropHostOSHandle>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxPropHostOSHandle`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`OfxPropVersion <prop_OfxPropVersion>` - Type: int, Dimension: Variable (doc: :c:macro:`kOfxPropVersion`)
- :ref:`OfxPropVersionLabel <prop_OfxPropVersionLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropVersionLabel`)

.. _propset_InteractDescriptor:

**InteractDescriptor**
^^^^^^^^^^^^^^^^^^

- **Write Access**: host

**Properties**

- :ref:`OfxInteractPropBitDepth <prop_OfxInteractPropBitDepth>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxInteractPropBitDepth`)
- :ref:`OfxInteractPropHasAlpha <prop_OfxInteractPropHasAlpha>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxInteractPropHasAlpha`)

.. _propset_InteractInstance:

**InteractInstance**
^^^^^^^^^^^^^^^^

- **Write Access**: host

**Properties**

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (doc: :c:macro:`kOfxInteractPropBackgroundColour`)
- :ref:`OfxInteractPropBitDepth <prop_OfxInteractPropBitDepth>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxInteractPropBitDepth`)
- :ref:`OfxInteractPropHasAlpha <prop_OfxInteractPropHasAlpha>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxInteractPropHasAlpha`)
- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxInteractPropPixelScale`)
- :ref:`OfxInteractPropSlaveToParam <prop_OfxInteractPropSlaveToParam>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxInteractPropSlaveToParam`)
- :ref:`OfxInteractPropSuggestedColour <prop_OfxInteractPropSuggestedColour>` - Type: double, Dimension: 3 (doc: :c:macro:`kOfxInteractPropSuggestedColour`)
- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxPropEffectInstance`)
- :ref:`OfxPropInstanceData <prop_OfxPropInstanceData>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxPropInstanceData`)

.. _propset_ParamDouble1D:

**ParamDouble1D**
^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropDigits <prop_OfxParamPropDigits>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxParamPropDigits`)
- :ref:`OfxParamPropDisplayMax <prop_OfxParamPropDisplayMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMax`)
- :ref:`OfxParamPropDisplayMin <prop_OfxParamPropDisplayMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMin`)
- :ref:`OfxParamPropDoubleType <prop_OfxParamPropDoubleType>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropDoubleType`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropIncrement <prop_OfxParamPropIncrement>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropIncrement`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropMax <prop_OfxParamPropMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMax`)
- :ref:`OfxParamPropMin <prop_OfxParamPropMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMin`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropShowTimeMarker <prop_OfxParamPropShowTimeMarker>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropShowTimeMarker`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParameterSet:

**ParameterSet**
^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxPluginPropParamPageOrder <prop_OfxPluginPropParamPageOrder>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxPluginPropParamPageOrder`)
- :ref:`OfxPropParamSetNeedsSyncing <prop_OfxPropParamSetNeedsSyncing>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxPropParamSetNeedsSyncing`)

.. _propset_ParamsByte:

**ParamsByte**
^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropDisplayMax <prop_OfxParamPropDisplayMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMax`)
- :ref:`OfxParamPropDisplayMin <prop_OfxParamPropDisplayMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMin`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropMax <prop_OfxParamPropMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMax`)
- :ref:`OfxParamPropMin <prop_OfxParamPropMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMin`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsChoice:

**ParamsChoice**
^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropChoiceOption <prop_OfxParamPropChoiceOption>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxParamPropChoiceOption`)
- :ref:`OfxParamPropChoiceOrder <prop_OfxParamPropChoiceOrder>` - Type: int, Dimension: Variable (doc: :c:macro:`kOfxParamPropChoiceOrder`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsCustom:

**ParamsCustom**
^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropCustomCallbackV1 <prop_OfxParamPropCustomCallbackV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropCustomInterpCallbackV1`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsDouble2D3D:

**ParamsDouble2D3D**
^^^^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropDigits <prop_OfxParamPropDigits>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxParamPropDigits`)
- :ref:`OfxParamPropDisplayMax <prop_OfxParamPropDisplayMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMax`)
- :ref:`OfxParamPropDisplayMin <prop_OfxParamPropDisplayMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMin`)
- :ref:`OfxParamPropDoubleType <prop_OfxParamPropDoubleType>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropDoubleType`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropIncrement <prop_OfxParamPropIncrement>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropIncrement`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropMax <prop_OfxParamPropMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMax`)
- :ref:`OfxParamPropMin <prop_OfxParamPropMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMin`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsGroup:

**ParamsGroup**
^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropGroupOpen <prop_OfxParamPropGroupOpen>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropGroupOpen`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)

.. _propset_ParamsInt2D3D:

**ParamsInt2D3D**
^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropDimensionLabel <prop_OfxParamPropDimensionLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropDimensionLabel`)
- :ref:`OfxParamPropDisplayMax <prop_OfxParamPropDisplayMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMax`)
- :ref:`OfxParamPropDisplayMin <prop_OfxParamPropDisplayMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMin`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropMax <prop_OfxParamPropMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMax`)
- :ref:`OfxParamPropMin <prop_OfxParamPropMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMin`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsNormalizedSpatial:

**ParamsNormalizedSpatial**
^^^^^^^^^^^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropDefaultCoordinateSystem <prop_OfxParamPropDefaultCoordinateSystem>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropDefaultCoordinateSystem`)
- :ref:`OfxParamPropDigits <prop_OfxParamPropDigits>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxParamPropDigits`)
- :ref:`OfxParamPropDisplayMax <prop_OfxParamPropDisplayMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMax`)
- :ref:`OfxParamPropDisplayMin <prop_OfxParamPropDisplayMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMin`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropIncrement <prop_OfxParamPropIncrement>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropIncrement`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropMax <prop_OfxParamPropMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMax`)
- :ref:`OfxParamPropMin <prop_OfxParamPropMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMin`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsPage:

**ParamsPage**
^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropPageChild <prop_OfxParamPropPageChild>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxParamPropPageChild`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)

.. _propset_ParamsParametric:

**ParamsParametric**
^^^^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropParametricDimension <prop_OfxParamPropParametricDimension>` - Type: int, Dimension: 1 (doc: :c:macro:`kOfxParamPropParametricDimension`)
- :ref:`OfxParamPropParametricInteractBackground <prop_OfxParamPropParametricInteractBackground>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropParametricInteractBackground`)
- :ref:`OfxParamPropParametricRange <prop_OfxParamPropParametricRange>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropParametricRange`)
- :ref:`OfxParamPropParametricUIColour <prop_OfxParamPropParametricUIColour>` - Type: double, Dimension: Variable (doc: :c:macro:`kOfxParamPropParametricUIColour`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsStrChoice:

**ParamsStrChoice**
^^^^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropChoiceEnum <prop_OfxParamPropChoiceEnum>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropChoiceEnum`)
- :ref:`OfxParamPropChoiceOption <prop_OfxParamPropChoiceOption>` - Type: string, Dimension: Variable (doc: :c:macro:`kOfxParamPropChoiceOption`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)

.. _propset_ParamsString:

**ParamsString**
^^^^^^^^^^^^

- **Write Access**: plugin

**Properties**

- :ref:`OfxParamPropAnimates <prop_OfxParamPropAnimates>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropAnimates`)
- :ref:`OfxParamPropCacheInvalidation <prop_OfxParamPropCacheInvalidation>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropCacheInvalidation`)
- :ref:`OfxParamPropCanUndo <prop_OfxParamPropCanUndo>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropCanUndo`)
- :ref:`OfxParamPropDataPtr <prop_OfxParamPropDataPtr>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropDataPtr`)
- :ref:`OfxParamPropDefault <prop_OfxParamPropDefault>` - Type: int/double/string/pointer, Dimension: Variable (doc: :c:macro:`kOfxParamPropDefault`)
- :ref:`OfxParamPropDisplayMax <prop_OfxParamPropDisplayMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMax`)
- :ref:`OfxParamPropDisplayMin <prop_OfxParamPropDisplayMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropDisplayMin`)
- :ref:`OfxParamPropEnabled <prop_OfxParamPropEnabled>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEnabled`)
- :ref:`OfxParamPropEvaluateOnChange <prop_OfxParamPropEvaluateOnChange>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropEvaluateOnChange`)
- :ref:`OfxParamPropHasHostOverlayHandle <prop_OfxParamPropHasHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropHasHostOverlayHandle`)
- :ref:`OfxParamPropHint <prop_OfxParamPropHint>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropHint`)
- :ref:`OfxParamPropInteractMinimumSize <prop_OfxParamPropInteractMinimumSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractMinimumSize`)
- :ref:`OfxParamPropInteractPreferedSize <prop_OfxParamPropInteractPreferedSize>` - Type: int, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractPreferedSize`)
- :ref:`OfxParamPropInteractSize <prop_OfxParamPropInteractSize>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInteractSize`)
- :ref:`OfxParamPropInteractSizeAspect <prop_OfxParamPropInteractSizeAspect>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractSizeAspect`)
- :ref:`OfxParamPropInteractV1 <prop_OfxParamPropInteractV1>` - Type: pointer, Dimension: 1 (doc: :c:macro:`kOfxParamPropInteractV1`)
- :ref:`OfxParamPropIsAnimating <prop_OfxParamPropIsAnimating>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAnimating`)
- :ref:`OfxParamPropIsAutoKeying <prop_OfxParamPropIsAutoKeying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropIsAutoKeying`)
- :ref:`OfxParamPropMax <prop_OfxParamPropMax>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMax`)
- :ref:`OfxParamPropMin <prop_OfxParamPropMin>` - Type: int/double, Dimension: Variable (doc: :c:macro:`kOfxParamPropMin`)
- :ref:`OfxParamPropParent <prop_OfxParamPropParent>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropParent`)
- :ref:`OfxParamPropPersistant <prop_OfxParamPropPersistant>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPersistant`)
- :ref:`OfxParamPropPluginMayWrite <prop_OfxParamPropPluginMayWrite>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropPluginMayWrite`)
- :ref:`OfxParamPropScriptName <prop_OfxParamPropScriptName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropScriptName`)
- :ref:`OfxParamPropSecret <prop_OfxParamPropSecret>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropSecret`)
- :ref:`OfxParamPropStringFilePathExists <prop_OfxParamPropStringFilePathExists>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropStringFilePathExists`)
- :ref:`OfxParamPropStringMode <prop_OfxParamPropStringMode>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxParamPropStringMode`)
- :ref:`OfxParamPropType <prop_OfxParamPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxParamPropType`)
- :ref:`OfxPropIcon <prop_OfxPropIcon>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxPropIcon`)
- :ref:`OfxPropLabel <prop_OfxPropLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLabel`)
- :ref:`OfxPropLongLabel <prop_OfxPropLongLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropLongLabel`)
- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropName`)
- :ref:`OfxPropShortLabel <prop_OfxPropShortLabel>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropShortLabel`)
- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxPropType`)
- :ref:`kOfxParamPropUseHostOverlayHandle <prop_kOfxParamPropUseHostOverlayHandle>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxParamPropUseHostOverlayHandle`)


Actions Property Sets
-------------------

Actions in OFX have input and output property sets that are used to pass data between the host and plugin.
For each action, the required input properties (passed from host to plugin) and output properties (set by the plugin for the host to read) are documented.

**Actions Quick Reference**

* :ref:`CustomParamInterpFunc <action_CustomParamInterpFunc>`
* :ref:`OfxActionBeginInstanceChanged <action_OfxActionBeginInstanceChanged>`
* :ref:`OfxActionBeginInstanceEdit <action_OfxActionBeginInstanceEdit>`
* :ref:`OfxActionCreateInstance <action_OfxActionCreateInstance>`
* :ref:`OfxActionDescribe <action_OfxActionDescribe>`
* :ref:`OfxActionDestroyInstance <action_OfxActionDestroyInstance>`
* :ref:`OfxActionEndInstanceChanged <action_OfxActionEndInstanceChanged>`
* :ref:`OfxActionEndInstanceEdit <action_OfxActionEndInstanceEdit>`
* :ref:`OfxActionInstanceChanged <action_OfxActionInstanceChanged>`
* :ref:`OfxActionLoad <action_OfxActionLoad>`
* :ref:`OfxActionPurgeCaches <action_OfxActionPurgeCaches>`
* :ref:`OfxActionSyncPrivateData <action_OfxActionSyncPrivateData>`
* :ref:`OfxActionUnload <action_OfxActionUnload>`
* :ref:`OfxImageEffectActionBeginSequenceRender <action_OfxImageEffectActionBeginSequenceRender>`
* :ref:`OfxImageEffectActionDescribeInContext <action_OfxImageEffectActionDescribeInContext>`
* :ref:`OfxImageEffectActionEndSequenceRender <action_OfxImageEffectActionEndSequenceRender>`
* :ref:`OfxImageEffectActionGetClipPreferences <action_OfxImageEffectActionGetClipPreferences>`
* :ref:`OfxImageEffectActionGetFramesNeeded <action_OfxImageEffectActionGetFramesNeeded>`
* :ref:`OfxImageEffectActionGetOutputColourspace <action_OfxImageEffectActionGetOutputColourspace>`
* :ref:`OfxImageEffectActionGetRegionOfDefinition <action_OfxImageEffectActionGetRegionOfDefinition>`
* :ref:`OfxImageEffectActionGetRegionsOfInterest <action_OfxImageEffectActionGetRegionsOfInterest>`
* :ref:`OfxImageEffectActionGetTimeDomain <action_OfxImageEffectActionGetTimeDomain>`
* :ref:`OfxImageEffectActionIsIdentity <action_OfxImageEffectActionIsIdentity>`
* :ref:`OfxImageEffectActionRender <action_OfxImageEffectActionRender>`
* :ref:`OfxInteractActionDraw <action_OfxInteractActionDraw>`
* :ref:`OfxInteractActionGainFocus <action_OfxInteractActionGainFocus>`
* :ref:`OfxInteractActionKeyDown <action_OfxInteractActionKeyDown>`
* :ref:`OfxInteractActionKeyRepeat <action_OfxInteractActionKeyRepeat>`
* :ref:`OfxInteractActionKeyUp <action_OfxInteractActionKeyUp>`
* :ref:`OfxInteractActionLoseFocus <action_OfxInteractActionLoseFocus>`
* :ref:`OfxInteractActionPenDown <action_OfxInteractActionPenDown>`
* :ref:`OfxInteractActionPenMotion <action_OfxInteractActionPenMotion>`
* :ref:`OfxInteractActionPenUp <action_OfxInteractActionPenUp>`

.. _action_CustomParamInterpFunc:

**CustomParamInterpFunc**
^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxParamPropCustomValue <prop_OfxParamPropCustomValue>` - Type: string, Dimension: 2 (:c:macro:`kOfxParamPropCustomValue`)

- :ref:`OfxParamPropInterpolationTime <prop_OfxParamPropInterpolationTime>` - Type: double, Dimension: 2 (:c:macro:`kOfxParamPropInterpolationTime`)

- :ref:`OfxParamPropInterpolationAmount <prop_OfxParamPropInterpolationAmount>` - Type: double, Dimension: 1 (:c:macro:`kOfxParamPropInterpolationAmount`)

**Output Arguments**

- :ref:`OfxParamPropCustomValue <prop_OfxParamPropCustomValue>` - Type: string, Dimension: 2 (doc: :c:macro:`kOfxParamPropCustomValue`)

- :ref:`OfxParamPropInterpolationTime <prop_OfxParamPropInterpolationTime>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxParamPropInterpolationTime`)

.. _action_OfxActionBeginInstanceChanged:

**OfxActionBeginInstanceChanged**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropChangeReason <prop_OfxPropChangeReason>` - Type: enum, Dimension: 1 (:c:macro:`kOfxPropChangeReason`)

.. _action_OfxActionBeginInstanceEdit:

**OfxActionBeginInstanceEdit**
^^^^^^^^^^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionCreateInstance:

**OfxActionCreateInstance**
^^^^^^^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionDescribe:

**OfxActionDescribe**
^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionDestroyInstance:

**OfxActionDestroyInstance**
^^^^^^^^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionEndInstanceChanged:

**OfxActionEndInstanceChanged**
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropChangeReason <prop_OfxPropChangeReason>` - Type: enum, Dimension: 1 (:c:macro:`kOfxPropChangeReason`)

.. _action_OfxActionEndInstanceEdit:

**OfxActionEndInstanceEdit**
^^^^^^^^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionInstanceChanged:

**OfxActionInstanceChanged**
^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropType <prop_OfxPropType>` - Type: string, Dimension: 1 (:c:macro:`kOfxPropType`)

- :ref:`OfxPropName <prop_OfxPropName>` - Type: string, Dimension: 1 (:c:macro:`kOfxPropName`)

- :ref:`OfxPropChangeReason <prop_OfxPropChangeReason>` - Type: enum, Dimension: 1 (:c:macro:`kOfxPropChangeReason`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxActionLoad:

**OfxActionLoad**
^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionPurgeCaches:

**OfxActionPurgeCaches**
^^^^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionSyncPrivateData:

**OfxActionSyncPrivateData**
^^^^^^^^^^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxActionUnload:

**OfxActionUnload**
^^^^^^^^^^^^^^^

-- no in/out args --

.. _action_OfxImageEffectActionBeginSequenceRender:

**OfxImageEffectActionBeginSequenceRender**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxImageEffectPropFrameRange <prop_OfxImageEffectPropFrameRange>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropFrameRange`)

- :ref:`OfxImageEffectPropFrameStep <prop_OfxImageEffectPropFrameStep>` - Type: double, Dimension: 1 (:c:macro:`kOfxImageEffectPropFrameStep`)

- :ref:`OfxPropIsInteractive <prop_OfxPropIsInteractive>` - Type: bool, Dimension: 1 (:c:macro:`kOfxPropIsInteractive`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

- :ref:`OfxImageEffectPropSequentialRenderStatus <prop_OfxImageEffectPropSequentialRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropSequentialRenderStatus`)

- :ref:`OfxImageEffectPropInteractiveRenderStatus <prop_OfxImageEffectPropInteractiveRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropInteractiveRenderStatus`)

- :ref:`OfxImageEffectPropCudaEnabled <prop_OfxImageEffectPropCudaEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaEnabled`)

- :ref:`OfxImageEffectPropCudaRenderSupported <prop_OfxImageEffectPropCudaRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaRenderSupported`)

- :ref:`OfxImageEffectPropCudaStream <prop_OfxImageEffectPropCudaStream>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaStream`)

- :ref:`OfxImageEffectPropCudaStreamSupported <prop_OfxImageEffectPropCudaStreamSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaStreamSupported`)

- :ref:`OfxImageEffectPropMetalCommandQueue <prop_OfxImageEffectPropMetalCommandQueue>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalCommandQueue`)

- :ref:`OfxImageEffectPropMetalEnabled <prop_OfxImageEffectPropMetalEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalEnabled`)

- :ref:`OfxImageEffectPropMetalRenderSupported <prop_OfxImageEffectPropMetalRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalRenderSupported`)

- :ref:`OfxImageEffectPropOpenCLCommandQueue <prop_OfxImageEffectPropOpenCLCommandQueue>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLCommandQueue`)

- :ref:`OfxImageEffectPropOpenCLEnabled <prop_OfxImageEffectPropOpenCLEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLEnabled`)

- :ref:`OfxImageEffectPropOpenCLImage <prop_OfxImageEffectPropOpenCLImage>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLImage`)

- :ref:`OfxImageEffectPropOpenCLRenderSupported <prop_OfxImageEffectPropOpenCLRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLRenderSupported`)

- :ref:`OfxImageEffectPropOpenCLSupported <prop_OfxImageEffectPropOpenCLSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLSupported`)

- :ref:`OfxImageEffectPropOpenGLEnabled <prop_OfxImageEffectPropOpenGLEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLEnabled`)

- :ref:`OfxImageEffectPropOpenGLTextureIndex <prop_OfxImageEffectPropOpenGLTextureIndex>` - Type: int, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLTextureIndex`)

- :ref:`OfxImageEffectPropOpenGLTextureTarget <prop_OfxImageEffectPropOpenGLTextureTarget>` - Type: int, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLTextureTarget`)

- :ref:`OfxImageEffectPropInteractiveRenderStatus <prop_OfxImageEffectPropInteractiveRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropInteractiveRenderStatus`)

.. _action_OfxImageEffectActionDescribeInContext:

**OfxImageEffectActionDescribeInContext**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxImageEffectPropContext <prop_OfxImageEffectPropContext>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropContext`)

.. _action_OfxImageEffectActionEndSequenceRender:

**OfxImageEffectActionEndSequenceRender**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxImageEffectPropFrameRange <prop_OfxImageEffectPropFrameRange>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropFrameRange`)

- :ref:`OfxImageEffectPropFrameStep <prop_OfxImageEffectPropFrameStep>` - Type: double, Dimension: 1 (:c:macro:`kOfxImageEffectPropFrameStep`)

- :ref:`OfxPropIsInteractive <prop_OfxPropIsInteractive>` - Type: bool, Dimension: 1 (:c:macro:`kOfxPropIsInteractive`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

- :ref:`OfxImageEffectPropSequentialRenderStatus <prop_OfxImageEffectPropSequentialRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropSequentialRenderStatus`)

- :ref:`OfxImageEffectPropInteractiveRenderStatus <prop_OfxImageEffectPropInteractiveRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropInteractiveRenderStatus`)

- :ref:`OfxImageEffectPropCudaEnabled <prop_OfxImageEffectPropCudaEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaEnabled`)

- :ref:`OfxImageEffectPropCudaRenderSupported <prop_OfxImageEffectPropCudaRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaRenderSupported`)

- :ref:`OfxImageEffectPropCudaStream <prop_OfxImageEffectPropCudaStream>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaStream`)

- :ref:`OfxImageEffectPropCudaStreamSupported <prop_OfxImageEffectPropCudaStreamSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaStreamSupported`)

- :ref:`OfxImageEffectPropMetalCommandQueue <prop_OfxImageEffectPropMetalCommandQueue>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalCommandQueue`)

- :ref:`OfxImageEffectPropMetalEnabled <prop_OfxImageEffectPropMetalEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalEnabled`)

- :ref:`OfxImageEffectPropMetalRenderSupported <prop_OfxImageEffectPropMetalRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalRenderSupported`)

- :ref:`OfxImageEffectPropOpenCLCommandQueue <prop_OfxImageEffectPropOpenCLCommandQueue>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLCommandQueue`)

- :ref:`OfxImageEffectPropOpenCLEnabled <prop_OfxImageEffectPropOpenCLEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLEnabled`)

- :ref:`OfxImageEffectPropOpenCLImage <prop_OfxImageEffectPropOpenCLImage>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLImage`)

- :ref:`OfxImageEffectPropOpenCLRenderSupported <prop_OfxImageEffectPropOpenCLRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLRenderSupported`)

- :ref:`OfxImageEffectPropOpenCLSupported <prop_OfxImageEffectPropOpenCLSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLSupported`)

- :ref:`OfxImageEffectPropOpenGLEnabled <prop_OfxImageEffectPropOpenGLEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLEnabled`)

- :ref:`OfxImageEffectPropOpenGLTextureIndex <prop_OfxImageEffectPropOpenGLTextureIndex>` - Type: int, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLTextureIndex`)

- :ref:`OfxImageEffectPropOpenGLTextureTarget <prop_OfxImageEffectPropOpenGLTextureTarget>` - Type: int, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLTextureTarget`)

- :ref:`OfxImageEffectPropInteractiveRenderStatus <prop_OfxImageEffectPropInteractiveRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropInteractiveRenderStatus`)

.. _action_OfxImageEffectActionGetClipPreferences:

**OfxImageEffectActionGetClipPreferences**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Output Arguments**

- :ref:`OfxImageEffectPropFrameRate <prop_OfxImageEffectPropFrameRate>` - Type: double, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropFrameRate`)

- :ref:`OfxImageClipPropFieldOrder <prop_OfxImageClipPropFieldOrder>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropFieldOrder`)

- :ref:`OfxImageEffectPropPreMultiplication <prop_OfxImageEffectPropPreMultiplication>` - Type: enum, Dimension: 1 (doc: :c:macro:`kOfxImageEffectPropPreMultiplication`)

- :ref:`OfxImageClipPropContinuousSamples <prop_OfxImageClipPropContinuousSamples>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropContinuousSamples`)

- :ref:`OfxImageEffectFrameVarying <prop_OfxImageEffectFrameVarying>` - Type: bool, Dimension: 1 (doc: :c:macro:`kOfxImageEffectFrameVarying`)

.. _action_OfxImageEffectActionGetFramesNeeded:

**OfxImageEffectActionGetFramesNeeded**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

**Output Arguments**

- :ref:`OfxImageEffectPropFrameRange <prop_OfxImageEffectPropFrameRange>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropFrameRange`)

.. _action_OfxImageEffectActionGetOutputColourspace:

**OfxImageEffectActionGetOutputColourspace**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxImageClipPropPreferredColourspaces <prop_OfxImageClipPropPreferredColourspaces>` - Type: string, Dimension: Variable (:c:macro:`kOfxImageClipPropPreferredColourspaces`)

**Output Arguments**

- :ref:`OfxImageClipPropColourspace <prop_OfxImageClipPropColourspace>` - Type: string, Dimension: 1 (doc: :c:macro:`kOfxImageClipPropColourspace`)

.. _action_OfxImageEffectActionGetRegionOfDefinition:

**OfxImageEffectActionGetRegionOfDefinition**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

**Output Arguments**

- :ref:`OfxImageEffectPropRegionOfDefinition <prop_OfxImageEffectPropRegionOfDefinition>` - Type: double, Dimension: 4 (doc: :c:macro:`kOfxImageEffectPropRegionOfDefinition`)

.. _action_OfxImageEffectActionGetRegionsOfInterest:

**OfxImageEffectActionGetRegionsOfInterest**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

- :ref:`OfxImageEffectPropRegionOfInterest <prop_OfxImageEffectPropRegionOfInterest>` - Type: double, Dimension: 4 (:c:macro:`kOfxImageEffectPropRegionOfInterest`)

.. _action_OfxImageEffectActionGetTimeDomain:

**OfxImageEffectActionGetTimeDomain**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Output Arguments**

- :ref:`OfxImageEffectPropFrameRange <prop_OfxImageEffectPropFrameRange>` - Type: double, Dimension: 2 (doc: :c:macro:`kOfxImageEffectPropFrameRange`)

.. _action_OfxImageEffectActionIsIdentity:

**OfxImageEffectActionIsIdentity**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropFieldToRender <prop_OfxImageEffectPropFieldToRender>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropFieldToRender`)

- :ref:`OfxImageEffectPropRenderWindow <prop_OfxImageEffectPropRenderWindow>` - Type: int, Dimension: 4 (:c:macro:`kOfxImageEffectPropRenderWindow`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxImageEffectActionRender:

**OfxImageEffectActionRender**
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropSequentialRenderStatus <prop_OfxImageEffectPropSequentialRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropSequentialRenderStatus`)

- :ref:`OfxImageEffectPropInteractiveRenderStatus <prop_OfxImageEffectPropInteractiveRenderStatus>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropInteractiveRenderStatus`)

- :ref:`OfxImageEffectPropRenderQualityDraft <prop_OfxImageEffectPropRenderQualityDraft>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropRenderQualityDraft`)

- :ref:`OfxImageEffectPropCudaEnabled <prop_OfxImageEffectPropCudaEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaEnabled`)

- :ref:`OfxImageEffectPropCudaRenderSupported <prop_OfxImageEffectPropCudaRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaRenderSupported`)

- :ref:`OfxImageEffectPropCudaStream <prop_OfxImageEffectPropCudaStream>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaStream`)

- :ref:`OfxImageEffectPropCudaStreamSupported <prop_OfxImageEffectPropCudaStreamSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropCudaStreamSupported`)

- :ref:`OfxImageEffectPropMetalCommandQueue <prop_OfxImageEffectPropMetalCommandQueue>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalCommandQueue`)

- :ref:`OfxImageEffectPropMetalEnabled <prop_OfxImageEffectPropMetalEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalEnabled`)

- :ref:`OfxImageEffectPropMetalRenderSupported <prop_OfxImageEffectPropMetalRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropMetalRenderSupported`)

- :ref:`OfxImageEffectPropOpenCLCommandQueue <prop_OfxImageEffectPropOpenCLCommandQueue>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLCommandQueue`)

- :ref:`OfxImageEffectPropOpenCLEnabled <prop_OfxImageEffectPropOpenCLEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLEnabled`)

- :ref:`OfxImageEffectPropOpenCLImage <prop_OfxImageEffectPropOpenCLImage>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLImage`)

- :ref:`OfxImageEffectPropOpenCLRenderSupported <prop_OfxImageEffectPropOpenCLRenderSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLRenderSupported`)

- :ref:`OfxImageEffectPropOpenCLSupported <prop_OfxImageEffectPropOpenCLSupported>` - Type: enum, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenCLSupported`)

- :ref:`OfxImageEffectPropOpenGLEnabled <prop_OfxImageEffectPropOpenGLEnabled>` - Type: bool, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLEnabled`)

- :ref:`OfxImageEffectPropOpenGLTextureIndex <prop_OfxImageEffectPropOpenGLTextureIndex>` - Type: int, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLTextureIndex`)

- :ref:`OfxImageEffectPropOpenGLTextureTarget <prop_OfxImageEffectPropOpenGLTextureTarget>` - Type: int, Dimension: 1 (:c:macro:`kOfxImageEffectPropOpenGLTextureTarget`)

.. _action_OfxInteractActionDraw:

**OfxInteractActionDraw**
^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`OfxInteractPropDrawContext <prop_OfxInteractPropDrawContext>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxInteractPropDrawContext`)

- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPixelScale`)

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (:c:macro:`kOfxInteractPropBackgroundColour`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxInteractActionGainFocus:

**OfxInteractActionGainFocus**
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPixelScale`)

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (:c:macro:`kOfxInteractPropBackgroundColour`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxInteractActionKeyDown:

**OfxInteractActionKeyDown**
^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`kOfxPropKeySym <prop_kOfxPropKeySym>` - Type: int, Dimension: 1 (:c:macro:`kOfxPropKeySym`)

- :ref:`kOfxPropKeyString <prop_kOfxPropKeyString>` - Type: string, Dimension: 1 (:c:macro:`kOfxPropKeyString`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxInteractActionKeyRepeat:

**OfxInteractActionKeyRepeat**
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`kOfxPropKeySym <prop_kOfxPropKeySym>` - Type: int, Dimension: 1 (:c:macro:`kOfxPropKeySym`)

- :ref:`kOfxPropKeyString <prop_kOfxPropKeyString>` - Type: string, Dimension: 1 (:c:macro:`kOfxPropKeyString`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxInteractActionKeyUp:

**OfxInteractActionKeyUp**
^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`kOfxPropKeySym <prop_kOfxPropKeySym>` - Type: int, Dimension: 1 (:c:macro:`kOfxPropKeySym`)

- :ref:`kOfxPropKeyString <prop_kOfxPropKeyString>` - Type: string, Dimension: 1 (:c:macro:`kOfxPropKeyString`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxInteractActionLoseFocus:

**OfxInteractActionLoseFocus**
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPixelScale`)

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (:c:macro:`kOfxInteractPropBackgroundColour`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

.. _action_OfxInteractActionPenDown:

**OfxInteractActionPenDown**
^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPixelScale`)

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (:c:macro:`kOfxInteractPropBackgroundColour`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

- :ref:`OfxInteractPropPenPosition <prop_OfxInteractPropPenPosition>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPenPosition`)

- :ref:`OfxInteractPropPenViewportPosition <prop_OfxInteractPropPenViewportPosition>` - Type: int, Dimension: 2 (:c:macro:`kOfxInteractPropPenViewportPosition`)

- :ref:`OfxInteractPropPenPressure <prop_OfxInteractPropPenPressure>` - Type: double, Dimension: 1 (:c:macro:`kOfxInteractPropPenPressure`)

.. _action_OfxInteractActionPenMotion:

**OfxInteractActionPenMotion**
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPixelScale`)

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (:c:macro:`kOfxInteractPropBackgroundColour`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

- :ref:`OfxInteractPropPenPosition <prop_OfxInteractPropPenPosition>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPenPosition`)

- :ref:`OfxInteractPropPenViewportPosition <prop_OfxInteractPropPenViewportPosition>` - Type: int, Dimension: 2 (:c:macro:`kOfxInteractPropPenViewportPosition`)

- :ref:`OfxInteractPropPenPressure <prop_OfxInteractPropPenPressure>` - Type: double, Dimension: 1 (:c:macro:`kOfxInteractPropPenPressure`)

.. _action_OfxInteractActionPenUp:

**OfxInteractActionPenUp**
^^^^^^^^^^^^^^^^^^^^^^

**Input Arguments**

- :ref:`OfxPropEffectInstance <prop_OfxPropEffectInstance>` - Type: pointer, Dimension: 1 (:c:macro:`kOfxPropEffectInstance`)

- :ref:`OfxInteractPropPixelScale <prop_OfxInteractPropPixelScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPixelScale`)

- :ref:`OfxInteractPropBackgroundColour <prop_OfxInteractPropBackgroundColour>` - Type: double, Dimension: 3 (:c:macro:`kOfxInteractPropBackgroundColour`)

- :ref:`OfxPropTime <prop_OfxPropTime>` - Type: double, Dimension: 1 (:c:macro:`kOfxPropTime`)

- :ref:`OfxImageEffectPropRenderScale <prop_OfxImageEffectPropRenderScale>` - Type: double, Dimension: 2 (:c:macro:`kOfxImageEffectPropRenderScale`)

- :ref:`OfxInteractPropPenPosition <prop_OfxInteractPropPenPosition>` - Type: double, Dimension: 2 (:c:macro:`kOfxInteractPropPenPosition`)

- :ref:`OfxInteractPropPenViewportPosition <prop_OfxInteractPropPenViewportPosition>` - Type: int, Dimension: 2 (:c:macro:`kOfxInteractPropPenViewportPosition`)

- :ref:`OfxInteractPropPenPressure <prop_OfxInteractPropPenPressure>` - Type: double, Dimension: 1 (:c:macro:`kOfxInteractPropPenPressure`)

