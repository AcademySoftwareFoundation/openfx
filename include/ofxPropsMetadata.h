// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
// NOTE: This file is auto-generated by gen-props.py. DO NOT EDIT.

#pragma once

#include <string>
#include <vector>
#include "ofxImageEffect.h"
#include "ofxGPURender.h"
#include "ofxColour.h"
#include "ofxDrawSuite.h"
#include "ofxParametricParam.h"
#include "ofxKeySyms.h"
#include "ofxOld.h"

namespace OpenFX {
enum class PropType {
   Int,
   Double,
   Enum,
   Bool,
   String,
   Bytes,
   Pointer
};

enum class Writable {
   Host,
   Plugin,
   All
};

struct PropsMetadata {
  std::string name;
  std::vector<PropType> types;
  int dimension;
  Writable writable;
  bool host_optional;
  std::vector<const char *> values; // for enums
};

const std::vector<struct PropsMetadata> props_metadata {
{ kOfxImageClipPropColourspace, {PropType::String}, 1, Writable::All, false, {} },
{ kOfxImageClipPropConnected, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageClipPropContinuousSamples, {PropType::Bool}, 1, Writable::All, false, {} },
{ kOfxImageClipPropFieldExtraction, {PropType::Enum}, 1, Writable::Plugin, false, {"kOfxImageFieldNone","kOfxImageFieldLower","kOfxImageFieldUpper","kOfxImageFieldBoth","kOfxImageFieldSingle","kOfxImageFieldDoubled"} },
{ kOfxImageClipPropFieldOrder, {PropType::Enum}, 1, Writable::All, false, {"kOfxImageFieldNone","kOfxImageFieldLower","kOfxImageFieldUpper"} },
{ kOfxImageClipPropIsMask, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxImageClipPropOptional, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxImageClipPropPreferredColourspaces, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxImageClipPropUnmappedComponents, {PropType::Enum}, 1, Writable::Host, false, {"kOfxImageComponentNone","kOfxImageComponentRGBA","kOfxImageComponentRGB","kOfxImageComponentAlpha"} },
{ kOfxImageClipPropUnmappedPixelDepth, {PropType::Enum}, 1, Writable::Host, false, {"kOfxBitDepthNone","kOfxBitDepthByte","kOfxBitDepthShort","kOfxBitDepthHalf","kOfxBitDepthFloat"} },
{ kOfxImageEffectFrameVarying, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectHostPropIsBackground, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectHostPropNativeOrigin, {PropType::Enum}, 1, Writable::Host, false, {"kOfxImageEffectHostPropNativeOriginBottomLeft","kOfxImageEffectHostPropNativeOriginTopLeft","kOfxImageEffectHostPropNativeOriginCenter"} },
{ kOfxImageEffectInstancePropEffectDuration, {PropType::Double}, 1, Writable::Host, false, {} },
{ kOfxImageEffectInstancePropSequentialRender, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPluginPropFieldRenderTwiceAlways, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPluginPropGrouping, {PropType::String}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPluginPropHostFrameThreading, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPluginPropOverlayInteractV1, {PropType::Pointer}, 1, Writable::All, false, {} },
{ kOfxImageEffectPluginPropOverlayInteractV2, {PropType::Pointer}, 1, Writable::All, false, {} },
{ kOfxImageEffectPluginPropSingleInstance, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPluginRenderThreadSafety, {PropType::String}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPropClipPreferencesSlaveParam, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxImageEffectPropColourManagementAvailableConfigs, {PropType::String}, 0, Writable::All, false, {} },
{ kOfxImageEffectPropColourManagementConfig, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropColourManagementStyle, {PropType::Enum}, 1, Writable::All, false, {"kOfxImageEffectPropColourManagementNone","kOfxImageEffectPropColourManagementBasic","kOfxImageEffectPropColourManagementCore","kOfxImageEffectPropColourManagementFull","kOfxImageEffectPropColourManagementOCIO"} },
{ kOfxImageEffectPropComponents, {PropType::Enum}, 1, Writable::Host, false, {"kOfxImageComponentNone","kOfxImageComponentRGBA","kOfxImageComponentRGB","kOfxImageComponentAlpha"} },
{ kOfxImageEffectPropContext, {PropType::Enum}, 1, Writable::Host, false, {"kOfxImageEffectContextGenerator","kOfxImageEffectContextFilter","kOfxImageEffectContextTransition","kOfxImageEffectContextPaint","kOfxImageEffectContextGeneral","kOfxImageEffectContextRetimer"} },
{ kOfxImageEffectPropCudaEnabled, {PropType::Bool}, 1, Writable::All, false, {} },
{ kOfxImageEffectPropCudaRenderSupported, {PropType::Enum}, 1, Writable::Plugin, false, {"false","true","needed"} },
{ kOfxImageEffectPropCudaStream, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropCudaStreamSupported, {PropType::Enum}, 1, Writable::Plugin, false, {"false","true","needed"} },
{ kOfxImageEffectPropDisplayColourspace, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropFieldToRender, {PropType::Enum}, 1, Writable::Host, false, {"kOfxImageFieldNone","kOfxImageFieldBoth","kOfxImageFieldLower","kOfxImageFieldUpper"} },
{ kOfxImageEffectPropFrameRange, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxImageEffectPropFrameRate, {PropType::Double}, 1, Writable::All, false, {} },
{ kOfxImageEffectPropFrameStep, {PropType::Double}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropInAnalysis, {PropType::Bool}, 1, Writable::All, false, {} },
{ kOfxImageEffectPropInteractiveRenderStatus, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropMetalCommandQueue, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropMetalEnabled, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropMetalRenderSupported, {PropType::Enum}, 1, Writable::Plugin, false, {"false","true","needed"} },
{ kOfxImageEffectPropOCIOConfig, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOCIODisplay, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOCIOView, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOpenCLCommandQueue, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOpenCLEnabled, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOpenCLImage, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOpenCLRenderSupported, {PropType::Enum}, 1, Writable::Plugin, false, {"false","true","needed"} },
{ kOfxImageEffectPropOpenCLSupported, {PropType::Enum}, 1, Writable::All, false, {"false","true"} },
{ kOfxImageEffectPropOpenGLEnabled, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOpenGLRenderSupported, {PropType::Enum}, 1, Writable::Plugin, false, {"false","true","needed"} },
{ kOfxImageEffectPropOpenGLTextureIndex, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropOpenGLTextureTarget, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropPixelDepth, {PropType::Enum}, 1, Writable::Host, false, {"kOfxBitDepthNone","kOfxBitDepthByte","kOfxBitDepthShort","kOfxBitDepthHalf","kOfxBitDepthFloat"} },
{ kOfxImageEffectPropPluginHandle, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropPreMultiplication, {PropType::Enum}, 1, Writable::All, false, {"kOfxImageOpaque","kOfxImagePreMultiplied","kOfxImageUnPreMultiplied"} },
{ kOfxImageEffectPropProjectExtent, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxImageEffectPropProjectOffset, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxImageEffectPropProjectPixelAspectRatio, {PropType::Double}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropProjectSize, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxImageEffectPropRegionOfDefinition, {PropType::Int}, 4, Writable::Host, false, {} },
{ kOfxImageEffectPropRegionOfInterest, {PropType::Int}, 4, Writable::Host, false, {} },
{ kOfxImageEffectPropRenderQualityDraft, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropRenderScale, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxImageEffectPropRenderWindow, {PropType::Int}, 4, Writable::Host, false, {} },
{ kOfxImageEffectPropSequentialRenderStatus, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropSetableFielding, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropSetableFrameRate, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropSupportedComponents, {PropType::Enum}, 0, Writable::Host, false, {"kOfxImageComponentNone","kOfxImageComponentRGBA","kOfxImageComponentRGB","kOfxImageComponentAlpha"} },
{ kOfxImageEffectPropSupportedContexts, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxImageEffectPropSupportedPixelDepths, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxImageEffectPropSupportsMultiResolution, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPropSupportsMultipleClipDepths, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPropSupportsMultipleClipPARs, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPropSupportsOverlays, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxImageEffectPropSupportsTiles, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPropTemporalClipAccess, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxImageEffectPropUnmappedFrameRange, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxImageEffectPropUnmappedFrameRate, {PropType::Double}, 1, Writable::Host, false, {} },
{ kOfxImagePropBounds, {PropType::Int}, 4, Writable::Host, false, {} },
{ kOfxImagePropData, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxImagePropField, {PropType::Enum}, 1, Writable::Host, false, {"kOfxImageFieldNone","kOfxImageFieldBoth","kOfxImageFieldLower","kOfxImageFieldUpper"} },
{ kOfxImagePropPixelAspectRatio, {PropType::Double}, 1, Writable::All, false, {} },
{ kOfxImagePropRegionOfDefinition, {PropType::Int}, 4, Writable::Host, false, {} },
{ kOfxImagePropRowBytes, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxImagePropUniqueIdentifier, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxInteractPropBackgroundColour, {PropType::Double}, 3, Writable::Host, false, {} },
{ kOfxInteractPropBitDepth, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxInteractPropDrawContext, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxInteractPropHasAlpha, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxInteractPropPenPosition, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxInteractPropPenPressure, {PropType::Double}, 1, Writable::Host, false, {} },
{ kOfxInteractPropPenViewportPosition, {PropType::Int}, 2, Writable::Host, false, {} },
{ kOfxInteractPropPixelScale, {PropType::Double}, 2, Writable::Host, false, {} },
{ kOfxInteractPropSlaveToParam, {PropType::String}, 0, Writable::All, false, {} },
{ kOfxInteractPropSuggestedColour, {PropType::Double}, 3, Writable::Host, false, {} },
{ kOfxInteractPropViewportSize, {PropType::Int}, 2, Writable::Host, false, {} },
{ kOfxOpenGLPropPixelDepth, {PropType::Enum}, 0, Writable::Host, false, {"kOfxBitDepthNone","kOfxBitDepthByte","kOfxBitDepthShort","kOfxBitDepthHalf","kOfxBitDepthFloat"} },
{ kOfxParamHostPropMaxPages, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropMaxParameters, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropPageRowColumnCount, {PropType::Int}, 2, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsBooleanAnimation, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsChoiceAnimation, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsCustomAnimation, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsCustomInteract, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsParametricAnimation, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsStrChoice, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsStrChoiceAnimation, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamHostPropSupportsStringAnimation, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamPropAnimates, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamPropCacheInvalidation, {PropType::Enum}, 1, Writable::All, false, {"kOfxParamInvalidateValueChange","kOfxParamInvalidateValueChangeToEnd","kOfxParamInvalidateAll"} },
{ kOfxParamPropCanUndo, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropChoiceEnum, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamPropChoiceOption, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropChoiceOrder, {PropType::Int}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropCustomInterpCallbackV1, {PropType::Pointer}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropCustomValue, {PropType::String}, 2, Writable::Plugin, false, {} },
{ kOfxParamPropDataPtr, {PropType::Pointer}, 1, Writable::All, false, {} },
{ kOfxParamPropDefault, {PropType::Int,PropType::Double,PropType::String,PropType::Bytes}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropDefaultCoordinateSystem, {PropType::Enum}, 1, Writable::Plugin, false, {"kOfxParamCoordinatesCanonical","kOfxParamCoordinatesNormalised"} },
{ kOfxParamPropDigits, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropDimensionLabel, {PropType::String}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropDisplayMax, {PropType::Int,PropType::Double}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropDisplayMin, {PropType::Int,PropType::Double}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropDoubleType, {PropType::Enum}, 1, Writable::Plugin, false, {"kOfxParamDoubleTypePlain","kOfxParamDoubleTypeAngle","kOfxParamDoubleTypeScale","kOfxParamDoubleTypeTime","kOfxParamDoubleTypeAbsoluteTime","kOfxParamDoubleTypeX","kOfxParamDoubleTypeXAbsolute","kOfxParamDoubleTypeY","kOfxParamDoubleTypeYAbsolute","kOfxParamDoubleTypeXY","kOfxParamDoubleTypeXYAbsolute"} },
{ kOfxParamPropEnabled, {PropType::Bool}, 1, Writable::All, false, {} },
{ kOfxParamPropEvaluateOnChange, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropGroupOpen, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropHasHostOverlayHandle, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamPropHint, {PropType::String}, 1, Writable::All, false, {} },
{ kOfxParamPropIncrement, {PropType::Double}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropInteractMinimumSize, {PropType::Double}, 2, Writable::Plugin, false, {} },
{ kOfxParamPropInteractPreferedSize, {PropType::Int}, 2, Writable::Plugin, false, {} },
{ kOfxParamPropInteractSize, {PropType::Double}, 2, Writable::Plugin, false, {} },
{ kOfxParamPropInteractSizeAspect, {PropType::Double}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropInteractV1, {PropType::Pointer}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropInterpolationAmount, {PropType::Double}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropInterpolationTime, {PropType::Double}, 2, Writable::Plugin, false, {} },
{ kOfxParamPropIsAnimating, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamPropIsAutoKeying, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxParamPropMax, {PropType::Int,PropType::Double}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropMin, {PropType::Int,PropType::Double}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropPageChild, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropParametricDimension, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropParametricInteractBackground, {PropType::Pointer}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropParametricRange, {PropType::Double}, 2, Writable::Plugin, false, {} },
{ kOfxParamPropParametricUIColour, {PropType::Double}, 0, Writable::Plugin, false, {} },
{ kOfxParamPropParent, {PropType::String}, 1, Writable::All, false, {} },
{ kOfxParamPropPersistant, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropPluginMayWrite, {PropType::Int}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropScriptName, {PropType::String}, 1, Writable::All, false, {} },
{ kOfxParamPropSecret, {PropType::Bool}, 1, Writable::All, false, {} },
{ kOfxParamPropShowTimeMarker, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropStringFilePathExists, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxParamPropStringMode, {PropType::Enum}, 1, Writable::Plugin, false, {"kOfxParamStringIsSingleLine","kOfxParamStringIsMultiLine","kOfxParamStringIsFilePath","kOfxParamStringIsDirectoryPath","kOfxParamStringIsLabel","kOfxParamStringIsRichTextFormat"} },
{ kOfxParamPropType, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxParamPropUseHostOverlayHandle, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxPluginPropFilePath, {PropType::Enum}, 1, Writable::Plugin, false, {"false","true","needed"} },
{ kOfxPluginPropParamPageOrder, {PropType::String}, 0, Writable::Plugin, false, {} },
{ kOfxPropAPIVersion, {PropType::Int}, 0, Writable::Host, false, {} },
{ kOfxPropChangeReason, {PropType::Enum}, 1, Writable::Host, false, {"kOfxChangeUserEdited","kOfxChangePluginEdited","kOfxChangeTime"} },
{ kOfxPropEffectInstance, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxPropHostOSHandle, {PropType::Pointer}, 1, Writable::Host, false, {} },
{ kOfxPropIcon, {PropType::String}, 2, Writable::Plugin, true, {} },
{ kOfxPropInstanceData, {PropType::Pointer}, 1, Writable::Plugin, false, {} },
{ kOfxPropIsInteractive, {PropType::Bool}, 1, Writable::Host, false, {} },
{ kOfxPropKeyString, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxPropKeySym, {PropType::Int}, 1, Writable::Host, false, {} },
{ kOfxPropLabel, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxPropLongLabel, {PropType::String}, 1, Writable::Plugin, true, {} },
{ kOfxPropName, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxPropParamSetNeedsSyncing, {PropType::Bool}, 1, Writable::Plugin, false, {} },
{ kOfxPropPluginDescription, {PropType::String}, 1, Writable::Plugin, false, {} },
{ kOfxPropShortLabel, {PropType::String}, 1, Writable::Plugin, true, {} },
{ kOfxPropTime, {PropType::Double}, 1, Writable::All, false, {} },
{ kOfxPropType, {PropType::String}, 1, Writable::Host, false, {} },
{ kOfxPropVersion, {PropType::Int}, 0, Writable::Host, false, {} },
{ kOfxPropVersionLabel, {PropType::String}, 1, Writable::Host, false, {} },
};
} // namespace OpenFX