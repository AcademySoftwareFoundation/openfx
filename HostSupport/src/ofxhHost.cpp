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

#include "ofxhHost.h"
#include "ofxhPluginCache.h"

typedef OfxPlugin* (*OfxGetPluginType)(int);

namespace OFX {
  namespace Host {
    namespace ImageEffect {

      OfxStatus getPropertySet(OfxImageEffectHandle h1, OfxPropertySetHandle *h2)
      {
        ImageEffectDescriptor *effect = (ImageEffectDescriptor*)h1;
        *h2 = effect->getProps().getHandle();
        return kOfxStatOK;
      }
      
#define UNIMPLEMENTED_BODY { std::cerr << "Unimplemented " << __FUNCTION__ << " called." << std::endl; ::abort(); return kOfxStatErrBadHandle; }

      OfxStatus getParamSet(OfxImageEffectHandle h1, OfxParamSetHandle *h2)
      {
        ImageEffectDescriptor *effect = (ImageEffectDescriptor*)h1;
        *h2 = effect->getParams().getHandle();
        return kOfxStatOK;
      }
      
      OfxStatus clipDefine(OfxImageEffectHandle h1, const char *name, OfxPropertySetHandle *h2)
      {
        ImageEffectDescriptor *effect = (ImageEffectDescriptor*)h1;
        Clip::ClipDescriptor *clip = effect->defineClip(name);
        *h2 = clip->getPropHandle();
        return kOfxStatOK;
      }
      
      OfxStatus clipGetPropertySet(OfxImageClipHandle clip,
                                   OfxPropertySetHandle *propHandle)
        UNIMPLEMENTED_BODY
      
      OfxStatus clipGetImage(OfxImageClipHandle h1, OfxTime time, OfxRectD *h2, OfxPropertySetHandle *h3)
        UNIMPLEMENTED_BODY
      
      OfxStatus clipReleaseImage(OfxPropertySetHandle h1)
        UNIMPLEMENTED_BODY
      
      OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect,
                              const char *name,
                              OfxImageClipHandle *clip,
                              OfxPropertySetHandle *propertySet)
        UNIMPLEMENTED_BODY
      
      OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip,
                                          OfxTime time,
                                          OfxRectD *bounds)
        UNIMPLEMENTED_BODY
      
      int abort(OfxImageEffectHandle imageEffect)
        UNIMPLEMENTED_BODY
      
      OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
                                 size_t nBytes,
                                 OfxImageMemoryHandle *memoryHandle)
        UNIMPLEMENTED_BODY
      
      OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle)
        UNIMPLEMENTED_BODY
      
      OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                                void **returnedPtr)
        UNIMPLEMENTED_BODY
      
      OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle)
        UNIMPLEMENTED_BODY

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

      static Property::PropSpec imageEffectDescriptorStuff[] = {
        /* name                                 type                          dim.   r/o    default value */
        { kOfxPropType,											    Property::eString,     1, true,  kOfxTypeImageEffect },
        { kOfxPropLabel, 									      Property::eString,     1, false, "" },
        { kOfxPropShortLabel,  						      Property::eString,     1, false, "" },
        { kOfxPropLongLabel,  						      Property::eString,     1, false, "" },
        { kOfxImageEffectPropSupportedContexts, Property::eString,     0, false, "" },
        { kOfxImageEffectPluginPropGrouping,    Property::eString,     1, false, "" },
        { kOfxImageEffectPluginPropSingleInstance, Property::eInt,     1, false, "0" },
        { kOfxImageEffectPluginRenderThreadSafety, Property::eString,  1, false, kOfxImageEffectRenderInstanceSafe },
        { kOfxImageEffectPluginPropHostFrameThreading, Property::eInt, 1, false, "1" },
        { kOfxImageEffectPluginPropOverlayInteractV1, Property::ePointer, 1, false, 0 },
        { kOfxImageEffectPropSupportsMultiResolution, Property::eInt,  1, false, "1" } ,
        { kOfxImageEffectPropSupportsTiles,     Property::eInt,        1, false, "1" }, 
        { kOfxImageEffectPropTemporalClipAccess, Property::eInt,       1, false, "0" },
        { kOfxImageEffectPropSupportedPixelDepths, Property::eString,  0, false, "" }, 
        { kOfxImageEffectPluginPropFieldRenderTwiceAlways, Property::eInt, 1, false, "1" } ,
        { kOfxImageEffectPropSupportsMultipleClipDepths, Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropSupportsMultipleClipPARs,   Property::eInt, 1, false, "0" },
        { kOfxImageEffectPropClipPreferencesSlaveParam, Property::eString, 0, false, "" },

        { 0 }
      };

      ImageEffectDescriptor::ImageEffectDescriptor(Plugin *plug) : _properties(imageEffectDescriptorStuff) {

        Property::PropSpec filePath[] = {
          { kOfxPluginPropFilePath, Property::eString, 1, true, plug->getBinary()->getBundlePath().c_str() },
          { 0 }
        };

        _properties.addProperties(filePath);
      }
    }
    
    namespace Param {

      struct TypeMap {
        const char *paramType;
        Property::TypeEnum propType;
        int propDimension;
      };

      bool getType(const std::string paramType, Property::TypeEnum &propType, int &propDim) {
        static TypeMap typeMap[] = {
          { kOfxParamTypeInteger,   Property::eInt,    1 },
          { kOfxParamTypeDouble,    Property::eDouble, 1 },
          { kOfxParamTypeBoolean,   Property::eInt,    1 },
          { kOfxParamTypeChoice,    Property::eInt,    1 },
          { kOfxParamTypeRGBA,      Property::eDouble, 4 },
          { kOfxParamTypeRGB,       Property::eDouble, 3 },
          { kOfxParamTypeDouble2D,  Property::eDouble, 2 },
          { kOfxParamTypeInteger2D, Property::eInt,    2 },
          { kOfxParamTypeDouble3D,  Property::eDouble, 3 },
          { kOfxParamTypeInteger3D, Property::eInt,    3 },
          { kOfxParamTypeString,    Property::eString, 1 },
          { kOfxParamTypeCustom,    Property::eString, 1 },
          { kOfxParamTypeGroup,	    Property::eNone },
          { kOfxParamTypePage,      Property::eNone },
          { kOfxParamTypePushButton,Property::eNone },
          { 0 }
        };
  
        TypeMap *tm = typeMap;
        while (tm->paramType) {
          if (tm->paramType == paramType) {
            propType = tm->propType;
            propDim = tm->propDimension;
            return true;
          }
          tm++;
        }
        return false;
      }

      Param::Param(const std::string &type, const std::string &name) : _properties(false) {
        const char *ctype = type.c_str();
        const char *cname = name.c_str();

        Property::TypeEnum propType = Property::eString;
        int propDim = 1;
        getType(type, propType, propDim);

        Property::PropSpec universalProps[] = {
          { kOfxPropType,    Property::eString,    1,    true,    kOfxTypeParameter },
          { kOfxParamPropSecret,  Property::eInt,    1,    false,    "0"},
          { kOfxParamPropCanUndo, Property::eInt,    1,    false,    "1"},
          { kOfxParamPropHint,    Property::eString,    1,    false,    ""},
          { kOfxParamPropScriptName, Property::eString, 1, false, cname },
          { kOfxParamPropParent,  Property::eString,    1,    false,    "" },
          { kOfxParamPropEnabled, Property::eInt,    1,    false,    "1" },
          { kOfxParamPropDataPtr, Property::ePointer,    1,    false,    0 },
          { kOfxParamPropType,  Property::eString, 1, true,  ctype },
          { kOfxPropName,       Property::eString, 1, false, cname },
          { kOfxPropLabel,      Property::eString, 1, false, cname },
          { kOfxPropShortLabel, Property::eString, 1, false, cname },
          { kOfxPropLongLabel,  Property::eString, 1, false, cname },
          { 0 }
        };

        Property::PropSpec allButGroupPageProps[] = {
          { kOfxParamPropInteractV1,          Property::ePointer, 1, false, 0 },
          { kOfxParamPropInteractSize,        Property::eDouble,  2, false, "0" },
          { kOfxParamPropInteractSizeAspect,  Property::eDouble,  1, false, "1" },
          { kOfxParamPropInteractMinimumSize, Property::eDouble,  2, false, "10" },
          { kOfxParamPropInteractPreferedSize,Property::eInt,     2, false, "10" },
          { 0 }
        };

        Property::PropSpec allWithValues[] = {
          { kOfxParamPropDefault,     propType,                propDim, false, propType == Property::eString ? "" : "0" },
          { kOfxParamPropAnimates,    Property::eInt, 1,       false, "1" },
          { kOfxParamPropIsAnimating, Property::eInt, 1,       false, "0" },
          { kOfxParamPropIsAutoKeying,Property::eInt, 1,       false, "0" },
          { kOfxParamPropPersistant,  Property::eInt, 1,       false, "1" },
          { kOfxParamPropEvaluateOnChange, Property::eInt, 1,  false, "1" },
          { kOfxParamPropPluginMayWrite,    Property::eInt,    1,    false,    "0" },
          { kOfxParamPropCacheInvalidation,    Property::eString,    1,    false,    kOfxParamInvalidateValueChange },
          { 0 }
        };
		
		std::ostringstream dbl_min, dbl_max, int_min, int_max;

        dbl_min << -DBL_MAX;
        dbl_max << DBL_MAX;
        int_min << INT_MIN;
        int_max << INT_MAX;

        Property::PropSpec allNumeric[] = {
          { kOfxParamPropDisplayMin, propType, propDim, false, (propType == Property::eDouble ? dbl_min : int_min).str().c_str() },
          { kOfxParamPropDisplayMax, propType, propDim, false, (propType == Property::eDouble ? dbl_max : int_max).str().c_str() },
          { 0 }
        };

        Property::PropSpec allDouble[] = {
          { kOfxParamPropIncrement,  Property::eDouble,    1,    false,    "1" },
          { kOfxParamPropDigits,     Property::eInt,       1,    false,    "2" },
          { kOfxParamPropDoubleType, Property::eString,    1,    false,    kOfxParamDoubleTypePlain },
          { 0}
        };

        Property::PropSpec allDouble1D[] = {
          { kOfxParamPropShowTimeMarker, Property::eInt,   1,    false,    "0" },    
          { 0 }
        };

        Property::PropSpec allString[] = {
          { kOfxParamPropStringMode,  Property::eString,    1,    false,    kOfxParamStringIsSingleLine },
          { kOfxParamPropStringFilePathExists,    Property::eString,    1,    false,    "1" },
          { 0 }
        };
    
        Property::PropSpec allChoice[] = {
          { kOfxParamPropChoiceOption,    Property::eString,    0,    false,    "" },
          { 0 }
        };

        Property::PropSpec all2D3D[] = {
          { kOfxParamPropDimensionLabel,  Property::eString, propDim, false, "" },
          { 0 },
        };  

        Property::PropSpec allCustom[] = {
          { kOfxParamPropCustomInterpCallbackV1,    Property::ePointer,    1,    false,    0 },
          { 0 },
        };

        Property::PropSpec allPage[] = {
          { kOfxParamPropPageChild,    Property::eString,    0,    false,    "" },
          { 0 }
        };

        _properties.addProperties(universalProps);

        if (propType != Property::eNone) {
          _properties.addProperties(allWithValues);
        }

        if (propType == Property::eString) {
          _properties.addProperties(allString);
        }
  
        if (propType == Property::eInt || propType == Property::eDouble) {   
          _properties.addProperties(allNumeric);

          if (propDim == 2 || propDim == 3) {
            _properties.addProperties(all2D3D);
            _properties.setProperty<Property::StringValue>(kOfxParamPropDimensionLabel, 0, "X");
            _properties.setProperty<Property::StringValue>(kOfxParamPropDimensionLabel, 0, "Y");
            if (propDim == 3) {
              _properties.setProperty<Property::StringValue>(kOfxParamPropDimensionLabel, 0, "Z");
            }
          }
        }

        if (propType == Property::eDouble) {
          _properties.addProperties(allDouble);

          if (propDim == 1) {
            _properties.addProperties(allDouble1D);
          }
        }

        if (type != kOfxParamTypeGroup && kOfxParamTypePage) {
          _properties.addProperties(allButGroupPageProps);
        }

        if (type == kOfxParamTypeChoice) {
          _properties.addProperties(allChoice);
        }

        if (type == kOfxParamTypeCustom) {
          _properties.addProperties(allCustom);
        }

        if (type == kOfxParamTypePage) {
          _properties.addProperties(allPage);
        }
      }


      
      OfxStatus paramDefine(OfxParamSetHandle paramSet,
                            const char *paramType,
                            const char *name,
                            OfxPropertySetHandle *propertySet)
      {
        ParamSet *pset = reinterpret_cast<ParamSet*>(paramSet);
        Param *parm = new Param(paramType, name);
        pset->getParams()[name] = parm;
        *propertySet = parm->getPropHandle();
        return kOfxStatOK;
      }
      
      OfxStatus paramGetHandle(OfxParamSetHandle paramSet,
                               const char *name,
                               OfxParamHandle *param,
                               OfxPropertySetHandle *propertySet)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet,
                                       OfxPropertySetHandle *propHandle) 
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetPropertySet(OfxParamHandle param,
                                    OfxPropertySetHandle *propHandle) 
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetValue(OfxParamHandle  paramHandle,
                              ...) 
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle,
                                    OfxTime time,
                                    ...)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetDerivative(OfxParamHandle  paramHandle,
                                   OfxTime time,
                                   ...)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetIntegral(OfxParamHandle  paramHandle,
                                 OfxTime time1, OfxTime time2,
                                 ...)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramSetValue(OfxParamHandle  paramHandle,
                              ...) 
        UNIMPLEMENTED_BODY
      
      OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle,
                                    OfxTime time,  // time in frames
                                    ...)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetNumKeys(OfxParamHandle  paramHandle,
                                unsigned int  *numberOfKeys)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetKeyTime(OfxParamHandle  paramHandle,
                                unsigned int nthKey,
                                OfxTime *time)
        UNIMPLEMENTED_BODY
      
      OfxStatus paramGetKeyIndex(OfxParamHandle  paramHandle,
                                 OfxTime time,
                                 int     direction,
                                 int    *index) 
        UNIMPLEMENTED_BODY
      
      OfxStatus paramDeleteKey(OfxParamHandle  paramHandle,
                               OfxTime time)
        UNIMPLEMENTED_BODY

      
      OfxStatus paramDeleteAllKeys(OfxParamHandle  paramHandle) 
        UNIMPLEMENTED_BODY
      
      OfxStatus paramCopy(OfxParamHandle  paramTo, OfxParamHandle  paramFrom, OfxTime dstOffset, OfxRangeD *frameRange) 
        UNIMPLEMENTED_BODY

      
      OfxStatus paramEditBegin(OfxParamSetHandle paramSet, const char *name)
        UNIMPLEMENTED_BODY

      
      OfxStatus paramEditEnd(OfxParamSetHandle paramSet)
        UNIMPLEMENTED_BODY
      
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
        UNIMPLEMENTED_BODY

      OfxStatus multiThreadNumCPUs(unsigned int *nCPUs)
        UNIMPLEMENTED_BODY

      OfxStatus multiThreadIndex(unsigned int *threadIndex)
        UNIMPLEMENTED_BODY

      int multiThreadIsSpawnedThread(void)
        UNIMPLEMENTED_BODY

      OfxStatus mutexCreate(const OfxMutexHandle *mutex, int lockCount)
        UNIMPLEMENTED_BODY

      OfxStatus mutexDestroy(const OfxMutexHandle mutex)
        UNIMPLEMENTED_BODY

      OfxStatus mutexLock(const OfxMutexHandle mutex)
        UNIMPLEMENTED_BODY

      OfxStatus mutexUnLock(const OfxMutexHandle mutex)
        UNIMPLEMENTED_BODY

      OfxStatus mutexTryLock(const OfxMutexHandle mutex)
        UNIMPLEMENTED_BODY

      struct OfxMultiThreadSuiteV1 suite = {
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
      OfxStatus interactSwapBuffers(OfxInteractHandle interactInstance)
        UNIMPLEMENTED_BODY
      
      OfxStatus interactRedraw(OfxInteractHandle interactInstance)
        UNIMPLEMENTED_BODY
      
      OfxStatus interactGetPropertySet(OfxInteractHandle interactInstance,
                                       OfxPropertySetHandle *property)
        UNIMPLEMENTED_BODY

      struct OfxInteractSuiteV1 suite = {
        interactSwapBuffers,
        interactRedraw,
        interactGetPropertySet
      };
    }

    namespace Clip {

      static Property::PropSpec clipStuffs[] = {
        { kOfxPropType, Property::eString, 1, false, kOfxTypeImageEffectHost },
        { kOfxPropName, Property::eString, 1, false, "uk.co.uk.thefoundry.PluginCache" },
        { kOfxPropLabel, Property::eString, 1, false, "Plugin Cache" } ,
        
        { kOfxImageEffectPropSupportedComponents, Property::eString, 0, true, "" },
        /// xxx ??
        
        { kOfxImageEffectPropTemporalClipAccess,   Property::eInt, 1, false, "0" },
        { kOfxImageClipPropOptional, Property::eInt, 1, false, "0" },
        { kOfxImageClipPropIsMask,   Property::eInt, 1, false, "0" },
        { kOfxImageClipPropFieldExtraction, Property::eString, 1, false, kOfxImageFieldDoubled },
        { kOfxImageEffectPropSupportsTiles,   Property::eInt, 1, false, "1" },  
        { 0 },
      };
      
      ClipDescriptor::ClipDescriptor() : _properties(clipStuffs) {
      }
      
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
  
    OfxStatus getPropertySet(OfxImageEffectHandle h1, OfxPropertySetHandle *h2);

    void *fetchSuite(OfxPropertySetHandle host, const char *suiteName, int suiteVersion)
    {
      if (strcmp(suiteName, "OfxImageEffectSuite")==0 && suiteVersion==1) {
        return (void*)&ImageEffect::suite;
      }
      if (strcmp(suiteName, "OfxPropertySuite")==0 && suiteVersion==1) {
        return (void*)&Property::Set::suite;
      }
      if (strcmp(suiteName, "OfxParameterSuite")==0) {
        return (void*)&Param::suite;
      }
  
      if (strcmp(suiteName, "OfxMemorySuite")==0) {
        return (void*)&Memory::suite;
      }
  
      if (strcmp(suiteName, "OfxMultiThreadSuite")==0) {
        return (void*)&MultiThread::suite;
      }
  
      if (strcmp(suiteName, "OfxMessageSuite")==0) {
        return (void*)&Message::suite;
      }
  
      if (strcmp(suiteName, "OfxInteractSuite")==0) {
        return (void*)&Interact::suite;
      }
      printf("fetchSuite failed with host = %p, name = %s, version = %i\n", host, suiteName, suiteVersion);
      return 0;
    }

    HostDescriptor::HostDescriptor() : _properties(hostStuffs) {
      _host.host = _properties.getHandle();
      _host.fetchSuite = fetchSuite;
    }


  }
}
