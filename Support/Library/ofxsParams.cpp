/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  The Foundry Visionmongers Ltd
  35-36 Gt Marlborough St 
  London W1V 7FN
  England
*/

/** @brief This file contains code that skins the ofx param suite */

#include "./ofxsSupportPrivate.H"

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries. */
namespace OFX {  

    /** @brief dummy page positioning parameter to be passed to @ref OFX::PageParamDescriptor::addChild */
    DummyParamDescriptor gSkipRow(kOfxParamPageSkipRow);

    /** @brief dummy page positioning parameter to be passed to @ref OFX::PageParamDescriptor::addChild */
    DummyParamDescriptor gSkipColumn(kOfxParamPageSkipColumn);

    /** @brief turns a ParamTypeEnum into the char * that raw OFX uses */
    const char *
    mapParamTypeEnumToString(ParamTypeEnum v)
    {
        switch(v) {
        case eStringParam : return kOfxParamTypeString ;
        case eIntParam : return kOfxParamTypeInteger ;
        case eInt2DParam : return kOfxParamTypeInteger2D ;
        case eInt3DParam : return kOfxParamTypeInteger3D ;
        case eDoubleParam : return kOfxParamTypeDouble ;
        case eDouble2DParam : return kOfxParamTypeDouble2D ;
        case eDouble3DParam : return kOfxParamTypeDouble3D ;
        case eRGBParam : return kOfxParamTypeRGB ;
        case eRGBAParam : return kOfxParamTypeRGBA ;
        case eBooleanParam : return kOfxParamTypeBoolean ;
        case eChoiceParam : return kOfxParamTypeChoice ;
        case eCustomParam : return kOfxParamTypeCustom ;
        case eGroupParam : return kOfxParamTypeGroup ;
        case ePageParam : return kOfxParamTypePage ;
        case ePushButtonParam : return kOfxParamTypePushButton ;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // the base class for all param descriptors

    /** @brief ctor */
    ParamDescriptor::ParamDescriptor(const std::string &name, ParamTypeEnum type, OfxPropertySetHandle props)
      : _paramName(name)
      , _paramType(type)
      , _paramProps(props)
    {
        // validate the properities on this descriptor
        if(type != eDummyParam)
            OFX::Validation::validateParameterProperties(type, props, true); 
    }

    ParamDescriptor::~ParamDescriptor()
    {
    }

    /** @brief set the label properties */
    void 
    ParamDescriptor::setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel)
    {
        _paramProps.propSetString(kOfxPropLabel, label);
        _paramProps.propSetString(kOfxPropShortLabel, shortLabel);
        _paramProps.propSetString(kOfxPropLongLabel, longLabel);
    }

    /** @brief set the param hint */
    void 
    ParamDescriptor::setHint(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropHint, v);
    }

    /** @brief set the script name, default is the name it was defined with */
    void
    ParamDescriptor::setScriptName(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropScriptName, v);
    }

    /** @brief set the secretness of the param, defaults to false */
    void 
    ParamDescriptor::setIsSecret(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropSecret, v);
    }

    /** @brief set the secretness of the param, defaults to false */
    void 
    ParamDescriptor::setEnabled(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropEnabled, v);
    }

    /** @brief set the group param that is the parent of this one, default is to be ungrouped at the root level */
    void 
    ParamDescriptor::setParent(const GroupParamDescriptor &v)
    {
        _paramProps.propSetString(kOfxParamPropParent, v.name());
    }
    

    ////////////////////////////////////////////////////////////////////////////////
    // the base class for all params that can hold a value

    /** @brief ctor */
    ValueParamDescriptor::ValueParamDescriptor(const std::string &name, ParamTypeEnum type, OfxPropertySetHandle props)
      : ParamDescriptor(name, type, props)
    {
    }

    /** @brief dtor */
    ValueParamDescriptor::~ValueParamDescriptor()
    {
    }

    /** @brief set whether the param can animate, defaults to true in most cases */
    void ValueParamDescriptor::setAnimates(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropAnimates, v);
    }

    /** @brief set whether the param is persistant, defaults to true */
    void ValueParamDescriptor::setIsPersistant(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropPersistant, v);
    }

    /** @brief Set's whether the value of the param is significant (ie: affects the rendered image), defaults to true */
    void ValueParamDescriptor::setEvaluateOnChange(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropEvaluateOnChange, v);
    }
    
    /** @brief Set's how any cache should be invalidated if the parameter is changed, defaults to eCacheInvalidateValueChange */
    void ValueParamDescriptor::setCacheInvalidation(CacheInvalidationEnum v)
    {
        switch(v) {
        case eCacheInvalidateValueChange :
            _paramProps.propSetString(kOfxParamPropCacheInvalidation, kOfxParamInvalidateValueChange);
            break;

        case eCacheInvalidateValueChangeToEnd :
            _paramProps.propSetString(kOfxParamPropCacheInvalidation, kOfxParamInvalidateValueChangeToEnd);
            break;

        case eCacheInvalidateValueAll :
            _paramProps.propSetString(kOfxParamPropCacheInvalidation, kOfxParamInvalidateAll);
            break;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // int param descriptor

    /** @brief ctor */
    IntParamDescriptor::IntParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eIntParam, props)
    {
    }

    /** @brief set the default value, default is 0 */
    void 
    IntParamDescriptor::setDefault(int v)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, v);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    IntParamDescriptor::setRange(int min, int max)
    {
        _paramProps.propSetInt(kOfxParamPropMin, min);
        _paramProps.propSetInt(kOfxParamPropMax, max);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    IntParamDescriptor::setDislayRange(int min, int max)
    {
        _paramProps.propSetInt(kOfxParamPropDisplayMin, min);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, max);
    }
  
    ////////////////////////////////////////////////////////////////////////////////
    // 2D int param descriptor

    /** @brief ctor */
    Int2DParamDescriptor::Int2DParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eInt2DParam, props)
    {
    }

    /** @brief set the default value, default is 0 */
    void 
    Int2DParamDescriptor::setDefault(int x, int y)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, x, 0);
        _paramProps.propSetInt(kOfxParamPropDefault, y, 1);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    Int2DParamDescriptor::setRange(int xmin, int ymin,
                                   int xmax, int ymax)
    {
        _paramProps.propSetInt(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropMax, ymax, 1);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Int2DParamDescriptor::setDislayRange(int xmin, int ymin,
                                         int xmax, int ymax)
    {
        _paramProps.propSetInt(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, ymax, 1);
    }
  
  
    ////////////////////////////////////////////////////////////////////////////////
    // 3D int param descriptor

    /** @brief ctor */
    Int3DParamDescriptor::Int3DParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eInt3DParam, props)
    {
    }

    /** @brief set the default value, default is 0 */
    void 
    Int3DParamDescriptor::setDefault(int x, int y, int z)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, x, 0);
        _paramProps.propSetInt(kOfxParamPropDefault, y, 1);
        _paramProps.propSetInt(kOfxParamPropDefault, z, 2);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    Int3DParamDescriptor::setRange(int xmin, int ymin, int zmin,
                                   int xmax, int ymax, int zmax)
    {
        _paramProps.propSetInt(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropMin, zmin, 2);
        _paramProps.propSetInt(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropMax, ymax, 1);
        _paramProps.propSetInt(kOfxParamPropMax, zmax, 2);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Int3DParamDescriptor::setDislayRange(int xmin, int ymin, int zmin,
                                         int xmax, int ymax, int zmax)
    {
        _paramProps.propSetInt(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropDisplayMin, zmin, 2);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, ymax, 1);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, zmax, 2);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // base class for all double param descriptors

    /** @brief hidden constructor */
    BaseDoubleParamDescriptor::BaseDoubleParamDescriptor(const std::string &name, ParamTypeEnum type, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, type, props)
    {
    }

    /** @brief set the type of the double param, defaults to eDoubleTypePlain */
    void BaseDoubleParamDescriptor::setDoubleType(DoubleTypeEnum v)
    {
        switch(v) {
        case eDoubleTypePlain :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypePlain);
            break;
        case eDoubleTypeAngle :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeAngle);
            break;
        case eDoubleTypeScale :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeScale);
            break;
        case eDoubleTypeTime :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeTime);
            break;
        case eDoubleTypeAbsoluteTime :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeAbsoluteTime);
            break;
        case eDoubleTypeNormalisedX :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedX);
            break;
        case eDoubleTypeNormalisedY :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedY);
            break;
        case eDoubleTypeNormalisedXAbsolute :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedXAbsolute);
            break;
        case eDoubleTypeNormalisedYAbsolute :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedYAbsolute);
            break;
        case eDoubleTypeNormalisedXY :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedXY);
            break;
        case eDoubleTypeNormalisedXYAbsolute :
            _paramProps.propSetString(kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedXYAbsolute);
            break;
        }
    }

    /** @brief set the sensitivity of any gui slider */
    void BaseDoubleParamDescriptor::setIncrement(double v)
    {
        _paramProps.propSetDouble(kOfxParamPropIncrement, v);
    }

    /** @brief set the number of digits printed after a decimal point in any gui */
    void BaseDoubleParamDescriptor::setDigits(int v)
    {
        _paramProps.propSetInt(kOfxParamPropDigits, v);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // double param descriptor

    /** @brief ctor */
    DoubleParamDescriptor::DoubleParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : BaseDoubleParamDescriptor(name, eDoubleParam, props)
    {
    }

    /** @brief set the default value, default is 0 */
    void 
    DoubleParamDescriptor::setDefault(double v)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, v);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    DoubleParamDescriptor::setRange(double min, double max)
    {
        _paramProps.propSetDouble(kOfxParamPropMin, min);
        _paramProps.propSetDouble(kOfxParamPropMax, max);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    DoubleParamDescriptor::setDislayRange(double min, double max)
    {
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, min);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, max);
    }
  
    ////////////////////////////////////////////////////////////////////////////////
    // 2D double param descriptor

    /** @brief ctor */
    Double2DParamDescriptor::Double2DParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : BaseDoubleParamDescriptor(name, eDouble2DParam, props)
    {
    }

    /** @brief set the default value, default is 0 */
    void 
    Double2DParamDescriptor::setDefault(double x, double y)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, x, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, y, 1);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    Double2DParamDescriptor::setRange(double xmin, double ymin,
                                      double xmax, double ymax)
    {
        _paramProps.propSetDouble(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropMax, ymax, 1);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Double2DParamDescriptor::setDislayRange(double xmin, double ymin,
                                            double xmax, double ymax)
    {
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, ymax, 1);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // 3D double param descriptor
 
    /** @brief ctor */
    Double3DParamDescriptor::Double3DParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : BaseDoubleParamDescriptor(name, eDouble3DParam, props)
    {
    }

    /** @brief set the default value, default is 0 */
    void 
    Double3DParamDescriptor::setDefault(double x, double y, double z)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, x, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, y, 1);
        _paramProps.propSetDouble(kOfxParamPropDefault, z, 2);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    Double3DParamDescriptor::setRange(double xmin, double ymin, double zmin,
                                      double xmax, double ymax, double zmax)
    {
        _paramProps.propSetDouble(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropMin, zmin, 2);
        _paramProps.propSetDouble(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropMax, ymax, 1);
        _paramProps.propSetDouble(kOfxParamPropMax, zmax, 2);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Double3DParamDescriptor::setDislayRange(double xmin, double ymin, double zmin,
                                            double xmax, double ymax, double zmax)
    {
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, zmin, 2);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, ymax, 1);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, zmax, 2);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // RGB param descriptor

    /** @brief hidden constructor */
    RGBParamDescriptor::RGBParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eRGBParam, props)
    {
    }
      
    /** @brief set the default value */
    void  RGBParamDescriptor::setDefault(double r, double g, double b)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, r, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, g, 1);
        _paramProps.propSetDouble(kOfxParamPropDefault, b, 2);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // RGBA param descriptor

    /** @brief hidden constructor */
    RGBAParamDescriptor::RGBAParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eRGBAParam, props)
    {
    }
      
    /** @brief set the default value */
    void  RGBAParamDescriptor::setDefault(double r, double g, double b, double a)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, r, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, g, 1);
        _paramProps.propSetDouble(kOfxParamPropDefault, b, 2);
        _paramProps.propSetDouble(kOfxParamPropDefault, a, 3);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // bool param descriptor

    /** @brief hidden constructor */
    BooleanParamDescriptor::BooleanParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eBooleanParam, props)
    {
    }
      
    /** @brief set the default value */
    void BooleanParamDescriptor::setDefault(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, int(v));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // choice param descriptor

    /** @brief hidden constructor */
    ChoiceParamDescriptor::ChoiceParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eChoiceParam, props)
    {
    }
      
    /** @brief set the default value */
    void ChoiceParamDescriptor::setDefault(int v)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, v);
    }

    /** @brief how many options do we have */
    int ChoiceParamDescriptor::nOptions(void)
    {
        int nCurrentValues = _paramProps.propGetDimension(kOfxParamPropChoiceOption);
        return nCurrentValues;
    }

    /** @brief set the default value */
    void ChoiceParamDescriptor::appendOption(const std::string &v)
    {
        int nCurrentValues = _paramProps.propGetDimension(kOfxParamPropChoiceOption);
        _paramProps.propSetString(kOfxParamPropChoiceOption, v, nCurrentValues);
    }

    /** @brief set the default value */
    void ChoiceParamDescriptor::resetOptions(void)
    {
        _paramProps.propReset(kOfxParamPropChoiceOption);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // string param descriptor

    /** @brief hidden ctor */
    StringParamDescriptor::StringParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eStringParam, props)
    {
    }
  
    /** @brief set the default value, default is 0 */
    void StringParamDescriptor::setDefault(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropDefault, v);
    }

    /** @brief sets the kind of the string param, defaults to eStringSingleLine */
    void StringParamDescriptor::setStringType(StringTypeEnum v)
    {
        switch (v) {
        case eStringTypeSingleLine :
            _paramProps.propSetString(kOfxParamPropStringMode,  kOfxParamStringIsSingleLine);
            break;
        case eStringTypeMultiLine :
            _paramProps.propSetString(kOfxParamPropStringMode,  kOfxParamStringIsMultiLine);
            break;
        case eStringTypeFilePath :
            _paramProps.propSetString(kOfxParamPropStringMode,  kOfxParamStringIsFilePath);
            break;
        case eStringTypeDirectoryPath :
            _paramProps.propSetString(kOfxParamPropStringMode,  kOfxParamStringIsDirectoryPath);
            break;
        case eStringTypeLabel :
            _paramProps.propSetString(kOfxParamPropStringMode,  kOfxParamStringIsLabel);
            break;
        }
    }

    /** @brief if the string param is a file path, say that we are picking an existing file, defaults to true */
    void StringParamDescriptor::setFilePathExists(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropStringFilePathExists, int(v));
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // custom param descriptor

    /** @brief hidden ctor */
    CustomParamDescriptor::CustomParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ValueParamDescriptor(name, eCustomParam, props)
    {
    }
  
    /** @brief set the default value, default is 0 */
    void CustomParamDescriptor::setDefault(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropDefault, v);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // group param descriptor

    /** @brief hidden constructor */
    GroupParamDescriptor::GroupParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ParamDescriptor(name, eGroupParam, props)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    // page param descriptor

    /** @brief hidden constructor */
    PageParamDescriptor::PageParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ParamDescriptor(name, eGroupParam, props)
    {
    }

    /** @brief adds a child parameter. Note the two existing pseudo params, gColumnSkip  and gRowSkip */
    void PageParamDescriptor::addChild(const ParamDescriptor &p)
    {
        int nKids = _paramProps.propGetDimension(kOfxParamPropPageChild);
        _paramProps.propSetString(kOfxParamPropPageChild, p.name(), nKids);
    }
  
    ////////////////////////////////////////////////////////////////////////////////
    // pushbutton param descriptor

    /** @brief hidden constructor */
    PushButtonParamDescriptor::PushButtonParamDescriptor(const std::string &name, OfxPropertySetHandle props)
      : ParamDescriptor(name, ePushButtonParam, props)
    {
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Descriptor for a set of parameters
    /** @brief hidden ctor */
    ParamSetDescriptor::ParamSetDescriptor(void)
      : _paramSetHandle(0)
    {
    }
  
    /** @brief set the param set handle */
    void
    ParamSetDescriptor::setParamSetHandle(OfxParamSetHandle h)
    {
        // set me handle
        _paramSetHandle = h;

        if(h) {
            // fetch me props
            OfxPropertySetHandle props;
            OfxStatus stat = OFX::Private::gParamSuite->paramSetGetPropertySet(h, &props);
            _paramSetProps.propSetHandle(props);
            throwSuiteStatusException(stat);
        }
        else {
            _paramSetProps.propSetHandle(0);
        }
    }
  
    /** @brief dtor */
    ParamSetDescriptor::~ParamSetDescriptor()
    {
        // delete any descriptor we may have constructed
        std::map<std::string, ParamDescriptor *>::iterator iter;
        for(iter = _definedParams.begin(); iter != _definedParams.end(); ++iter) {
            if(iter->second) {
                delete iter->second;
                iter->second = NULL;
            }
        }
    }
  
    /** @brief estabilishes the order of page params. Do it by calling it in turn for each page */
    void 
    ParamSetDescriptor::setPageParamOrder(PageParamDescriptor &p)
    {
        int nPages = _paramSetProps.propGetDimension(kOfxPluginPropParamPageOrder);
        _paramSetProps.propSetString(kOfxPluginPropParamPageOrder, p.name().c_str(), nPages);
    }


    /** @brief calls the raw OFX routine to define a param */
    void ParamSetDescriptor::defineRawParam(const std::string &name, ParamTypeEnum paramType, OfxPropertySetHandle &props)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramDefine(_paramSetHandle, mapParamTypeEnumToString(paramType), name.c_str(), &props);
        throwSuiteStatusException(stat);
    }

    /** @brief if a param has been defined in this set, go find it */
    ParamDescriptor *
    ParamSetDescriptor::findPreviouslyDefinedParam(const std::string &name)
    {
        // search
        std::map<std::string, ParamDescriptor *>::const_iterator search;
        search = _definedParams.find(name);
        if(search == _definedParams.end())
            return NULL;
        return search->second;
    }
    
    /** @brief Define an integer param, only callable from describe in context */
    IntParamDescriptor * 
    ParamSetDescriptor::defineIntParam(const std::string &name)
    { 
        IntParamDescriptor *param = NULL;
        defineParamDescriptor(name, eIntParam, param);
        return param;
    }
  
    /** @brief Define a 2D integer param */
    Int2DParamDescriptor *ParamSetDescriptor::defineInt2DParam(const std::string &name)
    {
        Int2DParamDescriptor *param = NULL;
        defineParamDescriptor(name, eInt2DParam, param);
        return param;
    }

    /** @brief Define a 3D integer param */
    Int3DParamDescriptor *ParamSetDescriptor::defineInt3DParam(const std::string &name)
    {
        Int3DParamDescriptor *param = NULL;
        defineParamDescriptor(name, eInt3DParam, param);
        return param;
    }
  
    /** @brief Define an double param, only callable from describe in context */
    DoubleParamDescriptor * 
    ParamSetDescriptor::defineDoubleParam(const std::string &name)
    { 
        DoubleParamDescriptor *param = NULL;
        defineParamDescriptor(name, eDoubleParam, param);
        return param;
    }
  
    /** @brief Define a 2D double param */
    Double2DParamDescriptor *ParamSetDescriptor::defineDouble2DParam(const std::string &name)
    {
        Double2DParamDescriptor *param = NULL;
        defineParamDescriptor(name, eDouble2DParam, param);
        return param;
    }

    /** @brief Define a 3D double param */
    Double3DParamDescriptor *ParamSetDescriptor::defineDouble3DParam(const std::string &name)
    {
        Double3DParamDescriptor *param = NULL;
        defineParamDescriptor(name, eDouble3DParam, param);
        return param;
    }
    
    /** @brief Define a string param */
    StringParamDescriptor *ParamSetDescriptor::defineStringParam(const std::string &name)
    {
        StringParamDescriptor *param = NULL;
        defineParamDescriptor(name, eStringParam, param);
        return param;
    }

    /** @brief Define a RGBA param */
    RGBAParamDescriptor *ParamSetDescriptor::defineRGBAParam(const std::string &name)
    {
        RGBAParamDescriptor *param = NULL;
        defineParamDescriptor(name, eRGBAParam, param);
        return param;
    }

    /** @brief Define an RGB  param */
    RGBParamDescriptor *ParamSetDescriptor::defineRGBParam(const std::string &name)
    {
        RGBParamDescriptor *param = NULL;
        defineParamDescriptor(name, eRGBParam, param);
        return param;
    }

    /** @brief Define a Boolean  param */
    BooleanParamDescriptor *ParamSetDescriptor::defineBooleanParam(const std::string &name)
    {
        BooleanParamDescriptor *param = NULL;
        defineParamDescriptor(name, eBooleanParam, param);
        return param;
    }

    /** @brief Define a Choice param */
    ChoiceParamDescriptor *ParamSetDescriptor::defineChoiceParam(const std::string &name)
    {
        ChoiceParamDescriptor *param = NULL;
        defineParamDescriptor(name, eChoiceParam, param);
        return param;
    }

    /** @brief Define a group param */
    GroupParamDescriptor *ParamSetDescriptor::defineGroupParam(const std::string &name)
    {
        GroupParamDescriptor *param = NULL;
        defineParamDescriptor(name, eGroupParam, param);
        return param;
    }

    /** @brief Define a Page param */
    PageParamDescriptor *ParamSetDescriptor::definePageParam(const std::string &name)
    {
        PageParamDescriptor *param = NULL;
        defineParamDescriptor(name, ePageParam, param);
        return param;
    }

    /** @brief Define a push button  param */
    PushButtonParamDescriptor *ParamSetDescriptor::definePushButtonParam(const std::string &name)
    {
        PushButtonParamDescriptor *param = NULL;
        defineParamDescriptor(name, ePushButtonParam, param);
        return param;
    }

    /** @brief Define a custom param */
    CustomParamDescriptor *ParamSetDescriptor::defineCustomParam(const std::string &name)
    {
        CustomParamDescriptor *param = NULL;
        defineParamDescriptor(name, eCustomParam, param);
        return param;
    }
  
  
    ////////////////////////////////////////////////////////////////////////////////
    /** @brief Base class for all param instances */
    Param::Param(ParamSet *paramSet, const std::string &name, ParamTypeEnum type, OfxParamHandle handle)
      : _paramSet(paramSet)
      , _paramName(name)
      , _paramType(type)
      , _paramHandle(handle)      
    {
        // fetch our property handle
        OfxPropertySetHandle propHandle;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetPropertySet(handle, &propHandle);
        throwSuiteStatusException(stat);
        _paramProps.propSetHandle(propHandle);

        // and validate the properties
        OFX::Validation::validateParameterProperties(type, _paramProps, false);
    }
  
    /** @brief dtor */
    Param::~Param()
    {
    }

    /** @brief get name */
    const std::string &Param::name(void) const
    {
        return _paramName;
    }
    
    /** @brief, set the label properties in a plugin */
    void Param::setLabels(const std::string &label, const std::string &shortLabel, const std::string &longLabel)
    {
        _paramProps.propSetString(kOfxPropLabel, label);
        _paramProps.propSetString(kOfxPropShortLabel, shortLabel);
        _paramProps.propSetString(kOfxPropLongLabel, longLabel);
    }

    /** @brief set the secretness of the param, defaults to false */
    void Param::setIsSecret(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropSecret, v);
    }

    /** @brief set the param hint */
    void Param::setHint(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropHint, v);
    }

    /** @brief whether the param is enabled */
    void Param::setEnabled(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropEnabled, v);
    }

    /** @brief fetch the labels */
    void Param::getLabels(std::string &label, std::string &shortLabel, std::string &longLabel) const
    {
        label      = _paramProps.propGetString(kOfxPropLabel);
        shortLabel = _paramProps.propGetString(kOfxPropShortLabel);
        longLabel  = _paramProps.propGetString(kOfxPropLongLabel);
    }
    
    /** @brief get whether the param is secret */
    bool Param::getIsSecret(void) const
    {
        bool v = _paramProps.propGetInt(kOfxParamPropSecret) != 0;
        return v;
    }

    /** @brief whether the param is enabled */
    bool Param::getIsEnable(void) const
    {
        bool v = _paramProps.propGetInt(kOfxParamPropEnabled) != 0;
        return v;
    }
      
    /** @brief get the param hint */
    std::string Param::getHint(void) const
    {
        std::string v  = _paramProps.propGetString(kOfxParamPropHint);
        return v;
    }

    /** @brief get the script name */
    std::string Param::getScriptName(void) const
    {
        std::string v  = _paramProps.propGetString(kOfxParamPropScriptName);
        return v;
    }
  
    /** @brief get the group param that is the parent of this one */
    GroupParam *Param::getParent(void) const
    {
        std::string v  = _paramProps.propGetString(kOfxParamPropParent);
        if(v == "") return NULL;
        return _paramSet->fetchGroupParam(v);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    /** @brief Wraps up a value holding param */
  
    /** @brief hidden constructor */
    ValueParam::ValueParam(ParamSet *paramSet, const std::string &name, ParamTypeEnum type, OfxParamHandle handle)
      : Param(paramSet, name, type, handle)
    {
    }
      
    /** @brief dtor */
    ValueParam::~ValueParam()
    {
    }

    /** @brief Set's whether the value of the param is significant (ie: affects the rendered image) */
    void 
    ValueParam::setEvaluateOnChange(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropEvaluateOnChange, v);
    }
    
    /** @brief is the param animating */
    bool 
    ValueParam::getIsAnimating(void) const
    {
        return _paramProps.propGetInt(kOfxParamPropIsAnimating) != 0;
    }

    /** @brief is the param animating */
    bool 
    ValueParam::getIsAutoKeying(void) const
    {
        return _paramProps.propGetInt(kOfxParamPropIsAutoKeying) != 0;
    }
    
    /** @brief is the param animating */
    bool 
    ValueParam::getIsPersistant(void) const
    {
        return _paramProps.propGetInt(kOfxParamPropPersistant) != 0;
    }
    
    /** @brief Get's whether the value of the param is significant (ie: affects the rendered image) */
    bool 
    ValueParam::getEvaluateOnChange(void) const
    {
        return _paramProps.propGetInt(kOfxParamPropEvaluateOnChange) != 0;
    }

    /** @brief Get's whether the value of the param is significant (ie: affects the rendered image) */
    CacheInvalidationEnum 
    ValueParam::getCacheInvalidation(void) const
    {
        std::string v = _paramProps.propGetString(kOfxParamPropCacheInvalidation);
        if(v == kOfxParamInvalidateValueChange)
            return eCacheInvalidateValueChange;
        else if(v == kOfxParamInvalidateValueChangeToEnd)
            return eCacheInvalidateValueChangeToEnd;
        else // if(v == kOfxParamInvalidateAll)
            return eCacheInvalidateValueAll;
    }

    /** @brief if the param is animating, the number of keys in it, otherwise 0 */
    unsigned int 
    ValueParam::getNumKeys(void)
    {
        unsigned int v = 0;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetNumKeys(_paramHandle, &v);
        throwSuiteStatusException(stat);
        return v;
    }

    /** @brief get the time of the nth key, nth must be between 0 and getNumKeys-1 */
    double 
    ValueParam::getKeyTime(int nthKey) throw(OFX::Exception::Suite, std::out_of_range)
    {
        double v = 0;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetKeyTime(_paramHandle, nthKey, &v);

        // oops?
        if(stat == kOfxStatFailed) throw std::out_of_range("ValueParam::getKeyTime key index out of range");
        throwSuiteStatusException(stat); 
        return v;
    }

    /** @brief find the index of a key by a time */
    int 
    ValueParam::getKeyIndex(double time, 
                            KeySearchEnum searchDir)
    {
        int v = 0;

        // turn enum into -1,0,1
        int dir = searchDir == eKeySearchBackwards ? -1 : (searchDir == eKeySearchNear ? 0 : 1);

        // call raw param function
        OfxStatus stat = OFX::Private::gParamSuite->paramGetKeyIndex(_paramHandle, time, dir, &v);

        // oops?
        if(stat == kOfxStatFailed) return -1; // if search failed, return -1
        throwSuiteStatusException(stat); 
        return v;
    }
    
    /** @brief deletes a key at the given time */
    void 
    ValueParam::deleteKeyAtTime(double time)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramDeleteKey(_paramHandle, time);
        if(stat == kOfxStatFailed) return; // if no key at time, fail quietly
        throwSuiteStatusException(stat); 
    }

    /** @brief delete all the keys */
    void 
    ValueParam::paramDeleteAllKeys(void)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramDeleteAllKeys(_paramHandle);
        throwSuiteStatusException(stat); 
    }

  
    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up an integer param */

    /** @brief hidden constructor */
    IntParam::IntParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eIntParam, handle)
    {
    }

    /** @brief set the default value */
    void IntParam::setDefault(int v)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, v);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void IntParam::setRange(int min, int max)
    {
        _paramProps.propSetInt(kOfxParamPropMin, min);
        _paramProps.propSetInt(kOfxParamPropMax, max);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void IntParam::setDislayRange(int min, int max)
    {
        _paramProps.propSetInt(kOfxParamPropDisplayMin, min);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, max);
    }

    /** @brief het the default value */
    void IntParam::getDefault(int &v)
    {    
        v = _paramProps.propGetInt(kOfxParamPropDefault);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void IntParam::getRange(int &min, int &max)
    {
        min = _paramProps.propGetInt(kOfxParamPropMin);
        max = _paramProps.propGetInt(kOfxParamPropMax);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void IntParam::getDislayRange(int &min, int &max)
    {
        min = _paramProps.propGetInt(kOfxParamPropDisplayMin);
        max = _paramProps.propGetInt(kOfxParamPropDisplayMax);
    }

    /** @brief get value */
    void IntParam::getValue(int &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &v);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void IntParam::getValueAtTime(double t, int &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &v);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void IntParam::setValue(int v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, v);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void IntParam::setValueAtTime(double t, int v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, v);
        throwSuiteStatusException(stat);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // 2D Int params

    /** @brief hidden constructor */
    Int2DParam::Int2DParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eInt2DParam, handle)
    {
    }

    /** @brief set the default value */
    void Int2DParam::setDefault(int x, int y)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, x, 0);
        _paramProps.propSetInt(kOfxParamPropDefault, y, 1);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    Int2DParam::setRange(int xmin, int ymin,
                         int xmax, int ymax)
    {
        _paramProps.propSetInt(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropMax, ymax, 1);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Int2DParam::setDislayRange(int xmin, int ymin,
                               int xmax, int ymax)
    {
        _paramProps.propSetInt(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, ymax, 1);
    }

    /** @brief het the default value */
    void Int2DParam::getDefault(int &x, int &y)
    {    
        x = _paramProps.propGetInt(kOfxParamPropDefault, 0);
        y = _paramProps.propGetInt(kOfxParamPropDefault, 1);
    }

  
    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    Int2DParam::getRange(int &xmin, int &ymin,
                         int &xmax, int &ymax)
    {
        xmin = _paramProps.propGetInt(kOfxParamPropMin, 0);
        ymin = _paramProps.propGetInt(kOfxParamPropMin, 1);
        xmax = _paramProps.propGetInt(kOfxParamPropMax, 0);
        ymax = _paramProps.propGetInt(kOfxParamPropMax, 1);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Int2DParam::getDislayRange(int &xmin, int &ymin,
                               int &xmax, int &ymax)
    {
        xmin = _paramProps.propGetInt(kOfxParamPropDisplayMin, 0);
        ymin = _paramProps.propGetInt(kOfxParamPropDisplayMin, 1);
        xmax = _paramProps.propGetInt(kOfxParamPropDisplayMax, 0);
        ymax = _paramProps.propGetInt(kOfxParamPropDisplayMax, 1);
    }

    /** @brief get value */
    void Int2DParam::getValue(int &x, int &y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &x, &y);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void Int2DParam::getValueAtTime(double t, int &x, int &y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &x, &y);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void Int2DParam::setValue(int x, int y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, x, y);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void Int2DParam::setValueAtTime(double t, int x, int y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, x, y);
        throwSuiteStatusException(stat);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // 3D Int params

    /** @brief hidden constructor */
    Int3DParam::Int3DParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eInt3DParam, handle)
    {
    }

    /** @brief set the default value */
    void Int3DParam::setDefault(int x, int y, int z)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, x, 0);
        _paramProps.propSetInt(kOfxParamPropDefault, y, 1);
        _paramProps.propSetInt(kOfxParamPropDefault, z, 2);
    }

    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    Int3DParam::setRange(int xmin, int ymin, int zmin,
                         int xmax, int ymax, int zmax)
    {
        _paramProps.propSetInt(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropMin, zmin, 2);
        _paramProps.propSetInt(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropMax, ymax, 1);
        _paramProps.propSetInt(kOfxParamPropMin, zmax, 2);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Int3DParam::setDislayRange(int xmin, int ymin, int zmin,
                               int xmax, int ymax, int zmax)
    {
        _paramProps.propSetInt(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetInt(kOfxParamPropDisplayMin, zmin, 2);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, ymax, 1);
        _paramProps.propSetInt(kOfxParamPropDisplayMax, zmax, 2);
    }

    /** @brief het the default value */
    void Int3DParam::getDefault(int &x, int &y, int &z)
    {    
        x = _paramProps.propGetInt(kOfxParamPropDefault, 0);
        y = _paramProps.propGetInt(kOfxParamPropDefault, 1);
        z = _paramProps.propGetInt(kOfxParamPropDefault, 2);
    }

  
    /** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
    void 
    Int3DParam::getRange(int &xmin, int &ymin, int &zmin,
                         int &xmax, int &ymax, int &zmax)
    {
        xmin = _paramProps.propGetInt(kOfxParamPropMin, 0);
        ymin = _paramProps.propGetInt(kOfxParamPropMin, 1);
        zmin = _paramProps.propGetInt(kOfxParamPropMin, 2);
        xmax = _paramProps.propGetInt(kOfxParamPropMax, 0);
        ymax = _paramProps.propGetInt(kOfxParamPropMax, 1);
        zmax = _paramProps.propGetInt(kOfxParamPropMax, 2);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Int3DParam::getDislayRange(int &xmin, int &ymin, int &zmin,
                               int &xmax, int &ymax, int &zmax)
    {
        xmin = _paramProps.propGetInt(kOfxParamPropDisplayMin, 0);
        ymin = _paramProps.propGetInt(kOfxParamPropDisplayMin, 1);
        zmin = _paramProps.propGetInt(kOfxParamPropDisplayMin, 2);
        xmax = _paramProps.propGetInt(kOfxParamPropDisplayMax, 0);
        ymax = _paramProps.propGetInt(kOfxParamPropDisplayMax, 1);
        zmax = _paramProps.propGetInt(kOfxParamPropDisplayMax, 2);
    }

    /** @brief get value */
    void Int3DParam::getValue(int &x, int &y, int &z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &x, &y, &z);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void Int3DParam::getValueAtTime(double t, int &x, int &y, int &z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &x, &y, &z);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void Int3DParam::setValue(int x, int y, int z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, x, y, z);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void Int3DParam::setValueAtTime(double t, int x, int y, int z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, x, y, z);
        throwSuiteStatusException(stat);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // common base to all double params
  
    /** @brief hidden constructor */
    BaseDoubleParam::BaseDoubleParam(ParamSet *paramSet, const std::string &name, ParamTypeEnum type, OfxParamHandle handle)
      : ValueParam(paramSet, name, type, handle)
    {
    }

    /** @brief set the sensitivity of any gui slider */
    void BaseDoubleParam::setIncrement(double v)
    {
        _paramProps.propSetDouble(kOfxParamPropIncrement, v);
    }

    /** @brief set the number of digits printed after a decimal point in any gui */
    void BaseDoubleParam::setDigits(int v)
    {
        _paramProps.propSetInt(kOfxParamPropDigits, v);
    }
  
    /** @brief set the sensitivity of any gui slider */
    void BaseDoubleParam::getIncrement(double &v)
    {
        v = _paramProps.propGetDouble(kOfxParamPropIncrement);
    }

    /** @brief set the number of digits printed after a decimal point in any gui */
    void BaseDoubleParam::getDigits(int &v)
    {
        v = _paramProps.propGetInt(kOfxParamPropDigits);
    }

    /** @brief set the number of digits printed after a decimal point in any gui */
    void BaseDoubleParam::getDoubleType(DoubleTypeEnum &v)
    {
        std::string str = _paramProps.propGetString(kOfxParamPropDoubleType);

        if(str == kOfxParamDoubleTypePlain)
            v = eDoubleTypePlain;
        else if(str == kOfxParamDoubleTypeAngle)
            v = eDoubleTypeAngle;
        else if(str == kOfxParamDoubleTypeScale)
            v = eDoubleTypeScale;
        else if(str == kOfxParamDoubleTypeTime)
            v = eDoubleTypeTime;
        else if(str == kOfxParamDoubleTypeAbsoluteTime)
            v = eDoubleTypeAbsoluteTime;
        else if(str == kOfxParamDoubleTypeNormalisedX)
            v = eDoubleTypeNormalisedX;
        else if(str == kOfxParamDoubleTypeNormalisedY)
            v = eDoubleTypeNormalisedY;
        else if(str == kOfxParamDoubleTypeNormalisedXAbsolute)
            v = eDoubleTypeNormalisedXAbsolute;
        else if(str == kOfxParamDoubleTypeNormalisedYAbsolute)
            v = eDoubleTypeNormalisedYAbsolute;
        else if(str == kOfxParamDoubleTypeNormalisedXY)
            v = eDoubleTypeNormalisedXY;
        else if(str == kOfxParamDoubleTypeNormalisedXYAbsolute)
            v = eDoubleTypeNormalisedXYAbsolute;
        else
            v = eDoubleTypePlain;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up an double param */

    /** @brief hidden constructor */
    DoubleParam::DoubleParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : BaseDoubleParam(paramSet, name, eDoubleParam, handle)
    {
    }

    /** @brief set the default value */
    void DoubleParam::setDefault(double v)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, v);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void DoubleParam::setRange(double min, double max)
    {
        _paramProps.propSetDouble(kOfxParamPropMin, min);
        _paramProps.propSetDouble(kOfxParamPropMax, max);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void DoubleParam::setDislayRange(double min, double max)
    {
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, min);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, max);
    }

    /** @brief het the default value */
    void DoubleParam::getDefault(double &v)
    {    
        v = _paramProps.propGetDouble(kOfxParamPropDefault);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void DoubleParam::getRange(double &min, double &max)
    {
        min = _paramProps.propGetDouble(kOfxParamPropMin);
        max = _paramProps.propGetDouble(kOfxParamPropMax);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void DoubleParam::getDislayRange(double &min, double &max)
    {
        min = _paramProps.propGetDouble(kOfxParamPropDisplayMin);
        max = _paramProps.propGetDouble(kOfxParamPropDisplayMax);
    }

    /** @brief get value */
    void DoubleParam::getValue(double &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &v);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void DoubleParam::getValueAtTime(double t, double &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &v);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void DoubleParam::setValue(double v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, v);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void DoubleParam::setValueAtTime(double t, double v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, v);
        throwSuiteStatusException(stat);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // 2D Double params

    /** @brief hidden constructor */
    Double2DParam::Double2DParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : BaseDoubleParam(paramSet, name, eDouble2DParam, handle)
    {
    }

    /** @brief set the default value */
    void Double2DParam::setDefault(double x, double y)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, x, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, y, 1);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    Double2DParam::setRange(double xmin, double ymin,
                            double xmax, double ymax)
    {
        _paramProps.propSetDouble(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropMax, ymax, 1);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Double2DParam::setDislayRange(double xmin, double ymin,
                                  double xmax, double ymax)
    {
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, ymax, 1);
    }

    /** @brief het the default value */
    void Double2DParam::getDefault(double &x, double &y)
    {    
        x = _paramProps.propGetDouble(kOfxParamPropDefault, 0);
        y = _paramProps.propGetDouble(kOfxParamPropDefault, 1);
    }

  
    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    Double2DParam::getRange(double &xmin, double &ymin,
                            double &xmax, double &ymax)
    {
        xmin = _paramProps.propGetDouble(kOfxParamPropMin, 0);
        ymin = _paramProps.propGetDouble(kOfxParamPropMin, 1);
        xmax = _paramProps.propGetDouble(kOfxParamPropMax, 0);
        ymax = _paramProps.propGetDouble(kOfxParamPropMax, 1);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Double2DParam::getDislayRange(double &xmin, double &ymin,
                                  double &xmax, double &ymax)
    {
        xmin = _paramProps.propGetDouble(kOfxParamPropDisplayMin, 0);
        ymin = _paramProps.propGetDouble(kOfxParamPropDisplayMin, 1);
        xmax = _paramProps.propGetDouble(kOfxParamPropDisplayMax, 0);
        ymax = _paramProps.propGetDouble(kOfxParamPropDisplayMax, 1);
    }

    /** @brief get value */
    void Double2DParam::getValue(double &x, double &y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &x, &y);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void Double2DParam::getValueAtTime(double t, double &x, double &y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &x, &y);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void Double2DParam::setValue(double x, double y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, x, y);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void Double2DParam::setValueAtTime(double t, double x, double y)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, x, y);
        throwSuiteStatusException(stat);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // 3D Double params

    /** @brief hidden constructor */
    Double3DParam::Double3DParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : BaseDoubleParam(paramSet, name, eDouble3DParam, handle)
    {
    }

    /** @brief set the default value */
    void Double3DParam::setDefault(double x, double y, double z)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, x, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, y, 1);
        _paramProps.propSetDouble(kOfxParamPropDefault, z, 2);
    }

    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    Double3DParam::setRange(double xmin, double ymin, double zmin,
                            double xmax, double ymax, double zmax)
    {
        _paramProps.propSetDouble(kOfxParamPropMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropMin, zmin, 2);
        _paramProps.propSetDouble(kOfxParamPropMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropMax, ymax, 1);
        _paramProps.propSetDouble(kOfxParamPropMin, zmax, 2);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Double3DParam::setDislayRange(double xmin, double ymin, double zmin,
                                  double xmax, double ymax, double zmax)
    {
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, xmin, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, ymin, 1);
        _paramProps.propSetDouble(kOfxParamPropDisplayMin, zmin, 2);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, xmax, 0);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, ymax, 1);
        _paramProps.propSetDouble(kOfxParamPropDisplayMax, zmax, 2);
    }

    /** @brief het the default value */
    void Double3DParam::getDefault(double &x, double &y, double &z)
    {    
        x = _paramProps.propGetDouble(kOfxParamPropDefault, 0);
        y = _paramProps.propGetDouble(kOfxParamPropDefault, 1);
        z = _paramProps.propGetDouble(kOfxParamPropDefault, 2);
    }

  
    /** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
    void 
    Double3DParam::getRange(double &xmin, double &ymin, double &zmin,
                            double &xmax, double &ymax, double &zmax)
    {
        xmin = _paramProps.propGetDouble(kOfxParamPropMin, 0);
        ymin = _paramProps.propGetDouble(kOfxParamPropMin, 1);
        zmin = _paramProps.propGetDouble(kOfxParamPropMin, 2);
        xmax = _paramProps.propGetDouble(kOfxParamPropMax, 0);
        ymax = _paramProps.propGetDouble(kOfxParamPropMax, 1);
        zmax = _paramProps.propGetDouble(kOfxParamPropMax, 2);
    }

    /** @brief set the display min and max, default is to be the same as the range param */
    void 
    Double3DParam::getDislayRange(double &xmin, double &ymin, double &zmin,
                                  double &xmax, double &ymax, double &zmax)
    {
        xmin = _paramProps.propGetDouble(kOfxParamPropDisplayMin, 0);
        ymin = _paramProps.propGetDouble(kOfxParamPropDisplayMin, 1);
        zmin = _paramProps.propGetDouble(kOfxParamPropDisplayMin, 2);
        xmax = _paramProps.propGetDouble(kOfxParamPropDisplayMax, 0);
        ymax = _paramProps.propGetDouble(kOfxParamPropDisplayMax, 1);
        zmax = _paramProps.propGetDouble(kOfxParamPropDisplayMax, 2);
    }

    /** @brief get value */
    void Double3DParam::getValue(double &x, double &y, double &z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &x, &y, &z);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void Double3DParam::getValueAtTime(double t, double &x, double &y, double &z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &x, &y, &z);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void Double3DParam::setValue(double x, double y, double z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, x, y, z);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void Double3DParam::setValueAtTime(double t, double x, double y, double z)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, x, y, z);
        throwSuiteStatusException(stat);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // RGB colour param
    /** @brief hidden constructor */
    RGBParam::RGBParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eRGBParam, handle)
    {
    }

    /** @brief set the default value */
    void RGBParam::setDefault(double r, double g, double b)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, r, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, g, 1);
        _paramProps.propSetDouble(kOfxParamPropDefault, b, 2);
    }


    /** @brief het the default value */
    void RGBParam::getDefault(double &r, double &g, double &b)
    {    
        r = _paramProps.propGetDouble(kOfxParamPropDefault, 0);
        g = _paramProps.propGetDouble(kOfxParamPropDefault, 1);
        b = _paramProps.propGetDouble(kOfxParamPropDefault, 2);
    }

    /** @brief get value */
    void RGBParam::getValue(double &r, double &g, double &b)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &r, &g, &b);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void RGBParam::getValueAtTime(double t, double &r, double &g, double &b)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &r, &g, &b);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void RGBParam::setValue(double r, double g, double b)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, r, g, b);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void RGBParam::setValueAtTime(double t, double r, double g, double b)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, r, g, b);
        throwSuiteStatusException(stat);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // RGBA colour param
    /** @brief hidden constructor */
    RGBAParam::RGBAParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eRGBAParam, handle)
    {
    }

    /** @brief set the default value */
    void RGBAParam::setDefault(double r, double g, double b, double a)
    {
        _paramProps.propSetDouble(kOfxParamPropDefault, r, 0);
        _paramProps.propSetDouble(kOfxParamPropDefault, g, 1);
        _paramProps.propSetDouble(kOfxParamPropDefault, b, 2);
        _paramProps.propSetDouble(kOfxParamPropDefault, a, 3);
    }


    /** @brief het the default value */
    void RGBAParam::getDefault(double &r, double &g, double &b, double &a)
    {    
        r = _paramProps.propGetDouble(kOfxParamPropDefault, 0);
        g = _paramProps.propGetDouble(kOfxParamPropDefault, 1);
        b = _paramProps.propGetDouble(kOfxParamPropDefault, 2);
        a = _paramProps.propGetDouble(kOfxParamPropDefault, 3);
    }

    /** @brief get value */
    void RGBAParam::getValue(double &r, double &g, double &b, double &a)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &r, &g, &b);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void RGBAParam::getValueAtTime(double t, double &r, double &g, double &b, double &a)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &r, &g, &b, &a);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void RGBAParam::setValue(double r, double g, double b, double a)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, r, g, b, a);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void RGBAParam::setValueAtTime(double t, double r, double g, double b, double a)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, r, g, b, a);
        throwSuiteStatusException(stat);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a string param */

    /** @brief hidden constructor */
    StringParam::StringParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eStringParam, handle)
    {
    }

    /** @brief set the default value */
    void StringParam::setDefault(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropDefault, v);
    }

    /** @brief het the default value */
    void StringParam::getDefault(std::string &v)
    {    
        v = _paramProps.propGetString(kOfxParamPropDefault);
    }

    /** @brief get value */
    void StringParam::getValue(std::string &v)
    {
        char *cStr;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &cStr);
        throwSuiteStatusException(stat);
        v = cStr;
    }

    /** @brief get the value at a time */
    void StringParam::getValueAtTime(double t, std::string &v)
    {
        char *cStr;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &cStr);
        throwSuiteStatusException(stat);
        v = cStr;
    }

    /** @brief set value */
    void StringParam::setValue(const std::string &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, v.c_str());
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void StringParam::setValueAtTime(double t, const std::string &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, v.c_str());
        throwSuiteStatusException(stat);
    }
  
    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a Boolean integer param */

    /** @brief hidden constructor */
    BooleanParam::BooleanParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eBooleanParam, handle)
    {
    }

    /** @brief set the default value */
    void BooleanParam::setDefault(bool v)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, v);
    }

    /** @brief het the default value */
    void BooleanParam::getDefault(bool &v)
    {    
        v = _paramProps.propGetInt(kOfxParamPropDefault) != 0;
    }

    /** @brief get value */
    void BooleanParam::getValue(bool &v)
    {
        int iVal;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &iVal);
        throwSuiteStatusException(stat);
        v = iVal != 0;
    }

    /** @brief get the value at a time */
    void BooleanParam::getValueAtTime(double t, bool &v)
    {
        int iVal;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &iVal);
        throwSuiteStatusException(stat);
        v = iVal != 0;
    }

    /** @brief set value */
    void BooleanParam::setValue(bool v)
    {
        int iVal = v;
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, iVal);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void BooleanParam::setValueAtTime(double t, bool v)
    {
        int iVal = v;
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, iVal);
        throwSuiteStatusException(stat);
    }

  
    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a choice integer param */

    /** @brief hidden constructor */
    ChoiceParam::ChoiceParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eChoiceParam, handle)
    {
    }

    /** @brief set the default value */
    void ChoiceParam::setDefault(int v)
    {
        _paramProps.propSetInt(kOfxParamPropDefault, v);
    }

    /** @brief het the default value */
    void ChoiceParam::getDefault(int &v)
    {    
        v = _paramProps.propGetInt(kOfxParamPropDefault);
    }

    /** @brief get value */
    void ChoiceParam::getValue(int &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &v);
        throwSuiteStatusException(stat);
    }

    /** @brief get the value at a time */
    void ChoiceParam::getValueAtTime(double t, int &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &v);
        throwSuiteStatusException(stat);
    }

    /** @brief set value */
    void ChoiceParam::setValue(int v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, v);
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void ChoiceParam::setValueAtTime(double t, int v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, v);
        throwSuiteStatusException(stat);
    }

    /** @brief how many options do we have */
    int ChoiceParam::nOptions(void)
    {
        int nCurrentValues = _paramProps.propGetDimension(kOfxParamPropChoiceOption);
        return nCurrentValues;
    }

    /** @brief set the default value */
    void ChoiceParam::appendOption(const std::string &v)
    {
        int nCurrentValues = _paramProps.propGetDimension(kOfxParamPropChoiceOption);
        _paramProps.propSetString(kOfxParamPropChoiceOption, v, nCurrentValues);
    }

    /** @brief set the default value */
    void ChoiceParam::resetOptions(void)
    {
        _paramProps.propReset(kOfxParamPropChoiceOption);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a custom param */

    /** @brief hidden constructor */
    CustomParam::CustomParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : ValueParam(paramSet, name, eCustomParam, handle)
    {
    }

    /** @brief set the default value */
    void CustomParam::setDefault(const std::string &v)
    {
        _paramProps.propSetString(kOfxParamPropDefault, v);
    }

    /** @brief het the default value */
    void CustomParam::getDefault(std::string &v)
    {    
        v = _paramProps.propGetString(kOfxParamPropDefault);
    }

    /** @brief get value */
    void CustomParam::getValue(std::string &v)
    {
        char *cStr;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValue(_paramHandle, &cStr);
        throwSuiteStatusException(stat);
        v = cStr;
    }

    /** @brief get the value at a time */
    void CustomParam::getValueAtTime(double t, std::string &v)
    {
        char *cStr;
        OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime(_paramHandle, t, &cStr);
        throwSuiteStatusException(stat);
        v = cStr;
    }

    /** @brief set value */
    void CustomParam::setValue(const std::string &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValue(_paramHandle, v.c_str());
        throwSuiteStatusException(stat);
    }

    /** @brief set the value at a time, implicitly adds a keyframe */
    void CustomParam::setValueAtTime(double t, const std::string &v)
    {
        OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime(_paramHandle, t, v.c_str());
        throwSuiteStatusException(stat);
    }
  
    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a group param
    /** @brief hidden constructor */
    GroupParam::GroupParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : Param(paramSet, name, eGroupParam, handle)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a page param

    /** @brief hidden constructor */
    PageParam::PageParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : Param(paramSet, name, ePageParam, handle)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Wraps up a PushButton param

    /** @brief hidden constructor */
    PushButtonParam::PushButtonParam(ParamSet *paramSet, const std::string &name, OfxParamHandle handle)
      : Param(paramSet, name, ePushButtonParam, handle)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  for a set of parameters
    /** @brief hidden ctor */
    ParamSet::ParamSet(void)
      : _paramSetHandle(0)
    {
    }
  
    /** @brief set the param set handle */
    void
    ParamSet::setParamSetHandle(OfxParamSetHandle h)
    {
        // set me handle
        _paramSetHandle = h;

        if(h) {
            // fetch me props
            OfxPropertySetHandle props;
            OfxStatus stat = OFX::Private::gParamSuite->paramSetGetPropertySet(h, &props);
            _paramSetProps.propSetHandle(props);
            throwSuiteStatusException(stat);
        }
        else {
            _paramSetProps.propSetHandle(0);
        }
    }
  
    /** @brief dtor */
    ParamSet::~ParamSet()
    {
        // delete any descriptor we may have constructed
        std::map<std::string, Param *>::iterator iter;
        for(iter = _fetchedParams.begin(); iter != _fetchedParams.end(); ++iter) {
            if(iter->second) {
                delete iter->second;
                iter->second = NULL;
            }
        }
    }
  
    /** @brief calls the raw OFX routine to fetch a param */
    void ParamSet::fetchRawParam(const std::string &name, ParamTypeEnum paramType, OfxParamHandle &handle)
    {
        OfxPropertySetHandle propHandle;

        OfxStatus stat = OFX::Private::gParamSuite->paramGetHandle(_paramSetHandle, name.c_str(), &handle, &propHandle);
        throwSuiteStatusException(stat);

        PropertySet props(propHandle);

        // make sure it is of our type
        std::string paramTypeStr = props.propGetString(kOfxParamPropType);
        if(paramTypeStr != mapParamTypeEnumToString(paramType)) {
            throw OFX::Exception::TypeRequest("Parameter exists but is of the wrong type");
        }
    }

    /** @brief if a param has been fetched in this set, go find it */
    Param *
    ParamSet::findPreviouslyFetchedParam(const std::string &name)
    {
        // search
        std::map<std::string, Param *>::const_iterator search;
        search = _fetchedParams.find(name);
        if(search == _fetchedParams.end())
            return NULL;
        return search->second;
    }
    
    /** @brief Fetch an integer param, only callable from describe in context */
    IntParam * 
    ParamSet::fetchIntParam(const std::string &name)
    { 
        IntParam *param = NULL;
        fetchParam(name, eIntParam, param);
        return param;
    }
  
    /** @brief Fetch a 2D integer param */
    Int2DParam *ParamSet::fetchInt2DParam(const std::string &name)
    {
        Int2DParam *param = NULL;
        fetchParam(name, eInt2DParam, param);
        return param;
    }

    /** @brief Fetch a 3D integer param */
    Int3DParam *ParamSet::fetchInt3DParam(const std::string &name)
    {
        Int3DParam *param = NULL;
        fetchParam(name, eInt3DParam, param);
        return param;
    }
  
    /** @brief Fetch an double param, only callable from describe in context */
    DoubleParam * 
    ParamSet::fetchDoubleParam(const std::string &name)
    { 
        DoubleParam *param = NULL;
        fetchParam(name, eDoubleParam, param);
        return param;
    }
  
    /** @brief Fetch a 2D double param */
    Double2DParam *ParamSet::fetchDouble2DParam(const std::string &name)
    {
        Double2DParam *param = NULL;
        fetchParam(name, eDouble2DParam, param);
        return param;
    }

    /** @brief Fetch a 3D double param */
    Double3DParam *ParamSet::fetchDouble3DParam(const std::string &name)
    {
        Double3DParam *param = NULL;
        fetchParam(name, eDouble3DParam, param);
        return param;
    }
    
    /** @brief Fetch a string param */
    StringParam *ParamSet::fetchStringParam(const std::string &name)
    {
        StringParam *param = NULL;
        fetchParam(name, eStringParam, param);
        return param;
    }

    /** @brief Fetch a RGBA param */
    RGBAParam *ParamSet::fetchRGBAParam(const std::string &name)
    {
        RGBAParam *param = NULL;
        fetchParam(name, eRGBAParam, param);
        return param;
    }

    /** @brief Fetch an RGB  param */
    RGBParam *ParamSet::fetchRGBParam(const std::string &name)
    {
        RGBParam *param = NULL;
        fetchParam(name, eRGBParam, param);
        return param;
    }

    /** @brief Fetch a Boolean  param */
    BooleanParam *ParamSet::fetchBooleanParam(const std::string &name)
    {
        BooleanParam *param = NULL;
        fetchParam(name, eBooleanParam, param);
        return param;
    }

    /** @brief Fetch a Choice param */
    ChoiceParam *ParamSet::fetchChoiceParam(const std::string &name)
    {
        ChoiceParam *param = NULL;
        fetchParam(name, eChoiceParam, param);
        return param;
    }

    /** @brief Fetch a group param */
    GroupParam *ParamSet::fetchGroupParam(const std::string &name)
    {
        GroupParam *param = NULL;
        fetchParam(name, eGroupParam, param);
        return param;
    }

    /** @brief Fetch a Page param */
    PageParam *ParamSet::fetchPageParam(const std::string &name)
    {
        PageParam *param = NULL;
        fetchParam(name, ePageParam, param);
        return param;
    }

    /** @brief Fetch a push button  param */
    PushButtonParam *ParamSet::fetchPushButtonParam(const std::string &name)
    {
        PushButtonParam *param = NULL;
        fetchParam(name, ePushButtonParam, param);
        return param;
    }

    /** @brief Fetch a custom param */
    CustomParam *ParamSet::fetchCustomParam(const std::string &name)
    {
        CustomParam *param = NULL;
        fetchParam(name, eCustomParam, param);
        return param;
    }
};
