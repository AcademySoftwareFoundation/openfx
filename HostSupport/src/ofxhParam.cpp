/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Open Effects Association Ltd, nor the names of its 
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

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhParam.h"
#include "ofxhImageEffect.h"

#ifdef OFX_EXTENSIONS_NUKE
//nuke extensions (added on 08/16/13 by Alex).
//We need to add to the rep. the extensions so it finds it automatically.
#include <nuke/fnPublicOfxExtensions.h>
#endif

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>

namespace OFX {

  namespace Host {

    namespace Param {

      //
      // Base
      //
      Base::Base(const std::string &name, const std::string& type) : 
        _paramName(name),
        _paramType(type)
      {
        assert(_paramType.c_str());
      }

      Base::Base(const std::string &name, const std::string &type, const Property::Set &properties) :
        _paramName(name),
        _paramType(type), 
        _properties(properties)
      {
        assert(_paramType.c_str());
      }


      Base::~Base() {}

      /// grab a handle on the parameter for passing to the C API
      OfxParamHandle Base::getHandle() const {
        return (OfxParamHandle)this;
      }

      /// grab a handle on the properties of this parameter for the C api
      OfxPropertySetHandle Base::getPropHandle() const {
        return _properties.getHandle();
      }

      Property::Set &Base::getProperties() {
        return _properties;
      }

      const Property::Set &Base::getProperties() const {
        return _properties;
      }

      const std::string &Base::getType() const {
        return _paramType;
      } 

      const std::string &Base::getName() const {
        return _paramName;
      }

      const std::string &Base::getParentName() const {
        return _properties.getStringProperty(kOfxParamPropParent);
      }

      const std::string &Base::getScriptName() const {
        return _properties.getStringProperty(kOfxParamPropScriptName);
      }

      const std::string &Base::getLabel() const {
        return _properties.getStringProperty(kOfxPropLabel);
      }

      const std::string &Base::getLongLabel() const {
        return _properties.getStringProperty(kOfxPropLongLabel);
      }

      const std::string &Base::getShortLabel() const {
        return _properties.getStringProperty(kOfxPropShortLabel);
      }

      const std::string &Base::getDoubleType() const {
        return _properties.getStringProperty(kOfxParamPropDoubleType, 0);
      }

      const std::string &Base::getHint() const {
        return _properties.getStringProperty(kOfxParamPropHint, 0);
      }

      bool Base::getEnabled() const {
        return _properties.getIntProperty(kOfxParamPropEnabled, 0) != 0;
      }
      
      bool Base::getSecret() const {
        return _properties.getIntProperty(kOfxParamPropSecret, 0) != 0;
      }

      bool Base::getEvaluateOnChange() const {
        return _properties.getIntProperty(kOfxParamPropEvaluateOnChange, 0) != 0;
      }
      
      bool Base::getCanUndo() const {
        if (_properties.fetchProperty(kOfxParamPropCanUndo))  {
          return _properties.getIntProperty(kOfxParamPropCanUndo) != 0;
        }
        return false;
      }
      
      bool Base::getCanAnimate() const {
        if (_properties.fetchProperty(kOfxParamPropAnimates))  {
          return _properties.getIntProperty(kOfxParamPropAnimates) != 0;
        }
        return false;
      }

      //
      // Descriptor
      //

      struct TypeMap {
        const char *paramType;
        Property::TypeEnum propType;
        int propDimension;
      };

      bool isDoubleParam(const std::string &paramType) 
      {
        return paramType == kOfxParamTypeDouble ||
               paramType == kOfxParamTypeDouble2D ||
               paramType == kOfxParamTypeDouble3D;
      }

      bool isColourParam(const std::string &paramType) 
      {
        return 
          paramType == kOfxParamTypeRGBA ||
          paramType == kOfxParamTypeRGB;
      }

      bool isIntParam(const std::string &paramType) 
      {
        return paramType == kOfxParamTypeInteger ||
               paramType == kOfxParamTypeInteger2D ||
               paramType == kOfxParamTypeInteger3D;
      }

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
        { kOfxParamTypeGroup,     Property::eNone },
        { kOfxParamTypePage,      Property::eNone },
        { kOfxParamTypePushButton,Property::eNone },
        { 0 }
      };
      
      /// is this a standard type
      bool isStandardType(const std::string &type)
      {
        TypeMap *tm = typeMap;
        while (tm->paramType) {
          if (tm->paramType == type) 
            return true;
          tm++;
        }
        return false;
      }
  
      bool findType(const std::string paramType, Property::TypeEnum &propType, int &propDim) 
      {
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

      /// make a parameter, with the given type and name
      Descriptor::Descriptor(const std::string &type, 
                             const std::string &name) : Base(name, type)
      {
        const char *ctype = type.c_str();
        const char *cname = name.c_str();

        Property::PropSpec universalProps[] = {
          { kOfxPropType,    Property::eString,    1,    true,    kOfxTypeParameter },
          { kOfxParamPropSecret,  Property::eInt,    1,    false,    "0"},
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
#ifdef OFX_EXTENSIONS_NUKE
          { kOfxParamPropLayoutHint, Property::eInt, 1, false, "0" }, //!< Nuke extension (@Alex on 08/16/13)
          { kOfxParamPropLayoutPadWidth, Property::eInt, 1, false, "0"}, //!< Nuke extension (@Alex on 08/16/13)
#endif
          { 0 }
        };
        
        _properties.addProperties(universalProps);
      }

      /// make a parameter, with the given type and name
           void Descriptor::addStandardParamProps(const std::string &type)
           {
             Property::TypeEnum propType = Property::eString;
             int propDim = 1;
             findType(type, propType, propDim);


             static Property::PropSpec allString[] = {
               { kOfxParamPropStringMode,  Property::eString,    1,    false,    kOfxParamStringIsSingleLine },
                 { kOfxParamPropStringFilePathExists, Property::eInt,    1,    false,    "1" },
               { 0 }
             };

             static Property::PropSpec allChoice[] = {
               { kOfxParamPropChoiceOption,    Property::eString,    0,    false,    "" },
                 // { kOfxParamPropChoiceLabelOption, Property::eString, 0, false, "" }//<< @Alex: add this to support tuttle choice param extension
               { 0 }
             };

             static Property::PropSpec allCustom[] = {
               { kOfxParamPropCustomInterpCallbackV1,    Property::ePointer,    1,    false,    0 },
               { 0 },
             };

             static Property::PropSpec allPage[] = {
               { kOfxParamPropPageChild,    Property::eString,    0,    false,    "" },
               { 0 }
             };

             if (propType != Property::eNone) {
               addValueParamProps(type, propType, propDim);
             }

             if (type == kOfxParamTypeString) {
               _properties.addProperties(allString);
             }

             if (isDoubleParam(type) || isIntParam(type) || isColourParam(type)) {
               addNumericParamProps(type, propType, propDim);
             }

             if (type != kOfxParamTypeGroup && type != kOfxParamTypePage) {
               addInteractParamProps(type);
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
             /*Fix @Alex on 08/13/13: added this to deal with group params. Otherwise they would not work.*/
             if( type == kOfxParamTypeGroup )
             {
                 static const Property::PropSpec allGroup[] = {
                     { kOfxParamPropGroupOpen, Property::eInt, 1, false, "1" },
#ifdef OFX_EXTENSIONS_NUKE
                     { kFnOfxParamPropGroupIsTab, Property::eInt, 1, false, "0" }, //!< Nuke extension (@Alex on 08/16/13)
#endif
                     { 0 }
                 };
                 getProperties().addProperties(allGroup);
             }
           }

      /// add standard properties to a params that can take an interact
      void Descriptor::addInteractParamProps(const std::string &type)
      {
        static Property::PropSpec allButGroupPageProps[] = {
          { kOfxParamPropInteractV1,          Property::ePointer, 1, false, 0 },
          { kOfxParamPropInteractSize,        Property::eDouble,  2, false, "0" },
          { kOfxParamPropInteractSizeAspect,  Property::eDouble,  1, false, "1" },
          { kOfxParamPropInteractMinimumSize, Property::eDouble,  2, false, "10" },
          { kOfxParamPropInteractPreferedSize,Property::eInt,     2, false, "10" },
          { 0 }
        };

        
        _properties.addProperties(allButGroupPageProps);
      }

      /// add standard properties to a value holding param
      void Descriptor::addValueParamProps(const std::string &type, Property::TypeEnum valueType, int dim)
      {
        static Property::PropSpec invariantProps[] = {
          { kOfxParamPropAnimates,    Property::eInt, 1,       false, "1" },
          { kOfxParamPropIsAnimating, Property::eInt, 1,       false, "0" },
          { kOfxParamPropIsAutoKeying,Property::eInt, 1,       false, "0" },
          { kOfxParamPropPersistant,  Property::eInt, 1,       false, "1" },
          { kOfxParamPropEvaluateOnChange, Property::eInt, 1,  false, "1" },
          { kOfxParamPropPluginMayWrite,    Property::eInt,    1,    false,    "0" },
          { kOfxParamPropCanUndo,    Property::eInt,    1,    false,    "1" },
          { kOfxParamPropCacheInvalidation,    Property::eString,    1,    false,    kOfxParamInvalidateValueChange },
          { 0 }
        };

        Property::PropSpec variantProps[] = {
          { kOfxParamPropDefault,     valueType,               dim, false, valueType == Property::eString ? "" : "0" },
          { 0 }
        };

        _properties.addProperties(invariantProps);
        _properties.addProperties(variantProps);
      }
      
      /// add standard properties to a value holding param
      void Descriptor::addNumericParamProps(const std::string &type, Property::TypeEnum valueType, int dim)
      {
        static std::string dbl_minstr, dbl_maxstr, int_minstr, int_maxstr;
        bool doneOne = false;

        if(!doneOne) {
          std::ostringstream dbl_min, dbl_max, int_min, int_max;
          doneOne = true;
          dbl_min << -DBL_MAX;
          dbl_max << DBL_MAX;
          int_min << INT_MIN;
          int_max << INT_MAX;
          
          dbl_minstr = dbl_min.str();
          dbl_maxstr = dbl_max.str();
          int_minstr = int_min.str();
          int_maxstr = int_max.str();
        }
          
        Property::PropSpec allNumeric[] = {
          { kOfxParamPropDisplayMin, valueType, dim, false, isColourParam(type) ? "0" : (valueType == Property::eDouble ? dbl_minstr : int_minstr).c_str() },
          { kOfxParamPropDisplayMax, valueType, dim, false, isColourParam(type) ? "1" : (valueType == Property::eDouble ? dbl_maxstr : int_maxstr).c_str() },
          { kOfxParamPropMin, valueType, dim, false, (valueType == Property::eDouble ? dbl_minstr : int_minstr).c_str() },
          { kOfxParamPropMax, valueType, dim, false, (valueType == Property::eDouble ? dbl_maxstr : int_maxstr).c_str() },
          { 0 }
        };

        _properties.addProperties(allNumeric);

        /// if any double or a colour
        if (valueType == Property::eDouble) {
          static Property::PropSpec allDouble[] = {
            { kOfxParamPropIncrement,  Property::eDouble,    1,    false,    "1" },
            { kOfxParamPropDigits,     Property::eInt,       1,    false,    "2" },
            { 0}
          };
          _properties.addProperties(allDouble);
        }

        /// if a double param type
        if(isDoubleParam(type)) {
          static Property::PropSpec allDouble[] = {
            { kOfxParamPropDoubleType, Property::eString,    1,    false,    kOfxParamDoubleTypePlain },
            { 0}
          };
          _properties.addProperties(allDouble);
          
          if(dim == 1) {
            static Property::PropSpec allDouble1D[] = {
              { kOfxParamPropShowTimeMarker, Property::eInt,   1,    false,    "0" },    
              { 0 }
            };
            
            _properties.addProperties(allDouble1D);
          }
        }

        /// if a multi dimensional param
        if (isDoubleParam(type) && (dim == 2 || dim == 3)) {
          Property::PropSpec all2D3D[] = {
            { kOfxParamPropDimensionLabel,  Property::eString, dim, false, "" },
            { 0 },
          };  

          _properties.addProperties(all2D3D);
          _properties.setStringProperty(kOfxParamPropDimensionLabel, "X", 0);
          _properties.setStringProperty(kOfxParamPropDimensionLabel, "Y", 1);
          if (dim == 3) {
            _properties.setStringProperty(kOfxParamPropDimensionLabel, "Z", 2);
          }
        }

        /// if a multi dimensional param
        if (isColourParam(type)) {
          Property::PropSpec allColor[] = {
            { kOfxParamPropDimensionLabel,  Property::eString, dim, false, "" },
            { 0 },
          };  

          _properties.addProperties(allColor);
          _properties.setStringProperty(kOfxParamPropDimensionLabel, "R", 0);
          _properties.setStringProperty(kOfxParamPropDimensionLabel, "G", 1);
          _properties.setStringProperty(kOfxParamPropDimensionLabel, "B", 2);
          if (dim == 4) {
            _properties.setStringProperty(kOfxParamPropDimensionLabel, "A", 3);
          }
        }
      }

      BaseSet::~BaseSet() {}
      
      /// obtain a handle on this set for passing to the C api
      SetDescriptor::SetDescriptor()
      {
      }

       /// obtain a handle on this set for passing to the C api
      OfxParamSetHandle BaseSet::getParamSetHandle() const
      {
        return (OfxParamSetHandle)this;
      }

      SetDescriptor::~SetDescriptor()
      {
      }

      const std::map<std::string, Descriptor*> &SetDescriptor::getParams() const
      {
        return _paramMap;
      }

      const std::list<Descriptor*> &SetDescriptor::getParamList() const
      {
        return _paramList;
      }

      void SetDescriptor::addParam(const std::string &name, Descriptor *p) {
        _paramList.push_back(p);
        _paramMap[name] = p;
      }

      /// define a param on this effect
      Descriptor *SetDescriptor::paramDefine(const char *paramType,
                                             const char *name)
      {
        if(!isStandardType(paramType)) 
          return NULL; /// << EEK! This is bad.

        Descriptor *desc = new Descriptor(paramType, name); 
        desc->addStandardParamProps(paramType);
        addParam(name, desc);
        return desc;
      }

      ////////////////////////////////////////////////////////////////////////////////
      //
      // Instance
      //

      /// the description of a plugin parameter
      Instance::~Instance() {}

      /// make a parameter, with the given type and name
      Instance::Instance(Descriptor& descriptor, Param::SetInstance* paramSet) 
        : Base(descriptor.getName(), descriptor.getType(), descriptor.getProperties())
        , _paramSetInstance(paramSet)
        , _parentInstance(0)
      {
        _properties.addNotifyHook(kOfxParamPropEnabled, this);
        _properties.addNotifyHook(kOfxParamPropSecret, this);
        _properties.addNotifyHook(kOfxPropLabel, this);
        _properties.addNotifyHook(kOfxParamPropDisplayMin, this);
        _properties.addNotifyHook(kOfxParamPropDisplayMax, this);
      }

      // callback which should set enabled state as appropriate
      void Instance::setEnabled()
      {
      }

      // callback which should set secret state as appropriate
      void Instance::setSecret()
      {
      }
      
      // callback which should update label
      void Instance::setLabel()
      {
      }
      
      /// callback which should set 
      void Instance::setDisplayRange()
      {
      }

      /// get a value, implemented by instances to deconstruct var args
      OfxStatus Instance::getV(va_list arg)
      {
        return kOfxStatErrUnsupported;
      }
      
      /// get a value, implemented by instances to deconstruct var args
      OfxStatus Instance::getV(OfxTime time, va_list arg)
      {
        return kOfxStatErrUnsupported;
      }

      /// set a value, implemented by instances to deconstruct var args
      OfxStatus Instance::setV(va_list arg)
      {
        return kOfxStatErrUnsupported;
      }

      /// key a value, implemented by instances to deconstruct var args
      OfxStatus Instance::setV(OfxTime time, va_list arg)
      {
        return kOfxStatErrUnsupported;
      }

      /// derive a value, implemented by instances to deconstruct var args
      OfxStatus Instance::deriveV(OfxTime time, va_list arg)
      {
        return kOfxStatErrUnsupported;
      }

      /// integrate a value, implemented by instances to deconstruct var args
      OfxStatus Instance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        return kOfxStatErrUnsupported;
      }

      /// overridden from Property::NotifyHook
      void Instance::notify(const std::string &name, bool single, int num) OFX_EXCEPTION_SPEC 
      {
        if (name == kOfxPropLabel) {
          setLabel();
        }
        if (name == kOfxParamPropEnabled) {
          setEnabled();
        }
        if (name == kOfxParamPropSecret) {
          setSecret();
        }
        if (name == kOfxParamPropDisplayMin || name == kOfxParamPropDisplayMax) {
          setDisplayRange();
        }
      }

      // copy one parameter to another
      OfxStatus Instance::copy(const Instance &instance, OfxTime offset) { 
        return kOfxStatErrMissingHostFeature; 
      }

      // copy one parameter to another, with a range
      OfxStatus Instance::copy(const Instance &instance, OfxTime offset, OfxRangeD range) { 
        return kOfxStatErrMissingHostFeature; 
      }

      void Instance::setParentInstance(Instance* instance){
        _parentInstance = instance;
      }

      Instance* Instance::getParentInstance(){
        return _parentInstance;
      }

      //
      // KeyframeParam
      // 

      OfxStatus KeyframeParam::getNumKeys(unsigned int &nKeys) const { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::getKeyTime(int nth, OfxTime& time) const { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::getKeyIndex(OfxTime time, int direction, int & index) const { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::deleteKey(OfxTime time) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::deleteAllKeys() { 
        return kOfxStatErrMissingHostFeature; 
      }

      void GroupInstance::setChildren(std::vector<Param::Instance*> children)
      {
        _children = children;
        for(std::vector<Param::Instance*>::iterator it=children.begin();it!=children.end();it++){
          if(*it){
            (*it)->setParentInstance(this);
          }
        }
      }
      
      const std::vector<Param::Instance*> &GroupInstance::getChildren() const
      {
        return _children;
      }

      //
      // Page Instance
      //

      const std::map<int,Param::Instance*> &PageInstance::getChildren() const 
      {
        // HACK!!!! this really should be done with a notify hook so we don't force
        // _children to be mutable
        if(_children.size() == 0 )
        {
          int nChildren = _properties.getDimension(kOfxParamPropPageChild);
          for(int i=0;i<nChildren;i++)
          {
            std::string childName = _properties.getStringProperty(kOfxParamPropPageChild,i);        
            Param::Instance* child = _paramSetInstance->getParam(childName);
            if(child)
              _children[i]=child;
          }
        }
        return _children;
      }

      //
      // ChoiceInstance
      //
      /// implementation of var args function
      OfxStatus ChoiceInstance::getV(va_list arg)
      {
        int *value = va_arg(arg, int*);
        return get(*value);
      }
      
      /// implementation of var args function
      OfxStatus ChoiceInstance::getV(OfxTime time, va_list arg)
      {
        int *value = va_arg(arg, int*);
        return get(time, *value);
      }
      
      /// implementation of var args function
      OfxStatus ChoiceInstance::setV(va_list arg)
      {
        int value = va_arg(arg, int);
        return set(value);
      }

      /// implementation of var args function
      OfxStatus ChoiceInstance::setV(OfxTime time, va_list arg)
      {
        int value = va_arg(arg, int);
        return set(time, value);
      }
      
      //
      // IntegerInstance
      //
      OfxStatus IntegerInstance::derive(OfxTime time, int&) { 
        return kOfxStatErrUnsupported; 
      }

      OfxStatus IntegerInstance::integrate(OfxTime time1, OfxTime time2, int&) { 
        return kOfxStatErrUnsupported; 
      }

      /// implementation of var args function
      OfxStatus IntegerInstance::getV(va_list arg)
      {
        int *value = va_arg(arg, int*);
        return get(*value);
      }
      
      /// implementation of var args function
      OfxStatus IntegerInstance::getV(OfxTime time, va_list arg)
      {
        int *value = va_arg(arg, int*);
        return get(time, *value);
      }
      
      /// implementation of var args function
      OfxStatus IntegerInstance::setV(va_list arg)
      {
        int value = va_arg(arg, int);
        return set(value);
      }

      /// implementation of var args function
      OfxStatus IntegerInstance::setV(OfxTime time, va_list arg)
      {
        int value = va_arg(arg, int);
        return set(time, value);
      }
      
      /// implementation of var args function
      OfxStatus IntegerInstance::deriveV(OfxTime time, va_list arg)
      {
        int *value = va_arg(arg, int*);
        return derive(time, *value);
      }
      
      /// implementation of var args function
      OfxStatus IntegerInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        int *value = va_arg(arg, int*);
        return integrate(time1, time2, *value);
      }

      //
      // DoubleInstance
      //
      /// implementation of var args function
      OfxStatus DoubleInstance::getV(va_list arg)
      {
        double *value = va_arg(arg, double*);
        return get(*value);
      }
      
      /// implementation of var args function
      OfxStatus DoubleInstance::getV(OfxTime time, va_list arg)
      {
        double *value = va_arg(arg, double*);
        return get(time, *value);
      }
      
      /// implementation of var args function
      OfxStatus DoubleInstance::setV(va_list arg)
      {
        double value = va_arg(arg, double);
        return set(value);
      }

      /// implementation of var args function
      OfxStatus DoubleInstance::setV(OfxTime time, va_list arg)
      {
        double value = va_arg(arg, double);
        return set(time, value);
      }
      
      /// implementation of var args function
      OfxStatus DoubleInstance::deriveV(OfxTime time, va_list arg)
      {
        double *value = va_arg(arg, double*);
        return derive(time, *value);
      }
      
      /// implementation of var args function
      OfxStatus DoubleInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        double *value = va_arg(arg, double*);
        return integrate(time1, time2, *value);
      }

      //
      // BooleanInstance
      //
      /// implementation of var args function
      OfxStatus BooleanInstance::getV(va_list arg)
      {
        bool v;
        OfxStatus stat = get(v);

        int *value = va_arg(arg, int*);
        *value = v;
        return stat;
      }
      
      /// implementation of var args function
      OfxStatus BooleanInstance::getV(OfxTime time, va_list arg)
      {
        bool v;
        OfxStatus stat = get(time, v);

        int *value = va_arg(arg, int*);
        *value = v;
        return stat;
      }
      
      /// implementation of var args function
      OfxStatus BooleanInstance::setV(va_list arg)
      {
        bool value = va_arg(arg, int) != 0;
        return set(value);
      }

      /// implementation of var args function
      OfxStatus BooleanInstance::setV(OfxTime time, va_list arg)
      {
        bool value = va_arg(arg, int) != 0;
        return set(time, value);
      }
      

      // 
      // RGBAInstance
      // 

      OfxStatus RGBAInstance::derive(OfxTime time, double&, double&, double&, double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus RGBAInstance::integrate(OfxTime time1, OfxTime time2, double&,double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      /// implementation of var args function
      OfxStatus RGBAInstance::getV(va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        double *a = va_arg(arg, double*);
        return get(*r, *g, *b, *a);
      }
      
      /// implementation of var args function
      OfxStatus RGBAInstance::getV(OfxTime time, va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        double *a = va_arg(arg, double*);
        return get(time, *r, *g, *b, *a);
      }
      
      /// implementation of var args function
      OfxStatus RGBAInstance::setV(va_list arg)
      {
        double r = va_arg(arg, double);
        double g = va_arg(arg, double);
        double b = va_arg(arg, double);
        double a = va_arg(arg, double);
        return set(r, g, b, a);
      }

      /// implementation of var args function
      OfxStatus RGBAInstance::setV(OfxTime time, va_list arg)
      {
        double r = va_arg(arg, double);
        double g = va_arg(arg, double);
        double b = va_arg(arg, double);
        double a = va_arg(arg, double);
        return set(time, r, g, b, a);
      }
      
      /// implementation of var args function
      OfxStatus RGBAInstance::deriveV(OfxTime time, va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        double *a = va_arg(arg, double*);
        return derive(time, *r, *g, *b, *a);
      }
      
      /// implementation of var args function
      OfxStatus RGBAInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        double *a = va_arg(arg, double*);
        return integrate(time1, time2, *r, *g, *b, *a);
      }

      //
      // RGBInstance
      //
      OfxStatus RGBInstance::derive(OfxTime time, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus RGBInstance::integrate(OfxTime time1, OfxTime time2, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      /// implementation of var args function
      OfxStatus RGBInstance::getV(va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        return get(*r, *g, *b);
      }
      
      /// implementation of var args function
      OfxStatus RGBInstance::getV(OfxTime time, va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        return get(time, *r, *g, *b);
      }
      
      /// implementation of var args function
      OfxStatus RGBInstance::setV(va_list arg)
      {
        double r = va_arg(arg, double);
        double g = va_arg(arg, double);
        double b = va_arg(arg, double);
        return set(r, g, b);
      }

      /// implementation of var args function
      OfxStatus RGBInstance::setV(OfxTime time, va_list arg)
      {
        double r = va_arg(arg, double);
        double g = va_arg(arg, double);
        double b = va_arg(arg, double);
        return set(time, r, g, b);
      }
      
      /// implementation of var args function
      OfxStatus RGBInstance::deriveV(OfxTime time, va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        return derive(time, *r, *g, *b);
      }
      
      /// implementation of var args function
      OfxStatus RGBInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        double *r = va_arg(arg, double*);
        double *g = va_arg(arg, double*);
        double *b = va_arg(arg, double*);
        return integrate(time1, time2, *r, *g, *b);
      }

      //
      // Double2DInstance
      //

      OfxStatus Double2DInstance::derive(OfxTime time, double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Double2DInstance::integrate(OfxTime time1, OfxTime time2, double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Double2DInstance::getV(va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        return get(*value1, *value2);
      }
      
      /// implementation of var args function
      OfxStatus Double2DInstance::getV(OfxTime time, va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        return get(time, *value1, *value2);
      }
      
      /// implementation of var args function
      OfxStatus Double2DInstance::setV(va_list arg)
      {
        double value1 = va_arg(arg, double);
        double value2 = va_arg(arg, double);
        return set(value1, value2);
      }

      /// implementation of var args function
      OfxStatus Double2DInstance::setV(OfxTime time, va_list arg)
      {
        double value1 = va_arg(arg, double);
        double value2 = va_arg(arg, double);
        return set(time, value1, value2);
      }
      
      /// implementation of var args function
      OfxStatus Double2DInstance::deriveV(OfxTime time, va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        return derive(time, *value1, *value2);
      }
      
      /// implementation of var args function
      OfxStatus Double2DInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        return integrate(time1, time2, *value1, *value2);
      }

      //
      // Integer2DInstance
      //

      OfxStatus Integer2DInstance::derive(OfxTime time, int&,int&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Integer2DInstance::integrate(OfxTime time1, OfxTime time2, int&,int&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Integer2DInstance::getV(va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        return get(*value1, *value2);
      }
      
      /// implementation of var args function
      OfxStatus Integer2DInstance::getV(OfxTime time, va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        return get(time, *value1, *value2);
      }
      
      /// implementation of var args function
      OfxStatus Integer2DInstance::setV(va_list arg)
      {
        int value1 = va_arg(arg, int);
        int value2 = va_arg(arg, int);
        return set(value1, value2);
      }

      /// implementation of var args function
      OfxStatus Integer2DInstance::setV(OfxTime time, va_list arg)
      {
        int value1 = va_arg(arg, int);
        int value2 = va_arg(arg, int);
        return set(time, value1, value2);
      }
      
      /// implementation of var args function
      OfxStatus Integer2DInstance::deriveV(OfxTime time, va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        return derive(time, *value1, *value2);
      }
      
      /// implementation of var args function
      OfxStatus Integer2DInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        return integrate(time1, time2, *value1, *value2);
      }

      //
      // Double3DInstance
      //

      OfxStatus Double3DInstance::derive(OfxTime time, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Double3DInstance::integrate(OfxTime time1, OfxTime time2, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Double3DInstance::getV(va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        double *value3 = va_arg(arg, double*);
        return get(*value1, *value2, *value3);
      }
      
      /// implementation of var args function
      OfxStatus Double3DInstance::getV(OfxTime time, va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        double *value3 = va_arg(arg, double*);
        return get(time, *value1, *value2, *value3);
      }
      
      /// implementation of var args function
      OfxStatus Double3DInstance::setV(va_list arg)
      {
        double value1 = va_arg(arg, double);
        double value2 = va_arg(arg, double);
        double value3 = va_arg(arg, double);
        return set(value1, value2, value3);
      }

      /// implementation of var args function
      OfxStatus Double3DInstance::setV(OfxTime time, va_list arg)
      {
        double value1 = va_arg(arg, double);
        double value2 = va_arg(arg, double);
        double value3 = va_arg(arg, double);
        return set(time, value1, value2, value3);
      }
      
      /// implementation of var args function
      OfxStatus Double3DInstance::deriveV(OfxTime time, va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        double *value3 = va_arg(arg, double*);
        return derive(time, *value1, *value2, *value3);
      }
      
      /// implementation of var args function
      OfxStatus Double3DInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        double *value1 = va_arg(arg, double*);
        double *value2 = va_arg(arg, double*);
        double *value3 = va_arg(arg, double*);
        return integrate(time1, time2, *value1, *value2, *value3);
      }

      //
      // Integer3DInstance
      //
      OfxStatus Integer3DInstance::derive(OfxTime time, int&,int&,int&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Integer3DInstance::integrate(OfxTime time1, OfxTime time2, int&,int&,int&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Integer3DInstance::getV(va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        int *value3 = va_arg(arg, int*);
        return get(*value1, *value2, *value3);
      }
      
      /// implementation of var args function
      OfxStatus Integer3DInstance::getV(OfxTime time, va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        int *value3 = va_arg(arg, int*);
        return get(time, *value1, *value2, *value3);
      }
      
      /// implementation of var args function
      OfxStatus Integer3DInstance::setV(va_list arg)
      {
        int value1 = va_arg(arg, int);
        int value2 = va_arg(arg, int);
        int value3 = va_arg(arg, int);
        return set(value1, value2, value3);
      }

      /// implementation of var args function
      OfxStatus Integer3DInstance::setV(OfxTime time, va_list arg)
      {
        int value1 = va_arg(arg, int);
        int value2 = va_arg(arg, int);
        int value3 = va_arg(arg, int);
        return set(time, value1, value2, value3);
      }
      
      /// implementation of var args function
      OfxStatus Integer3DInstance::deriveV(OfxTime time, va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        int *value3 = va_arg(arg, int*);
        return derive(time, *value1, *value2, *value3);
      }
      
      /// implementation of var args function
      OfxStatus Integer3DInstance::integrateV(OfxTime time1, OfxTime time2, va_list arg)
      {
        int *value1 = va_arg(arg, int*);
        int *value2 = va_arg(arg, int*);
        int *value3 = va_arg(arg, int*);
        return integrate(time1, time2, *value1, *value2, *value3);
      }

      ////////////////////////////////////////////////////////////////////////////////
      // string param
      OfxStatus StringInstance::getV(va_list arg)
      {
        const char **value = va_arg(arg, const char **);

        OfxStatus stat = get(_returnValue); // I so don't like this, temp storage should be delegated to the implementation
        *value = _returnValue.c_str();
        return stat;
      }
      
      /// implementation of var args function
      OfxStatus StringInstance::getV(OfxTime time, va_list arg)
      {
        const char **value = va_arg(arg, const char **);

        OfxStatus stat = get(time, _returnValue); // I so don't like this, temp storage should be delegated to the implementation
        *value = _returnValue.c_str();
        return stat;
      }
      
      /// implementation of var args function
      OfxStatus StringInstance::setV(va_list arg)
      {
        char *value = va_arg(arg, char*);
        return set(value);
      }

      /// implementation of var args function
      OfxStatus StringInstance::setV(OfxTime time, va_list arg)
      {
        char *value = va_arg(arg, char*);
        return set(time, value);
      }
      
      //////////////////////////////////////////////////////////////////////////////////
      // Param::SetInstance
      //

      /// ctor
      SetInstance::SetInstance()
      {}

      /// dtor. 
      SetInstance::~SetInstance()
      {
        // iterate the params and delete them
        std::list<Instance *>::iterator i;
        for(i = _paramList.begin(); i != _paramList.end(); ++i) {
          if(*i) 
            delete (*i);
        }
      }

      const std::map<std::string, Instance*> &SetInstance::getParams() const
      {
        return _params;
      }

      const std::list<Instance*> &SetInstance::getParamList() const
      {
        return _paramList;
      }

      OfxStatus SetInstance::addParam(const std::string& name, Instance* instance)
      {
        if(_params.find(name)==_params.end()){
          _params[name] = instance;
          _paramList.push_back(instance);
        }
        else
          return kOfxStatErrExists;

        return kOfxStatOK;
      }

      ////////////////////////////////////////////////////////////////////////////////
      // Suite functions below

      static OfxStatus paramDefine(OfxParamSetHandle paramSet,
                                   const char *paramType,
                                   const char *name,
                                   OfxPropertySetHandle *propertySet)
      {
        SetDescriptor *paramSetDescriptor = reinterpret_cast<SetDescriptor*>(paramSet);

        if (!paramSetDescriptor) {
          return kOfxStatErrBadHandle;
        }

        Descriptor *desc = paramSetDescriptor->paramDefine(paramType, name);

        if(desc) {
          *propertySet = desc->getPropHandle();
          return kOfxStatOK;
        }
        else 
          return kOfxStatErrUnsupported;
      }
      
      static OfxStatus paramGetHandle(OfxParamSetHandle paramSet,
                                      const char *name,
                                      OfxParamHandle *param,
                                      OfxPropertySetHandle *propertySet)
      {

        BaseSet *baseSet = reinterpret_cast<BaseSet*>(paramSet);

        if (!baseSet) {
          return kOfxStatErrBadHandle;
        }

        SetInstance *setInstance = dynamic_cast<SetInstance*>(baseSet);

        if(setInstance){          
          const std::map<std::string,Instance*>& params = setInstance->getParams();
          std::map<std::string,Instance*>::const_iterator it = params.find(name);         

          // if we can't find it return an error...
          if(it==params.end()) return kOfxStatErrUnknown;

          // get the param
          *param = (it->second)->getHandle(); 

          // get the param property set
          if(propertySet)
            *propertySet = (it->second)->getPropHandle();

          return kOfxStatOK;
        }

        SetDescriptor *setDescriptor = dynamic_cast<SetDescriptor*>(baseSet);
        
        if(setDescriptor){            
          const std::map<std::string,Descriptor*>& params = setDescriptor->getParams();
          std::map<std::string,Descriptor*>::const_iterator it = params.find(name);         
          
          // if we can't find it return an error...
          if(it==params.end()) return kOfxStatErrUnknown;
          
          // get the param
          *param = (it->second)->getHandle();  
          
          // get the param property set
          if(propertySet)
            *propertySet = (it->second)->getPropHandle();
          
          return kOfxStatOK;
        }

        return kOfxStatErrBadHandle;        
      }
      
      static OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet,
                                              OfxPropertySetHandle *propHandle)
      {
        BaseSet *baseSet = reinterpret_cast<BaseSet*>(paramSet);

        if (baseSet) {
          *propHandle = baseSet->getParamSetProps().getHandle();
          return kOfxStatOK;
        }
        return kOfxStatErrBadHandle;
      } 
      
      static OfxStatus paramGetPropertySet(OfxParamHandle param,
                                           OfxPropertySetHandle *propHandle)
      {
        Param::Instance *paramInstance = reinterpret_cast<Param::Instance*>(param);
        
        if(paramInstance && paramInstance->verifyMagic()){
          // get the param property set
          *propHandle = paramInstance->getPropHandle();

          return kOfxStatOK;
        }
        else
          return kOfxStatErrBadHandle;        
      }

      /// get the current param value
      static OfxStatus paramGetValue(OfxParamHandle  paramHandle,
                                     ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        va_list ap;
        va_start(ap,paramHandle);
        OfxStatus stat = kOfxStatErrUnsupported;

        try {
          stat = paramInstance->getV(ap);
        }
        catch(...) {}

        va_end(ap);

        return stat;
      }

      /// get the param value at a time
      static OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle,
                                           OfxTime time,
                                           ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        va_list ap;
        va_start(ap, time);
        OfxStatus stat = kOfxStatErrUnsupported;

        try {
          stat = paramInstance->getV(time, ap);
        }
        catch(...) {}

        va_end(ap);

        return stat;
      }
      
      /// get the param's derivative at the given time
      static OfxStatus paramGetDerivative(OfxParamHandle  paramHandle,
                                          OfxTime time,
                                          ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        va_list ap;
        va_start(ap, time);
        OfxStatus stat = kOfxStatErrUnsupported;

        try {
          stat = paramInstance->deriveV(time, ap);
        }
        catch(...) {}

        va_end(ap);

        return stat;
      }

      static OfxStatus paramGetIntegral(OfxParamHandle  paramHandle,
                                        OfxTime time1, OfxTime time2,
                                        ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        va_list ap;
        va_start(ap, time2);
        OfxStatus stat = kOfxStatErrUnsupported;

        try {
          stat = paramInstance->integrateV(time1, time2, ap);
        }
        catch(...) {}

        va_end(ap);

        return stat;
      }

      /// set the param's value at the 'current' time
      static OfxStatus paramSetValue(OfxParamHandle  paramHandle,
                                     ...) 
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        va_list ap;
        va_start(ap, paramHandle);
        OfxStatus stat = kOfxStatErrUnsupported;

        try {
          stat = paramInstance->setV(ap);
        }
        catch(...) {}

        va_end(ap);

        if (stat == kOfxStatOK) {
          paramInstance->getParamSetInstance()->paramChangedByPlugin(paramInstance);
        }

        return stat;
      }

      
      /// set the param's value at the indicated time, and set a key
      static OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle,
                                           OfxTime time,  // time in frames
                                           ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        va_list ap;
        va_start(ap, time);
        OfxStatus stat = kOfxStatErrUnsupported;

        try {
          stat = paramInstance->setV(time, ap);
        }
        catch(...) {}

        va_end(ap);

        if (stat == kOfxStatOK) {
          paramInstance->getParamSetInstance()->paramChangedByPlugin(paramInstance);
        }

        return stat;
      }

      static OfxStatus paramGetNumKeys(OfxParamHandle  paramHandle,
                                       unsigned int  *numberOfKeys)
      {
        Param::Instance *pInstance = reinterpret_cast<Param::Instance*>(paramHandle);

        if (!pInstance || !pInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        KeyframeParam *paramInstance = dynamic_cast<KeyframeParam*>(pInstance);
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->getNumKeys(*numberOfKeys);
      }
      
      static OfxStatus paramGetKeyTime(OfxParamHandle  paramHandle,
                                       unsigned int nthKey,
                                       OfxTime *time)
      {
        Param::Instance *pInstance = reinterpret_cast<Param::Instance*>(paramHandle);

        if (!pInstance || !pInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        KeyframeParam *paramInstance = dynamic_cast<KeyframeParam*>(pInstance);
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->getKeyTime(nthKey,*time);
      }
      
      static OfxStatus paramGetKeyIndex(OfxParamHandle  paramHandle,
                                        OfxTime time,
                                        int     direction,
                                        int    *index) 
      {
        Param::Instance *pInstance = reinterpret_cast<Param::Instance*>(paramHandle);

        if (!pInstance || !pInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        KeyframeParam *paramInstance = dynamic_cast<KeyframeParam*>(pInstance);
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->getKeyIndex(time,direction,*index);
      }
      
      static OfxStatus paramDeleteKey(OfxParamHandle  paramHandle,
                                      OfxTime time)
      {
        Param::Instance *pInstance = reinterpret_cast<Param::Instance*>(paramHandle);

        if (!pInstance || !pInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        KeyframeParam *paramInstance = dynamic_cast<KeyframeParam*>(pInstance);
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->deleteKey(time);
      }
      
      static OfxStatus paramDeleteAllKeys(OfxParamHandle  paramHandle) 
      {
        Param::Instance *pInstance = reinterpret_cast<Param::Instance*>(paramHandle);
        
        if (!pInstance || !pInstance->verifyMagic()) {
          return kOfxStatErrBadHandle;
        }

        KeyframeParam *paramInstance = dynamic_cast<KeyframeParam*>(pInstance);
        if(!paramInstance) return kOfxStatErrBadHandle;        
        return paramInstance->deleteAllKeys();
      }
      
      static OfxStatus paramCopy(OfxParamHandle  paramTo,
                                 OfxParamHandle  paramFrom, 
                                 OfxTime dstOffset, OfxRangeD *frameRange) 
      {
        Instance *paramInstanceTo = reinterpret_cast<Instance*>(paramTo);        
        Instance *paramInstanceFrom = reinterpret_cast<Instance*>(paramFrom);        

        if(!paramInstanceTo || !paramInstanceTo->verifyMagic()) return kOfxStatErrBadHandle;        
        if(!paramInstanceFrom || !paramInstanceFrom->verifyMagic()) return kOfxStatErrBadHandle;        

        if(!frameRange)
          return paramInstanceTo->copy(*paramInstanceFrom,dstOffset);
        else
          return paramInstanceTo->copy(*paramInstanceFrom,dstOffset,*frameRange);
      }
      
      static OfxStatus paramEditBegin(OfxParamSetHandle paramSet, const char *name)
      {
        SetInstance *setInstance = reinterpret_cast<SetInstance*>(paramSet);
        if(!setInstance) return kOfxStatErrBadHandle;        
        return setInstance->editBegin(std::string(name));
      }

      
      static OfxStatus paramEditEnd(OfxParamSetHandle paramSet) {
        SetInstance *setInstance = reinterpret_cast<SetInstance*>(paramSet);
        if(!setInstance) return kOfxStatErrBadHandle;        
        return setInstance->editEnd();
      }
      
      static OfxParameterSuiteV1 gParamSuiteV1 = {
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


      void *GetSuite(int version) {
        if(version ==1)
          return &gParamSuiteV1;
        return NULL;
      }

    } // Param

  } // Host

} // OFX
