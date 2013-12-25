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

#include "./ofxsSupportPrivate.h"

namespace OFX {
    namespace Color {
        
        void clip(OfxRectI* what,const OfxRectI& to){
            if(what->x1 < to.x1){
                what->x1 = to.x1;
            }
            if(what->x2 > to.x2){
                what->x2 = to.x2;
            }
            if(what->y1 < to.y1){
                what->y1 = to.y1;
            }
            if(what->y2 > to.y2){
                what->y2 = to.y2;
            }
        }
        
        bool intersects(const OfxRectI& what,const OfxRectI& other){
            return (what.x2 >= other.x1 && what.x1 < other.x1 ) ||
                    ( what.x1 < other.x2 && what.x2 >= other.x2) ||
                    ( what.y2 >= other.y1 && what.y1 < other.y1) ||
                    ( what.y1 < other.y2 && what.y2 >= other.y2);
        }
        
        const char* pixelPackingEnumToString(PixelPacking e){
            switch (e) {
                case PACKING_RGBA:
                    return kOfxPixelPackingRGBA;
                    break;
                case PACKING_BGRA:
                    return kOfxPixelPackingBGRA;
                    break;

                case PACKING_RGB:
                    return kOfxPixelPackingRGB;
                    break;

                case PACKING_BGR:
                    return kOfxPixelPackingBGR;
                    break;
                case PACKING_PLANAR:
                    return kOfxPixelPackingPlanar;
                default:
                    assert(false);
                    return NULL;
                    break;
            }
        }
        
        
        void toFunction(void* to,const void* from,const void* alpha,
                  const OfxRectI& srcRoD,const OfxRectI& dstRod,const OfxRectI& conversionRect,
                  const char* inputPacking,const char* outputPacking,
                  const char* fromBitDepth,const char* toBitDepth,bool premult,bool invertY,int delta){
            
            bool isPlanar = !strcmp(inputPacking,kOfxPixelPackingPlanar);
            if(delta < 1 || !to || !from){
                OFX::throwSuiteStatusException(kOfxStatErrFatal);
            }
            if(!conversionRect.intersects(srcRoD) || !conversionRect.intersects(dstRod)){
                OFX::throwSuiteStatusException(kOfxStatFailed);
            }
            
            OfxRectI conversionRectCpy = conversionRect;
            clip(&conversionRectCpy,srcRoD);
            clip(&conversionRectCpy,dstRod);
            
            Property::PropSpec stuff[] = {
                { kOfxInputBufferPointer, Property::ePointer, 1, true, 0 },
                { kOfxInputBufferBitDepth, Property::eString, 1, true, "" },
                { kOfxInputBufferBounds, Property::eInt, 4, true, "" },
                { kOfxInputBufferPixelPacking, Property::eString, 1, true, "" },
                { kOfxExtraAlphaInputBufferPointer, Propety::ePointer, 1 , true, 0 }
                { kOfxConversionRect, Property::eInt, 4, true, "" },
                { kOfxElementsSpacing, Property::eInt,4,true, 1},
                { kOfxOutputBufferPointer, Property::ePointer, 1, true, 0 },
                { kOfxOutputBufferBitDepth, Property::eString, 1, true, "" },
                { kOfxOutputBufferBounds, Property::eInt, 4, true, "" },
                { kOfxOutputBufferPixelPacking, Property::eString, 1, true, "" },
                Property::propSpecEnd
            };
            
            Property::Set args(stuff);
            
            args.setPointerProperty(kOfxInputBufferPointer,from, 0);
            args.setStringProperty(kOfxInputBufferBitDepth,fromBitDepth,0);
            args.setIntPropertyN(kOfxInputBufferBounds,srcRoD, 4);
            args.setStringProperty(kOfxInputBufferPixelPacking,inputPacking,0);
            if(alpha && isPlanar && premult){
                args.setPointerProperty(kOfxExtraAlphaInputBufferPointer,alpha,0);
            }
            
            args.setIntPropertyN(kOfxConversionRect,conversionRectCpy, 4);
            
            args.setIntProperty(kOfxElementsSpacing,delta,0);

            args.setPointerProperty(kOfxOutputBufferPointer,to, 0);
            args.setStringProperty(kOfxOutputBufferBitDepth,toBitDepth,0);
            args.setIntPropertyN(kOfxOutputBufferBounds,dstRod, 4);
            args.setStringProperty(kOfxOutputBufferPixelPacking,outputPacking,0);
            
            OFX::Private::gColorSpaceConversionSuite->toColorSpace(args.getHandle(),premult,invertY);
        }
        
        void to_byte_planar(unsigned char* to, const float* from,int W,const float* alpha = NULL,int delta){
            OfxRectI r;
            r.x1 = 0;
            r.x2 = W;
            r.y1 = 0;
            r.y2 = 0;
            toFunction((void*)to,(const void*)from,(const void*)alpha,
                       r,r,r,
                       kOfxPixelPackingPlanar,kOfxPixelPackingPlanar,
                       kOfxBitDepthFloat,kOfxBitDepthByte,
                       alpha != NULL,false,delta);
        }
        
        void to_short_planar(unsigned short* to, const float* from,int W,const float* alpha = NULL,int delta){
            OfxRectI r;
            r.x1 = 0;
            r.x2 = W;
            r.y1 = 0;
            r.y2 = 0;
            toFunction((void*)to,(const void*)from,(const void*)alpha,
                       r,r,r,
                       kOfxPixelPackingPlanar,kOfxPixelPackingPlanar,
                       kOfxBitDepthFloat,kOfxBitDepthShort,
                       alpha != NULL,false,delta);
        }
        
        void to_float_planar(float* to, const float* from,int W,const float* alpha = NULL,int delta){
            OfxRectI r;
            r.x1 = 0;
            r.x2 = W;
            r.y1 = 0;
            r.y2 = 0;
            toFunction((void*)to,(const void*)from,(const void*)alpha,
                       r,r,r,
                       kOfxPixelPackingPlanar,kOfxPixelPackingPlanar,
                       kOfxBitDepthFloat,kOfxBitDepthFloat,
                       alpha != NULL,false,delta);
        }
        
        void to_byte_packed(unsigned char* to, const float* from,const OfxRectI& conversionRect,
                            const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                            PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult){
            toFunction((void*)to,(const void*)from,(const void*)alpha,
                       srcRoD,dstRod,conversionRect,
                       pixelPackingEnumToString(inputPacking),pixelPackingEnumToString(outputPacking),
                       kOfxBitDepthFloat,kOfxBitDepthByte,
                       premult,invertY,1);

        }
        
        void to_short_packed(unsigned short* to, const float* from,const OfxRectI& conversionRect,
                             const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                             PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult){
            toFunction((void*)to,(const void*)from,(const void*)alpha,
                       srcRoD,dstRod,conversionRect,
                       pixelPackingEnumToString(inputPacking),pixelPackingEnumToString(outputPacking),
                       kOfxBitDepthFloat,kOfxBitDepthShort,
                       premult,invertY,1);
        }
        
        void to_float_packed(float* to, const float* from,const OfxRectI& conversionRect,
                             const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                             PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult){
            toFunction((void*)to,(const void*)from,(const void*)alpha,
                       srcRoD,dstRod,conversionRect,
                       pixelPackingEnumToString(inputPacking),pixelPackingEnumToString(outputPacking),
                       kOfxBitDepthFloat,kOfxBitDepthFloat,
                       premult,invertY,1);
        }
        
        
        void fromFunction(void* to,const void* from,const void* alpha,
                        const OfxRectI& srcRoD,const OfxRectI& dstRod,const OfxRectI& conversionRect,
                        const char* inputPacking,const char* outputPacking,
                        const char* fromBitDepth,const char* toBitDepth,bool premult,bool invertY,int delta){
            
            bool isPlanar = !strcmp(inputPacking,kOfxPixelPackingPlanar);
            if(delta < 1 || !to || !from){
                OFX::throwSuiteStatusException(kOfxStatErrFatal);
            }
            if(!conversionRect.intersects(srcRoD) || !conversionRect.intersects(dstRod)){
                OFX::throwSuiteStatusException(kOfxStatFailed);
            }
            
            OfxRectI conversionRectCpy = conversionRect;
            clip(&conversionRectCpy,srcRoD);
            clip(&conversionRectCpy,dstRod);
            
            Property::PropSpec stuff[] = {
                { kOfxInputBufferPointer, Property::ePointer, 1, true, 0 },
                { kOfxInputBufferBitDepth, Property::eString, 1, true, "" },
                { kOfxInputBufferBounds, Property::eInt, 4, true, "" },
                { kOfxInputBufferPixelPacking, Property::eString, 1, true, "" },
                { kOfxExtraAlphaInputBufferPointer, Propety::ePointer, 1 , true, 0 }
                { kOfxConversionRect, Property::eInt, 4, true, "" },
                { kOfxElementsSpacing, Property::eInt,4,true, 1},
                { kOfxOutputBufferPointer, Property::ePointer, 1, true, 0 },
                { kOfxOutputBufferBitDepth, Property::eString, 1, true, "" },
                { kOfxOutputBufferBounds, Property::eInt, 4, true, "" },
                { kOfxOutputBufferPixelPacking, Property::eString, 1, true, "" },
                Property::propSpecEnd
            };
            
            Property::Set args(stuff);
            
            args.setPointerProperty(kOfxInputBufferPointer,from, 0);
            args.setStringProperty(kOfxInputBufferBitDepth,fromBitDepth,0);
            args.setIntPropertyN(kOfxInputBufferBounds,srcRoD, 4);
            args.setStringProperty(kOfxInputBufferPixelPacking,inputPacking,0);
            if(alpha && isPlanar && premult){
                args.setPointerProperty(kOfxExtraAlphaInputBufferPointer,alpha,0);
            }
            
            args.setIntPropertyN(kOfxConversionRect,conversionRectCpy, 4);
            
            args.setIntProperty(kOfxElementsSpacing,delta,0);
            
            args.setPointerProperty(kOfxOutputBufferPointer,to, 0);
            args.setStringProperty(kOfxOutputBufferBitDepth,toBitDepth,0);
            args.setIntPropertyN(kOfxOutputBufferBounds,dstRod, 4);
            args.setStringProperty(kOfxOutputBufferPixelPacking,outputPacking,0);
            
            OFX::Private::gColorSpaceConversionSuite->toColorSpace(args.getHandle(),premult,invertY);
        }
        
        
        void from_byte_planar(float* to,const unsigned char* from,int W,const unsigned char* alpha = NULL,int delta){
            OfxRectI r;
            r.x1 = 0;
            r.x2 = W;
            r.y1 = 0;
            r.y2 = 0;
            fromFunction((void*)to,(const void*)from,(const void*)alpha,
                         r,r,r,
                         kOfxPixelPackingPlanar,kOfxPixelPackingPlanar,
                         kOfxBitDepthByte,kOfxBitDepthFloat,
                         alpha != NULL,false,delta);

        }
        
        void from_short_planar(float* to,const unsigned short* from,int W,const unsigned short* alpha = NULL,int delta){
            OfxRectI r;
            r.x1 = 0;
            r.x2 = W;
            r.y1 = 0;
            r.y2 = 0;
            fromFunction((void*)to,(const void*)from,(const void*)alpha,
                         r,r,r,
                         kOfxPixelPackingPlanar,kOfxPixelPackingPlanar,
                         kOfxBitDepthShort,kOfxBitDepthFloat,
                         alpha != NULL,false,delta);

        }
        
        void from_float_planar(float* to,const float* from,int W,const float* alpha = NULL,int delta){
            OfxRectI r;
            r.x1 = 0;
            r.x2 = W;
            r.y1 = 0;
            r.y2 = 0;
            fromFunction((void*)to,(const void*)from,(const void*)alpha,
                       r,r,r,
                       kOfxPixelPackingPlanar,kOfxPixelPackingPlanar,
                       kOfxBitDepthFloat,kOfxBitDepthFloat,
                       alpha != NULL,false,delta);
        }
        
        void from_byte_packed(float* to, const unsigned char* from,const OfxRectI& conversionRect,
                              const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                              PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult){
            fromFunction((void*)to,(const void*)from,(const void*)alpha,
                         srcRoD,dstRod,conversionRect,
                         pixelPackingEnumToString(inputPacking),pixelPackingEnumToString(outputPacking),
                         kOfxBitDepthByte,kOfxBitDepthFloat,
                         premult,invertY,1);
        }
        
        void from_short_packed(float* to, const unsigned short* from,const OfxRectI& conversionRect,
                              const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                              PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult){
            fromFunction((void*)to,(const void*)from,(const void*)alpha,
                         srcRoD,dstRod,conversionRect,
                         pixelPackingEnumToString(inputPacking),pixelPackingEnumToString(outputPacking),
                         kOfxBitDepthShort,kOfxBitDepthFloat,
                         premult,invertY,1);
        }
        
        void from_float_packed(float* to, const float* from,const OfxRectI& conversionRect,
                              const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                              PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult){
            fromFunction((void*)to,(const void*)from,(const void*)alpha,
                       srcRoD,dstRod,conversionRect,
                       pixelPackingEnumToString(inputPacking),pixelPackingEnumToString(outputPacking),
                       kOfxBitDepthFloat,kOfxBitDepthFloat,
                       premult,invertY,1);
        }

        void registerColorSpace(const char* name,OfxFromColorSpaceFunctionV1 fromFunc,OfxToColorSpaceFunctionV1 toFunc){
            OFX::Private::gColorSpaceConversionSuite->registerColorSpace(name,fromFunc,toFunc);
        }
    }
}