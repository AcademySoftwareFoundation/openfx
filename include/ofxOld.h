// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _ofxOLD_h_
#define _ofxOLD_h_

 
/** @brief String to label images with YUVA components
--ofxImageEffects.h
@deprecated - removed in v1.4. Note, this has been deprecated in v1.3

 */
#define kOfxImageComponentYUVA "OfxImageComponentYUVA"

/** @brief Indicates whether an effect is performing an analysis pass.
--ofxImageEffects.h
@deprecated - This feature has been deprecated - officially commented out v1.4.
    
    @propdef
    type: bool
    dimension: 1
    deprecated: "1.4"
*/
#define kOfxImageEffectPropInAnalysis "OfxImageEffectPropInAnalysis"


/** @brief Defines an 8 bit per component YUVA pixel 
-- ofxPixels.h
Deprecated in 1.3, removed in 1.4
*/

typedef struct OfxYUVAColourB {
  unsigned char y, u, v, a;
}OfxYUVAColourB;


/** @brief Defines an 16 bit per component YUVA pixel 
-- ofxPixels.h
@deprecated -  Deprecated in 1.3, removed in 1.4
*/

typedef struct OfxYUVAColourS {
  unsigned short y, u, v, a;
}OfxYUVAColourS;

/** @brief Defines an floating point component YUVA pixel
-- ofxPixels.h
@deprecated -  Deprecated in 1.3, removed in 1.4
 */
typedef struct OfxYUVAColourF {
  float y, u, v, a;
}OfxYUVAColourF;


/** @brief The size of an interact's openGL viewport
-- ofxInteract.h
@deprecated - V1.3: This property is the redundant and its use will be deprecated in future releases.
V1.4: Removed
    
    @propdef
    type: int
    dimension: 2
    cname: kOfxInteractPropViewportSize
    deprecated: "1.3"
 */
#define kOfxInteractPropViewportSize "OfxInteractPropViewport"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating a size normalised to the X dimension. See \ref ::kOfxParamPropDoubleType.
-- ofxParam.h
@deprecated - V1.3: Deprecated in favour of ::OfxParamDoubleTypeX
V1.4: Removed
 */
#define kOfxParamDoubleTypeNormalisedX  "OfxParamDoubleTypeNormalisedX"


/** @brief value for the ::kOfxParamPropDoubleType property, indicating a size normalised to the Y dimension. See \ref ::kOfxParamPropDoubleType.
-- ofxParam.h
@deprecated - V1.3: Deprecated in favour of ::OfxParamDoubleTypeY
V1.4: Removed
 */
#define kOfxParamDoubleTypeNormalisedY  "OfxParamDoubleTypeNormalisedY"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating an absolute position normalised to the X dimension. See \ref ::kOfxParamPropDoubleType. 
-- ofxParam.h
@deprecated - V1.3: Deprecated in favour of ::OfxParamDoubleTypeXAbsolute
V1.4: Removed
 */
#define kOfxParamDoubleTypeNormalisedXAbsolute  "OfxParamDoubleTypeNormalisedXAbsolute"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating an absolute position  normalised to the Y dimension. See \ref ::kOfxParamPropDoubleType.
-- ofxParam.h
@deprecated - V1.3: Deprecated in favour of ::OfxParamDoubleTypeYAbsolute
V1.4: Removed
 */
#define kOfxParamDoubleTypeNormalisedYAbsolute  "OfxParamDoubleTypeNormalisedYAbsolute"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating normalisation to the X and Y dimension for 2D params. See \ref ::kOfxParamPropDoubleType. 
-- ofxParam.h
@deprecated - V1.3: Deprecated in favour of ::OfxParamDoubleTypeXY
V1.4: Removed
 */
#define kOfxParamDoubleTypeNormalisedXY  "OfxParamDoubleTypeNormalisedXY"

/** @brief value for the ::kOfxParamPropDoubleType property, indicating normalisation to the X and Y dimension for a 2D param that can be interpreted as an absolute spatial position. See \ref ::kOfxParamPropDoubleType. 
-- ofxParam.h
@deprecated - V1.3: Deprecated in favour of ::kOfxParamDoubleTypeXYAbsolute 
V1.4: Removed
 */
#define kOfxParamDoubleTypeNormalisedXYAbsolute  "OfxParamDoubleTypeNormalisedXYAbsolute"



#endif
