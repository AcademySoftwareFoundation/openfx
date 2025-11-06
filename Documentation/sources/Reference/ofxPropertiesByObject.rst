.. SPDX-License-Identifier: CC-BY-4.0
.. warning::

    This section is outdated and should be properly generated automatically from source code instead
    of maintaining it aside

Properties by object reference
==============================

.. ImageEffectHostProperties:

Properties on the Image Effect Host
-----------------------------------

-  kOfxPropAPIVersion
   - (read only) the version of the API implemented by the host. If not present, it is safe to assume "1.0"
-  kOfxPropType
   - (read only) type of the object. Set to kOfxTypeImageEffectHost
-  kOfxPropName
   - (read only) the globally unique name of the application, eg: "com.acmesofware.funkyCompositor"
-  kOfxPropLabel
   - (read only) the user visible name of the application
-  kOfxPropVersion
   - (read only) the version number of the host
-  kOfxPropVersionLabel
   - (read only) a user readable version label
-  kOfxImageEffectHostPropIsBackground
   - (read only) is the application a background renderer
-  kOfxImageEffectPropSupportsOverlays
   - (read only) does the application support overlay interactive GUIs
-  kOfxImageEffectPropSupportsMultiResolution
   - (read only) does the application support images of different sizes
-  kOfxImageEffectPropSupportsTiles
   - (read only) does the application support image tiling
-  kOfxImageEffectPropTemporalClipAccess
   - (read only) does the application allow random temporal access to
   source images
-  kOfxImageEffectPropSupportedComponents
   - (read only) a list of supported colour components
-  kOfxImageEffectPropSupportedContexts
   - (read only) a list of supported effect contexts
-  kOfxImageEffectPropSupportsMultipleClipDepths
   - (read only) does the application allow inputs and output clips to
   have differing bit depths
-  kOfxImageEffectPropSupportsMultipleClipPARs
   - (read only) does the application allow inputs and output clips to
   have differing pixel aspect ratios
-  kOfxImageEffectPropSetableFrameRate
   - (read only) does the application allow an effect to change the
   frame rate of the output clip
-  kOfxImageEffectPropSetableFielding
   - (read only) does the application allow an effect to change the
   fielding of the output clip
-  kOfxParamHostPropSupportsCustomInteract
   - (read only) does the application
-  kOfxParamHostPropSupportsStringAnimation
   - (read only) does the application allow the animation of string
   parameters
-  kOfxParamHostPropSupportsChoiceAnimation
   - (read only) does the application allow the animation of choice
   parameters
-  kOfxParamHostPropSupportsBooleanAnimation
   - (read only does the application allow the animation of boolean
   parameters)
-  kOfxParamHostPropSupportsCustomAnimation
   - (read only) does the application allow the animation of custom
   parameters
-  kOfxParamHostPropMaxParameters
   - (read only) the maximum number of parameters the application allows
   a plug-in to have
-  kOfxParamHostPropMaxPages
   - (read only) the maximum number of parameter pages the application
   allows a plug-in to have
-  kOfxParamHostPropPageRowColumnCount
   - (read only) the number of rows and columns on a page parameter
-  kOfxPropHostOSHandle
   - (read only) a pointer to an OS specific application handle (eg: the
   root hWnd on Windows)
-  kOfxParamHostPropSupportsParametricAnimation
   - (read only) does the host support animation of parametric
   parameters
-  kOfxImageEffectInstancePropSequentialRender
   - (read only) does the host support sequential rendering
-  kOfxImageEffectPropOpenGLRenderSupported
   - (read only) does the host support OpenGL accelerated rendering
-  kOfxImageEffectPropRenderQualityDraft
   - (read only) does the host support draft quality rendering
-  kOfxImageEffectHostPropNativeOrigin
   - (read only) native origin of the host

.. EffectDescriptorProperties:

Properties on an Effect Descriptor
----------------------------------

An image effect plugin (ie: that thing passed to the initial 'describe'
action) has the following properties, these can only be set inside the
'describe' actions ...

-  kOfxPropType
   - (read only)
-  kOfxPropLabel
   - (read/write)
-  kOfxPropShortLabel
   - (read/write)
-  kOfxPropLongLabel
   - (read/write)
-  kOfxPropVersion
   - (read only) the version number of the plugin
-  kOfxPropVersionLabel
   - (read only) a user readable version label
-  kOfxPropPluginDescription
   - (read/write), a short description of the plugin
-  kOfxImageEffectPropSupportedContexts
   - (read/write)
-  kOfxImageEffectPluginPropGrouping
   - (read/write)
-  kOfxImageEffectPluginPropSingleInstance
   - (read/write)
-  kOfxImageEffectPluginRenderThreadSafety
   - (read/write)
-  kOfxImageEffectPluginPropHostFrameThreading
   - (read/write)
-  kOfxImageEffectPluginPropOverlayInteractV1
   - (read/write)
-  kOfxImageEffectPropSupportsMultiResolution
   - (read/write)
-  kOfxImageEffectPropSupportsTiles
   - (read/write)
-  kOfxImageEffectPropTemporalClipAccess
   - (read/write)
-  kOfxImageEffectPropSupportedPixelDepths
   - (read/write)
-  kOfxImageEffectPluginPropFieldRenderTwiceAlways
   - (read/write)
-  kOfxImageEffectPropSupportsMultipleClipDepths
   - (read/write)
-  kOfxImageEffectPropSupportsMultipleClipPARs
   - (read/write)
-  kOfxImageEffectPluginRenderThreadSafety
   - (read/write)
-  kOfxImageEffectPropClipPreferencesSlaveParam
   - (read/write)
-  kOfxImageEffectPropOpenGLRenderSupported
   - (read and write)
-  kOfxPluginPropFilePath
   (read only)

.. EffectInstanceProperties:

Properties on an Effect Instance
--------------------------------

An image effect instance has the following properties, all but
kOfxPropInstanceData and kOfxImageEffectInstancePropSequentialRender are
read only...

-  kOfxPropType
   - (read only)
-  kOfxImageEffectPropContext
   - (read only)
-  kOfxPropInstanceData
   - (read and write)
-  kOfxImageEffectPropProjectSize
   - (read only)
-  kOfxImageEffectPropProjectOffset
   - (read only)
-  kOfxImageEffectPropProjectExtent
   - (read only)
-  kOfxImageEffectPropProjectPixelAspectRatio
   - (read only)
-  kOfxImageEffectInstancePropEffectDuration
   - (read only)
-  kOfxImageEffectInstancePropSequentialRender
   - (read and write)
-  kOfxImageEffectPropSupportsTiles
   - (read/write)
-  kOfxImageEffectPropOpenGLRenderSupported
   - (read and write)
-  kOfxImageEffectPropFrameRate
   - (read only)
-  kOfxPropIsInteractive
   - (read only)

.. ClipDescriptorProperties:

Properties on a Clip Descriptor
-------------------------------

All OfxImageClipHandle accessed inside the ``kOfxActionDescribe`` or
``kOfxActionDescribeInContext`` are clip descriptors, used to describe
the behaviour of clips in a specific context.

-  kOfxPropType
   - (read only) set to
-  kOfxPropName
   - (read only) the name the clip was created with
-  kOfxPropLabel
   - (read/write) the user visible label for the clip
-  kOfxPropShortLabel
   - (read/write)
-  kOfxPropLongLabel
   - (read/write)
-  kOfxImageEffectPropSupportedComponents
   - (read/write)
-  kOfxImageEffectPropTemporalClipAccess
   - (read/write)
-  kOfxImageClipPropOptional
   - (read/write)
-  kOfxImageClipPropFieldExtraction
   - (read/write)
-  kOfxImageClipPropIsMask
   - (read/write)
-  kOfxImageEffectPropSupportsTiles
   - (read/write)

.. ClipInstanceProperties:

Properties on a Clip Instance
-----------------------------

-  kOfxPropType
   - (read only)
-  kOfxPropName
   - (read only)
-  kOfxPropLabel
   - (read only)
-  kOfxPropShortLabel
   - (read only)
-  kOfxPropLongLabel
   - (read only)
-  kOfxImageEffectPropSupportedComponents
   - (read only)
-  kOfxImageEffectPropTemporalClipAccess
   - (read only)
-  kOfxImageClipPropOptional
   - (read only)
-  kOfxImageClipPropFieldExtraction
   - (read only)
-  kOfxImageClipPropIsMask
   - (read only)
-  kOfxImageEffectPropSupportsTiles
   - (read only)
-  kOfxImageEffectPropPixelDepth
   - (read only)
-  kOfxImageEffectPropComponents
   - (read only)
-  kOfxImageClipPropUnmappedPixelDepth
   - (read only)
-  kOfxImageClipPropUnmappedComponents
   - (read only)
-  kOfxImageEffectPropPreMultiplication
   - (read only)
-  kOfxImagePropPixelAspectRatio
   - (read only)
-  kOfxImageEffectPropFrameRate
   - (read only)
-  kOfxImageEffectPropFrameRange
   - (read only)
-  kOfxImageClipPropFieldOrder
   - (read only)
-  kOfxImageClipPropConnected
   - (read only)
-  kOfxImageEffectPropUnmappedFrameRange
   - (read only)\*
-  kOfxImageEffectPropUnmappedFrameRate
   - (read only)\*
-  kOfxImageClipPropContinuousSamples
   - (read only)

.. ImageProperties:

Properties on an Image
----------------------

All images are instances, there is no such thing as an image descriptor.

-  kOfxPropType
   - (read only)
-  kOfxImageEffectPropPixelDepth
   - (read only)
-  kOfxImageEffectPropComponents
   - (read only)
-  kOfxImageEffectPropPreMultiplication
   - (read only)
-  kOfxImageEffectPropRenderScale
   - (read only)
-  kOfxImagePropPixelAspectRatio
   - (read only)
-  kOfxImagePropData
   - (read only)
-  kOfxImagePropBounds
   - (read only)
-  kOfxImagePropRegionOfDefinition
   - (read only) \*
-  kOfxImagePropRowBytes
   - (read only)
-  kOfxImagePropField
   - (read only)
-  kOfxImagePropUniqueIdentifier
   - (read only)

.. ParameterSetProperties:

Properties on Parameter Set Instances
-------------------------------------

kOfxPropParamSetNeedsSyncing
, which indicates if private data is dirty and may need re-syncing to a
parameter set
.. ParameterProperties:

Properties on Parameter Descriptors and Instances
-------------------------------------------------

Properties Common to All Parameters
-----------------------------------

The following properties are common to all parameters....

-  kOfxPropType
   , which will always be
   kOfxTypeParameter
   (read only)
-  kOfxPropName
   read/write in the descriptor, read only on an instance
-  kOfxPropLabel
   read/write in the descriptor and instance
-  kOfxPropShortLabel
   read/write in the descriptor and instance
-  kOfxPropLongLabel
   read/write in the descriptor and instance
-  kOfxParamPropType
   read only in the descriptor and instance, the value is set on
   construction
-  kOfxParamPropSecret
   read/write in the descriptor and instance
-  kOfxParamPropHint
   read/write in the descriptor and instance
-  kOfxParamPropScriptName
   read/write in the descriptor, read only on an instance
-  kOfxParamPropParent
   read/write in the descriptor, read only on an instance
-  kOfxParamPropEnabled
   read/write in the descriptor and instance
-  kOfxParamPropDataPtr
   read/write in the descriptor and instance
-  kOfxPropIcon
   , read/write on a descriptor, read only on an instance

Properties On Group Parameters
------------------------------

-  kOfxParamPropGroupOpen
   read/write in the descriptor, read only on an instance

Properties Common to All But Group and Page Parameters
------------------------------------------------------

-  kOfxParamPropInteractV1
   read/write in the descriptor, read only on an instance
-  kOfxParamPropInteractSize
   read/write in the descriptor, read only on an instance
-  kOfxParamPropInteractSizeAspect
   read/write in the descriptor, read only on an instance
-  kOfxParamPropInteractMinimumSize
   read/write in the descriptor, read only on an instance
-  kOfxParamPropInteractPreferedSize
   read/write in the descriptor, read only on an instance
-  kOfxParamPropHasHostOverlayHandle
   read only in the descriptor and instance
-  kOfxParamPropUseHostOverlayHandle
   read/write in the descriptor and read only in the instance

Properties Common to All Parameters That Hold Values
----------------------------------------------------

-  kOfxParamPropDefault
   read/write in the descriptor, read only on an instance
-  kOfxParamPropAnimates
   read/write in the descriptor, read only on an instance
-  kOfxParamPropIsAnimating
   read/write in the descriptor, read only on an instance
-  kOfxParamPropIsAutoKeying
   read/write in the descriptor, read only on an instance
-  kOfxParamPropPersistant
   read/write in the descriptor, read only on an instance
-  kOfxParamPropEvaluateOnChange
   read/write in the descriptor and instance
-  kOfxParamPropPluginMayWrite
   read/write in the descriptor, read only on an instance
-  kOfxParamPropCacheInvalidation
   read/write in the descriptor, read only on an instance
-  kOfxParamPropCanUndo
   read/write in the descriptor, read only on an instance

Properties Common to All Numeric Parameters
-------------------------------------------

-  kOfxParamPropMin
   read/write in the descriptor and instance
-  kOfxParamPropMax
   read/write in the descriptor and instance
-  kOfxParamPropDisplayMin
   read/write in the descriptor and instance
-  kOfxParamPropDisplayMax
   read/write in the descriptor and instance

Properties Common to All Double Parameters
------------------------------------------

-  kOfxParamPropIncrement
   read/write in the descriptor and instance
-  kOfxParamPropDigits
   read/write in the descriptor and instance

Properties On 1D Double Parameters
----------------------------------

-  kOfxParamPropShowTimeMarker
   read/write in the descriptor and instance
-  kOfxParamPropDoubleType
   read/write in the descriptor, read only on an instance

Properties On 2D and 3D Double Parameters
-----------------------------------------

-  kOfxParamPropDoubleType
   read/write in the descriptor, read only on an instance

Properties On Non Normalised Spatial Double Parameters
------------------------------------------------------

-  kOfxParamPropDefaultCoordinateSystem
   read/write in the descriptor, read only on an instance

Properties On 2D and 3D Integer Parameters
------------------------------------------

-  kOfxParamPropDimensionLabel
   read/write in the descriptor, read only on an instance

Properties On String Parameters
-------------------------------

-  kOfxParamPropStringMode
   read/write in the descriptor, read only on an instance
-  kOfxParamPropStringFilePathExists
   read/write in the descriptor, read only on an instance

Properties On Choice Parameters
-------------------------------

-  kOfxParamPropChoiceOption
   read/write in the descriptor and instance
-  kOfxParamPropChoiceOrder
   read/write in the descriptor and instance

Properties On Custom Parameters
-------------------------------

-  kOfxParamPropCustomInterpCallbackV1
   read/write in the descriptor, read only on an instance

Properties On Page Parameters
-----------------------------

-  kOfxParamPropPageChild
   read/write in the descriptor, read only on an instance

On Parametric Parameters
------------------------

-  kOfxParamPropAnimates
   read/write in the descriptor, read only on an instance
-  kOfxParamPropIsAnimating
   read/write in the descriptor, read only on an instance
-  kOfxParamPropIsAutoKeying
   read/write in the descriptor, read only on an instance
-  kOfxParamPropPersistant
   read/write in the descriptor, read only on an instance
-  kOfxParamPropEvaluateOnChange
   read/write in the descriptor and instance
-  kOfxParamPropPluginMayWrite
   read/write in the descriptor, read only on an instance
-  kOfxParamPropCacheInvalidation
   read/write in the descriptor, read only on an instance
-  kOfxParamPropCanUndo
   read/write in the descriptor, read only on an instance
-  kOfxParamPropParametricDimension
   read/write in the descriptor, read only on an instance
-  kOfxParamPropParametricUIColour
   read/write in the descriptor, read only on an instance
-  kOfxParamPropParametricInteractBackground
   read/write in the descriptor, read only on an instance
-  kOfxParamPropParametricRange
   read/write in the descriptor, read only on an instance

.. InteractDescriptorProperties:

Properties on Interact Descriptors
----------------------------------

-  kOfxInteractPropHasAlpha
   read only
-  kOfxInteractPropBitDepth
   read only

.. InteractInstanceProperties:

Properties on Interact Instances
--------------------------------

-  kOfxPropEffectInstance
   read only
-  kOfxPropInstanceData
   read/write only
-  kOfxInteractPropPixelScale
   read only
-  kOfxInteractPropBackgroundColour
   read only
-  kOfxInteractPropHasAlpha
   read only
-  kOfxInteractPropBitDepth
   read only
-  kOfxInteractPropSlaveToParam
   read/write
-  kOfxInteractPropSuggestedColour
   read only
