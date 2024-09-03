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

struct PropsMetadata {
  std::string name;
  std::vector<PropType> types;
  int dimension;
  std::vector<const char *> values; // for enums
};

static inline const std::array<struct PropsMetadata, 179> props_metadata { {
{ "OfxImageClipPropColourspace", {PropType::String}, 1, {} },
{ "OfxImageClipPropConnected", {PropType::Bool}, 1, {} },
{ "OfxImageClipPropContinuousSamples", {PropType::Bool}, 1, {} },
{ "OfxImageClipPropFieldExtraction", {PropType::Enum}, 1, {"OfxImageFieldNone","OfxImageFieldLower","OfxImageFieldUpper","OfxImageFieldBoth","OfxImageFieldSingle","OfxImageFieldDoubled"} },
{ "OfxImageClipPropFieldOrder", {PropType::Enum}, 1, {"OfxImageFieldNone","OfxImageFieldLower","OfxImageFieldUpper"} },
{ "OfxImageClipPropIsMask", {PropType::Bool}, 1, {} },
{ "OfxImageClipPropOptional", {PropType::Bool}, 1, {} },
{ "OfxImageClipPropPreferredColourspaces", {PropType::String}, 0, {} },
{ "OfxImageClipPropUnmappedComponents", {PropType::Enum}, 1, {"OfxImageComponentNone","OfxImageComponentRGBA","OfxImageComponentRGB","OfxImageComponentAlpha"} },
{ "OfxImageClipPropUnmappedPixelDepth", {PropType::Enum}, 1, {"OfxBitDepthNone","OfxBitDepthByte","OfxBitDepthShort","OfxBitDepthHalf","OfxBitDepthFloat"} },
{ "OfxImageEffectFrameVarying", {PropType::Bool}, 1, {} },
{ "OfxImageEffectHostPropIsBackground", {PropType::Bool}, 1, {} },
{ "OfxImageEffectHostPropNativeOrigin", {PropType::Enum}, 1, {"OfxImageEffectHostPropNativeOriginBottomLeft","OfxImageEffectHostPropNativeOriginTopLeft","OfxImageEffectHostPropNativeOriginCenter"} },
{ "OfxImageEffectInstancePropEffectDuration", {PropType::Double}, 1, {} },
{ "OfxImageEffectInstancePropSequentialRender", {PropType::Int}, 1, {} },
{ "OfxImageEffectPluginPropFieldRenderTwiceAlways", {PropType::Int}, 1, {} },
{ "OfxImageEffectPluginPropGrouping", {PropType::String}, 1, {} },
{ "OfxImageEffectPluginPropHostFrameThreading", {PropType::Int}, 1, {} },
{ "OfxImageEffectPluginPropOverlayInteractV1", {PropType::Pointer}, 1, {} },
{ "OfxImageEffectPluginPropOverlayInteractV2", {PropType::Pointer}, 1, {} },
{ "OfxImageEffectPluginPropSingleInstance", {PropType::Int}, 1, {} },
{ "OfxImageEffectPluginRenderThreadSafety", {PropType::String}, 1, {} },
{ "OfxImageEffectPropClipPreferencesSlaveParam", {PropType::String}, 0, {} },
{ "OfxImageEffectPropColourManagementAvailableConfigs", {PropType::String}, 0, {} },
{ "OfxImageEffectPropColourManagementConfig", {PropType::String}, 1, {} },
{ "OfxImageEffectPropColourManagementStyle", {PropType::Enum}, 1, {"OfxImageEffectPropColourManagementNone","OfxImageEffectPropColourManagementBasic","OfxImageEffectPropColourManagementCore","OfxImageEffectPropColourManagementFull","OfxImageEffectPropColourManagementOCIO"} },
{ "OfxImageEffectPropComponents", {PropType::Enum}, 1, {"OfxImageComponentNone","OfxImageComponentRGBA","OfxImageComponentRGB","OfxImageComponentAlpha"} },
{ "OfxImageEffectPropContext", {PropType::Enum}, 1, {"OfxImageEffectContextGenerator","OfxImageEffectContextFilter","OfxImageEffectContextTransition","OfxImageEffectContextPaint","OfxImageEffectContextGeneral","OfxImageEffectContextRetimer"} },
{ "OfxImageEffectPropCudaEnabled", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropCudaRenderSupported", {PropType::Enum}, 1, {"false","true","needed"} },
{ "OfxImageEffectPropCudaStream", {PropType::Pointer}, 1, {} },
{ "OfxImageEffectPropCudaStreamSupported", {PropType::Enum}, 1, {"false","true","needed"} },
{ "OfxImageEffectPropDisplayColourspace", {PropType::String}, 1, {} },
{ "OfxImageEffectPropFieldToRender", {PropType::Enum}, 1, {"OfxImageFieldNone","OfxImageFieldBoth","OfxImageFieldLower","OfxImageFieldUpper"} },
{ "OfxImageEffectPropFrameRange", {PropType::Double}, 2, {} },
{ "OfxImageEffectPropFrameRate", {PropType::Double}, 1, {} },
{ "OfxImageEffectPropFrameStep", {PropType::Double}, 1, {} },
{ "OfxImageEffectPropInAnalysis", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropInteractiveRenderStatus", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropMetalCommandQueue", {PropType::Pointer}, 1, {} },
{ "OfxImageEffectPropMetalEnabled", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropMetalRenderSupported", {PropType::Enum}, 1, {"false","true","needed"} },
{ "OfxImageEffectPropMultipleClipDepths", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropOCIOConfig", {PropType::String}, 1, {} },
{ "OfxImageEffectPropOCIODisplay", {PropType::String}, 1, {} },
{ "OfxImageEffectPropOCIOView", {PropType::String}, 1, {} },
{ "OfxImageEffectPropOpenCLCommandQueue", {PropType::Pointer}, 1, {} },
{ "OfxImageEffectPropOpenCLEnabled", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropOpenCLImage", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropOpenCLRenderSupported", {PropType::Enum}, 1, {"false","true","needed"} },
{ "OfxImageEffectPropOpenCLSupported", {PropType::Enum}, 1, {"false","true"} },
{ "OfxImageEffectPropOpenGLEnabled", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropOpenGLRenderSupported", {PropType::Enum}, 1, {"false","true","needed"} },
{ "OfxImageEffectPropOpenGLTextureIndex", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropOpenGLTextureTarget", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropPixelAspectRatio", {PropType::Double}, 1, {} },
{ "OfxImageEffectPropPixelDepth", {PropType::Enum}, 1, {"OfxBitDepthNone","OfxBitDepthByte","OfxBitDepthShort","OfxBitDepthHalf","OfxBitDepthFloat"} },
{ "OfxImageEffectPropPluginHandle", {PropType::Pointer}, 1, {} },
{ "OfxImageEffectPropPreMultiplication", {PropType::Enum}, 1, {"OfxImageOpaque","OfxImagePreMultiplied","OfxImageUnPreMultiplied"} },
{ "OfxImageEffectPropProjectExtent", {PropType::Double}, 2, {} },
{ "OfxImageEffectPropProjectOffset", {PropType::Double}, 2, {} },
{ "OfxImageEffectPropProjectSize", {PropType::Double}, 2, {} },
{ "OfxImageEffectPropRegionOfDefinition", {PropType::Int}, 4, {} },
{ "OfxImageEffectPropRegionOfInterest", {PropType::Int}, 4, {} },
{ "OfxImageEffectPropRenderQualityDraft", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropRenderScale", {PropType::Double}, 2, {} },
{ "OfxImageEffectPropRenderWindow", {PropType::Int}, 4, {} },
{ "OfxImageEffectPropSequentialRenderStatus", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropSetableFielding", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropSetableFrameRate", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropSupportedComponents", {PropType::Enum}, 0, {"OfxImageComponentNone","OfxImageComponentRGBA","OfxImageComponentRGB","OfxImageComponentAlpha"} },
{ "OfxImageEffectPropSupportedContexts", {PropType::String}, 0, {} },
{ "OfxImageEffectPropSupportedPixelDepths", {PropType::String}, 0, {} },
{ "OfxImageEffectPropSupportsMultiResolution", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropSupportsMultipleClipPARs", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropSupportsOverlays", {PropType::Bool}, 1, {} },
{ "OfxImageEffectPropSupportsTiles", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropTemporalClipAccess", {PropType::Int}, 1, {} },
{ "OfxImageEffectPropUnmappedFrameRange", {PropType::Double}, 2, {} },
{ "OfxImageEffectPropUnmappedFrameRate", {PropType::Double}, 1, {} },
{ "OfxImagePropBounds", {PropType::Int}, 4, {} },
{ "OfxImagePropData", {PropType::Pointer}, 1, {} },
{ "OfxImagePropField", {PropType::Enum}, 1, {"OfxImageFieldNone","OfxImageFieldBoth","OfxImageFieldLower","OfxImageFieldUpper"} },
{ "OfxImagePropPixelAspectRatio", {PropType::Double}, 1, {} },
{ "OfxImagePropRegionOfDefinition", {PropType::Int}, 4, {} },
{ "OfxImagePropRowBytes", {PropType::Int}, 1, {} },
{ "OfxImagePropUniqueIdentifier", {PropType::String}, 1, {} },
{ "OfxInteractPropBackgroundColour", {PropType::Double}, 3, {} },
{ "OfxInteractPropBitDepth", {PropType::Int}, 1, {} },
{ "OfxInteractPropDrawContext", {PropType::Pointer}, 1, {} },
{ "OfxInteractPropHasAlpha", {PropType::Bool}, 1, {} },
{ "OfxInteractPropPenPosition", {PropType::Double}, 2, {} },
{ "OfxInteractPropPenPressure", {PropType::Double}, 1, {} },
{ "OfxInteractPropPenViewportPosition", {PropType::Int}, 2, {} },
{ "OfxInteractPropPixelScale", {PropType::Double}, 2, {} },
{ "OfxInteractPropSlaveToParam", {PropType::String}, 0, {} },
{ "OfxInteractPropSuggestedColour", {PropType::Double}, 3, {} },
{ "OfxInteractPropViewport", {PropType::Int}, 2, {} },
{ "OfxOpenGLPropPixelDepth", {PropType::Enum}, 0, {"OfxBitDepthNone","OfxBitDepthByte","OfxBitDepthShort","OfxBitDepthHalf","OfxBitDepthFloat"} },
{ "OfxParamHostPropMaxPages", {PropType::Int}, 1, {} },
{ "OfxParamHostPropMaxParameters", {PropType::Int}, 1, {} },
{ "OfxParamHostPropPageRowColumnCount", {PropType::Int}, 2, {} },
{ "OfxParamHostPropSupportsBooleanAnimation", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsChoiceAnimation", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsCustomAnimation", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsCustomInteract", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsParametricAnimation", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsStrChoice", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsStrChoiceAnimation", {PropType::Bool}, 1, {} },
{ "OfxParamHostPropSupportsStringAnimation", {PropType::Bool}, 1, {} },
{ "OfxParamPropAnimates", {PropType::Bool}, 1, {} },
{ "OfxParamPropCacheInvalidation", {PropType::Enum}, 1, {"OfxParamInvalidateValueChange","OfxParamInvalidateValueChangeToEnd","OfxParamInvalidateAll"} },
{ "OfxParamPropCanUndo", {PropType::Bool}, 1, {} },
{ "OfxParamPropChoiceEnum", {PropType::Bool}, 1, {} },
{ "OfxParamPropChoiceOption", {PropType::String}, 0, {} },
{ "OfxParamPropChoiceOrder", {PropType::Int}, 0, {} },
{ "OfxParamPropCustomCallbackV1", {PropType::Pointer}, 1, {} },
{ "OfxParamPropCustomValue", {PropType::String}, 2, {} },
{ "OfxParamPropDataPtr", {PropType::Pointer}, 1, {} },
{ "OfxParamPropDefault", {PropType::Int,PropType::Double,PropType::String,PropType::Bytes}, 0, {} },
{ "OfxParamPropDefaultCoordinateSystem", {PropType::Enum}, 1, {"OfxParamCoordinatesCanonical","OfxParamCoordinatesNormalised"} },
{ "OfxParamPropDigits", {PropType::Int}, 1, {} },
{ "OfxParamPropDimensionLabel", {PropType::String}, 1, {} },
{ "OfxParamPropDisplayMax", {PropType::Int,PropType::Double}, 0, {} },
{ "OfxParamPropDisplayMin", {PropType::Int,PropType::Double}, 0, {} },
{ "OfxParamPropDoubleType", {PropType::Enum}, 1, {"OfxParamDoubleTypePlain","OfxParamDoubleTypeAngle","OfxParamDoubleTypeScale","OfxParamDoubleTypeTime","OfxParamDoubleTypeAbsoluteTime","OfxParamDoubleTypeX","OfxParamDoubleTypeXAbsolute","OfxParamDoubleTypeY","OfxParamDoubleTypeYAbsolute","OfxParamDoubleTypeXY","OfxParamDoubleTypeXYAbsolute"} },
{ "OfxParamPropEnabled", {PropType::Bool}, 1, {} },
{ "OfxParamPropEvaluateOnChange", {PropType::Bool}, 1, {} },
{ "OfxParamPropGroupOpen", {PropType::Bool}, 1, {} },
{ "OfxParamPropHasHostOverlayHandle", {PropType::Bool}, 1, {} },
{ "OfxParamPropHint", {PropType::String}, 1, {} },
{ "OfxParamPropIncrement", {PropType::Double}, 1, {} },
{ "OfxParamPropInteractMinimumSize", {PropType::Double}, 2, {} },
{ "OfxParamPropInteractPreferedSize", {PropType::Int}, 2, {} },
{ "OfxParamPropInteractSize", {PropType::Double}, 2, {} },
{ "OfxParamPropInteractSizeAspect", {PropType::Double}, 1, {} },
{ "OfxParamPropInteractV1", {PropType::Pointer}, 1, {} },
{ "OfxParamPropInterpolationAmount", {PropType::Double}, 1, {} },
{ "OfxParamPropInterpolationTime", {PropType::Double}, 2, {} },
{ "OfxParamPropIsAnimating", {PropType::Bool}, 1, {} },
{ "OfxParamPropIsAutoKeying", {PropType::Bool}, 1, {} },
{ "OfxParamPropMax", {PropType::Int,PropType::Double}, 0, {} },
{ "OfxParamPropMin", {PropType::Int,PropType::Double}, 0, {} },
{ "OfxParamPropPageChild", {PropType::String}, 0, {} },
{ "OfxParamPropParametricDimension", {PropType::Int}, 1, {} },
{ "OfxParamPropParametricInteractBackground", {PropType::Pointer}, 1, {} },
{ "OfxParamPropParametricRange", {PropType::Double}, 2, {} },
{ "OfxParamPropParametricUIColour", {PropType::Double}, 0, {} },
{ "OfxParamPropParent", {PropType::String}, 1, {} },
{ "OfxParamPropPersistant", {PropType::Int}, 1, {} },
{ "OfxParamPropPluginMayWrite", {PropType::Int}, 1, {} },
{ "OfxParamPropScriptName", {PropType::String}, 1, {} },
{ "OfxParamPropSecret", {PropType::Bool}, 1, {} },
{ "OfxParamPropShowTimeMarker", {PropType::Bool}, 1, {} },
{ "OfxParamPropStringFilePathExists", {PropType::Bool}, 1, {} },
{ "OfxParamPropStringMode", {PropType::Enum}, 1, {"OfxParamStringIsSingleLine","OfxParamStringIsMultiLine","OfxParamStringIsFilePath","OfxParamStringIsDirectoryPath","OfxParamStringIsLabel","OfxParamStringIsRichTextFormat"} },
{ "OfxParamPropType", {PropType::String}, 1, {} },
{ "OfxPluginPropFilePath", {PropType::Enum}, 1, {"false","true","needed"} },
{ "OfxPluginPropParamPageOrder", {PropType::String}, 0, {} },
{ "OfxPropAPIVersion", {PropType::Int}, 0, {} },
{ "OfxPropChangeReason", {PropType::Enum}, 1, {"OfxChangeUserEdited","OfxChangePluginEdited","OfxChangeTime"} },
{ "OfxPropEffectInstance", {PropType::Pointer}, 1, {} },
{ "OfxPropHostOSHandle", {PropType::Pointer}, 1, {} },
{ "OfxPropIcon", {PropType::String}, 2, {} },
{ "OfxPropInstanceData", {PropType::Pointer}, 1, {} },
{ "OfxPropIsInteractive", {PropType::Bool}, 1, {} },
{ "OfxPropLabel", {PropType::String}, 1, {} },
{ "OfxPropLongLabel", {PropType::String}, 1, {} },
{ "OfxPropName", {PropType::String}, 1, {} },
{ "OfxPropParamSetNeedsSyncing", {PropType::Bool}, 1, {} },
{ "OfxPropPluginDescription", {PropType::String}, 1, {} },
{ "OfxPropShortLabel", {PropType::String}, 1, {} },
{ "OfxPropTime", {PropType::Double}, 1, {} },
{ "OfxPropType", {PropType::String}, 1, {} },
{ "OfxPropVersion", {PropType::Int}, 0, {} },
{ "OfxPropVersionLabel", {PropType::String}, 1, {} },
{ "kOfxParamPropUseHostOverlayHandle", {PropType::Bool}, 1, {} },
{ "kOfxPropKeyString", {PropType::String}, 1, {} },
{ "kOfxPropKeySym", {PropType::Int}, 1, {} },
} };

// Static asserts to check #define names vs. strings
static_assert(std::string_view("OfxImageClipPropColourspace") == std::string_view(kOfxImageClipPropColourspace));
static_assert(std::string_view("OfxImageClipPropConnected") == std::string_view(kOfxImageClipPropConnected));
static_assert(std::string_view("OfxImageClipPropContinuousSamples") == std::string_view(kOfxImageClipPropContinuousSamples));
static_assert(std::string_view("OfxImageClipPropFieldExtraction") == std::string_view(kOfxImageClipPropFieldExtraction));
static_assert(std::string_view("OfxImageClipPropFieldOrder") == std::string_view(kOfxImageClipPropFieldOrder));
static_assert(std::string_view("OfxImageClipPropIsMask") == std::string_view(kOfxImageClipPropIsMask));
static_assert(std::string_view("OfxImageClipPropOptional") == std::string_view(kOfxImageClipPropOptional));
static_assert(std::string_view("OfxImageClipPropPreferredColourspaces") == std::string_view(kOfxImageClipPropPreferredColourspaces));
static_assert(std::string_view("OfxImageClipPropUnmappedComponents") == std::string_view(kOfxImageClipPropUnmappedComponents));
static_assert(std::string_view("OfxImageClipPropUnmappedPixelDepth") == std::string_view(kOfxImageClipPropUnmappedPixelDepth));
static_assert(std::string_view("OfxImageEffectFrameVarying") == std::string_view(kOfxImageEffectFrameVarying));
static_assert(std::string_view("OfxImageEffectHostPropIsBackground") == std::string_view(kOfxImageEffectHostPropIsBackground));
static_assert(std::string_view("OfxImageEffectHostPropNativeOrigin") == std::string_view(kOfxImageEffectHostPropNativeOrigin));
static_assert(std::string_view("OfxImageEffectInstancePropEffectDuration") == std::string_view(kOfxImageEffectInstancePropEffectDuration));
static_assert(std::string_view("OfxImageEffectInstancePropSequentialRender") == std::string_view(kOfxImageEffectInstancePropSequentialRender));
static_assert(std::string_view("OfxImageEffectPluginPropFieldRenderTwiceAlways") == std::string_view(kOfxImageEffectPluginPropFieldRenderTwiceAlways));
static_assert(std::string_view("OfxImageEffectPluginPropGrouping") == std::string_view(kOfxImageEffectPluginPropGrouping));
static_assert(std::string_view("OfxImageEffectPluginPropHostFrameThreading") == std::string_view(kOfxImageEffectPluginPropHostFrameThreading));
static_assert(std::string_view("OfxImageEffectPluginPropOverlayInteractV1") == std::string_view(kOfxImageEffectPluginPropOverlayInteractV1));
static_assert(std::string_view("OfxImageEffectPluginPropOverlayInteractV2") == std::string_view(kOfxImageEffectPluginPropOverlayInteractV2));
static_assert(std::string_view("OfxImageEffectPluginPropSingleInstance") == std::string_view(kOfxImageEffectPluginPropSingleInstance));
static_assert(std::string_view("OfxImageEffectPluginRenderThreadSafety") == std::string_view(kOfxImageEffectPluginRenderThreadSafety));
static_assert(std::string_view("OfxImageEffectPropClipPreferencesSlaveParam") == std::string_view(kOfxImageEffectPropClipPreferencesSlaveParam));
static_assert(std::string_view("OfxImageEffectPropColourManagementAvailableConfigs") == std::string_view(kOfxImageEffectPropColourManagementAvailableConfigs));
static_assert(std::string_view("OfxImageEffectPropColourManagementConfig") == std::string_view(kOfxImageEffectPropColourManagementConfig));
static_assert(std::string_view("OfxImageEffectPropColourManagementStyle") == std::string_view(kOfxImageEffectPropColourManagementStyle));
static_assert(std::string_view("OfxImageEffectPropComponents") == std::string_view(kOfxImageEffectPropComponents));
static_assert(std::string_view("OfxImageEffectPropContext") == std::string_view(kOfxImageEffectPropContext));
static_assert(std::string_view("OfxImageEffectPropCudaEnabled") == std::string_view(kOfxImageEffectPropCudaEnabled));
static_assert(std::string_view("OfxImageEffectPropCudaRenderSupported") == std::string_view(kOfxImageEffectPropCudaRenderSupported));
static_assert(std::string_view("OfxImageEffectPropCudaStream") == std::string_view(kOfxImageEffectPropCudaStream));
static_assert(std::string_view("OfxImageEffectPropCudaStreamSupported") == std::string_view(kOfxImageEffectPropCudaStreamSupported));
static_assert(std::string_view("OfxImageEffectPropDisplayColourspace") == std::string_view(kOfxImageEffectPropDisplayColourspace));
static_assert(std::string_view("OfxImageEffectPropFieldToRender") == std::string_view(kOfxImageEffectPropFieldToRender));
static_assert(std::string_view("OfxImageEffectPropFrameRange") == std::string_view(kOfxImageEffectPropFrameRange));
static_assert(std::string_view("OfxImageEffectPropFrameRate") == std::string_view(kOfxImageEffectPropFrameRate));
static_assert(std::string_view("OfxImageEffectPropFrameStep") == std::string_view(kOfxImageEffectPropFrameStep));
static_assert(std::string_view("OfxImageEffectPropInAnalysis") == std::string_view(kOfxImageEffectPropInAnalysis));
static_assert(std::string_view("OfxImageEffectPropInteractiveRenderStatus") == std::string_view(kOfxImageEffectPropInteractiveRenderStatus));
static_assert(std::string_view("OfxImageEffectPropMetalCommandQueue") == std::string_view(kOfxImageEffectPropMetalCommandQueue));
static_assert(std::string_view("OfxImageEffectPropMetalEnabled") == std::string_view(kOfxImageEffectPropMetalEnabled));
static_assert(std::string_view("OfxImageEffectPropMetalRenderSupported") == std::string_view(kOfxImageEffectPropMetalRenderSupported));
static_assert(std::string_view("OfxImageEffectPropMultipleClipDepths") == std::string_view(kOfxImageEffectPropSupportsMultipleClipDepths));
static_assert(std::string_view("OfxImageEffectPropOCIOConfig") == std::string_view(kOfxImageEffectPropOCIOConfig));
static_assert(std::string_view("OfxImageEffectPropOCIODisplay") == std::string_view(kOfxImageEffectPropOCIODisplay));
static_assert(std::string_view("OfxImageEffectPropOCIOView") == std::string_view(kOfxImageEffectPropOCIOView));
static_assert(std::string_view("OfxImageEffectPropOpenCLCommandQueue") == std::string_view(kOfxImageEffectPropOpenCLCommandQueue));
static_assert(std::string_view("OfxImageEffectPropOpenCLEnabled") == std::string_view(kOfxImageEffectPropOpenCLEnabled));
static_assert(std::string_view("OfxImageEffectPropOpenCLImage") == std::string_view(kOfxImageEffectPropOpenCLImage));
static_assert(std::string_view("OfxImageEffectPropOpenCLRenderSupported") == std::string_view(kOfxImageEffectPropOpenCLRenderSupported));
static_assert(std::string_view("OfxImageEffectPropOpenCLSupported") == std::string_view(kOfxImageEffectPropOpenCLSupported));
static_assert(std::string_view("OfxImageEffectPropOpenGLEnabled") == std::string_view(kOfxImageEffectPropOpenGLEnabled));
static_assert(std::string_view("OfxImageEffectPropOpenGLRenderSupported") == std::string_view(kOfxImageEffectPropOpenGLRenderSupported));
static_assert(std::string_view("OfxImageEffectPropOpenGLTextureIndex") == std::string_view(kOfxImageEffectPropOpenGLTextureIndex));
static_assert(std::string_view("OfxImageEffectPropOpenGLTextureTarget") == std::string_view(kOfxImageEffectPropOpenGLTextureTarget));
static_assert(std::string_view("OfxImageEffectPropPixelAspectRatio") == std::string_view(kOfxImageEffectPropProjectPixelAspectRatio));
static_assert(std::string_view("OfxImageEffectPropPixelDepth") == std::string_view(kOfxImageEffectPropPixelDepth));
static_assert(std::string_view("OfxImageEffectPropPluginHandle") == std::string_view(kOfxImageEffectPropPluginHandle));
static_assert(std::string_view("OfxImageEffectPropPreMultiplication") == std::string_view(kOfxImageEffectPropPreMultiplication));
static_assert(std::string_view("OfxImageEffectPropProjectExtent") == std::string_view(kOfxImageEffectPropProjectExtent));
static_assert(std::string_view("OfxImageEffectPropProjectOffset") == std::string_view(kOfxImageEffectPropProjectOffset));
static_assert(std::string_view("OfxImageEffectPropProjectSize") == std::string_view(kOfxImageEffectPropProjectSize));
static_assert(std::string_view("OfxImageEffectPropRegionOfDefinition") == std::string_view(kOfxImageEffectPropRegionOfDefinition));
static_assert(std::string_view("OfxImageEffectPropRegionOfInterest") == std::string_view(kOfxImageEffectPropRegionOfInterest));
static_assert(std::string_view("OfxImageEffectPropRenderQualityDraft") == std::string_view(kOfxImageEffectPropRenderQualityDraft));
static_assert(std::string_view("OfxImageEffectPropRenderScale") == std::string_view(kOfxImageEffectPropRenderScale));
static_assert(std::string_view("OfxImageEffectPropRenderWindow") == std::string_view(kOfxImageEffectPropRenderWindow));
static_assert(std::string_view("OfxImageEffectPropSequentialRenderStatus") == std::string_view(kOfxImageEffectPropSequentialRenderStatus));
static_assert(std::string_view("OfxImageEffectPropSetableFielding") == std::string_view(kOfxImageEffectPropSetableFielding));
static_assert(std::string_view("OfxImageEffectPropSetableFrameRate") == std::string_view(kOfxImageEffectPropSetableFrameRate));
static_assert(std::string_view("OfxImageEffectPropSupportedComponents") == std::string_view(kOfxImageEffectPropSupportedComponents));
static_assert(std::string_view("OfxImageEffectPropSupportedContexts") == std::string_view(kOfxImageEffectPropSupportedContexts));
static_assert(std::string_view("OfxImageEffectPropSupportedPixelDepths") == std::string_view(kOfxImageEffectPropSupportedPixelDepths));
static_assert(std::string_view("OfxImageEffectPropSupportsMultiResolution") == std::string_view(kOfxImageEffectPropSupportsMultiResolution));
static_assert(std::string_view("OfxImageEffectPropSupportsMultipleClipPARs") == std::string_view(kOfxImageEffectPropSupportsMultipleClipPARs));
static_assert(std::string_view("OfxImageEffectPropSupportsOverlays") == std::string_view(kOfxImageEffectPropSupportsOverlays));
static_assert(std::string_view("OfxImageEffectPropSupportsTiles") == std::string_view(kOfxImageEffectPropSupportsTiles));
static_assert(std::string_view("OfxImageEffectPropTemporalClipAccess") == std::string_view(kOfxImageEffectPropTemporalClipAccess));
static_assert(std::string_view("OfxImageEffectPropUnmappedFrameRange") == std::string_view(kOfxImageEffectPropUnmappedFrameRange));
static_assert(std::string_view("OfxImageEffectPropUnmappedFrameRate") == std::string_view(kOfxImageEffectPropUnmappedFrameRate));
static_assert(std::string_view("OfxImagePropBounds") == std::string_view(kOfxImagePropBounds));
static_assert(std::string_view("OfxImagePropData") == std::string_view(kOfxImagePropData));
static_assert(std::string_view("OfxImagePropField") == std::string_view(kOfxImagePropField));
static_assert(std::string_view("OfxImagePropPixelAspectRatio") == std::string_view(kOfxImagePropPixelAspectRatio));
static_assert(std::string_view("OfxImagePropRegionOfDefinition") == std::string_view(kOfxImagePropRegionOfDefinition));
static_assert(std::string_view("OfxImagePropRowBytes") == std::string_view(kOfxImagePropRowBytes));
static_assert(std::string_view("OfxImagePropUniqueIdentifier") == std::string_view(kOfxImagePropUniqueIdentifier));
static_assert(std::string_view("OfxInteractPropBackgroundColour") == std::string_view(kOfxInteractPropBackgroundColour));
static_assert(std::string_view("OfxInteractPropBitDepth") == std::string_view(kOfxInteractPropBitDepth));
static_assert(std::string_view("OfxInteractPropDrawContext") == std::string_view(kOfxInteractPropDrawContext));
static_assert(std::string_view("OfxInteractPropHasAlpha") == std::string_view(kOfxInteractPropHasAlpha));
static_assert(std::string_view("OfxInteractPropPenPosition") == std::string_view(kOfxInteractPropPenPosition));
static_assert(std::string_view("OfxInteractPropPenPressure") == std::string_view(kOfxInteractPropPenPressure));
static_assert(std::string_view("OfxInteractPropPenViewportPosition") == std::string_view(kOfxInteractPropPenViewportPosition));
static_assert(std::string_view("OfxInteractPropPixelScale") == std::string_view(kOfxInteractPropPixelScale));
static_assert(std::string_view("OfxInteractPropSlaveToParam") == std::string_view(kOfxInteractPropSlaveToParam));
static_assert(std::string_view("OfxInteractPropSuggestedColour") == std::string_view(kOfxInteractPropSuggestedColour));
static_assert(std::string_view("OfxInteractPropViewport") == std::string_view(kOfxInteractPropViewportSize));
static_assert(std::string_view("OfxOpenGLPropPixelDepth") == std::string_view(kOfxOpenGLPropPixelDepth));
static_assert(std::string_view("OfxParamHostPropMaxPages") == std::string_view(kOfxParamHostPropMaxPages));
static_assert(std::string_view("OfxParamHostPropMaxParameters") == std::string_view(kOfxParamHostPropMaxParameters));
static_assert(std::string_view("OfxParamHostPropPageRowColumnCount") == std::string_view(kOfxParamHostPropPageRowColumnCount));
static_assert(std::string_view("OfxParamHostPropSupportsBooleanAnimation") == std::string_view(kOfxParamHostPropSupportsBooleanAnimation));
static_assert(std::string_view("OfxParamHostPropSupportsChoiceAnimation") == std::string_view(kOfxParamHostPropSupportsChoiceAnimation));
static_assert(std::string_view("OfxParamHostPropSupportsCustomAnimation") == std::string_view(kOfxParamHostPropSupportsCustomAnimation));
static_assert(std::string_view("OfxParamHostPropSupportsCustomInteract") == std::string_view(kOfxParamHostPropSupportsCustomInteract));
static_assert(std::string_view("OfxParamHostPropSupportsParametricAnimation") == std::string_view(kOfxParamHostPropSupportsParametricAnimation));
static_assert(std::string_view("OfxParamHostPropSupportsStrChoice") == std::string_view(kOfxParamHostPropSupportsStrChoice));
static_assert(std::string_view("OfxParamHostPropSupportsStrChoiceAnimation") == std::string_view(kOfxParamHostPropSupportsStrChoiceAnimation));
static_assert(std::string_view("OfxParamHostPropSupportsStringAnimation") == std::string_view(kOfxParamHostPropSupportsStringAnimation));
static_assert(std::string_view("OfxParamPropAnimates") == std::string_view(kOfxParamPropAnimates));
static_assert(std::string_view("OfxParamPropCacheInvalidation") == std::string_view(kOfxParamPropCacheInvalidation));
static_assert(std::string_view("OfxParamPropCanUndo") == std::string_view(kOfxParamPropCanUndo));
static_assert(std::string_view("OfxParamPropChoiceEnum") == std::string_view(kOfxParamPropChoiceEnum));
static_assert(std::string_view("OfxParamPropChoiceOption") == std::string_view(kOfxParamPropChoiceOption));
static_assert(std::string_view("OfxParamPropChoiceOrder") == std::string_view(kOfxParamPropChoiceOrder));
static_assert(std::string_view("OfxParamPropCustomCallbackV1") == std::string_view(kOfxParamPropCustomInterpCallbackV1));
static_assert(std::string_view("OfxParamPropCustomValue") == std::string_view(kOfxParamPropCustomValue));
static_assert(std::string_view("OfxParamPropDataPtr") == std::string_view(kOfxParamPropDataPtr));
static_assert(std::string_view("OfxParamPropDefault") == std::string_view(kOfxParamPropDefault));
static_assert(std::string_view("OfxParamPropDefaultCoordinateSystem") == std::string_view(kOfxParamPropDefaultCoordinateSystem));
static_assert(std::string_view("OfxParamPropDigits") == std::string_view(kOfxParamPropDigits));
static_assert(std::string_view("OfxParamPropDimensionLabel") == std::string_view(kOfxParamPropDimensionLabel));
static_assert(std::string_view("OfxParamPropDisplayMax") == std::string_view(kOfxParamPropDisplayMax));
static_assert(std::string_view("OfxParamPropDisplayMin") == std::string_view(kOfxParamPropDisplayMin));
static_assert(std::string_view("OfxParamPropDoubleType") == std::string_view(kOfxParamPropDoubleType));
static_assert(std::string_view("OfxParamPropEnabled") == std::string_view(kOfxParamPropEnabled));
static_assert(std::string_view("OfxParamPropEvaluateOnChange") == std::string_view(kOfxParamPropEvaluateOnChange));
static_assert(std::string_view("OfxParamPropGroupOpen") == std::string_view(kOfxParamPropGroupOpen));
static_assert(std::string_view("OfxParamPropHasHostOverlayHandle") == std::string_view(kOfxParamPropHasHostOverlayHandle));
static_assert(std::string_view("OfxParamPropHint") == std::string_view(kOfxParamPropHint));
static_assert(std::string_view("OfxParamPropIncrement") == std::string_view(kOfxParamPropIncrement));
static_assert(std::string_view("OfxParamPropInteractMinimumSize") == std::string_view(kOfxParamPropInteractMinimumSize));
static_assert(std::string_view("OfxParamPropInteractPreferedSize") == std::string_view(kOfxParamPropInteractPreferedSize));
static_assert(std::string_view("OfxParamPropInteractSize") == std::string_view(kOfxParamPropInteractSize));
static_assert(std::string_view("OfxParamPropInteractSizeAspect") == std::string_view(kOfxParamPropInteractSizeAspect));
static_assert(std::string_view("OfxParamPropInteractV1") == std::string_view(kOfxParamPropInteractV1));
static_assert(std::string_view("OfxParamPropInterpolationAmount") == std::string_view(kOfxParamPropInterpolationAmount));
static_assert(std::string_view("OfxParamPropInterpolationTime") == std::string_view(kOfxParamPropInterpolationTime));
static_assert(std::string_view("OfxParamPropIsAnimating") == std::string_view(kOfxParamPropIsAnimating));
static_assert(std::string_view("OfxParamPropIsAutoKeying") == std::string_view(kOfxParamPropIsAutoKeying));
static_assert(std::string_view("OfxParamPropMax") == std::string_view(kOfxParamPropMax));
static_assert(std::string_view("OfxParamPropMin") == std::string_view(kOfxParamPropMin));
static_assert(std::string_view("OfxParamPropPageChild") == std::string_view(kOfxParamPropPageChild));
static_assert(std::string_view("OfxParamPropParametricDimension") == std::string_view(kOfxParamPropParametricDimension));
static_assert(std::string_view("OfxParamPropParametricInteractBackground") == std::string_view(kOfxParamPropParametricInteractBackground));
static_assert(std::string_view("OfxParamPropParametricRange") == std::string_view(kOfxParamPropParametricRange));
static_assert(std::string_view("OfxParamPropParametricUIColour") == std::string_view(kOfxParamPropParametricUIColour));
static_assert(std::string_view("OfxParamPropParent") == std::string_view(kOfxParamPropParent));
static_assert(std::string_view("OfxParamPropPersistant") == std::string_view(kOfxParamPropPersistant));
static_assert(std::string_view("OfxParamPropPluginMayWrite") == std::string_view(kOfxParamPropPluginMayWrite));
static_assert(std::string_view("OfxParamPropScriptName") == std::string_view(kOfxParamPropScriptName));
static_assert(std::string_view("OfxParamPropSecret") == std::string_view(kOfxParamPropSecret));
static_assert(std::string_view("OfxParamPropShowTimeMarker") == std::string_view(kOfxParamPropShowTimeMarker));
static_assert(std::string_view("OfxParamPropStringFilePathExists") == std::string_view(kOfxParamPropStringFilePathExists));
static_assert(std::string_view("OfxParamPropStringMode") == std::string_view(kOfxParamPropStringMode));
static_assert(std::string_view("OfxParamPropType") == std::string_view(kOfxParamPropType));
static_assert(std::string_view("OfxPluginPropFilePath") == std::string_view(kOfxPluginPropFilePath));
static_assert(std::string_view("OfxPluginPropParamPageOrder") == std::string_view(kOfxPluginPropParamPageOrder));
static_assert(std::string_view("OfxPropAPIVersion") == std::string_view(kOfxPropAPIVersion));
static_assert(std::string_view("OfxPropChangeReason") == std::string_view(kOfxPropChangeReason));
static_assert(std::string_view("OfxPropEffectInstance") == std::string_view(kOfxPropEffectInstance));
static_assert(std::string_view("OfxPropHostOSHandle") == std::string_view(kOfxPropHostOSHandle));
static_assert(std::string_view("OfxPropIcon") == std::string_view(kOfxPropIcon));
static_assert(std::string_view("OfxPropInstanceData") == std::string_view(kOfxPropInstanceData));
static_assert(std::string_view("OfxPropIsInteractive") == std::string_view(kOfxPropIsInteractive));
static_assert(std::string_view("OfxPropLabel") == std::string_view(kOfxPropLabel));
static_assert(std::string_view("OfxPropLongLabel") == std::string_view(kOfxPropLongLabel));
static_assert(std::string_view("OfxPropName") == std::string_view(kOfxPropName));
static_assert(std::string_view("OfxPropParamSetNeedsSyncing") == std::string_view(kOfxPropParamSetNeedsSyncing));
static_assert(std::string_view("OfxPropPluginDescription") == std::string_view(kOfxPropPluginDescription));
static_assert(std::string_view("OfxPropShortLabel") == std::string_view(kOfxPropShortLabel));
static_assert(std::string_view("OfxPropTime") == std::string_view(kOfxPropTime));
static_assert(std::string_view("OfxPropType") == std::string_view(kOfxPropType));
static_assert(std::string_view("OfxPropVersion") == std::string_view(kOfxPropVersion));
static_assert(std::string_view("OfxPropVersionLabel") == std::string_view(kOfxPropVersionLabel));
static_assert(std::string_view("kOfxParamPropUseHostOverlayHandle") == std::string_view(kOfxParamPropUseHostOverlayHandle));
static_assert(std::string_view("kOfxPropKeyString") == std::string_view(kOfxPropKeyString));
static_assert(std::string_view("kOfxPropKeySym") == std::string_view(kOfxPropKeySym));
} // namespace OpenFX
