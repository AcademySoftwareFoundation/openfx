#ifndef _ofxPropertyHost_h_
#define _ofxPropertyHost_h_

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

#ifdef __cplusplus
extern "C" {
#endif

/** @file ofxProperty.h
Contains the API for manipulating generic properties. For more details see \ref PropertiesPage.
*/

#define kOfxPropertySuite "OfxPropertySuite"

typedef struct OfxPropertySuiteV1 {
  /** @brief Set a single value in a pointer property 

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index is for multidimenstional properties and is dimension of the one we are setting
      \arg \e value is the value of the property we are setting

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue
  */
  OfxStatus (*propSetPointer)(OfxPropertySetHandle properties, const char *property, int index, void *value);

  /** @brief Set a single value in  a string property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index is for multidimenstional properties and is dimension of the one we are setting
      \arg \e value is the value of the property we are setting

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue
 */
  OfxStatus (*propSetString) (OfxPropertySetHandle properties, const char *property, int index, const char *value);

  /** @brief Set a single value in  a double property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index is for multidimenstional properties and is dimension of the one we are setting
      \arg \e value is the value of the property we are setting

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue
 */
  OfxStatus (*propSetDouble) (OfxPropertySetHandle properties, const char *property, int index, double value);

  /** @brief Set a single value in  an int property 

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index is for multidimenstional properties and is dimension of the one we are setting
      \arg \e value is the value of the property we are setting

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue
*/
  OfxStatus (*propSetInt)    (OfxPropertySetHandle properties, const char *property, int index, int value);

  /** @brief Set multiple values of the pointer property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are setting in that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of property values
 */
  OfxStatus (*propSetPointerN)(OfxPropertySetHandle properties, const char *property, int count, void **value);

  /** @brief Set multiple values of a string property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are setting in that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of property values

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue

  */
  OfxStatus (*propSetStringN) (OfxPropertySetHandle properties, const char *property, int count, const char **value);

  /** @brief Set multiple values of  a double property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are setting in that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of property values

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue

  */
  OfxStatus (*propSetDoubleN) (OfxPropertySetHandle properties, const char *property, int count, double *value);

  /** @brief Set multiple values of an int property 

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are setting in that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of property values

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
        - ::kOfxStatErrValue

 */
  OfxStatus (*propSetIntN)    (OfxPropertySetHandle properties, const char *property, int count, int *value);
  
  /** @brief Get a single value from a pointer property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index refers to the index of a multi-dimensional property
      \arg \e value is a pointer the return location

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
 */
  OfxStatus (*propGetPointer)(OfxPropertySetHandle properties, const char *property, int index, void **value);

  /** @brief Get a single value of a string property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index refers to the index of a multi-dimensional property
      \arg \e value is a pointer the return location

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
 */
  OfxStatus (*propGetString) (OfxPropertySetHandle properties, const char *property, int index, char **value);

  /** @brief Get a single value of a double property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index refers to the index of a multi-dimensional property
      \arg \e value is a pointer the return location

      See the note \ref ArchitectureStrings for how to deal with strings.

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
 */
  OfxStatus (*propGetDouble) (OfxPropertySetHandle properties, const char *property, int index, double *value);

  /** @brief Get a single value of an int property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e index refers to the index of a multi-dimensional property
      \arg \e value is a pointer the return location

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
 */
  OfxStatus (*propGetInt)    (OfxPropertySetHandle properties, const char *property, int index, int *value);

  /** @brief Get multiple values of a pointer property 

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are getting of that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of where we will return the property values

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
  */
  OfxStatus (*propGetPointerN)(OfxPropertySetHandle properties, const char *property, int count, void **value);

  /** @brief Get multiple values of a string property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are getting of that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of where we will return the property values

      See the note \ref ArchitectureStrings for how to deal with strings.

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
  */
  OfxStatus (*propGetStringN) (OfxPropertySetHandle properties, const char *property, int count, char **value);

  /** @brief Get multiple values of a double property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are getting of that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of where we will return the property values

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
  */
  OfxStatus (*propGetDoubleN) (OfxPropertySetHandle properties, const char *property, int count, double *value);

  /** @brief Get multiple values of an int property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property
      \arg \e count is the number of values we are getting of that property (ie: indicies 0..count-1)
      \arg \e value is a pointer to an array of where we will return the property values

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
        - ::kOfxStatErrBadIndex
  */
  OfxStatus (*propGetIntN)    (OfxPropertySetHandle properties, const char *property, int count, int *value);

  /** @brief Resets all dimensions of a property to it's default value

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property we are resetting

      returns
        - ::kOfxStatOK
	- ::kOfxStatErrBadHandle
        - ::kOfxStatErrUnknown
   */
  OfxStatus (*propReset)    (OfxPropertySetHandle properties, const char *property);

  /** @brief Gets the dimension of the property

      \arg \e properties is the handle of the thing holding the property
      \arg \e property is the string labelling the property we are resetting
      \arg \e count is a pointer to an integer where the value is returned

    returns
      - ::kOfxStatOK
      - ::kOfxStatErrBadHandle
      - ::kOfxStatErrUnknown
 */
  OfxStatus (*propGetDimension)  (OfxPropertySetHandle properties, const char *property, int *count);
} OfxPropertySuiteV1;

/**
   \addtogroup ErrorCodes 
*/
/*@{*/


/*@}*/


/** @page PropertiesPage OFX : Properties Suite

The objects within OFX have variant properties. These are named values of a defined type which can take different values. For example, the name of the host application being a UTF8 string, the minimum and maximum value of a parameter, whether the host supports animation and so on. The properties API is defined in the ofxProperty.h header.

Properties are manipulated via property handles and the OfxPropertySuiteV1 API. Any thing that holds properties has an associated OfxPropertySet pointer. This property pointer is passed into the various calls in the property host to get and set properties. For example the 'paramDefine' function in the OfxParameterSuiteV1 returns a parameter pointer, from which you can get the property pointer by calling OfxParameterSuiteV1::paramGetPropertySet.

Properties are tagged by C strings, which are #defined in various OFX files. For example, in ofxParam.h
@verbatim
   #define kOfxParamPropAnimates "OfxParamPropAnimates"
@endverbatim
is the tag for the property which controls whether a parameter animates.

Properties can only be one of four types, double precision floating point numbers, integers, null terminated C strings or a void * pointer. There is a function to get and set the property for each of these types.

Properties can also be multi-dimensional, for example the kOfxImageEffectPropProjectSize property is a 2D double precision floating point number. You can either get/set an indvidual dimension within a multi-dimensional property, or set them all, depending on the function used.

Example,

@verbatim
    OfxParamHandle param;
    OfxPropertySetHandle propSet;

    paramHost->paramDefine(pluginDescriptor, kOfxParamTypeRGB, "myColour", &param);
    paramHosy->paramGetPropertySet(param, &propSet);

    // set the default colour to be mid red, set dimensions one at a time
    propHost->propSetDouble(propSet, kOfxParamPropDefault, 0, 0.5);
    propHost->propSetDouble(propSet, kOfxParamPropDefault, 1, 0);
    propHost->propSetDouble(propSet, kOfxParamPropDefault, 2, 0);

    // or, set all dimensions at the same time
    double value[3]; value[0] = 0.5; value[1] = 0.0; value[2] = 0.0;
    propHost->propSetDoubleN(propSet, kOfxParamPropDefault, 3, value);

    // and turn off animation
    propHost->propSetInt(propSet, kOfxParamPropAnimates, 0, 0);

@endverbatim

Strings were chosen to label properties as it is simpler to make them unique and sensible to programmers. If integer constants were used it would be harder to ensure that properties were labelled uniquely without glompfing them in a single huge enum.

The dimension of a property can be accesed with the propGetDimension call.
*/


#ifdef __cplusplus
}
#endif


#endif
