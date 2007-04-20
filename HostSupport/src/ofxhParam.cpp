#include <float.h>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhParam.h"

namespace OFX {

  namespace Host {

    namespace Param {

      //
      // Base
      //

      Base::Base(const std::string& type) : 
        _paramType(type), 
        _properties(false)
      {}

      Base::Base(const std::string &type, const Property::Set &properties) :
        _paramType(type), 
        _properties(properties)
      {}


      Base::~Base() {}

      /// grab a handle on the parameter for passing to the C API
      OfxParamHandle Base::getHandle() {
        return (OfxParamHandle)this;
      }

      /// grab a handle on the properties of this parameter for the C api
      OfxPropertySetHandle Base::getPropHandle() {
        return _properties.getHandle();
      }

      Property::Set &Base::getProperties() {
        return _properties;
      }

      std::string &Base::getType() {
        return _paramType;
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

      /// make a parameter, with the given type and name
      Descriptor::Descriptor(const std::string &type, 
                                       const std::string &name) : Base(type)
      {
        const char *ctype = type.c_str();
        const char *cname = name.c_str();

        Property::TypeEnum propType = Property::eString;
        int propDim = 1;
        findType(type, propType, propDim);

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

      BaseSet::~BaseSet() {}

      /// obtain a handle on this set for passing to the C api
      OfxParamSetHandle BaseSet::getHandle() {
        return (OfxParamSetHandle)this;
      }

      std::map<std::string, Descriptor*> &SetDescriptor::getParams() 
      {
        return _params;
      }

      void SetDescriptor::addParam(const std::string &name, Descriptor *p) {
        _params[name] = p;
      }


      //
      // Instance
      //

      /// the description of a plugin parameter
      Instance::~Instance() {}

      /// make a parameter, with the given type and name
      Instance::Instance(Descriptor& descriptor) 
        : Base(descriptor.getType(),descriptor.getProperties())
      {}

      // copy one parameter to another
      OfxStatus Instance::copy(const Instance &instance, OfxTime offset) { 
        return kOfxStatErrMissingHostFeature; 
      }

      // copy one parameter to another, with a range
      OfxStatus Instance::copy(const Instance &instance, OfxTime offset, OfxRangeD range) { 
        return kOfxStatErrMissingHostFeature; 
      }

      //
      // KeyframeParam
      // 

      OfxStatus KeyframeParam::getNumKeys(unsigned int &nKeys) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::getKeyTime(int nth, OfxTime& time) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::getKeyIndex(OfxTime time, int direction, int & index) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::deleteKey(OfxTime time) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus KeyframeParam::deleteAllKeys() { 
        return kOfxStatErrMissingHostFeature; 
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

      OfxStatus derive(OfxTime time, double&,double&,double&) { 
        return kOfxStatErrMissingHostFeature; 
      }

      OfxStatus integrate(OfxTime time1, OfxTime time2, double&,double&,double&) { 
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

      //
      // SetInstance
      //

      std::map<std::string, Instance*> &SetInstance::getParams()
      {
        return _params;
      }

      OfxStatus SetInstance::addParam(const std::string& name, Instance* instance)
      {
        if(_params.find(name)==_params.end()){
          _params[name] = instance;
        }
        else
          return kOfxStatErrExists;
      }

    } // Param

  } // Host

} // OFX
