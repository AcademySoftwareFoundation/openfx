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

#ifndef Support_ofxsColorSpace_h
#define Support_ofxsColorSpace_h

#include <cmath>
#include "ofxsImageEffect.h"

#define kOfxLutNone "OfxLutNone"
#define kOfxLutSRGB "OfxLutSRGB"
#define kOfxLutRec709 "OfxLutRec709"
#define kOfxLutCineon "OfxLutCineon"
#define kOfxLutGamma1_8 "OfxLutGamma1_8"
#define kOfxLutGamma2_2 "OfxLutGamma2_2"
#define kOfxLutPanalog "OfxLutPanalog"
#define kOfxLutViperlog "OfxLutViperlog"
#define kOfxLutRedlog "OfxLutRedlog"
#define kOfxLutAlexaV3LogC "OfxLutAlexaV3LogC"

namespace OFX {
    namespace Color {
        
        
        enum PixelPacking {
            PACKING_RGBA = 0,
            PACKING_BGRA,
            PACKING_RGB,
            PACKING_BGR,
            PACKING_PLANAR
        };
        
        
        /////the following functions expects a float input buffer, one could extend it to cover all bitdepths.
        
        /**
         * @brief Convert an array of linear floating point pixel values to an
         * array of destination lut values, with error diffusion to avoid posterizing
         * artifacts.
         *
         * \a W is the number of pixels to convert.  \a delta is the distance
         * between the output bytes (useful for interlacing them).
         * The input and output buffers must not overlap in memory.
         **/
        void to_byte_planar(const char* colorspaceName,unsigned char* to, const float* from,int W,const float* alpha = NULL,int delta = -1);
        void to_short_planar(const char* colorspaceName,unsigned short* to, const float* from,int W,const float* alpha = NULL,int delta = -1);
        void to_float_planar(const char* colorspaceName,float* to, const float* from,int W,const float* alpha = NULL,int delta = -1);
        
        
        /**
         * @brief These functions work exactly like the to_X_planar functions but expect 2 buffers
         * pointing at (0,0) in the image and convert a rectangle of the image. It also supports
         * several pixel packing commonly used by openfx images.
         **/
        void to_byte_packed(const char* colorspaceName,unsigned char* to, const float* from,const OfxRectI& conversionRect,
                            const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                            PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult);
        void to_short_packed(const char* colorspaceName,unsigned short* to, const float* from,const OfxRectI& conversionRect,
                            const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                            PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult);
        void to_float_packed(const char* colorspaceName,float* to, const float* from,const OfxRectI& conversionRect,
                            const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                            PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult);
        
        
        
        /////the following functions expects a float output buffer, one could extend it to cover all bitdepths.
        
        /**
         * @brief Convert from a buffer in the input color-space to the output color-space.
         *
         * \a W is the number of pixels to convert.  \a delta is the distance
         * between the output bytes (useful for interlacing them).
         * The input and output buffers must not overlap in memory.
         **/

        void from_byte_planar(const char* colorspaceName,float* to,const unsigned char* from,
                              int W,const unsigned char* alpha = NULL,int delta = -1);
        void from_short_planar(const char* colorspaceName,float* to,const unsigned short* from,
                               int W,const unsigned short* alpha = NULL,int delta = -1);
        void from_float_planar(const char* colorspaceName,float* to,const float* from,
                               int W,const float* alpha = NULL,int delta = -1);
        
        
        /**
         * @brief These functions work exactly like the from_X_planar functions but expect 2 buffers
         * pointing at (0,0) in the image and convert a rectangle of the image. It also supports
         * several pixel packing commonly used by openfx images.
         **/
        void from_byte_packed(const char* colorspaceName,float* to, const unsigned char* from,const OfxRectI& conversionRect,
                            const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                            PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult);
        
        void from_byte_packed(const char* colorspaceName,float* to, const unsigned short* from,const OfxRectI& conversionRect,
                              const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                              PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult);
        
        void from_byte_packed(const char* colorspaceName,float* to, const float* from,const OfxRectI& conversionRect,
                              const OfxRectI& srcRoD,const OfxRectI& dstRoD,
                              PixelPacking inputPacking,PixelPacking outputPacking,bool invertY,bool premult);
        
        
        ///utility functions
        float to_float_linear(unsigned char v) { return (float)v / 255.f; }
        float to_float_linear(unsigned short v) { return (float)v / 65535.f; }
        float to_float_linear(float v) { return v; }
        
        unsigned char from_float_linear(float v) { return (unsigned char)(v * 255.f); }
        unsigned short from_float_linear(float v) { return (unsigned short)(v * 65535.f); }
        float from_float_linear(float v) { return v; }
        
        
        
        ///buit-ins color-spaces
        
        ///kOfxLutSRGB
        float from_func_srgb(float v){
            if (v < 0.04045f)
                return (v < 0.0f) ? 0.0f : v * (1.0f / 12.92f);
            else
                return std::pow((v + 0.055f) * (1.0f / 1.055f), 2.4f);
        }
        
        float to_func_srgb(float v){
            if (v < 0.0031308f)
                return (v < 0.0f) ? 0.0f : v * 12.92f;
            else
                return 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
        }
        
        ///kOfxLutRec709
        float from_func_Rec709(float v){
            if (v < 0.081f)
                return (v < 0.0f) ? 0.0f : v * (1.0f / 4.5f);
            else
                return std::pow((v + 0.099f) * (1.0f / 1.099f), (1.0f / 0.45f));
        }
        
        float to_func_Rec709(float v){
            if (v < 0.018f)
                return (v < 0.0f) ? 0.0f : v * 4.5f;
            else
                return 1.099f * std::pow(v, 0.45f) - 0.099f;
        }
        
        ///kOfxLutCineon
        /*
         Following the formula:
            offset = pow(10,(blackpoint - whitepoint) * 0.002 / gammaSensito)
            gain = 1/(1-offset)
            linear = gain * pow(10,(1023*v - whitepoint)*0.002/gammaSensito)
            cineon = (log10((v + offset) /gain)/ (0.002 / gammaSensito) + whitepoint)/1023
            Here we're using: blackpoint = 95.0
                              whitepoint = 685.0
                              gammasensito = 0.6
         */
        float from_func_Cineon(float v){
            return (1.f / (1.f - std::pow(10.f,1.97f))) * std::pow(10.f,((1023.f * v) - 685.f) * 0.002f / 0.6f);
        }
        
        float to_func_Cineon(float v){
            float offset = std::pow(10.f,1.97f);
            return (std::log10((v + offset) / (1.f / (1.f - offset))) / 0.0033f + 685.0f) / 1023.f;
        }
        ///kOfxLutGamma1_8
        float from_func_Gamma1_8(float v){
            v = powf(v, 0.55);
        }
        
        float to_func_Gamma1_8(float v){
            v = powf(v, 1.8);
        }
        ///kOfxLutGamma2_2
        float from_func_Gamma2_2(float v){
            v = powf(v, 0.45);
        }
        
        float to_func_Gamma2_2(float v){
            v = powf(v, 2.2);
        }
        ///kOfxLutPanalog
        float from_func_Panalog(float v){
            return (std::pow(10.f,(1023.f * v - 681.f) / 444.f) - 0.0408) / 0.96f;
        }
        
        float to_func_Panalog(float v){
            return (444.f * std::log10(0.0408 + 0.96f * v) + 681.f) / 1023.f;
        }
        ///kOfxLutViperlog
        float from_func_ViperLog(float v){
            return std::pow(10.f,(1023.f * v - 1023.f) / 500.f);
        }
        
        float to_func_ViperLog(float v){
            return (500.f * std::log10(v) + 1023.f) / 1023.f;
        }
        ///kOfxLutRedlog
        float from_func_RedLog(float v){
            return (std::pow(10.f,( 1023.f * v - 1023.f ) / 511.f) - 0.01f) / 0.99f;
        }
        
        float to_func_RedLog(float v){
            return (511.f * std::log10(0.01f + 0.99f * v) + 1023.f) / 1023.f;
        }
        
        ///kOfxLutAlexaV3LogC
        float from_func_AlexaV3LogC(float v){
            return v > 0.1496582f ? std::pow(10.f,(v - 0.385537f) / 0.2471896f) * 0.18f - 0.00937677f
            : ( v / 0.9661776f - 0.04378604) * 0.18f - 0.00937677f;
        }
        
        float to_func_AlexaV3LogC(float v){
            return v > 0.010591f ?  0.247190f * std::log10(5.555556f * v + 0.052272f) + 0.385537f
            : v * 5.367655f + 0.092809f;
        }
        
        
        
        /**
         * @brief Call this to register a new color-space to the host. You must register the color-space you'll be
         * using with all the to_ and from_ functions for this color-space.
         **/
        void registerColorSpace(const char* name,OfxFromColorSpaceFunctionV1 fromFunc,OfxToColorSpaceFunctionV1 toFunc);
        
        void registerSRGBColorSpace(){
            registerColorSpace(kOfxLutSRGB,from_func_srgb,to_func_srgb);
        }
        
        void registerRec709ColorSpace(){
            registerColorSpace(kOfxLutRec709,from_func_Rec709,to_func_Rec709);
        }
        
        void registerCineonColorSpace(){
            registerColorSpace(kOfxLutCineon,from_func_Cineon,to_func_Cineon);
        }
        
        void registerGamma1_8ColorSpace(){
            registerColorSpace(kOfxLutGamma1_8,from_func_Gamma1_8,to_func_Gamma1_8);
        }
        
        void registerGamma2_2ColorSpace(){
            registerColorSpace(kOfxLutGamma2_2,from_func_Gamma2_2,to_func_Gamma2_2);
        }
        
        void registerPanalogColorSpace(){
            registerColorSpace(kOfxLutPanalog,from_func_Panalog,to_func_Panalog);
        }
        
        void registerViperlogColorSpace(){
            registerColorSpace(kOfxLutViperlog,from_func_ViperLog,to_func_ViperLog);
        }
        
        void registerRedlogColorSpace(){
            registerColorSpace(kOfxLutRedlog,from_func_RedLog,to_func_RedLog);
        }
        
        void registerAlexaV3LogCColorSpace(){
            registerColorSpace(kOfxLutAlexaV3LogC,from_func_AlexaV3LogC,to_func_AlexaV3LogC);
        }
    }
}



#endif
