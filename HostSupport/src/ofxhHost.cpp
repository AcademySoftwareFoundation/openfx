/*
Software License :

Copyright (c) 2007, The Foundry Visionmongers Ltd. All rights reserved.

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

#include <limits.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhImageEffect.h"
#include "ofxhPluginAPICache.h"
#include "ofxhPluginCache.h"
#include "ofxhHost.h"
#include "ofxhImageEffectAPI.h"
#include "ofxhXml.h"
#include "ofxhInteract.h"

typedef OfxPlugin* (*OfxGetPluginType)(int);

namespace OFX {
  
  namespace Host {

    namespace ImageEffect {

      OfxStatus getPropertySet(OfxImageEffectHandle h1, 
                               OfxPropertySetHandle *h2)
      {        
        Base *effectBase = reinterpret_cast<Base*>(h1);
        
        if(effectBase) {
          *h2 = effectBase->getProps().getHandle();
          printf("ImageEffect::getPropertySet returning %p\n", *h2);
          printf("effectBase is %p, %s\n", h1, typeid(effectBase).name());
        }
        else {
          printf("ImageEffect::getPropertySet failed\n", *h2);
          return kOfxStatErrBadHandle;
        }

        return kOfxStatOK;
      }

      OfxStatus getParamSet(OfxImageEffectHandle h1, 
                            OfxParamSetHandle *h2)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(h1);

        ImageEffect::Descriptor *effectDescriptor = dynamic_cast<ImageEffect::Descriptor*>(effectBase);

        if(effectDescriptor) 
          *h2 = effectDescriptor->getParams().getHandle();
        else{
          ImageEffect::Instance *effectInstance = dynamic_cast<ImageEffect::Instance*>(effectBase);

          if(effectInstance) 
            *h2 = effectInstance->getParams().getHandle();
          else
            return kOfxStatErrBadHandle;
        }          

        return kOfxStatOK;
      }
      
      OfxStatus clipDefine(OfxImageEffectHandle h1, 
                           const char *name, 
                           OfxPropertySetHandle *h2)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(h1);

        ImageEffect::Descriptor *effectDescriptor = dynamic_cast<ImageEffect::Descriptor*>(effectBase);
        
        if(effectDescriptor){ 
          Clip::Descriptor *clip = effectDescriptor->defineClip(name);
          *h2 = clip->getPropHandle();        
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle;
      }
      
      OfxStatus clipGetPropertySet(OfxImageClipHandle clip,
                                   OfxPropertySetHandle *propHandle){        
        Clip::Instance *clipInstance = reinterpret_cast<Clip::Instance*>(clip);

        if(clipInstance) {
          *propHandle = clipInstance->getPropHandle();
          return kOfxStatOK;
        }
        
        return kOfxStatErrBadHandle;
      }
      
      OfxStatus clipGetImage(OfxImageClipHandle h1, 
                             OfxTime time, 
                             OfxRectD *h2, 
                             OfxPropertySetHandle *h3){
        Clip::Instance *clipInstance = reinterpret_cast<Clip::Instance*>(h1);

        if(clipInstance){
          Clip::Image* image;
          
          OfxStatus st = clipInstance->getImage(time,h2,image);
          if(st!=kOfxStatOK) return st;

          if(!image) return kOfxStatFailed;

          *h3 = (OfxPropertySetHandle)image->getHandle();
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrBadHandle;
      }
      
      OfxStatus clipReleaseImage(OfxPropertySetHandle h1){
        Clip::Image *image = reinterpret_cast<Clip::Image*>(h1);

        if(h1){
          // clip::image has a virtual destructor for derived classes
          delete image;
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrBadHandle;
      }
      
      OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect,
                              const char *name,
                              OfxImageClipHandle *clip,
                              OfxPropertySetHandle *propertySet)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(imageEffect);

        ImageEffect::Instance *effectInstance = reinterpret_cast<ImageEffect::Instance*>(effectBase);

        if(effectInstance){
          Clip::Instance* instance = effectInstance->getClip(name);
          *clip = instance->getHandle();
          if(propertySet)
            *propertySet = instance->getPropHandle();
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrBadHandle;
      }
      
      OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip,
                                          OfxTime time,
                                          OfxRectD *bounds)
      {
        Clip::Instance *clipInstance = reinterpret_cast<Clip::Instance*>(clip);

        if(clipInstance) {
          return clipInstance->getRegionOfDefinition(*bounds);
        }
        else 
          return kOfxStatErrBadHandle;
      }

        // should processing be aborted?
      int abort(OfxImageEffectHandle imageEffect)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(imageEffect);

        ImageEffect::Instance *effectInstance = dynamic_cast<ImageEffect::Instance*>(effectBase);

        if(effectInstance) 
          return effectInstance->abort();
        else 
          return kOfxStatErrBadHandle;        
      }
      
      OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
                                 size_t nBytes,
                                 OfxImageMemoryHandle *memoryHandle)
      {
        ImageEffect::Base *effectBase = reinterpret_cast<ImageEffect::Base*>(instanceHandle);

        ImageEffect::Instance *effectInstance = reinterpret_cast<ImageEffect::Instance*>(effectBase);

        if(effectInstance){
          Memory::Instance* memory = effectInstance->imageMemoryAlloc(nBytes);
          *memoryHandle = memory->getHandle();
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle; 
      }
      
      OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle){
        Memory::Instance *memoryInstance = reinterpret_cast<Memory::Instance*>(memoryHandle);

        if(memoryInstance){
          memoryInstance->free();
          delete memoryInstance;
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrBadHandle; 
      }
      
      OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                                void **returnedPtr){
        Memory::Instance *memoryInstance = reinterpret_cast<Memory::Instance*>(memoryHandle);

        if(memoryInstance){
          memoryInstance->lock();          
          *returnedPtr = memoryInstance->getPtr();
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle; 
      }
      
      OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle){
        Memory::Instance *memoryInstance = reinterpret_cast<Memory::Instance*>(memoryHandle);

        if(memoryInstance){
          memoryInstance->unlock();          
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle; 
      }

      struct OfxImageEffectSuiteV1 suite = {
        getPropertySet,
        getParamSet,
        clipDefine,
        clipGetHandle,
        clipGetPropertySet,
        clipGetImage,
        clipReleaseImage,
        clipGetRegionOfDefinition,
        abort,
        imageMemoryAlloc,
        imageMemoryFree,
        imageMemoryLock,
        imageMemoryUnlock
      };

    }
    
    namespace Param {

      OfxStatus paramDefine(OfxParamSetHandle paramSet,
                            const char *paramType,
                            const char *name,
                            OfxPropertySetHandle *propertySet)
      {
        SetDescriptor *paramSetDescriptor = reinterpret_cast<SetDescriptor*>(paramSet);
        if(paramSetDescriptor){
          Descriptor *param = new Descriptor(paramType, name);
          paramSetDescriptor->addParam(name,param);
          *propertySet = param->getPropHandle();
          return kOfxStatOK;
        }
        else
          return kOfxStatErrBadHandle;
      }
      
      OfxStatus paramGetHandle(OfxParamSetHandle paramSet,
                               const char *name,
                               OfxParamHandle *param,
                               OfxPropertySetHandle *propertySet)
      {

        BaseSet *baseSet = reinterpret_cast<BaseSet*>(paramSet);

        SetInstance *setInstance = dynamic_cast<SetInstance*>(baseSet);

        if(setInstance){          
          std::map<std::string,Instance*>& params = setInstance->getParams();
          std::map<std::string,Instance*>::iterator it = params.find(name);         

          // if we can't find it return an error...
          if(it==params.end()) return kOfxStatErrUnknown;

          // get the param
          *param = (it->second)->getHandle(); 

          // get the param property set
          *propertySet = (it->second)->getPropHandle();

          return kOfxStatOK;
        }
        else{
          SetDescriptor *setDescriptor = dynamic_cast<SetDescriptor*>(baseSet);
          
          if(setDescriptor){            
            std::map<std::string,Descriptor*>& params = setDescriptor->getParams();
            std::map<std::string,Descriptor*>::iterator it = params.find(name);         

            // if we can't find it return an error...
            if(it==params.end()) return kOfxStatErrUnknown;

            // get the param
            *param = (it->second)->getHandle();  

            // get the param property set
            *propertySet = (it->second)->getPropHandle();

            return kOfxStatOK;
          }
          else
            return kOfxStatErrBadHandle;        
        }          
      }
      
      OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet,
                                       OfxPropertySetHandle *propHandle)
      {
       /* BaseSet *baseSet = reinterpret_cast<BaseSet*>(paramSet);

        SetInstance *setInstance = dynamic_cast<SetInstance*>(baseSet);

        if(setInstance){            
          // get the param property set
          *propHandle = setInstance->getPropHandle();

          return kOfxStatOK;
        }
        else{
          SetDescriptor *paramSetDescriptor = dynamic_cast<SetDescriptor*>(baseSet);
      
          if(paramSetDescriptor){ 
            // get the param property set
            *propHandle = paramSetDescriptor->getPropHandle();            
          }
          else
            return kOfxStatErrBadHandle;                  

          return kOfxStatOK;
        } TODO */
        return kOfxStatOK;
      } 
      
      OfxStatus paramGetPropertySet(OfxParamHandle param,
                                    OfxPropertySetHandle *propHandle)
      {
        Param::Instance *paramInstance = reinterpret_cast<Param::Instance*>(param);
        
        if(paramInstance){
          // get the param property set
          *propHandle = paramInstance->getPropHandle();

          return kOfxStatOK;
        }
        else
          return kOfxStatErrBadHandle;        
      }

#define mDeclareTypedInstance(T) \
T* typedParamInstance = dynamic_cast<T*>(paramInstance);

      OfxStatus paramGetValue(OfxParamHandle  paramHandle,
                              ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,paramHandle);
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(IntegerInstance);
          return typedParamInstance->get(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,paramHandle);
          double *value = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          return typedParamInstance->get(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          va_list ap;

          va_start(ap,paramHandle);
          bool *value = va_arg(ap, bool*);
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          return typedParamInstance->get(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          va_list ap;

          va_start(ap,paramHandle);
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(ChoiceInstance);
          return typedParamInstance->get(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,paramHandle);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          double *a = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          return typedParamInstance->get(*r,*g,*b,*a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,paramHandle);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          return typedParamInstance->get(*r,*g,*b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,paramHandle);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          return typedParamInstance->get(*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,paramHandle);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          return typedParamInstance->get(*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,paramHandle);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          double *z = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          return typedParamInstance->get(*x,*y,*z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,paramHandle);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          int *z = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          return typedParamInstance->get(*x,*y,*z);
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          va_list ap;

          va_start(ap,paramHandle);
          char *value = va_arg(ap, char*);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          return typedParamInstance->get(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeCustom){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypeGroup){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePage){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePushButton){
          return kOfxStatErrMissingHostFeature;
        }
        else
          return kOfxStatErrUnsupported;
      }      
      
      OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle,
                                    OfxTime time,
                                    ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;

        std::string type = paramInstance->getType();

        if(type==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,time);
          int *value = va_arg(ap, int*);
          va_end(ap);
        
          mDeclareTypedInstance(IntegerInstance);
          return typedParamInstance->get(time,*value);
        }
        else if(type==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,time);
          double *value = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          return typedParamInstance->get(time,*value);
        }
        else if(type==kOfxParamTypeBoolean){
          va_list ap;

          va_start(ap,time);
          bool *value = va_arg(ap, bool*);
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          return typedParamInstance->get(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          va_list ap;

          va_start(ap,time);
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(ChoiceInstance);
          return typedParamInstance->get(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,time);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          double *a = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          return typedParamInstance->get(time,*r,*g,*b,*a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,time);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          return typedParamInstance->get(time,*r,*g,*b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,time);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          return typedParamInstance->get(time,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,time);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          return typedParamInstance->get(time,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,time);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          double *z = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          return typedParamInstance->get(time,*x,*y,*z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,time);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          int *z = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          return typedParamInstance->get(time,*x,*y,*z);
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          va_list ap;

          va_start(ap,time);
          char *value = va_arg(ap, char*);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          return typedParamInstance->get(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeCustom){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypeGroup){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePage){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePushButton){
          return kOfxStatErrMissingHostFeature;
        }
        else
          return kOfxStatErrUnsupported;
      }
      
      OfxStatus paramGetDerivative(OfxParamHandle  paramHandle,
                                   OfxTime time,
                                   ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,time);
          int *value = va_arg(ap, int*);
          va_end(ap);
        
          mDeclareTypedInstance(IntegerInstance);
          return typedParamInstance->derive(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,time);
          double *value = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          return typedParamInstance->derive(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,time);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          double *a = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          return typedParamInstance->derive(time,*r,*g,*b,*a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,time);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          return typedParamInstance->derive(time,*r,*g,*b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,time);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          return typedParamInstance->derive(time,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,time);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          return typedParamInstance->derive(time,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,time);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          double *z = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          return typedParamInstance->derive(time,*x,*y,*z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,time);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          int *z = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          return typedParamInstance->derive(time,*x,*y,*z);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          return kOfxStatErrMissingHostFeature;        
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          return kOfxStatErrMissingHostFeature;        
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          return kOfxStatErrMissingHostFeature;        
        }
        else if(paramInstance->getType()==kOfxParamTypeCustom){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypeGroup){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePage){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePushButton){
          return kOfxStatErrMissingHostFeature;
        }
        else
          return kOfxStatErrUnsupported;
      }
      
      OfxStatus paramGetIntegral(OfxParamHandle  paramHandle,
                                 OfxTime time1, OfxTime time2,
                                 ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,time2);
          int *value = va_arg(ap, int*);
          va_end(ap);
        
          mDeclareTypedInstance(IntegerInstance);
          return typedParamInstance->integrate(time1,time2,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,time2);
          double *value = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          return typedParamInstance->integrate(time1,time2,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,time2);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          double *a = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          return typedParamInstance->integrate(time1,time2,*r,*g,*b,*a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,time2);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          return typedParamInstance->integrate(time1,time2,*r,*g,*b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,time2);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          return typedParamInstance->integrate(time1,time2,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,time2);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          return typedParamInstance->integrate(time1,time2,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,time2);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          double *z = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          return typedParamInstance->integrate(time1,time2,*x,*y,*z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,time2);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          int *z = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          return typedParamInstance->integrate(time1,time2,*x,*y,*z);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          return kOfxStatErrMissingHostFeature;        
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          return kOfxStatErrMissingHostFeature;        
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          return kOfxStatErrMissingHostFeature;           
        }
        else if(paramInstance->getType()==kOfxParamTypeCustom){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypeGroup){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePage){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePushButton){
          return kOfxStatErrMissingHostFeature;
        }
        else
          return kOfxStatErrUnsupported;
      }
      
      OfxStatus paramSetValue(OfxParamHandle  paramHandle,
                              ...) 
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,paramHandle);
          int *value = va_arg(ap, int*);
          va_end(ap);
        
          mDeclareTypedInstance(IntegerInstance);
          return typedParamInstance->set(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,paramHandle);
          double *value = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          return typedParamInstance->set(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          va_list ap;

          va_start(ap,paramHandle);
          bool *value = va_arg(ap, bool*);
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          return typedParamInstance->set(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          va_list ap;

          va_start(ap,paramHandle);
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(ChoiceInstance);
          return typedParamInstance->set(*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,paramHandle);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          double *a = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          return typedParamInstance->set(*r,*g,*b,*a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,paramHandle);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          return typedParamInstance->set(*r,*g,*b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,paramHandle);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          return typedParamInstance->set(*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,paramHandle);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          return typedParamInstance->set(*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,paramHandle);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          double *z = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          return typedParamInstance->set(*x,*y,*z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,paramHandle);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          int *z = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          return typedParamInstance->set(*x,*y,*z);
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          va_list ap;

          va_start(ap,paramHandle);
          char *value = va_arg(ap, char*);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          return typedParamInstance->set(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeCustom){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypeGroup){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePage){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePushButton){
          return kOfxStatErrMissingHostFeature;
        }
        else
          return kOfxStatErrUnsupported;
      }
      
      OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle,
                                    OfxTime time,  // time in frames
                                    ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,time);
          int *value = va_arg(ap, int*);
          va_end(ap);
        
          mDeclareTypedInstance(IntegerInstance);
          return typedParamInstance->set(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,paramHandle);
          double *value = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          return typedParamInstance->set(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          va_list ap;

          va_start(ap,paramHandle);
          bool *value = va_arg(ap, bool*);
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          return typedParamInstance->set(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          va_list ap;

          va_start(ap,paramHandle);
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(ChoiceInstance);
          return typedParamInstance->set(time,*value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,paramHandle);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          double *a = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          return typedParamInstance->set(time,*r,*g,*b,*a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,paramHandle);
          double *r = va_arg(ap, double*);
          double *g = va_arg(ap, double*);
          double *b = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          return typedParamInstance->set(time,*r,*g,*b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,paramHandle);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          return typedParamInstance->set(time,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,paramHandle);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          return typedParamInstance->set(time,*x,*y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,paramHandle);
          double *x = va_arg(ap, double*);
          double *y = va_arg(ap, double*);
          double *z = va_arg(ap, double*);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          return typedParamInstance->set(time,*x,*y,*z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,paramHandle);
          int *x = va_arg(ap, int*);
          int *y = va_arg(ap, int*);
          int *z = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          return typedParamInstance->set(time,*x,*y,*z);
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          va_list ap;

          va_start(ap,paramHandle);
          char *value = va_arg(ap, char*);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          return typedParamInstance->set(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeCustom){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypeGroup){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePage){
          return kOfxStatErrMissingHostFeature;
        }
        else if(paramInstance->getType()==kOfxParamTypePushButton){
          return kOfxStatErrMissingHostFeature;
        }
        else
          return kOfxStatErrUnsupported;
      }
      
      OfxStatus paramGetNumKeys(OfxParamHandle  paramHandle,
                                unsigned int  *numberOfKeys)
      {
        KeyframeParam *paramInstance = reinterpret_cast<KeyframeParam*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->getNumKeys(*numberOfKeys);
      }
      
      OfxStatus paramGetKeyTime(OfxParamHandle  paramHandle,
                                unsigned int nthKey,
                                OfxTime *time)
      {
        KeyframeParam *paramInstance = reinterpret_cast<KeyframeParam*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->getKeyTime(nthKey,*time);
      }
      
      OfxStatus paramGetKeyIndex(OfxParamHandle  paramHandle,
                                 OfxTime time,
                                 int     direction,
                                 int    *index) 
      {
        KeyframeParam *paramInstance = reinterpret_cast<KeyframeParam*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->getKeyIndex(time,direction,*index);
      }
      
      OfxStatus paramDeleteKey(OfxParamHandle  paramHandle,
                               OfxTime time)
      {
        KeyframeParam *paramInstance = reinterpret_cast<KeyframeParam*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->deleteKey(time);
      }
      
      OfxStatus paramDeleteAllKeys(OfxParamHandle  paramHandle) 
      {
        KeyframeParam *paramInstance = reinterpret_cast<KeyframeParam*>(paramHandle);        
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->deleteAllKeys();
      }
      
      OfxStatus paramCopy(OfxParamHandle  paramTo, OfxParamHandle  paramFrom, OfxTime dstOffset, OfxRangeD *frameRange) 
      {
        Instance *paramInstanceTo = reinterpret_cast<Instance*>(paramTo);        
        Instance *paramInstanceFrom = reinterpret_cast<Instance*>(paramFrom);        

        if(!paramInstanceTo) return kOfxStatErrBadHandle;        
        if(!paramInstanceFrom) return kOfxStatErrBadHandle;        

        if(!frameRange)
          return paramInstanceTo->copy(*paramInstanceFrom,dstOffset);
        else
          return paramInstanceTo->copy(*paramInstanceFrom,dstOffset,*frameRange);
      }
      
      OfxStatus paramEditBegin(OfxParamSetHandle paramSet, const char *name)
      {
        SetInstance *setInstance = reinterpret_cast<SetInstance*>(paramSet);
        if(!setInstance) return kOfxStatErrBadHandle;        
        return setInstance->editBegin(std::string(name));
      }

      
      OfxStatus paramEditEnd(OfxParamSetHandle paramSet) {
        SetInstance *setInstance = reinterpret_cast<SetInstance*>(paramSet);
        if(!setInstance) return kOfxStatErrBadHandle;        
        return setInstance->editEnd();
      }
      
      struct OfxParameterSuiteV1 suite = {
        paramDefine,
        paramGetHandle,
        paramSetGetPropertySet,
        paramGetPropertySet,
        paramGetValue,
        paramGetValueAtTime,
        paramGetDerivative,
        paramGetIntegral,
        paramSetValue,
        paramSetValueAtTime,
        paramGetNumKeys,
        paramGetKeyTime,
        paramGetKeyIndex,
        paramDeleteKey,
        paramDeleteAllKeys,
        paramCopy,
        paramEditBegin,
        paramEditEnd
      };
    }

    namespace Memory {
      OfxStatus memoryAlloc(void *handle, size_t bytes, void **data)
      {
        *data = malloc(bytes);
        if (*data) {
          return kOfxStatOK;
        } else {
          return kOfxStatErrMemory;
        }
      }
      
      OfxStatus memoryFree(void *data)
      {
        free(data);
        return kOfxStatOK;
      }
      
      struct OfxMemorySuiteV1 suite = {
        memoryAlloc,
        memoryFree
      };
    }

    namespace Message {

      OfxStatus message(void *handle, const char *type, const char *id, const char *format, ...)
      {
        ImageEffect::Instance *effectInstance = reinterpret_cast<ImageEffect::Instance*>(handle);
        if(effectInstance){
          va_list args;
          va_start(args,format);
          effectInstance->vmessage(type,id,format,args);
          va_end(args);
        }
        else{
          va_list args;
          va_start(args,format);
          vprintf(format,args);
          va_end(args);
        }
        return kOfxStatOK;
      }

      struct OfxMessageSuiteV1 suite = {
        message
      };

    }

    namespace MultiThread {

      OfxStatus multiThread(OfxThreadFunctionV1 func,
                            unsigned int nThreads,
                            void *customArg)
      {
        func(0,1,customArg);
        return kOfxStatOK;
      }

      OfxStatus multiThreadNumCPUs(unsigned int *nCPUs)
      {
        *nCPUs = 1;
        return kOfxStatOK;
      }

      OfxStatus multiThreadIndex(unsigned int *threadIndex){
        threadIndex = 0;
        return kOfxStatOK;
      }

      int multiThreadIsSpawnedThread(void){
        return false;
      }

      OfxStatus mutexCreate(const OfxMutexHandle *mutex, int lockCount)
      {
        // do nothing single threaded
        mutex = 0;
        return kOfxStatOK;
      }

      OfxStatus mutexDestroy(const OfxMutexHandle mutex)
      {
        // do nothing single threaded
        return kOfxStatOK;
      }

      OfxStatus mutexLock(const OfxMutexHandle mutex){
        // do nothing single threaded
        return kOfxStatOK;
      }
       
      OfxStatus mutexUnLock(const OfxMutexHandle mutex){
        // do nothing single threaded
        return kOfxStatOK;
      }       

      OfxStatus mutexTryLock(const OfxMutexHandle mutex){
        // do nothing single threaded
        return kOfxStatOK;
      }
       
      struct OfxMultiThreadSuiteV1 singleThreadedSuite = {
        multiThread,
        multiThreadNumCPUs,
        multiThreadIndex,
        multiThreadIsSpawnedThread,
        mutexCreate,
        mutexDestroy,
        mutexLock,
        mutexUnLock,
        mutexTryLock
      };
      
    }

    namespace Interact {
      OfxStatus interactSwapBuffers(OfxInteractHandle handle)
      {
        Interact::Instance *interactInstance = reinterpret_cast<Interact::Instance*>(handle);
        if(interactInstance)
          return interactInstance->swapBuffers();
        else
          return kOfxStatErrBadHandle;
      }
      
      OfxStatus interactRedraw(OfxInteractHandle handle)
      {
        Interact::Instance *interactInstance = reinterpret_cast<Interact::Instance*>(handle);
        if(interactInstance)
          return interactInstance->redraw();
        else
          return kOfxStatErrBadHandle;
      }
      
      OfxStatus interactGetPropertySet(OfxInteractHandle handle, OfxPropertySetHandle *property)
      {
       /* Interact::Instance *interactInstance = reinterpret_cast<Interact::Instance*>(handle);
        if(interactInstance)
          *property = interactInstance->getProps().getHandle();
        else
          return kOfxStatErrBadHandle; TODO*/
        return kOfxStatOK;
      }

      struct OfxInteractSuiteV1 suite = {
        interactSwapBuffers,
        interactRedraw,
        interactGetPropertySet
      };
    }

  }

}


namespace OFX {
  namespace Host {

    static Property::PropSpec hostStuffs[] = {
      { kOfxPropType, Property::eString, 1, false, kOfxTypeImageEffectHost },
      { kOfxPropName, Property::eString, 1, false, "uk.co.uk.thefoundry.PluginCache" },
      { kOfxPropLabel, Property::eString, 1, false, "Plugin Cache" } ,
      { kOfxImageEffectHostPropIsBackground, Property::eInt, 1, true, "0" },
      { kOfxImageEffectPropSupportsOverlays, Property::eInt, 1, true, "1" },
      { kOfxImageEffectPropSupportsMultiResolution, Property::eInt, 1, true, "1" },
      { kOfxImageEffectPropSupportsTiles, Property::eInt, 1, true, "1" },
      { kOfxImageEffectPropTemporalClipAccess, Property::eInt, 1, true, "1"  },

      { kOfxImageEffectPropSupportedComponents, Property::eString, 0, true, "" },
      /// xxx this needs defaulting manually
  
      { kOfxImageEffectPropSupportedContexts, Property::eString, 0, true, "" },
      /// xxx this needs defaulting manually

      { kOfxImageEffectPropSupportsMultipleClipDepths, Property::eInt, 1, true, "1" },
      { kOfxImageEffectPropSupportsMultipleClipPARs, Property::eInt, 1, true, "1" },
      { kOfxImageEffectPropSetableFrameRate, Property::eInt, 1, true, "1" },
      { kOfxImageEffectPropSetableFielding, Property::eInt, 1, true, "1"  },
      { kOfxParamHostPropSupportsCustomInteract, Property::eInt, 1, true, "0" },
      { kOfxParamHostPropSupportsStringAnimation, Property::eInt, 1, true, "0" },
      { kOfxParamHostPropSupportsChoiceAnimation, Property::eInt, 1, true, "0"  },
      { kOfxParamHostPropSupportsBooleanAnimation, Property::eInt, 1, true, "0" },
      { kOfxParamHostPropSupportsCustomAnimation, Property::eInt, 1, true, "0" },
      { kOfxParamHostPropMaxParameters, Property::eInt, 1, true, "-1" },
      { kOfxParamHostPropMaxPages, Property::eInt, 1, true, "0" },
      { kOfxParamHostPropPageRowColumnCount, Property::eInt, 2, true, "0" },
      { 0 },
    };    

    // our own internal parameter from storing away our host descriptor
    #define kOfxHostSupportHostDescriptor "OfxHostSupportHostDescriptor"

    void *fetchSuite(OfxPropertySetHandle host, const char *suiteName, int suiteVersion)
    {      
      Property::Set* properties = reinterpret_cast<Property::Set*>(host);
      
      Descriptor* hostDescriptor = (Descriptor*)properties->getProperty<Property::PointerValue>(kOfxHostSupportHostDescriptor,0);
      
      if(hostDescriptor)
        return hostDescriptor->fetchSuite(suiteName,suiteVersion);
      else
        return 0;
    }

    //
    // Host Descriptor
    //

    Descriptor::Descriptor() : _properties(hostStuffs) {
      _host.host = _properties.getHandle();
      _host.fetchSuite = OFX::Host::fetchSuite;

      Property::PropSpec hostDescriptorSpec[] = {
        { kOfxHostSupportHostDescriptor,    Property::ePointer,    0,    false,    "0" },
        { 0 }
      };

      // add parameter
      _properties.addProperties(hostDescriptorSpec);

      // record the host descriptor in the propert set
      _properties.setProperty<Property::PointerValue>(kOfxHostSupportHostDescriptor,0,this);
    }

    OfxHost *Descriptor::getHandle() {
      return &_host;
    }

    void* Descriptor::fetchOfxImageEffectSuite(int suiteVersion){
      if(suiteVersion==1) {
        return (void*)&ImageEffect::suite;
      }
      return 0;
    }

    void* Descriptor::fetchOfxPropertySuite(int suiteVersion){
      if(suiteVersion==1) {
        return (void*)&Property::Set::suite;
      }
      return 0;
    }
 
    void* Descriptor::fetchOfxParameterSuite(int suiteVersion){
      return (void*)&Param::suite;
    }
  
    void* Descriptor::fetchOfxMemorySuite(int suiteVersion){
      return (void*)&Memory::suite;
    }
  
    // standard multithreading implementation - to override see above
    void* Descriptor::fetchOfxMultiThreadSuite(int suiteVersion){
      return (void*)&MultiThread::singleThreadedSuite;
    }

    void* Descriptor::fetchOfxMessageSuite(int suiteVersion){
      return (void*)&Message::suite;
    }

    void* Descriptor::fetchOfxInteractSuite(int suiteVersion){
      return (void*)&Interact::suite;
    }

    void *Descriptor::fetchSuite(const char *suiteName, int suiteVersion){
      if (strcmp(suiteName, "OfxImageEffectSuite")==0 && suiteVersion==1) {
        return fetchOfxImageEffectSuite(suiteVersion);
      }
      if (strcmp(suiteName, "OfxPropertySuite")==0 && suiteVersion==1) {
        return fetchOfxPropertySuite(suiteVersion);
      }
      if (strcmp(suiteName, "OfxParameterSuite")==0) {
        return fetchOfxParameterSuite(suiteVersion);
      }
  
      if (strcmp(suiteName, "OfxMemorySuite")==0) {
        return fetchOfxMemorySuite(suiteVersion);
      }
  
      if (strcmp(suiteName, "OfxMultiThreadSuite")==0) {
        return fetchOfxMultiThreadSuite(suiteVersion);
      }
  
      if (strcmp(suiteName, "OfxMessageSuite")==0) {
        return fetchOfxMessageSuite(suiteVersion);
      }
  
      if (strcmp(suiteName, "OfxInteractSuite")==0) {
        return fetchOfxInteractSuite(suiteVersion);
      }
      printf("fetchSuite failed with host = %p, name = %s, version = %i\n", this, suiteName, suiteVersion);
      return 0;
    }

  } // Host

} // OFX 
