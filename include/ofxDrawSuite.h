#ifndef _ofxDraw_h_
#define _ofxDraw_h_

#include "ofxCore.h"
#include "ofxPixels.h"

// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause


#ifdef __cplusplus
extern "C" {
#endif

/** @file ofxDrawSuite.h
Contains the API for host-independent drawing. Added for OFX 1.5, Jan 2022.
*/


/** @brief the string that names the DrawSuite, passed to OfxHost::fetchSuite */
#define kOfxDrawSuite "OfxDrawSuite"

/** @brief Blind declaration of an OFX drawing context
 */
typedef struct OfxDrawContext *OfxDrawContextHandle;

/** @brief The Draw Context handle

 - Type - pointer X 1
 - Property Set - read only property on the inArgs of the following actions...
 - ::kOfxInteractActionDraw
 */
#define kOfxInteractPropDrawContext "OfxInteractPropDrawContext"

/** @brief Defines valid values for OfxDrawSuiteV1::getColour */
typedef enum OfxStandardColour
{
	kOfxStandardColourOverlayBackground,
	kOfxStandardColourOverlayActive,
	kOfxStandardColourOverlaySelected,
	kOfxStandardColourOverlayDeselected,
	kOfxStandardColourOverlayMarqueeFG,
	kOfxStandardColourOverlayMarqueeBG,
	kOfxStandardColourOverlayText
} OfxStandardColour;

/** @brief Defines valid values for OfxDrawSuiteV1::setLineStipple */
typedef enum OfxDrawLineStipplePattern
{
	kOfxDrawLineStipplePatternSolid,	// -----
	kOfxDrawLineStipplePatternDot,		// .....
	kOfxDrawLineStipplePatternDash,		// - - -
	kOfxDrawLineStipplePatternAltDash,	//  - - -
	kOfxDrawLineStipplePatternDotDash	// .-.-.-
} OfxDrawLineStipplePattern;

/** @brief Defines valid values for OfxDrawSuiteV1::draw */

typedef enum OfxDrawPrimitive
{
	kOfxDrawPrimitiveLines,
	kOfxDrawPrimitiveLineStrip,
	kOfxDrawPrimitiveLineLoop,
	kOfxDrawPrimitiveRectangle, /* 2 points*/
	kOfxDrawPrimitivePolygon,
	kOfxDrawPrimitiveEllipse
} OfxDrawPrimitive;

/** @brief Defines text alignment values for OfxDrawSuiteV1::drawText */
typedef enum OfxDrawTextAlignment
{
	kOfxDrawTextAlignmentLeft = 0x0001,
	kOfxDrawTextAlignmentRight = 0x0002,
	kOfxDrawTextAlignmentTop = 0x0004,
	kOfxDrawTextAlignmentBottom = 0x008,
	kOfxDrawTextAlignmentBaseline = 0x010,
	kOfxDrawTextAlignmentCenterH = (kOfxDrawTextAlignmentLeft | kOfxDrawTextAlignmentRight),
	kOfxDrawTextAlignmentCenterV = (kOfxDrawTextAlignmentTop | kOfxDrawTextAlignmentBaseline)
} OfxDrawTextAlignment;

/** @brief OFX suite that allows an effect to draw to a host-defined display context.

*/
typedef struct OfxDrawSuiteV1 {
	/** @brief Retrieves the host's desired draw colour for

	 \arg context  - the draw context
	 \arg std_colour - the desired colour type
	 \arg colour      - the returned RGBA colour

	 @returns
	 - ::kOfxStatOK - the colour was returned
	 */
	OfxStatus (*getColour)(OfxDrawContextHandle context, OfxStandardColour std_colour, OfxRGBAColourF *colour);

  /** @brief Sets the current draw colour for future drawing operations

	\arg context  - the draw context
	\arg colour      - the RGBA colour

	@returns
	- ::kOfxStatOK - the colour was changed
   */
  OfxStatus (*setColour)(OfxDrawContextHandle context, const OfxRGBAColourF *colour);

	/** @brief Sets the current draw colour for future drawing operations

	 \arg context  - the draw context
	 \arg width     - the line width - use 0 for a single pixel line or non-zero for a smooth line of the desired width

	 The host should adjust for screen density.

	 @returns
	 - ::kOfxStatOK - the width was changed
	 */
  OfxStatus (*setLineWidth)(OfxDrawContextHandle context, float width);

	/** @brief Sets the current line stipple pattern

	 \arg context  - the draw context
	 \arg pattern  - the desired stipple pattern

	 @returns
	 - ::kOfxStatOK - the pattern was changed
	 */
  OfxStatus (*setLineStipple)(OfxDrawContextHandle context, OfxDrawLineStipplePattern pattern);

	/** @brief Draws a primitive of the desired type

	 \arg context  - the draw context
	 \arg primitive  - the desired primitive
	 \arg points  - the array of points in the primitive
	 \arg point_count  - the number of points in the array

	 The number of points required matches the equivalent OpenGL primitives.
	 For an ellipse, point_count should be 2, and points should form the top-left and bottom-right of a rectangle to draw the ellipse inside

	 @returns
	 - ::kOfxStatOK - the pattern was changed
	 - ::kOfxStatErrValue - point_count was not valid
	 */
  OfxStatus (*draw)(OfxDrawContextHandle context, OfxDrawPrimitive primitive, const OfxPointD *points, int point_count);


	/** @brief Draws text at the specified position in the current font size

	 \arg context  - the draw context
	 \arg text  - the text to draw (UTF-8 encoded)
	 \arg pos  - the position of the lower-left corner of the text baseline
	 \arg alignment  - the text alignment flags (see kOfxDrawTextAlignment*)

	 The text font face and size are determined by the host.

	 @returns
	 - ::kOfxStatOK - the text was drawn
	 - ::kOfxStatErrValue - text or pos were not defined
	 */
  OfxStatus (*drawText)(OfxDrawContextHandle context, const char *text, const OfxPointD *pos, int alignment);

} OfxDrawSuiteV1;

#ifdef __cplusplus
}
#endif

#endif
