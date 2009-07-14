/*
Software License :

Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.

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

#include <assert.h>

#include <float.h>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhParam.h"
#include "ofxhImageEffect.h"

#include <stdarg.h>

namespace OFX {

  namespace Host {

    namespace Param {

      //
      // Base
      //
      Base::Base(const std::string &name, const std::string& type) : _paramName(name), _paramType(type)
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

      const Property::Set &Base::getProperties() const {
        return _properties;
      }

      Property::Set &Base::getProperties() {
        return _properties;
      }

      const std::string &Base::getType() const {
        return _paramType;
      } 

      const std::string &Base::getName() const  {
        return _paramName;
      }

      const std::string &Base::getParentName() const {
        return _properties.getStringProperty(kOfxParamPropParent);
      }

      const std::string &Base::getScriptName() const  {
        return _properties.getStringProperty(kOfxParamPropScriptName);
      }

      const std::string &Base::getLabel() const  {
        return _properties.getStringProperty(kOfxPropLabel);
      }

      const std::string &Base::getLongLabel() const  {
        return _properties.getStringProperty(kOfxPropLongLabel);
      }

      const std::string &Base::getShortLabel() const  {
        return _properties.getStringProperty(kOfxPropShortLabel);
      }

      const std::string &Base::getDoubleType() const  {
        return _properties.getStringProperty(kOfxParamPropDoubleType, 0);
      }

      const std::string &Base::getHint() const  {
        return _properties.getStringProperty(kOfxParamPropHint, 0);
      }

      bool Base::getEnabled() const  {
        return _properties.getIntProperty(kOfxParamPropEnabled, 0) > 0 ? true : false;
      }

      bool Base::getSecret() const  {
        return _properties.getIntProperty(kOfxParamPropSecret, 0) > 0 ? true : false;
      }

      bool Base::getEvaluateOnChange() const  {
        return _properties.getIntProperty(kOfxParamPropEvaluateOnChange, 0) > 0 ? true : false;
      }

      bool Base::getCanUndo() const  {
        if (_properties.fetchProperty(kOfxParamPropCanUndo))  {
          return _properties.getIntProperty(kOfxParamPropCanUndo) > 0 ? true : false;
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

      bool findType(const std::string paramType, Property::TypeEnum &propType, int &propDim) {
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

        Property::TypeEnum propType = Property::eString;
        int propDim = 1;
        findType(type, propType, propDim);

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
          { kOfxParamPropCanUndo,    Property::eInt,    1,    false,    "1" },
          { kOfxParamPropCacheInvalidation,    Property::eString,    1,    false,    kOfxParamInvalidateValueChange },
          { 0 }
        };

        std::ostringstream dbl_min, dbl_max, int_min, int_max;
        std::string dbl_minstr, dbl_maxstr, int_minstr, int_maxstr;

        dbl_min << -DBL_MAX;
        dbl_max << DBL_MAX;
        int_min << INT_MIN;
        int_max << INT_MAX;

        dbl_minstr = dbl_min.str();
        dbl_maxstr = dbl_max.str();
        int_minstr = int_min.str();
        int_maxstr = int_max.str();

        Property::PropSpec allNumeric[] = {
          { kOfxParamPropDisplayMin, propType, propDim, false, (propType == Property::eDouble ? dbl_minstr : int_minstr).c_str() },
          { kOfxParamPropDisplayMax, propType, propDim, false, (propType == Property::eDouble ? dbl_maxstr : int_maxstr).c_str() },
          { kOfxParamPropMin, propType, propDim, false, (propType == Property::eDouble ? dbl_minstr : int_minstr).c_str() },
          { kOfxParamPropMax, propType, propDim, false, (propType == Property::eDouble ? dbl_maxstr : int_maxstr).c_str() },
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
            _properties.setStringProperty(kOfxParamPropDimensionLabel, "X", 0);
            _properties.setStringProperty(kOfxParamPropDimensionLabel, "Y", 1);
            if (propDim == 3) {
              _properties.setStringProperty(kOfxParamPropDimensionLabel, "Z", 2);
            }
          }
        }

        if (propType == Property::eDouble) {
          _properties.addProperties(allDouble);

          if (propDim == 1) {
            _properties.addProperties(allDouble1D);
          }
        }

        if (type != kOfxParamTypeGroup && type != kOfxParamTypePage) {
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

        OFX::Host::ImageEffect::gImageEffectHost->initDescriptor(this);
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
        for(std::list<Descriptor *>::iterator it = _paramList.begin(); it != _paramList.end(); ++it)
          delete (*it);
        _paramList.clear();
      }

      const std::map<std::string, Descriptor*>& SetDescriptor::getParams() const
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

      const std::vector<Param::Instance*>& GroupInstance::getChildren() const
      {
        return _children;
      }

      const std::map<int,Param::Instance*>& PageInstance::getChildren() const
      {
        if(_children.size() == 0 )
        {
          std::map<int,Param::Instance*> children;
          int nChildren = _properties.getDimension(kOfxParamPropPageChild);
          for(int i=0;i<nChildren;i++)
          {
            std::string childName = _properties.getStringProperty(kOfxParamPropPageChild,i);        
            Param::Instance* child = _paramSetInstance->getParam(childName);
            if(child)
              children[i]=child;
          }
        }
        return _children;
      }

      //
      // IntegerInstance
      //

      OfxStatus IntegerInstance::derive(OfxTime time, int&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus IntegerInstance::integrate(OfxTime time1, OfxTime time2, int&) { 
        return kOfxStatErrMissingHostFeature; 
      }


      // 
      // RGBAInstance
      // 

      OfxStatus RGBAInstance::derive(OfxTime time, double&,double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus RGBAInstance::integrate(OfxTime time1, OfxTime time2, double&,double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
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

      //
      // Double2DInstance
      //

      OfxStatus Double2DInstance::derive(OfxTime time, double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Double2DInstance::integrate(OfxTime time1, OfxTime time2, double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
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

      //
      // Double3DInstance
      //

      OfxStatus Double3DInstance::derive(OfxTime time, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus Double3DInstance::integrate(OfxTime time1, OfxTime time2, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
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

      const std::map<std::string, Instance*>& SetInstance::getParams() const
      {
        return _params;
      }

      const std::list<Instance*>& SetInstance::getParamList() const
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

        Descriptor *param = new Descriptor(paramType, name);
        paramSetDescriptor->addParam(name,param);
        *propertySet = param->getPropHandle();
        return kOfxStatOK;
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

#define mDeclareTypedInstance(T)                                \
  T* typedParamInstance = dynamic_cast<T*>(paramInstance);

      static OfxStatus paramGetValue(OfxParamHandle  paramHandle,
        ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

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
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          bool boolV;
          OfxStatus stat = typedParamInstance->get(boolV);
          *value = boolV;
          return stat;
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
          const char **value = va_arg(ap, const char**);
          va_end(ap);

          static std::string tmp;

          mDeclareTypedInstance(StringInstance);
          OfxStatus rval = typedParamInstance->get(tmp);

          if (rval == kOfxStatOK) {
            *value = tmp.c_str();
          }
          return rval;
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

      static OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle,
        OfxTime time,
        ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

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
          int *value = va_arg(ap, int*);
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          bool boolV;
          OfxStatus stat = typedParamInstance->get(time, boolV);
          *value = boolV;
          return stat;
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
          const char **value = va_arg(ap, const char**);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          static std::string tmp;

          OfxStatus rval = typedParamInstance->get(time, tmp);

          if (rval == kOfxStatOK) {
            *value = tmp.c_str();
          }
          return rval;
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

      static OfxStatus paramGetDerivative(OfxParamHandle  paramHandle,
        OfxTime time,
        ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

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

      static OfxStatus paramGetIntegral(OfxParamHandle  paramHandle,
        OfxTime time1, OfxTime time2,
        ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance && !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

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

      static OfxStatus paramSetValue(OfxParamHandle  paramHandle,
        ...) 
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        OfxStatus rval = kOfxStatErrMissingHostFeature;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap,paramHandle);
          int value = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(IntegerInstance);
          rval = typedParamInstance->set(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap,paramHandle);
          double value = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          rval = typedParamInstance->set(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          va_list ap;

          va_start(ap,paramHandle);
          bool value = va_arg(ap, int) > 0 ? true : false;
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          rval = typedParamInstance->set(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          va_list ap;

          va_start(ap,paramHandle);
          int value = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(ChoiceInstance);
          rval = typedParamInstance->set(value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap,paramHandle);
          double r = va_arg(ap, double);
          double g = va_arg(ap, double);
          double b = va_arg(ap, double);
          double a = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          rval = typedParamInstance->set(r,g,b,a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap,paramHandle);
          double r = va_arg(ap, double);
          double g = va_arg(ap, double);
          double b = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          rval = typedParamInstance->set(r,g,b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap,paramHandle);
          double x = va_arg(ap, double);
          double y = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          rval = typedParamInstance->set(x,y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap,paramHandle);
          int x = va_arg(ap, int);
          int y = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          rval = typedParamInstance->set(x,y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap,paramHandle);
          double x = va_arg(ap, double);
          double y = va_arg(ap, double);
          double z = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          rval = typedParamInstance->set(x,y,z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap,paramHandle);
          int x = va_arg(ap, int);
          int y = va_arg(ap, int);
          int z = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          rval = typedParamInstance->set(x,y,z);
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          va_list ap;

          va_start(ap,paramHandle);
          char *value = va_arg(ap, char*);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          rval = typedParamInstance->set(value);
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

        if (rval == kOfxStatOK) {
          paramInstance->getParamSetInstance()->paramChangedByPlugin(paramInstance);
          /*
          ImageEffect::Instance *effect = paramInstance->getParamSetInstance()->getEffectInstance();
          double frame  = effect->getFrameRecursive();
          OfxPointD renderScale; effect->getRenderScaleRecursive(renderScale.x, renderScale.y);

          effect->beginInstanceChangedAction(kOfxChangePluginEdited);
          effect->paramInstanceChangedAction(paramInstance->getName(), kOfxChangePluginEdited, frame, renderScale);
          effect->endInstanceChangedAction(kOfxChangePluginEdited);
          */
        }

        return rval;
      }

      static OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle,
        OfxTime time,  // time in frames
        ...)
      {
        Instance *paramInstance = reinterpret_cast<Instance*>(paramHandle);        
        if(!paramInstance || !paramInstance->verifyMagic()) return kOfxStatErrBadHandle;

        OfxStatus rval = kOfxStatErrMissingHostFeature;

        if(paramInstance->getType()==kOfxParamTypeInteger){
          va_list ap;

          va_start(ap, time);
          int value = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(IntegerInstance);
          rval = typedParamInstance->set(time, value);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble){
          va_list ap;

          va_start(ap, time);
          double value = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(DoubleInstance);
          rval = typedParamInstance->set(time, value);
        }
        else if(paramInstance->getType()==kOfxParamTypeBoolean){
          va_list ap;

          va_start(ap, time);
          bool value = va_arg(ap, int) > 0 ? true : false;
          va_end(ap);

          mDeclareTypedInstance(BooleanInstance);
          rval = typedParamInstance->set(time, value);
        }
        else if(paramInstance->getType()==kOfxParamTypeChoice){
          va_list ap;

          va_start(ap, time);
          int value = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(ChoiceInstance);
          rval = typedParamInstance->set(time, value);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGBA){
          va_list ap;

          va_start(ap, time);
          double r = va_arg(ap, double);
          double g = va_arg(ap, double);
          double b = va_arg(ap, double);
          double a = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(RGBAInstance);
          rval = typedParamInstance->set(time,r,g,b,a);
        }
        else if(paramInstance->getType()==kOfxParamTypeRGB){
          va_list ap;

          va_start(ap, time);
          double r = va_arg(ap, double);
          double g = va_arg(ap, double);
          double b = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(RGBInstance);
          rval = typedParamInstance->set(time,r,g,b);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble2D){
          va_list ap;

          va_start(ap, time);
          double x = va_arg(ap, double);
          double y = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(Double2DInstance);
          rval = typedParamInstance->set(time,x,y);
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger2D){
          va_list ap;

          va_start(ap, time);
          int x = va_arg(ap, int);
          int y = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(Integer2DInstance);
          rval = typedParamInstance->set(time,x,y);
        }
        else if(paramInstance->getType()==kOfxParamTypeDouble3D){
          va_list ap;

          va_start(ap, time);
          double x = va_arg(ap, double);
          double y = va_arg(ap, double);
          double z = va_arg(ap, double);
          va_end(ap);

          mDeclareTypedInstance(Double3DInstance);
          rval = typedParamInstance->set(time,x,y,z);            
        }
        else if(paramInstance->getType()==kOfxParamTypeInteger3D){
          va_list ap;

          va_start(ap, time);
          int x = va_arg(ap, int);
          int y = va_arg(ap, int);
          int z = va_arg(ap, int);
          va_end(ap);

          mDeclareTypedInstance(Integer3DInstance);
          rval = typedParamInstance->set(time,x,y,z);
        }
        else if(paramInstance->getType()==kOfxParamTypeString){
          va_list ap;

          va_start(ap, time);
          char *value = va_arg(ap, char*);
          va_end(ap);

          mDeclareTypedInstance(StringInstance);
          rval = typedParamInstance->set(value);
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

        if (rval == kOfxStatOK) {
          paramInstance->getParamSetInstance()->paramChangedByPlugin(paramInstance);
        }

        return rval;
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
