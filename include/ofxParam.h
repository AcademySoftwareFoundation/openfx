#ifndef _ofxParam_h_
#define _ofxParam_h_

/*
Software License :

Copyright (c) 2003-2004, The Foundry Visionmongers Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Foundry Visionmongers Ltd, nor the names of its 
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
*/

#include "ofxCore.h"
#include "ofxProperty.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief string value to the ::kOfxPropType property for all parameters */
#define kOfxParameterSuite "OfxParameterSuite"

/** @brief string value on the ::kOfxPropType property for all parameter definitions (ie: the handle returned in describe) */
#define kOfxTypeParameter "OfxTypeParameter"

/** @brief string value on the ::kOfxPropType property for all parameter instances */
#define kOfxTypeParameterInstance "OfxTypeParameterInstance"

/** @brief Blind declaration of an OFX param 
    
*/
typedef struct OfxParamStruct *OfxParamHandle;

/** @brief Blind declaration of an OFX parameter set

 */
typedef struct OfxParamSetStruct *OfxParamSetHandle;


/**
   \defgroup ParamTypeDefines Parameter Type definitions 

These strings are used to identify the type of the parameter when it is defined, they are also on the ::kOfxParamPropType in any parameter instance.
*/
/*@{*/

/** @brief String to identify a param as a single valued integer */
#define kOfxParamTypeInteger "OfxParamTypeInteger"
/** @brief String to identify a param as a Single valued floating point parameter  */
#define kOfxParamTypeDouble "OfxParamTypeDouble"
/** @brief String to identify a param as a Single valued boolean parameter */
#define kOfxParamTypeBoolean "OfxParamTypeBoolean"
/** @brief String to identify a param as a Single valued, 'one-of-many' parameter */
#define kOfxParamTypeChoice "OfxParamTypeChoice"
/** @brief String to identify a param as a Red, Green, Blue and Alpha colour parameter */
#define kOfxParamTypeRGBA "OfxParamTypeRGBA"
/** @brief String to identify a param as a Red, Green and Blue colour parameter */
#define kOfxParamTypeRGB "OfxParamTypeRGB"
/** @brief String to identify a param as a Two dimensional floating point parameter */
#define kOfxParamTypeDouble2D "OfxParamTypeDouble2D"
/** @brief String to identify a param as a Two dimensional integer point parameter */
#define kOfxParamTypeInteger2D "OfxParamTypeInteger2D"
/** @brief String to identify a param as a Three dimensional floating point parameter */
#define kOfxParamTypeDouble3D "OfxParamTypeDouble3D"
/** @brief String to identify a param as a Three dimensional integer parameter */
#define kOfxParamTypeInteger3D "OfxParamTypeInteger3D"
/** @brief String to identify a param as a String (UTF8) parameter */
#define kOfxParamTypeString "OfxParamTypeString"
/** @brief String to identify a param as a Plug-in defined parameter */
#define kOfxParamTypeCustom "OfxParamTypeCustom"
/** @brief String to identify a param as a Grouping parameter */
#define kOfxParamTypeGroup "OfxParamTypeGroup"
/** @brief String to identify a param as a page parameter */
#define kOfxParamTypePage "OfxParamTypePage"
/** @brief String to identify a param as a PushButton parameter */
#define kOfxParamTypePushButton "OfxParamTypePushButton"
/*@}*/

/**
   \addtogroup PropertiesAll
*/
/*@{*/
/**
   \defgroup ParamPropDefines Parameter Property Definitions 

These are the list of properties used by the parameters suite.
*/
/*@{*/

/** @brief Host Property, indicates if the host supports animation of custom parameters 

    - int X 1
    - 0 or 1
*/
#define kOfxParamHostPropSupportsCustomAnimation "OfxParamHostPropSupportsCustomAnimation"

/** @brief Host Property, indicates if the host supports animation of string params 

    - int X 1
    - 0 or 1
*/
#define kOfxParamHostPropSupportsStringAnimation "OfxParamHostPropSupportsStringAnimation"

/** @brief Host Property, indicates if the host supports animation of boolean params

    - int X 1
    - 0 or 1
*/
#define kOfxParamHostPropSupportsBooleanAnimation "OfxParamHostPropSupportsBooleanAnimation"

/** @brief Host Property, indicates if the host supports animation of choice params 

    - int X 1
    - 0 or 1
*/
#define kOfxParamHostPropSupportsChoiceAnimation "OfxParamHostPropSupportsChoiceAnimation"

/** @brief Host Property, indicates if the host supports custom interacts for parameters

    - int X 1
    - 0 or 1
*/
#define kOfxParamHostPropSupportsCustomInteract "OfxParamHostPropSupportsCustomInteract"

/** @brief Host Property, indicates the maximum numbers of parameters available on the host. -1 implies unlimitted


    - int X 1
*/
#define kOfxParamHostPropMaxParameters "OfxParamHostPropMaxParameters"

/** @brief Host Property, if the host has a paged layout, this is the maximum the number of pages on a host


    - int X 1

    If there is not limit to the number of pages on a host, set this to -1.
    
    Non paged hosts should set this to 0.
*/
#define kOfxParamHostPropMaxPages "OfxParamHostPropMaxPages"

/** @brief Host Property, if the host has a paged layout, the number of parameter rows and coloumns

    - int X 2
    - dimension 1 is the number of columns per page
    - dimension 2 is the number of rows per page
*/
#define kOfxParamHostPropPageRowColumnCount "OfxParamHostPropPageRowColumnCount"

/** @brief Pseudo parameter name used to skip a row in a page layout when passed as a value to the \ref kOfxParamPropPageChild property

See \ref ParametersInterfacesPagedLayouts for more details.
*/
#define kOfxParamPageSkipRow "OfxParamPageSkipRow"

/** @brief Pseudo parameter name used to skip a row in a page layout when passed as a value to the \ref kOfxParamPropPageChild property

See \ref ParametersInterfacesPagedLayouts for more details.
*/
#define kOfxParamPageSkipColumn "OfxParamPageSkipColumn"

/** @brief Param property, sets the interact structure to use as a user interface for the parameter

    - pointer X 1
    - must point to a OfxInteractClientApiV1
    - defaults to NULL for a plugin

  If set, the parameters normal interface is replaced completely by the interact gui.  
*/
#define kOfxParamPropInteractV1 "OfxParamPropInteractV1"

/** @brief Param interact property, the size of a custom interact in screen pixels
  
    - double x 2

  Plugin read only, set by host. If no custom interact is attached, then this is 0,0
*/
#define kOfxParamPropInteractSize "OfxParamPropInteractSizeAspect"

/** @brief Param interact property, sets the aspect (x size/y size) of the custom interact

    - double x 1
    - defaults to 1.0

  Plugin set.

  If set to anything other than 0.0, the interact for this parameter will be of a size that is of this aspect ratio.
*/
#define kOfxParamPropInteractSizeAspect "OfxParamPropInteractSizeAspect"

/** @brief Param interact property, sets the minimum size of the interact in screen pixels

    - int x 2
    - defaults to 10 10

  Plugin set.

  Any interact on a parameter will not be less than this size.
*/
#define kOfxParamPropInteractMinimumSize "OfxParamPropInteractMinimumSize"

/** @brief Param interact property, sets the prefered size of the interact in screen pixels

    - int x 2
    - defaults to 10 10

  Plugin set.

  An interact on a parameter will be this size if possible, otherwise it will be of ::kOfxParamPropInteractSizeAspect aspect but larger than ::kOfxParamPropInteractMinimumSize.
*/
#define kOfxParamPropInteractPreferedSize "OfxParamPropInteractPreferedSize"

/** @brief Parameter property, identifies the type of a parameter

    - char * X 1
*/
#define kOfxParamPropType "OfxParamPropType"

/** @brief Parameter property, flags whether the parameter animates,

    - int X 1
    - 0 or 1
    - defaults to true
*/
#define kOfxParamPropAnimates "OfxParamPropAnimates"

/** @brief Parameter property, sets whether parameter changes should be put on the undo/redo stack

    - int X 1
    - 0 or 1
    - defaults to true
    Plugin set.
*/
#define kOfxParamPropCanUndo "OfxParamPropCanUndo"

/** @brief Parameter property, returns whether the parameter contains animation

    - int X 1
    - 0 or 1
    Plugin read only.
*/
#define kOfxParamPropIsAnimating "OfxParamPropIsAnimating"

/** @brief Parameter property, flags whether the plugin will attempt to set the value of a parameter in some callback or analysis pass

    - int X 1
    - 0 or 1
    - default 0
*/
#define kOfxParamPropPluginMayWrite "OfxParamPropPluginMayWrite"

/** @brief Parameter property, flags whether the parameter persists between instances,

    - int X 1
    - 0 or 1
    - defaults to true
*/
#define kOfxParamPropPersistant "OfxParamPropPersistant"

/** @brief Parameter property, flags whether changing the parameter's value forces an evalution (ie: render),

    - int X 1
    - 0 or 1
    - defaults to true
This is used to indicate if the value of a parameter has any affect on an effect's output, eg: the parameter
may be purely for GUI purposes, and so changing it's value should not trigger a re-render.
*/
#define kOfxParamPropEvaluateOnChange "OfxParamPropEvaluateOnChange"

/** @brief Parameter property, flags whether the parameter is exposed to the user,

    - int X 1
    - 0 or 1
    - defaults to false
*/
#define kOfxParamPropSecret "OfxParamPropSecret"

/** @brief Parameter property, string used if the host supports scripting,

    - char * X 1
    - ASCII string
    - defaults to the name the param was created with
*/
#define kOfxParamPropScriptName "OfxParamPropScriptName"

/** @brief Parameter property, how modifying the value of a param will affect any output of an effect over time

    - char * X 1
    - ASCII
    - must be one of
       - ::kOfxParamInvalidateValueChange
       - ::kOfxParamInvalidateValueChangeToEnd
       - ::kOfxParamInvalidateAll
    - defaults to ::kOfxParamInvalidateValueChange

Imagine an effect with an animating parameter in a host that caches
renderred output. Think of the effect of adding a new key frame.

If the parameter represents something like an absolute position, the cache
will only need to be invalidated for the range of frames that keyframe
affects.

If the parameter represents something like a speed which is integrated,
the cache will be invalidated from the keyframe until the end of the clip.

There are potentially other situations where the entire cache will need to
be invalidated (though I can't think of one off the top of my head)

*/
#define kOfxParamPropCacheInvalidation "OfxParamPropCacheInvalidation"

 /** @brief Used as a value for the ::kOfxParamPropCacheInvalidation property */
#define kOfxParamInvalidateValueChange "OfxParamInvalidateValueChange"

 /** @brief Used as a value for the ::kOfxParamPropCacheInvalidation property */
#define kOfxParamInvalidateValueChangeToEnd "OfxParamInvalidateValueChangeToEnd"

 /** @brief Used as a value for the ::kOfxParamPropCacheInvalidation property */
#define kOfxParamInvalidateAll "OfxParamInvalidateAll"

/** @brief Parameter property, a hint to the user as to how the parameter is to be used

    - char * X 1
    - UTF8 string
    - defaults to ""
*/
#define kOfxParamPropHint "OfxParamPropHint"

/** @brief Parameter property, the default value of the parameter,

    - type is parameter type dependant, see @ref ParameterPropertiesVariantTypes
    - defaults to 0 cast to the relevant type (or "" for strings and custom parameters)
 */
#define kOfxParamPropDefault "OfxParamPropDefault"

/** @brief Describes how the double parameter should be interpretted by a host. Applies to 1, 2 and 3 dimensional double parameters. See \ref ParameterPropertiesDoubleTypes.

    - string X 1
    - defaults to ::kOfxParamDoubleTypePlain
    - must be one of...
      - ::kOfxParamDoubleTypePlain - parameter has no special interpretation,
      - ::kOfxParamDoubleTypeAngle - parameter is to be interpretted as an angle,
      - ::kOfxParamDoubleTypeScale - parameter is to be interpretted as a scale factor,
      - ::kOfxParamDoubleTypeTime  - parameter represents a time value (1D only),
      - ::kOfxParamDoubleTypeAbsoluteTime  - parameter represents an absolute time value (1D only),
      - ::kOfxParamDoubleTypeNormalisedX - normalised size wrt to the project's X dimension (1D only),
      - ::kOfxParamDoubleTypeNormalisedXAbsolute - normalised absolute position on the X axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedY - normalised size wrt to the project's Y dimension(1D only),
      - ::kOfxParamDoubleTypeNormalisedYAbsolute - normalised absolute position on the Y axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedXY - normalised to the project's X and Y size (2D only),
      - ::kOfxParamDoubleTypeNormalisedXYAbsolute - normalised to the projects X and Y size, and is an absolute position on the image plane.
*/
#define kOfxParamPropDoubleType "OfxParamPropDoubleType"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating the parameter has no special interpretation and should be interpretted as a raw numeric value. */
#define kOfxParamDoubleTypePlain "OfxParamDoubleTypePlain"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating the parameter is to be interpreted as a scale factor. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeScale "OfxParamDoubleTypeScale"

/** @brief value for the ::kOfxParamDoubleTypeAngle property, indicating the parameter is to be interpreted as an angle. See \ref ParameterPropertiesDoubleTypes.  */
#define kOfxParamDoubleTypeAngle "OfxParamDoubleTypeAngle"

/** @brief value for the ::kOfxParamDoubleTypeAngle property, indicating the parameter is to be interpreted as a time. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeTime "OfxParamDoubleTypeTime"

/** @brief value for the ::kOfxParamDoubleTypeAngle property, indicating the parameter is to be interpreted as an absolute time from the start of the effect. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeAbsoluteTime "OfxParamDoubleTypeAbsoluteTime"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating a size normalised to the X dimension. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeNormalisedX  "OfxParamDoubleTypeNormalisedX"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating a size normalised to the Y dimension. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeNormalisedY  "OfxParamDoubleTypeNormalisedY"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating an absolute position normalised to the X dimension. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeNormalisedXAbsolute  "OfxParamDoubleTypeNormalisedXAbsolute"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating an absolute position  normalised to the Y dimension. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeNormalisedYAbsolute  "OfxParamDoubleTypeNormalisedYAbsolute"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating normalisation to the X and Y dimension for 2D params. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeNormalisedXY  "OfxParamDoubleTypeNormalisedXY"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating normalisation to the X and Y dimension for a 2D param that can be interpretted as an absolute spatial position. See \ref ParameterPropertiesDoubleTypes. */
#define kOfxParamDoubleTypeNormalisedXYAbsolute  "OfxParamDoubleTypeNormalisedXYAbsolute"

/** @brief Parameter property for doubles with kOfxParamPropDoubleType set to ::kOfxParamDoubleTypeAbsoluteTime. Enables the display of a time marker on the host's time line to indicate the value of the absolute time param.

    - int X 1
    - 0 or 1
    - defaults to false 

This is optionally supported on hosts.
*/
#define kOfxParamPropShowTimeMarker "OfxParamPropShowTimeMarker"


/** @brief Instance property, sets the pages and order of pages in this context

    - char * X N
    - ASCII name of a pre-existing page parameter
    - defaults to none
*/
#define kOfxPluginPropParamPageOrder "OfxPluginPropParamPageOrder"

/** @brief Parameter property, page params only. The names of the parameters included on this page

    - char * X N
    - ASCII name of a pre-existing parameter (not a group or page param)
    - defaults to none
*/
#define kOfxParamPropPageChild "OfxParamPropPageChild"

/** @brief Parameter property, the name of the hierarchical group param to be considered a parent of this parameter

    - char * X 1
    - ASCII name of an already defined group parameter
    - defaults to "", which is the 'root' of the hierarchy
*/
#define kOfxParamPropParent "OfxParamPropParent"

/** @brief Parameter property, flags whether the parameter should be enabled in any gui it is displayed on.

    - int X 1
    - 0 or 1
    - defaults to true 

An disabled parameter will only be disabled in a user interace, the plugin can set values in it.

*/
#define kOfxParamPropEnabled "OfxParamPropEnabled"

/** @brief Parameter instance property, a private data pointer that the plug-in can store away for the duration of this instance,

    - void * X 1
    - defaults to NULL
 */
#define kOfxParamPropDataPtr "OfxParamPropDataPtr"

/** @brief Parameter property, sets a choice label in an option parameter

    - UTF8 string X N
    - defaults to ""
  see @ref ParametersChoice for more details
*/
#define kOfxParamPropChoiceOption "OfxParamPropChoiceOption"


/** @brief Numeric parameter property, minimum value the parameter can have

    - type is parameter type dependant, see @ref ParameterPropertiesVariantTypes
    - default to the smallest possible value
    of that type (eg: INT_MIN for an integer, -DBL_MAX for a double parameter)

    Setting this will also reset kOfxParamPropDisplayMin
*/
#define kOfxParamPropMin "OfxParamPropMin"

/** @brief Numeric parameter property, minimum value the parameter can have

    - type is parameter type dependant, see @ref ParameterPropertiesVariantTypes
    - default to the largest possible value of that type (eg: INT_MAX for an integer, DBL_MAX for a double parameter
    Setting this will also set kOfxParamPropDisplayMax
*/
#define kOfxParamPropMax "OfxParamPropMax"

/** @brief Numeric parameter property, the minimum value allowable in a GUI, which can be distinct to, but greater than kOfxParamPropMin

    - type is parameter type dependant, see @ref ParameterPropertiesVariantTypes
    - default to the smallest possible value
    of that type (eg: INT_MIN for an integer, -DBL_MAX for a double parameter)
*/
#define kOfxParamPropDisplayMin "OfxParamPropDisplayMin"

/** @brief Numeric parameter property, the maximum value allowable in a GUI, which can be distinct to, but greater than kOfxParamPropMin

    - type is parameter type dependant, see @ref ParameterPropertiesVariantTypes
    - defaults to 0 cast to the relevant type (or "" for strings and custom parameters)
*/
#define kOfxParamPropDisplayMax "OfxParamPropDisplayMax"

/** @brief Numeric parameter property, the granularity of any slider used to represent this parameter

    - double X 1
    - defaults to 1

This value is always in cannonical coordinates, even for double parameters that are normalised.

*/
#define kOfxParamPropIncrement "OfxParamPropIncrement"

/** @brief Numeric parameter property, how many decimal points to display of a double parameter in a GUI

    - int X 1
    - defaults to 2 */
#define kOfxParamPropDigits "OfxParamPropDigits"

/** @brief Multidimensional parameter property, which sets the label on individual dimensions 

    - char * X 2 or 3 (depending on parameter type)
    - UTF8
    - defaults to 'x', 'y' and 'z'

Use this to override the default labels on the individual dimensions of a multi dimensional parameter.
*/
#define kOfxParamPropDimensionLabel "OfxParamPropDimensionLabel"

/** @brief Parameter property, is the parameter automatically adding key frames or not at the moment

    - int X 1
    - 0 or 1
    - read only
*/
#define kOfxParamPropIsAutoKeying "OfxParamPropIsAutoKeying"

/** @brief Custom Parameter Property, a pointer to a custom parameter's interpolation function 

    - pointer X 1
    - must point to a OfxCustomParamInterpFuncV1
    - defaults to NULL
    It is an error not to set this property in a custom parameter during a plugin's define call if
    the custom parameter declares itself to be an animating parameter
 */
#define kOfxParamPropCustomInterpCallbackV1 "OfxParamPropCustomCallbackV1"

/** @brief String Parameter Property, what style of string param is it, it's value must be one of the kOfxParamStringIs* defines

    - char * X 1
    - defaults to kOfxParamStringIsSingleLine */
#define kOfxParamPropStringMode "OfxParamPropStringMode"

/** @brief String Parameter Property, whether a string with kOfxParamPropStringMode set to kOfxParamStringFilePath/Dir, requires that the file exists,

    - int X 1
    - 0 or 1
    - defaults to true */
#define kOfxParamPropStringFilePathExists    "OfxParamPropStringFilePathExists"

/** @brief Used to set a string parameter to be single line, 
    value to be passed to a kOfxParamPropStringMode property */
#define kOfxParamStringIsSingleLine    "OfxParamStringIsSingleLine"

/** @brief Used to set a string parameter to be multiple line, 
    value to be passed to a kOfxParamPropStringMode property */
#define kOfxParamStringIsMultiLine     "OfxParamStringIsMultiLine"

/** @brief Used to set a string parameter to be a file path,
    value to be passed to a kOfxParamPropStringMode property */
#define kOfxParamStringIsFilePath      "OfxParamStringIsFilePath"

/** @brief Used to set a string parameter to be a directory path,
    value to be passed to a kOfxParamPropStringMode property */
#define kOfxParamStringIsDirectoryPath "OfxParamStringIsDirectoryPath"

/** @brief Use to set a string parameter to be a simple label, 
    value to be passed to a kOfxParamPropStringMode property  */
#define kOfxParamStringIsLabel         "OfxParamStringIsLabel"


/** @brief Property used by ::OfxCustomParamInterpFuncV1 to get/set values of an interpolating custom parameter

 - C string
 - as an \e inArgs property it will be two 2 and contain the encoded value of the two keyframes to interpolate,
 - as an \e outArgs property, it will have a single value, which the plugin should set with the encoded interpolated value
 */
#define kOfxParamPropCustomValue "OfxParamPropCustomValue"

/** @brief Property used by ::OfxCustomParamInterpFuncV1 to indicate the times two keys occur at

 - double X 2
 - on the \e inArgs parameter 
 - indicates the absolute times that the two keyframes occur at
 */
#define kOfxParamPropInterpolationTime "OfxParamPropInterpolationTime"

/** @brief Property used by ::OfxCustomParamInterpFuncV1 to indicate the amount of interpolation to perform

 - on the \e inArgs parameter 
 - double X 1
 - from 0 to 1
 - this property indicates how far between the two ::kOfxParamPropCustomValue keyframes to interpolate.
 */
#define kOfxParamPropInterpolationAmount "OfxParamPropInterpolationAmount"

/*@}*/
/*@}*/

/** @brief Function prototype for custom parameter interpolation callback functions

  \arg \e instance   the plugin instance that this parameter occurs in
  \arg \e inArgs     handle holding the following properties...
    - kOfxPropName - the name of the custom parameter to interpolate
    - kOfxPropTime - absolute time the interpolation is ocurring at
    - kOfxParamPropCustomValue - string property that gives the value of the two keyframes to interpolate, in this case 2D
    - kOfxParamPropInterpolationTime - 2D double property that gives the time of the two keyframes we are interpolating
    - kOfxParamPropInterpolationAmount - 1D double property indicating how much to interpolate between the two keyframes

  \arg \e outArgs handle holding the following properties to be set
    - kOfxParamPropCustomValue - the value of the interpolated custom parameter, in this case 1D

This function allows custom parameters to animate by performing interpolation between keys.

The plugin needs to parse the two strings encoding keyframes on either side of the time 
we need a value for. It should then interpolate a new value for it, encode it into a string and set
the ::kOfxParamPropCustomValue property with this on the outArgs handle.

The interp value is a linear interpolation amount, however his may be derived from a cubic (or other) curve.
*/
typedef OfxStatus (OfxCustomParamInterpFuncV1)(OfxParamSetHandle instance,
					       OfxPropertySetHandle inArgs,
					       OfxPropertySetHandle outArgs);


/** @brief The suite used to define and use parameters 
 */
typedef struct OfxParameterSuiteV1 {
  /** @brief Defines a new parameter of the given type in a describe action

  \arg \e paramSet   handle to the parameter set descriptor that will hold this parameter
  \arg \e paramType   type of the parameter to create, one of the kOfxParamType* #defines
  \arg \e name        unique name of the parameter
  \arg \e propertySet  if not null, a pointer to the parameter descriptor's property set will be placed here.

  This function defines a parameter in a parameter set and returns a property set which is used to describe that parameter.

  This function does not actually create a parameter, it only says that one should exist in any subsequent instances. To fetch an
  parameter instance paramGetHandle must be called on an instance.

  This function can always be called in one of a plug-in's 'describe' functions which defines the parameter sets common to all instances of a plugin.

  returns
  - ::kOfxStatOK       - the parameter was created correctly
  - ::kOfxStatErrBadHandle  - if the plugin handle was invalid
  - ::kOfxStatErrExists     - if a parameter of that name exists already in this plugin
  - ::kOfxStatErrUnknown    - if the type is unknown
  - ::kOfxStatErrUnsuported - if the type is known but unsupported
  */
  OfxStatus (*paramDefine)(OfxParamSetHandle paramSet,
			   const char *paramType,
			   const char *name,
			   OfxPropertySetHandle *propertySet);

  /** @brief Retrieves the handle for a parameter in a given parameter set

  \arg \e paramSet    instance of the plug-in to fetch the property handle from
  \arg \e name        parameter to ask about
  \arg \e param       pointer to a param handle, the value is returned here
  \arg \e propertySet  if not null, a pointer to the parameter's property set will be placed here.

  Parameter handles retrieved from an instance are always distinct in each instance. The paramter handle is valid for the life-time of the instance. Parameter handles in instances are distinct from paramter handles in plugins. You cannot call this in a plugin's describe function, as it needs an instance to work on.

  returns
  - ::kOfxStatOK       - the parameter was found and returned
  - ::kOfxStatErrBadHandle  - if the plugin handle was invalid
  - ::kOfxStatErrUnknown    - if the type is unknown
  */
  OfxStatus (*paramGetHandle)(OfxParamSetHandle paramSet,
			      const char *name,
			      OfxParamHandle *param,
			      OfxPropertySetHandle *propertySet);

  /** @brief Retrieves the property set handle for the given parameter set

  \arg \e paramSet      parameter set to get the property set for
  \arg \e propHandle    pointer to a the property set handle, value is returedn her

  \note The property handle belonging to a parameter set is the same as the property handle belonging to the plugin instance.

  returns
  - ::kOfxStatOK       - the property set was found and returned
  - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
  - ::kOfxStatErrUnknown    - if the type is unknown
  */
  OfxStatus (*paramSetGetPropertySet)(OfxParamSetHandle paramSet,
				      OfxPropertySetHandle *propHandle);

  /** @brief Retrieves the property set handle for the given parameter

  \arg \e param         parameter to get the property set for
  \arg \e propHandle    pointer to a the property set handle, value is returedn her

  The property handle is valid for the lifetime of the parameter, which is the lifetime of the instance that owns the parameter

  returns
  - ::kOfxStatOK       - the property set was found and returned
  - ::kOfxStatErrBadHandle  - if the paramter handle was invalid
  - ::kOfxStatErrUnknown    - if the type is unknown
  */
  OfxStatus (*paramGetPropertySet)(OfxParamHandle param,
				   OfxPropertySetHandle *propHandle);

  /** @brief Gets the current value of a parameter,

  \arg \e paramHandle parameter handle to fetch value from
  \arg \e ...         one or more pointers to variables of the relevant type to hold the parameter's value

  This gets the current value of a parameter. The varargs ... argument needs to be pointer to C variables
  of the relevant type for this parameter. Note that params with multiple values (eg Colour) take
  multiple args here. For example...

  @verbatim
  OfxParamHandle myDoubleParam, *myColourParam;
  ofxHost->paramGetHandle(instance, "myDoubleParam", &myDoubleParam);
  double myDoubleValue;
  ofxHost->paramGetValue(myDoubleParam, &myDoubleValue);
  ofxHost->paramGetHandle(instance, "myColourParam", &myColourParam);
  double myR, myG, myB;
  ofxHost->paramGetValue(myColourParam, &myR, &myG, &myB);
  @endverbatim

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramGetValue)(OfxParamHandle  paramHandle,
			     ...);


  /** @brief Gets the value of a parameter at a specific time.

  \arg \e paramHandle parameter handle to fetch value from
  \arg \e time       at what point in time to look up the parameter
  \arg \e ...        one or more pointers to variables of the relevant type to hold the parameter's value

  This gets the current value of a parameter. The varargs needs to be pointer to C variables
  of the relevant type for this parameter. See OfxParameterSuiteV1::paramGetValue for notes on
  the varags list

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramGetValueAtTime)(OfxParamHandle  paramHandle,
				   OfxTime time,
				   ...);

  /** @brief Gets the derivative of a parameter at a specific time.

  \arg \e paramHandle parameter handle to fetch value from
  \arg \e time       at what point in time to look up the parameter
  \arg \e ...        one or more pointers to variables of the relevant type to hold the parameter's derivative

  This gets the derivative of the parameter at the indicated time. 

  The varargs needs to be pointer to C variables
  of the relevant type for this parameter. See OfxParameterSuiteV1::paramGetValue for notes on
  the varags list.

  Only double and colour params can have their derivatives found.

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramGetDerivative)(OfxParamHandle  paramHandle,
				  OfxTime time,
				  ...);

  /** @brief Gets the integral of a parameter over a specific time range,

  \arg \e paramHandle parameter handle to fetch integral from
  \arg \e time1      where to start evaluating the integral
  \arg \e time2      where to stop evaluating the integral
  \arg \e ...        one or more pointers to variables of the relevant type to hold the parameter's integral

  This gets the integral of the parameter over the specified time range.

  The varargs needs to be pointer to C variables
  of the relevant type for this parameter. See OfxParameterSuiteV1::paramGetValue for notes on
  the varags list.

  Only double and colour params can be integrated.

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramGetIntegral)(OfxParamHandle  paramHandle,
				OfxTime time1, OfxTime time2,
				...);

  /** @brief Sets the current value of a parameter

  \arg \e paramHandle parameter handle to set value in
  \arg \e ...        one or more variables of the relevant type to hold the parameter's value

  This sets the current value of a parameter. The varargs ... argument needs to be values
  of the relevant type for this parameter. Note that params with multiple values (eg Colour) take
  multiple args here. For example...
  @verbatim
  ofxHost->paramSetValue(instance, "myDoubleParam", double(10));
  ofxHost->paramSetValue(instance, "myColourParam", double(pix.r), double(pix.g), double(pix.b));
  @endverbatim

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramSetValue)(OfxParamHandle  paramHandle,
			     ...);

  /** @brief Keyframes the value of a parameter at a specific time.

  \arg \e paramHandle parameter handle to set value in
  \arg \e time       at what point in time to set the keyframe
  \arg \e ...        one or more variables of the relevant type to hold the parameter's value

  This sets a keyframe in the parameter at the indicated time to have the indicated value.
  The varargs ... argument needs to be values of the relevant type for this parameter. See the note on 
  OfxParameterSuiteV1::paramSetValue for more detail

  This function can be called the ::kOfxActionInstanceChanged action and during image effect analysis render passes.

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramSetValueAtTime)(OfxParamHandle  paramHandle,
				   OfxTime time,  // time in frames
				   ...);


  /** @brief Returns the number of keyframes in the parameter

  \arg \e paramHandle parameter handle to interogate
  \arg \e numberOfKeys  pointer to integer where the return value is placed

  This function can be called the ::kOfxActionInstanceChanged action and during image effect analysis render passes.

  Returns the number of keyframes in the parameter.

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramGetNumKeys)(OfxParamHandle  paramHandle,
			       unsigned int  *numberOfKeys);

  /** @brief Returns the time of the nth key

  \arg \e paramHandle parameter handle to interogate
  \arg \e nthKey      which key to ask about (0 to paramGetNumKeys -1), ordered by time
  \arg \e time	  pointer to OfxTime where the return value is placed

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  - ::kOfxStatErrBadIndex   - the nthKey does not exist
  */
  OfxStatus (*paramGetKeyTime)(OfxParamHandle  paramHandle,
			       unsigned int nthKey,
			       OfxTime *time);


  /** @brief Finds the index of a keyframe at/before/after a specified time.

  \arg \e paramHandle parameter handle to search
  \arg \e time          what time to search from
  \arg \e direction     == 0 indicates search for a key at the indicated time (some small delta)
  - > 0 indicates search for the next key after the indicated time
  - < 0 indicates search for the previous key before the indicated time
  \arg \e index	   pointer to an integer which in which the index is returned set to -1 if no key was found

  returns
  - ::kOfxStatOK            - all was OK
  - ::kOfxStatFailed        - if the search failed to find a key
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramGetKeyIndex)(OfxParamHandle  paramHandle,
				OfxTime time,
				int     direction,
				int    *index);

  /** @brief Deletes a keyframe if one exists at the given time.

  \arg \e paramHandle parameter handle to delete the key from
  \arg \e time      time at which a keyframe is

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  - ::kOfxStatErrBadIndex   - no key at the given time
  */
  OfxStatus (*paramDeleteKey)(OfxParamHandle  paramHandle,
			      OfxTime time);

  /** @brief Deletes all keyframes from a parameter.

  \arg \e paramHandle parameter handle to delete the keys from
  \arg \e name      parameter to delete the keyframes frome is

  This function can be called the ::kOfxActionInstanceChanged action and during image effect analysis render passes.

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramDeleteAllKeys)(OfxParamHandle  paramHandle);

  /** @brief Copies one parameter to another, including any animation etc...

  \arg \e paramTo  parameter to set
  \arg \e paramFrom parameter to copy from
  \arg \e dstOffset temporal offset to apply to keys when writing to the paramTo
  \arg \e frameRange if paramFrom has animation, and frameRange is not null, only this range of keys will be copied

  This copies the value of \e paramFrom to \e paramTo, including any animation it may have. All the previous values in \e paramTo will be lost.

  To choose all animation in \e paramFrom set \e frameRange to [0, 0]

  This function can be called the ::kOfxActionInstanceChanged action and during image effect analysis render passes.

  \pre
  - Both parameters must be of the same type.

  \return
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the parameter handle was invalid
  */
  OfxStatus (*paramCopy)(OfxParamHandle  paramTo, OfxParamHandle  paramFrom, OfxTime dstOffset, OfxRangeD *frameRange);


  /** @brief Used to group any parameter changes for undo/redo purposes

  \arg \e paramSet   the parameter set in which this is happening
  \arg \e name       label to attach to any undo/redo string UTF8

  If a plugin calls paramSetValue/paramSetValueAtTime on one or more parameters, either from custom GUI interaction
  or some analysis of imagery etc.. this is used to indicate the start of a set of a parameter
  changes that should be considered part of a single undo/redo block.

  See also OfxParameterSuiteV1::paramEditEnd

  \return
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the instance handle was invalid

  */
  OfxStatus (*paramEditBegin)(OfxParamSetHandle paramSet, const char *name); 

  /** @brief Used to group any parameter changes for undo/redo purposes

  \arg \e paramSet   the parameter set in which this is happening

  If a plugin calls paramSetValue/paramSetValueAtTime on one or more parameters, either from custom GUI interaction
  or some analysis of imagery etc.. this is used to indicate the end of a set of parameter
  changes that should be considerred part of a single undo/redo block

  See also OfxParameterSuiteV1::paramEditBegin

  returns
  - ::kOfxStatOK       - all was OK
  - ::kOfxStatErrBadHandle  - if the instance handle was invalid

  */
  OfxStatus (*paramEditEnd)(OfxParamSetHandle paramSet);
 } OfxParameterSuiteV1;

#ifdef __cplusplus
}
#endif


/** @file ofxParam.h
 
  This header contains the suite definition to manipulate host side parameters.

  For more details go see @ref ParametersPage
*/

/** @page ParametersPage  OFX : Parameters Suite
 
 @section ParametersIntro Introduction

 Nearly all plug-ins have some sort of parameters that control their behaviour, the radius
 of a circle drawer, the frequencies to filter out of an audio signal, the colour of a lens flare 
 and so on.

 Seeing as hosts already provide for the general management of their own native parameters (eg: persistance, 
 interface, animation etc...), it would make no sense to force plug-ins to do this all themselves. 

 The OFX Parameters Suite is the means by which  parameters are defined and used by the 
 plug-in but maintained the host. It is defined in the ofxParam.h header file.
 
<HR>

 @section ParametersDefining Defining Parameters

 A plugin needs to define it's parameters during it's 'describe' call. It does this with the OfxParameterSuiteV1::paramDefine function, which returns a handle to a parameter \em description. Parameters cannot be defined outside of the plugins 'describe' actions.

 Parameters are uniquely labelled within a plugin with an ASCII null terminated C-string. This name is not necassarily meant to be end-user readable, various properties are provided to set the user visible labels on the param.

 All parameters hold properties, though the exact set of properties on a parameter is dependant on the type of the parameter. For more information on properties see \ref PropertiesPage.

 A parameter's handle comes in two slightly different flavours. The handle returned inside a plugin's describe action is not an actual instance of a parameter, it is there for the purpose description only. You can only set properties on that handle (eg: label, min/max value, default ...), you cannot get values from it or set values in it. The parameters defined in the describe action will common to all instances of a plugin.

 The handle returned by OfxParameterSuiteV1::paramGetHandle or by paramDefine outside of a describe action will be a working instance of a parameter, you can still set (some) properties of the parameter, but all the get/set value functions are now useable.

<HR>

@section ParametersTypes Parameter Types
 There are sixteen types of parameter. These are 
 - integers, 1, 2 and 3 dimensional
 - doubles, 1, 2 and 3 dimensional
 - colour, RGB and RGB + Alpha
 - booleans
 - choice 
 - string
 - custom
 - push button
 - group
 - page

<HR>

@section ParametersCommonProperties  Parameter Properties

 A parameter host has the following property, 
   - ::kOfxParamHostPropSupportsStringAnimation
   - ::kOfxParamHostPropSupportsCustomInteract
   - ::kOfxParamHostPropSupportsChoiceAnimation
   - ::kOfxParamHostPropSupportsBooleanAnimation
   - ::kOfxParamHostPropSupportsCustomAnimation
   - ::kOfxParamHostPropMaxParameters
   - ::kOfxParamHostPropMaxPages
   - ::kOfxParamHostPropPageRowColumnCount

A parameter set has the following properties...
  - ::kOfxPluginPropParamPageOrder

 The following properties are common to all parameters....
   - ::kOfxPropType , which will always be ::kOfxTypeParameter
   - ::kOfxPropName
   - ::kOfxPropLabel *
   - ::kOfxPropShortLabel *
   - ::kOfxPropLongLabel *
   - ::kOfxParamPropType
   - ::kOfxParamPropSecret *
   - ::kOfxParamPropCanUndo 
   - ::kOfxParamPropHint *
   - ::kOfxParamPropScriptName
   - ::kOfxParamPropParent 
   - ::kOfxParamPropEnabled *
   - ::kOfxParamPropDataPtr *

 All but the group and page parameters have the following properties...
   - ::kOfxParamPropInteractV1
   - ::kOfxParamPropInteractSize
   - ::kOfxParamPropInteractSizeAspect
   - ::kOfxParamPropInteractMinimumSize
   - ::kOfxParamPropInteractPreferedSize

 All parameters that hold values (ie: all but the page, group and push button params) have the following...
   - ::kOfxParamPropDefault
   - ::kOfxParamPropAnimates
   - ::kOfxParamPropIsAnimating
   - ::kOfxParamPropIsAutoKeying
   - ::kOfxParamPropPersistant
   - ::kOfxParamPropEvaluateOnChange *
   - ::kOfxParamPropDefault *
   - ::kOfxParamPropPluginMayWrite
   - ::kOfxParamPropCacheInvalidation

 All numeric parameter types (ie: int 1, 2 & 3D and double 1, 2 and 3D) parameters have...
   - ::kOfxParamPropMin *
   - ::kOfxParamPropMax *
   - ::kOfxParamPropDisplayMin *
   - ::kOfxParamPropDisplayMax *

 All double parameter types (ie: double 1, 2 and 3D) parameters have...
   - ::kOfxParamPropIncrement *
   - ::kOfxParamPropDigits *

 1D double parameter types have...
   - ::kOfxParamPropShowTimeMarker *
   - ::kOfxParamPropDoubleType

 2D and 3D double parameter types have...
   - ::kOfxParamPropDoubleType

 2D and 3D double and integer parameter types have...
   - ::kOfxParamPropDimensionLabel

 String parameters have...
   - ::kOfxParamPropStringMode
   - ::kOfxParamPropStringFilePathExists

 Choice parameters have...
   - ::kOfxParamPropChoiceOption *

 Custom parameters have...
   - ::kOfxParamPropCustomInterpCallbackV1

 Paged parameters have...
   - ::kOfxParamPropPageChild

\b Note : 
- the properties labelled with a * may be changed by a plugin outside of the describe actions
- the choice option property can be reset or have options added to it outside the describe actions (so to delete an option reset and add again)

<HR>

 @subsection ParametersMultiDimensional Multidimensional Parameters

 Some parameter types are multi dimensional, these are...
 
   - ::kOfxParamTypeDouble2D    - 2 doubles
   - ::kOfxParamTypeInteger2D   - int X 2
   - ::kOfxParamTypeDouble3D    - double X 3
   - ::kOfxParamTypeInteger3D   - int X 3
   - ::kOfxParamTypeRGB         - double X 3
   - ::kOfxParamTypeRGBA        - double X 4

 These parameters are treated in an atomic manner, so that all dimensions are set/retrieved simultaneously. This applies to keyframes as well.

 The non colour parameters have an implicit 'X', 'Y' and 'Z' dimension, and any interface should display them with such labels.

 @subsection ParametersInteger Integer Parameters

 These are typed by ::kOfxParamTypeInteger, ::kOfxParamTypeInteger2D and kOfxParamTypeInteger3D.
 
 Integer parameters are of 1, 2 and 3D varieties and contain integer values, between INT_MIN and INT_MAX. 

 @subsection ParametersDouble Double Parameters

 These are typed by ::kOfxParamTypeDouble, ::kOfxParamTypeDouble2D and kOfxParamTypeDouble3D.

 Double parameters are of 1, 2 and 3D varieties and contain double precision floating point values. The host may be implementing them as floats, however they must be passed across as doubles.

 @subsection ParametersColour Colour Parameters

 These are typed by ::kOfxParamTypeRGB and ::kOfxParamTypeRGBA.

 Colour parameters are 3 or 4 dimensional double precision floating point parameters. They are displayed using the host's appropriate interface for a colour. Values are always normalised in the range [0 .. 1], with 0 being the nominal black point and 1 being the white point.
 
 @subsection ParametersBoolean Boolean Parameters

 This is typed by ::kOfxParamTypeBoolean.

 Boolean parameters are integer values that can have only one of two values, 0 or 1. 
 
 @subsection ParametersChoice Choice Parameters

 This is typed by ::kOfxParamTypeChoice.

 Choice parameters are integer values from 0 to N-1, which correspond to N labeled options. 

 Choice parameters have their individual options set via the ::kOfxParamPropChoiceOption property, for example

@verbatim
  gPropHost->propSetString(myChoiceParam, kOfxParamPropChoiceOption, 0, "1st Choice");
  gPropHost->propSetString(myChoiceParam, kOfxParamPropChoiceOption, 1, "2nd Choice");
  gPropHost->propSetString(myChoiceParam, kOfxParamPropChoiceOption, 2, "3nd Choice");
  ...
  gPropHost->propSetString(myChoiceParam, kOfxParamPropChoiceOption, n, "nth Choice");
@endverbatim

 It is an error to have gaps in the choices after the define call has returned.

 @subsection ParametersString String Parameters

 This is typed by ::kOfxParamTypeString.

 String parameters contain null terminated char * C strings. They can be of several different variants, which is controlled by the ::kOfxParamPropStringMode property, these are 
    
  - ::kOfxParamStringIsSingleLine - single line string
  - ::kOfxParamStringIsMultiLine  - multi line string
  - ::kOfxParamStringIsFilePath   - single line string interpretted as a file path on the computer
                                  this should have appropriate file browser UIs available
  - ::kOfxParamStringIsDirectoryPath - similar to the above, but refers to a directory rather than a file
  - ::kOfxParamStringIsLabel      - string behaves as a pseudo parameter and acts as a label on the interface
                                  in this case the string is not editable by the user

 @subsection ParametersGroup Group Parameters

 This is typed by ::kOfxParamTypeGroup.

 Group parameters allow all parameters to be arranged in a tree hierarchy. They have no value, they are purely a grouping element.

 All parameters have a ::kOfxParamPropParent property, which is a string property naming the group parameter which is its parent. 

 The empty string "" is used to label the root of the parameter hierarchy, which is the default parent for all parameters.

 Parameters inside a group are ordered by their order of addition to that group, which implies parameters in the root group are added in order of definition.

 Any host based hierarchical GUI should use this hierarchy to order parameters (eg: animation sheets).

 @subsection ParametersPageParam Page Parameters

 This is typed by ::kOfxParamTypePage.
 
 Page parameters are coverred in \ref ParametersInterfacesPagedLayouts.

 @subsection ParametersCustom Custom Parameters

 This is typed by ::kOfxParamTypeCustom.
 
 Custom parameters contain null terminated char * C strings, and may animate. They are designed to provide plugins with a way of storing data that is too complicated or impossible to store in a set of ordinary parameters. 

 If a custom parameter animates, it must set its ::kOfxParamPropCustomInterpCallbackV1 property, which points to a OfxCustomParamInterpFuncV1 function. This function is used to interpolate keyframes in custom params.

 Custom parameters have no interface by default. However,
  - if they animate, the host's animation sheet/editor should present a keyframe/curve representation to allow positioning of keys and control of interpolation. The 'normal' (ie: paged or hierarchical) interface should not show any gui.
  - if the custom param sets its ::kOfxParamPropInteractV1 property, this should be used by the host in any normal (ie: paged or hierarchical) interface for the parameter.
  Custom parameters are mandatory, as thet are simply ASCII C strings. However, animation of custom parameters an support for an in editor interact is optional.

 @subsection ParametersPushButton Push Button Parameters
 This is typed by ::kOfxParamTypePushButton.

 Push button parameters have no value, they are there so a plugin can detect if they have been pressed and perform some action. If pressed, a ::kOfxActionInstanceChanged action will be issued on the parameter with a ::kOfxPropChangeReason of ::kOfxChangeUserEdited.
 
<HR>
 @section ParametersAnimation Animation

 By default the following parameter types animate...
   - ::kOfxParamTypeInteger
   - ::kOfxParamTypeInteger2D
   - ::kOfxParamTypeInteger3D
   - ::kOfxParamTypeDouble
   - ::kOfxParamTypeDouble2D
   - ::kOfxParamTypeDouble3D
   - ::kOfxParamTypeRGBA
   - ::kOfxParamTypeRGB

 The following types cannot animate...
   - ::kOfxParamTypeGroup
   - ::kOfxParamTypePage
   - ::kOfxParamTypePushButton

 The following may animate, depending on the host. Properties exist on the host to check this. If the host does support animation on them, then they do _not_ animate by default. They are...
   - ::kOfxParamTypeCustom
   - ::kOfxParamTypeString
   - ::kOfxParamTypeBoolean
   - ::kOfxParamTypeChoice

 By default the OfxParameterSuiteV1::paramGetValue will get the 'current' value of the parameter. To access values in a potentially animating parameter, use the OfxParameterSuiteV1::paramGetValueAtTime function.

 Keys can be manipulated in a parameter using a variety of functions, these are...
   - OfxParameterSuiteV1::paramSetValueAtTime
   - OfxParameterSuiteV1::paramGetNumKeys
   - OfxParameterSuiteV1::paramGetKeyTime
   - OfxParameterSuiteV1::paramGetKeyIndex
   - OfxParameterSuiteV1::paramDeleteKey
   - OfxParameterSuiteV1::paramDeleteAllKeys

<HR>
 @section ParametersInterfaces Parameter Interfaces
 
 Parameters will be presented to the user in some form of interface. Typically on most host systems, this comes in three varieties...
   - a paged layout, with parameters spread over multiple controls pages (eg: the FLAME control pages)
   - a hierarchical layout, with parameters presented in a grouped tree (eg: the After Effects 'effects' window)
   - an animation sheet, showing animation curves and key frames. Typically this is hierarchical.

Most systems have an animation sheet and present one of either the paged or the hierarchical layouts. 

Because a hierarchy of controls is explicitly set during plugin definition, the case of the animation sheet and hierarchial GUIs are taken care of explicitly.

@subsection ParametersInterfacesPagedLayouts Paged Parameter Editors

 A paged layout of controls is difficult to standardise, as the size of the page and controls, how the controls are positioned on the page, how many controls appear on a page etc... depend very much upon the host implementation. A paged layout is ideally best described in the .XML resource supplied by the plugin, however a fallback page layout can be specified in OFX via the ::kOfxParamTypePage parameter type.

Several host properties are associated with paged layouts, these are...
  - kOfxParamHostPropMaxPages - the maximum number of pages you may use, 0 implies an unpaged layout
  - kOfxParamHostPropPageRowColumnCount - the number of rows and columns for parameters in the paged layout.

 Each page parameter represents a page of controls. The controls in that page are set by the plugin using the ::kOfxParamPropPageChild multi-dimensional string. For example...

@verbatim
    OfxParamHandle  page;
    gHost->paramDefine(plugin, kOfxParamTypePage, "Main", &page);

    propHost->propSetString(page, kOfxParamPropPageChild, 0, "size");      // add the size parameter to the top left of the page
    propHost->propSetString(page, kOfxParamPropPageChild, 1, kOfxParamPageSkipRow); // skip a row
    propHost->propSetString(page, kOfxParamPropPageChild, 2, "centre");    // add the centre parameter
    propHost->propSetString(page, kOfxParamPropPageChild, 3, kOfxParamPageSkipColumn); // skip a column, we are now at the top of the next column
    propHost->propSetString(page, kOfxParamPropPageChild, 4, "colour"); // add the colour parameter
@endverbatim

 The host then places the parameters on that page in the order they were added, starting at the top left and going down columns, then across rows as they fill.

 Note that there are two pseudo parameters names used to help control layout, the ::kOfxParamPageSkipRow and ::kOfxParamPageSkipColumn. These will help control how parameters are added to a page, allowing vertical or horizontal slots to be skipped.
 
 A host sets the order of pages by using the instance's ::kOfxPluginPropParamPageOrder property. Note that this property can vary from context to context, so you can exclude pages in contexts they are not useful in. For example...

@verbatim
OfxStatus
describeInContext(OfxImageEffectHandle plugin)
{
...
    // order our pages of controls
    propHost->propSetString(paramSetProp, kOfxPluginPropParamPageOrder, 0, "Main");
    propHost->propSetString(paramSetProp, kOfxPluginPropParamPageOrder, 1, "Sampling");
    propHost->propSetString(paramSetProp, kOfxPluginPropParamPageOrder, 2, "Colour Correction");
    if(isGeneralContext)
       propHost->propSetString(paramSetProp, kOfxPluginPropParamPageOrder, 3, "Dance! Dance! Dance!");
...
}
@endverbatim

 Note, 
   - parameters \em can placed on more than a single page (this is often useful),
   - group parameters cannot be added to a page,
   - page parameters cannot be added to a page.

@subsection ParametersInterfacesUndoRedo Parameter Undo/Redo

Hosts usually retain an undo/redo stack, so users can undo changes they make to a parameter. Often undos and redos are grouped together into an undo/redo block, where multiple parameters are dealt with as a single undo/redo event. Plugins need to be able to deal with this cleanly. 

Parameters can be excluded from being undone/redone if they set the ::kOfxParamPropCanUndo property to 0.

If the plugin changes parameters' values by calling the get and set value functions, they will ordinarily be put on the undo stack, one event per parameter that is changed. If the plugin wants to group sets of parameter changes into a single undo block and label that block, it should use the OfxParameterSuiteV1::paramEditBegin and OfxParameterSuiteV1::paramEditEnd functions.

An example would be a 'preset' choice parameter in a sky simulation whose job is to set other parameters to values that achieve certain looks, eg "Dusk", "Midday", "Stormy", "Night" etc... This parameter has a value change callback which looks for  ::kOfxParamEventUserEdited then sets other parameters, sky colour, cloud density, sun position etc.... It also resets itself to the first choice, which says "Example Skys...". 

Rather than have many undo events appear on the undo stack for each individual parameter change, the effect groups them via the paramEditBegin/paramEditEnd and gets a single undo event. The 'preset' parameter would also not want to be undoable as it such an event is redunant. Note that as the 'preset' has been changed it will be sent another instance changed action, however it will have a reason of ::kOfxChangePluginEdited, which it ignores and so stops an infinite loop occuring.

@subsection ParametersInteracts Parameter Custom Interfaces

All parameters, except for custom parameters, have some default interface that the host creates for them. Be it a numeric slider, colour swatch etc... Effects can override the default interface (or set an interface for a custom parameter) by setting the ::kOfxParamPropInteractV1 to point to a OfxInteractClientStructV1. This will completely replace the parameters default user interface in the 'paged' and 'hierarchical' interfaces, but it will not replace the parameter's interface in any animation sheet. For more details on interacts see \ref CustomInteractionPage.

Properties affecting custom interacts for parameters are...
    - ::kOfxParamPropInteractSizeAspect
    - ::kOfxParamPropInteractMinimumSize
    - ::kOfxParamPropInteractPreferedSize

The viewport for such interacts will be dependant upon the various properties above, and possibly a per host override in any XML resource file.
 
The GL_PROJECTION matrix will be an orthographic 2D view with -0.5,-0.5 at the bottom left and viewport width-0.5, viewport height-0.5 at the top right. 

The GL_MODELVIEW matrix will be the identity matrix.

The bit depth will be double buffered 24 bit RGB.

A parameter's interact draw function will have full responsibility for drawing the interact, including clearing the background and swapping buffers.

@subsection ParametersXML XML Resource Specification for Parameters

Parameters can have various properties overriden via a seperate XML based resource file. For more information see \ref ExternalResourcesPage.

<HR>
@section ParametersPersistance Parameter Persistance 

All parameters flagged with the ::kOfxParamPropPersistant property will persist when an effect is saved. How the effect is saved is completely up to the host, it may be in a file, a data base, where ever. We call a saved set of parameters a \b setup. A host will need to save the major version number of the plugin, as well as the plugin's unique identifier, in any setup.

When an host loads a set up it should do so in the following manner...
-# examines the setup for the major version number.
-# find a matching plugin with that major version number, if multiple minor versions exist, the plugin with the largest minor version should be used.
-# creates an instance of that plugin with its set of parameters.
-# sets all those parameters to the defaults specified by the plugin.
-# examines the setup for any persistant parameters, then sets the instance's parameters to any found in it.
-# calls create instance on the plugin.

It is \em not an error for a parameter to exist in the plugin but not the setup, and vice versa. This allows a plugin developer to modify parameter sets between point releases, generally by adding new params. The developer should be sure that the default values of any new parameters yield the same behaviour as before they were added, otherwise it would be a breach of the 'major version means compatibility' rule.


<HR>
@section ParameterPropertiesVariantTypes Parameter Properties Whose Type Vary

 Some properties type depends on the kind of the parameter, eg: ::kOfxParamPropDefault is an int for a integer parameter but a double X 2 for a ::kOfxParamTypeDouble2D parameter.

 The variant property types are as follows....
 - ::kOfxParamTypeInteger    - int X 1
 - ::kOfxParamTypeDouble     - double X 1
 - ::kOfxParamTypeBoolean    - int X 1
 - ::kOfxParamTypeChoice     - int X 1
 - ::kOfxParamTypeRGBA       - double X 4 (normalised to 0..1 range)
 - ::kOfxParamTypeRGB        - double X 3 (normalised to 0..1 range)
 - ::kOfxParamTypeDouble2D   - double X 2
 - ::kOfxParamTypeInteger2D  - int X 2
 - ::kOfxParamTypeDouble3D   - double X 3
 - ::kOfxParamTypeInteger3D  - int X 3
 - ::kOfxParamTypeString     - char * X 1
 - ::kOfxParamTypeCustom     - char * X 1
 - ::kOfxParamTypePushButton - none

<HR>
@section ParameterPropertiesDoubleTypes Types of Double Parameters

Double parameters can be used to represent a variety of data, by flagging what a double parameter is representing, a plug-in allows a host to represent to the user a more appropriate interface than a raw numerical value. Double parameters have the ::kOfxParamPropDoubleType property, which gives some meaning to the value. This can be one of...
      - ::kOfxParamDoubleTypePlain - parameter has no special interpretation,
      - ::kOfxParamDoubleTypeAngle - parameter is to be interpretted as an angle,
      - ::kOfxParamDoubleTypeScale - parameter is to be interpretted as a scale factor,
      - ::kOfxParamDoubleTypeTime  - parameter represents a time value (1D only),
      - ::kOfxParamDoubleTypeAbsoluteTime  - parameter represents an absolute time value (1D only),
      - ::kOfxParamDoubleTypeNormalisedX - normalised size wrt to the project's X dimension (1D only),
      - ::kOfxParamDoubleTypeNormalisedXAbsolute - normalised absolute position on the X axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedY - normalised size wrt to the project's Y dimension(1D only),
      - ::kOfxParamDoubleTypeNormalisedYAbsolute - normalised absolute position on the Y axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedXY - normalised to the project's X and Y size (2D only),
      - ::kOfxParamDoubleTypeNormalisedXYAbsolute - normalised to the projects X and Y size, and is an absolute position on the image plane.

@subsection ParameterPropertiesDoubleTypesPlain Plain Double Parameters 

Double parameters with their ::kOfxParamPropDoubleType property set to ::kOfxParamDoubleTypePlain are uninterpretted. The values represented to the user are what is reported back to the effect when values are retrieved. 1, 2 and 3D parameters can be flagged as ::kOfxParamDoubleTypePlain, which is the default.

For example a physical simulation plugin might have a 'mass' double parameter, which is in kilograms, which should be displayed and used as a raw value.

@subsection ParameterPropertiesDoubleTypesAngle Angle Double Parameters 

Double parameters with their ::kOfxParamPropDoubleType property set to ::kOfxParamDoubleTypeAngle are interpretted as angles. The host could use some fancy angle widget in it's interface, representing degrees, angles mils whatever. However, the values returned to a plugin are always in degrees. Applicable to 1, 2 and 3D parameters.

For example a plugin that rotates an image in 3D would declare a 3D double parameter and flag that as an angle parameter and use the values as Euler angles for the rotation.


@subsection ParameterPropertiesDoubleTypesScale Scale Double Parameters 

Double parameters with their ::kOfxParamPropDoubleType property set to ::kOfxParamDoubleTypeScale are interpretted as scale factors. The host can represent these as 1..100 percentages, 0..1 scale factors, fractions or whatever is appropriate for its interface. However, the plugin sees these as a straight scale factor, in the 0..1 range. Applicable to 1, 2 and 3D parameters.

For example a plugin that scales the size of an image would declare a 'image scale' parameter and use the raw value of that to scale the image.

@subsection ParameterPropertiesDoubleTypesTime Time Double Parameters 

Double parameters with their ::kOfxParamPropDoubleType property set to ::kOfxParamDoubleTypeTime are interpretted as a time. The host can represent these as frames, seconds, milliseconds, millenia or whatever it feels is appropriate. However, a plugin sees such values in 'frames'. Applicable only to 1D double parameters. It is an error to set this on any other type of double parameter.

For example a plugin that does motion blur would have a 'shutter time' parameter and flags that as a time parameter. The value returned would be used as the length of the shutter, in frames.

@subsection ParameterPropertiesDoubleTypesAbsoluteTime Absolute Time Double Parameters 

Double parameters with their ::kOfxParamPropDoubleType property set to ::kOfxParamDoubleTypeAbsoluteTime are interpretted as an absolute time from the begining of the effect. The host can represent these as frames, seconds, milliseconds, millenia or whatever it feels is appropriate. However, a plugin sees such values in 'frames' from the beginning of a clip. Applicable only to 1D double parameters. It is an error to set this on any other type of double parameter.

For example a plugin that stabalises all the images in a clip to a specific frame would have a 'reference frame' parameter and declare that as an absolute time parameter and use its value to fetch a frame to stablise against.

@subsection ParameterPropertiesDoubleTypesNormalised Normalised Double Parameters
 
There are several values of the ::kOfxParamPropDoubleType that allow a double parameters (1, 2 and 3 dimensional) to be made proportional to the current project's size. This will allow the parameter to scale cleanly with project size changes and to be represented to the user in an appropriate range.
 
For example, the sensible X range of a visual effect plugin is the project's width, say 768 pixels
for a PAL D1 definition video project. The user sees the parameter as 0..768, the effect sees it as 0..1. So if
the plug-in wanted to set the default value of an effect to be the centre of the image, it would flag a 2D 
parameter as normalised and set the defaults to be 0.5. The user would see this in the centre of the
image, no matter the resolution of the project in question. The plugin would retrieve the parameter as 0..1 and scale
it up to the project size to size to use.

Parameters can choose to be normalised in several ways...
      - ::kOfxParamDoubleTypeNormalisedX - normalised size wrt to the project's X dimension (1D only),
      - ::kOfxParamDoubleTypeNormalisedXAbsolute - normalised absolute position on the X axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedY - normalised size wrt to the project's Y dimension(1D only),
      - ::kOfxParamDoubleTypeNormalisedYAbsolute - normalised absolute position on the Y axis (1D only)
      - ::kOfxParamDoubleTypeNormalisedXY - normalised to the project's X and Y size (2D only),
      - ::kOfxParamDoubleTypeNormalisedXYAbsolute - normalised to the projects X and Y size, and is an absolute position on the image plane.

See \ref ImageEffectsCoordinates from more on coordinate systems and how to scale parameters.


@subsection ParameterPropertiesDoubleTypesDefaultsAndAll Double Parameters Defaults, Increments, Mins and Maxs

In all cases double parameters' defaults, minimums and maximums are specified in the same space as the parameter, as is the increment in all cases but normalised parameters.

Normalised parameters specify thier increments in cannonical coordinates, rather than in normalised coordinates. So an increment of '1' means 1 pixel, not '1 project width', otherwise sliders would be a bit wild.

*/


#endif
