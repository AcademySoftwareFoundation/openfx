/* 
 Copyright (C) 2013 INRIA
 Author Alexandre Gauthier-Foichat alexandre.gauthier-foichat@inria.fr
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.
 
 Neither the name of the {organization} nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 INRIA
 Domaine de Voluceau
 Rocquencourt - B.P. 105
 78153 Le Chesnay Cedex - France
 
 */
#include "ofxImageEffect.h"

#define kOfxColorSpaceConversionSuite "OfxColorSpaceConversionSuite"

/*
 Color-space conversion suite v1.
 It comes along with the openfx-io repository.
 
 This suite is relevant for any host which handles all its pipeline with linear
 colors. It aims to provide a clean and optimised way for a plugin (mainly readers/writers) 
 and a host to handle color-space conversion. We propose this suite in order to avoid
 all readers to be appended with a color-space conversion effect and all writers
 to be prepended with another color-space conversion effect, which would require multiple
 useless image copies back and forth.
 The way it works is quite simple: The plugin states which color-space the host should
 convert and whether it should convert "from" the indicated color-space or "to" the 
 indicated color-space. 
 
 We define a color-space in the v1 as 3 components:
 - a name (e.g: sRGB, Rec709, etc...)
 - a "from" function, taking in argument a float ranging in [0.,1.] and returning a float
 ranging in [0.,1.]. This function should convert from the color-space to linear.
 - a "to" function, which has the exact same signature as the "from" function but 
 converts from linear to the indicated color-space.
 
*/


typedef float (*OfxFromColorSpaceFunctionV1)(float v);
typedef float (*OfxToColorSpaceFunctionV1)(float v);


/** @brief A pointer to the input buffer used for the toColorSpace() and fromColorSpace() 
functions.
   - Type - pointer X 1
   - default value - NULL
*/
#define kOfxInputBufferPointer "OfxInputBufferPointer"

/** @brief A string indicating the bit depth of the input buffer (i.e: byte,short,float)
   - Type - string X 1
   - valid values - It must be one of:
   		- kOfxBitDepthByte
   		- kOfxBitDepthShort
   		- kOfxBitDepthFloat
*/
#define kOfxInputBufferBitDepth "OfxInputBufferBitDepth"

/** @brief The bounds of the input buffer.

    - Type - integer X 4
    
The order of the values is x1, y1, x2, y2.

X values are x1 <= X < x2 
Y values are y1 <= Y < y2
 */
#define kOfxInputBufferBounds "OfxInputBufferBounds"


/** @brief The pixel packing of the input buffer. It is used to inform the
	toColorSpace() and fromColorSpace() function how the pixels are laid out in the
	memory.
	
	- Type - string x1
	- Valid values - It must be one of:
		- kOfxPixelPackingPlanar - The buffer is a single channel (i.e: either R,G,B or A)
		- kOfxPixelPackingRGBA - The pixels are packed together using the following
				sequence : RGBARGBARGBA....
		- kOfxPixelPackingBGRA - The pixels are packed together using the following
				sequence : BGRABGRABGRA....
		- kOfxPixelPackingBGR - The pixels are packed together using the following
				sequence : BGRBGRBGR....
		- kOfxPixelPackingRGB - The pixels are packed together using the following
				sequence : RGBRGBRGB....
	
 */
#define kOfxInputBufferPixelPacking "OfxInputBufferPixelPacking"

/*- The buffer is a single channel (i.e: either R,G,B or A)*/
#define kOfxPixelPackingPlanar "OfxPixelPackingPlanar"

/*- The pixels are packed together using the following sequence : RGBARGBARGBA....*/
#define kOfxPixelPackingRGBA "OfxPixelPackingRGBA"

/*- The pixels are packed together using the following sequence : BGRABGRABGRA....*/
#define kOfxPixelPackingBGRA "OfxPixelPackingBGRA"

/*- The pixels are packed together using the following sequence : BGRBGRBGR....*/
#define kOfxPixelPackingBGR "OfxPixelPackingBGRA"

/*- The pixels are packed together using the following sequence : RGBRGBRGB....*/
#define kOfxPixelPackingRGB "OfxPixelPackingBGRA"

/** @brief A pointer to a planar alpha buffer. This buffer must meet exactly the same
	bounds as kOfxInputBufferPointer and kOfxInputBufferPixelPacking must have the value
	kOfxPixelPackingPlanar. This is used in the toColorSpace() and fromColorSpace() 
	functions when the premult flag is on.
   - Type - pointer X 1
   - default value - NULL
*/
#define kOfxExtraAlphaInputBufferPointer "OfxExtraAlphaInputBufferPointer"

/** @brief A pointer to the output buffer used for the toColorSpace() and fromColorSpace() 
functions.
   - Type - pointer X 1
   - default value - NULL
*/
#define kOfxtOutputBufferPointer "OfxOutputBufferPointer"

/** @brief A string indicating the bit depth of the output buffer (i.e: byte,short,float)
   - Type - string X 1
   - default value - 
   - valid values - It must be one of:
   		- kOfxBitDepthByte
   		- kOfxBitDepthShort
   		- kOfxBitDepthFloat
*/
#define kOfxOutputBufferBitDepth "OfxOutputBufferBitDepth"


/** @brief The bounds of the output buffer.

    - Type - integer X 4
    
The order of the values is x1, y1, x2, y2.

X values are x1 <= X < x2 
Y values are y1 <= Y < y2
 */
#define kOfxOutputBufferBounds "OfxOutputBufferBounds"

/** @brief The pixel packing of the output buffer. It is used to inform the
	toColorSpace() and fromColorSpace() function how the pixels are laid out in the
	memory.
	
	- Type - string x1
	- Valid values - It must be one of:
		- kOfxPixelPackingPlanar - The buffer is a single channel (i.e: either R,G,B or A)
		- kOfxPixelPackingRGBA - The pixels are packed together using the following
				sequence : RGBARGBARGBA....
		- kOfxPixelPackingBGRA - The pixels are packed together using the following
				sequence : BGRABGRABGRA....
		- kOfxPixelPackingBGR - The pixels are packed together using the following
				sequence : BGRBGRBGR....
		- kOfxPixelPackingRGB - The pixels are packed together using the following
				sequence : RGBRGBRGB....
	
 */
#define kOfxOutputBufferPixelPacking "OfxOutputBufferPixelPacking"

/** @brief A pointer to a planar alpha buffer. This buffer must meet exactly the same
	bounds as kOfxOutputBufferPointer and kOfxOutputBufferPixelPacking must have the value
	kOfxPixelPackingPlanar. This is used in the toColorSpace() and fromColorSpace() 
	functions when the premult flag is on.
   - Type - pointer X 1
   - default value - NULL
*/
#define kOfxExtraAlphaOutputBufferPointer "OfxExtraAlphaOutputBufferPointer"

/** @brief The portion of an image we want to convert using the toColorSpace() and 
	fromColorSpace() functions.

    - Type - integer X 4
    
The order of the values is x1, y1, x2, y2.

X values are x1 <= X < x2 
Y values are y1 <= Y < y2
 */
#define kOfxConversionRect "OfxConversionRect"

/** @brief If kOfxOutputBufferPixelPacking is kOfxPixelPackingPlanar it indicates the spacing
  we want to apply between elements when writing to the output buffer.
 
 - Type - integer X 1
 - Default value - 1
 - Valid values - Any number >= 1
 
 */
#define kOfxElementsSpacing "OfxElementsSpacing"

typedef struct OfxColorSpaceConversionSuite { 

  /** @brief Registers a new color-space to the host.

 The host can then use the fromFunc and toFunc for use
 with the color-space conversions functions.


  @returns
    - ::kOfxStatOK - the color-space was successfully registered by the host
    	and the use of all the other functions in the suite is now possible with
    	the registered color-space.
    - ::kOfxStatFailed - The host couldn't register the color-space for some reason.
    	You should therefore prevent from using this color-space to convert your data.
    	The fallback solution is to not apply the color-space conversion, as though
    	the host didn't support the OfxColorSpaceConversionSuite.
*/ 
  OfxStatus (*registerColorSpace)(const char* name,
								  OfxFromColorSpaceFunctionV1 fromFunc,
								  OfxToColorSpaceFunctionV1 toFunc);
			
  /** @brief Converts a buffer from linear to the indicated color-space.
  			This function can be used for all encoders (i.e: writers) to convert
  			conveniently to the color-space of their choice.

	\arg args - A property set containing the following properties...
		- kOfxInputBufferPointer - A pointer to the input buffer, pointing at (0,0) in the image.
		- kOfxInputBufferBitDepth - The bitdepth of the input buffer
		- kOfxInputBufferBounds - The region of definition of the input buffer.
		- kOfxInputBufferPixelPacking - The pixel packing of the input buffer.
		- kOfxExtraAlphaInputBufferPointer - A pointer to the alpha channel in case premult
		is true and the pixel packing is kOfxPixelPackingPlanar.
		
		- kOfxConversionRect - The region we want to convert. This rectangle must be 
			clipped against the kOfxInputBufferBounds and kOfxOutputBufferBounds. 
			The host should take great care of not causing a buffer overflow 
			of the output buffer.
        - kOfxElementsSpacing - The spacing between elements to apply if kOfxInputBufferBitDepth is
            kOfxPixelPackingPlanar and kOfxOutputBufferPixelPacking is not.
			
		- kOfxOutputBufferPointer - A pointer to the output buffer, pointing at (0,0) in the image.
		- kOfxOutputBufferBitDepth - The bitDepth of the output buffer
		- kOfxOutputBufferBounds - The region of definition of the output buffer.
		- kOfxOutputBufferPixelPacking - The pixel packing of the output buffer.
				
	\arg premult - If true, it indicates we want to premultiply by the alpha channel
	the R,G and B channels.
	
	\arg invertY - If true then the output scan-line y of the kOfxOutputBufferPointer
	should be converted with the scan-line (kOfxInputBufferBounds.y2 - y - 1) of the
	kOfxInputBufferPointer.
	
  @returns
    - ::kOfxStatOK - The conversion was done successfully
    - ::kOfxStatFailed - Something went wrong, most probably the host couldn't locate
    					the color-space specified, or the portion to convert didn't intersect
                        the bounds of the input/output buffer.
    - ::kOfxStatErrFatal - The arguments were not consistent (probably the bounds were
    	not clipped correctly or the pixel packing is not valid and it would cause
    	a bad memory access.)
*/ 					  
  OfxStatus (*toColorSpace)(OfxPropertySetHandle args,bool premult,bool invertY);
  
  /** @brief Converts a buffer  the indicated color-space to linear.
  			This function can be used for all decoders (i.e: readers) to convert
  			conveniently from the color-space of their choice.

	\arg args - A property set containing the following properties...
		- kOfxInputBufferPointer - A pointer to the input buffer, pointing at (0,0) in the image.
		- kOfxInputBufferBitDepth - The bitdepth of the input buffer
		- kOfxInputBufferBounds - The region of definition of the input buffer.
		- kOfxInputBufferPixelPacking - The pixel packing of the input buffer.
		- kOfxExtraAlphaInputBufferPointer - A pointer to the alpha channel in case premult
		is true and the pixel packing is kOfxPixelPackingPlanar.
		
		- kOfxConversionRect - The region we want to convert. This rectangle must be 
			clipped against the kOfxInputBufferBounds and kOfxOutputBufferBounds. 
			The host should take great care of not causing a buffer overflow 
			of the output buffer.
        - kOfxElementsSpacing - The spacing between elements to apply if kOfxInputBufferBitDepth is
            kOfxPixelPackingPlanar and kOfxOutputBufferPixelPacking is not.
			
		- kOfxOutputBufferPointer - A pointer to the output buffer, pointing at (0,0) in the image.
		- kOfxOutputBufferBitDepth - The bitDepth of the output buffer
		- kOfxOutputBufferBounds - The region of definition of the output buffer.
		- kOfxOutputBufferPixelPacking - The pixel packing of the output buffer.
				
	\arg premult - If true, it indicates we want to premultiply by the alpha channel
	the R,G and B channels.
	
	\arg invertY - If true then the output scan-line y of the kOfxOutputBufferPointer
	should be converted with the scan-line (kOfxInputBufferBounds.y2 - y - 1) of the
	kOfxInputBufferPointer.
	
  @returns
    - ::kOfxStatOK - The conversion was done successfully
    - ::kOfxStatFailed - Something went wrong, most probably the host couldn't locate
                    the color-space specified, or the portion to convert didn't intersect
                    the bounds of the input/output buffer.
    - ::kOfxStatErrFatal - The arguments were not consistent (probably the bounds were
    	not clipped correctly or the pixel packing is not valid and it would cause
    	a bad memory access.)
*/ 
  OfxStatus (*fromColorSpace)(OfxPropertySetHandle args,bool premult,bool invertY);

} OfxColorSpaceConversionSuiteV1;
