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

#include <iostream>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"

namespace OFX {
  namespace Host {
    namespace Property {

      inline int castToHook(int i) { return i; }
      inline void *castToHook(void *v) { return v; }
      inline double castToHook(double d) { return d; }
      inline const char *castToHook(const std::string &s) { return s.c_str(); }

      template<class T> PropertyTemplate<T>::PropertyTemplate(const std::string &name,
                                                              int dimension,
                                                              bool pluginReadOnly,
                                                              OuterType defaultValue)
        : Property(name, T::typeCode, dimension)
        , _getHook(0)          
      {
        
        if (dimension) {
          _value.resize(dimension);
          _defaultValue.resize(dimension);
        }
        
        if (dimension) {
          for (int i=0;i<dimension;i++) {
            _defaultValue[i] = defaultValue;
            _value[i] = defaultValue;
          }
        }
      }
      

      /// get one value
      template<class T> const typename T::OuterType PropertyTemplate<T>::getValue(int index) OFX_EXCEPTION_SPEC {
        if (_getHook) {
          OuterType t;
          _getHook->getProperty(_name, t, index);
          return t;
        } else {
          if (index < 0 || ((size_t)index >= _value.size())) {
            throw Exception(kOfxStatErrBadIndex);
          }
          return castToHook(_value[index]);
        }
      }

      // get multiple values
      template<class T> void PropertyTemplate<T>::getValueN(typename T::OuterType *value, int count) OFX_EXCEPTION_SPEC {
        if (_getHook) {
          _getHook->getPropertyN(_name, value, count);
        } else {

          size_t size = count;
          if (size > _value.size()) {
            size = _value.size();
          }

          for (size_t i=0;i<size;i++) {
            value[i] = castToHook(_value[i]);
          }

          // what if count was bigger than _value.size() ? 
        }
      }

      /// set one value
      template<class T> void PropertyTemplate<T>::setValue(const typename T::OuterType &value, int index) OFX_EXCEPTION_SPEC {

        if (!_getHook) {
          if (index < 0 || ((size_t)index >= _value.size() && _dimension)) {
            throw Exception(kOfxStatErrBadIndex);
          }
          if (_value.size() <= (size_t)index) {
            _value.resize(index+1);
          }
          _value[index] = value;
        }

        if (_setHooks.size()) {
          for (size_t i=0;i<_setHooks.size();i++) {
            _setHooks[i]->setProperty(_name, value, index);
          }
        }
      }

      /// set multiple values
      template<class T> void PropertyTemplate<T>::setValueN(typename T::OuterType *value, int count) OFX_EXCEPTION_SPEC {
        if (!_getHook) {
          if (_dimension && ((size_t)count >= _value.size())) {
            throw Exception(kOfxStatErrBadIndex);              
          }
          if (_value.size() < (size_t)count) {
            _value.resize(count);
          }
          for (int i=0;i<count;i++) {
            _value[i] = value[i];
          }
        }

        if (_setHooks.size()) {
          for (size_t i=0;i<_setHooks.size();i++) {
            _setHooks[i]->setPropertyN(_name, value, count);
          }
        }
      }
        
      /// get the size of the vector
      template <class T> int PropertyTemplate<T>::getDimension() OFX_EXCEPTION_SPEC {
        if (_dimension != 0) {
          return _dimension;
        } else {
          /// code to get it from the hook
          if (_getHook) {
            return _getHook->getDimension(_name);
          } else {
            return (int)_value.size();
          }
        }
      }

      template <class T> void PropertyTemplate<T>::reset() OFX_EXCEPTION_SPEC {
        if (_getHook) {
          _getHook->reset(_name);
        } else {
          if (_dimension) {
            _value = _defaultValue;
            for (int i=0;i<_dimension;i++) {
              for (size_t j=0;j<_setHooks.size();j++) {
                _setHooks[j]->setProperty(_name, castToHook(_value[i]), i);
              }
            }
          } else {
            _value.resize(0);
          }
        }
      }



      inline int castAwayConst(int i) { return i; }
      inline double castAwayConst(double d) { return d; }
      inline void *castAwayConst(void *v) { return v; }
      inline char *castAwayConst(const char *s) { return const_cast<char*>(s); }

      inline int *castToConst(int *i) { return i; }
      inline double *castToConst(double *d) { return d; }
      inline const char **castToConst(char **s) { return const_cast<const char**>(s); }
      inline void **castToConst(void **v) { return v; }



      template<class T> OfxStatus Set::underlyingGetProperty(const std::string&name, T *&prop) {
        if (_props.find(name) == _props.end()) {
          return kOfxStatErrUnknown;
        }
        Property *myprop = _props[name];
        T *p = dynamic_cast<T *>(myprop);
        if (p == 0) {
          return kOfxStatErrValue;
        }
        prop = p;
        return kOfxStatOK;
      }

      template<class T> OfxStatus Set::getProperty(const std::string&name, T *&prop) {
        if (_sloppy && _props.find(name) == _props.end()) {
          _props[name] = new T(name, 0, false, 0);
        }
        OfxStatus s = underlyingGetProperty(name, prop);
        return s;
      }

      void Set::addProperties(const PropSpec spec[]) {
        while (spec->name) {

          if (_props.find(spec->name) != _props.end()) {
            std::cerr << "error : duplicate names" << std::endl;
            abort();
            /// XXX error - duplicate name
          }

          switch (spec->type) {
          case eInt: 
            _props[spec->name] = new Int(spec->name, spec->dimension, spec->readonly, spec->defaultValue?atoi(spec->defaultValue):0);
            break;
          case eDouble: 
            _props[spec->name] = new Double(spec->name, spec->dimension, spec->readonly, spec->defaultValue?atof(spec->defaultValue):0);
            break;
          case eString: 
            _props[spec->name] = new String(spec->name, spec->dimension, spec->readonly, spec->defaultValue?spec->defaultValue:"");
            break;
          case ePointer: 
            _props[spec->name] = new Pointer(spec->name, spec->dimension, spec->readonly, (void*)spec->defaultValue);
            break;
          default: // XXX  error - unrecognised type
            break;
          }
          
          spec++;
        }
      }

      Set::Set(const PropSpec spec[]) {
        addProperties(spec);
      }

      Set::Set(const Set &other) : _sloppy(other._sloppy) {
        bool failed = false;

        for (std::map<std::string, Property *>::const_iterator i = other._props.begin();
             i != other._props.end();
             i++) 
          {
            Property *copyProp = i->second->deepCopy();
            if (!copyProp) {
              failed = true;
              break;
            }
            _props[i->first] = copyProp;
          }
        
        if (failed)
          
          for (std::map<std::string, Property *>::iterator j = _props.begin();
               j != _props.end();
               j++) {
            delete j->second;
          }
        
      }

      Set::~Set()
      {
        std::map<std::string, Property *>::iterator i = _props.begin();
        while (i != _props.end()) {
          delete i->second;
          i++;
        }
      }

      /// static functions for the suite
      template<class T> OfxStatus Set::propSet(OfxPropertySetHandle properties,
                                                      const char *property,
                                                      int index,
                                                      typename T::OuterType value) {          
#ifdef DEBUG
        std::cout << "propSet - " << properties << " " << property << "[" << index << "] = " << value << " \n";
#endif
        try {            
          Set *thisSet = reinterpret_cast<Set*>(properties);
          PropertyTemplate<T> *prop = 0;
          OfxStatus stat = thisSet->getProperty(property, prop);
          if (stat != kOfxStatOK) {
#ifdef DEBUG
            std::cout << " returning status " << stat << "\n";
#endif
            return stat;
          }
          prop->setValue(value, index);
        } catch (Exception e) {
#ifdef DEBUG
          std::cout << " returning status " << e.getStatus() << "\n";
#endif
          return e.getStatus();
        }
        return kOfxStatOK;
      }
      
      /// static functions for the suite
      template<class T> OfxStatus Set::propSetN(OfxPropertySetHandle properties,
                                                       const char *property,
                                                       int count,
                                                       typename T::OuterType *value) {          
#ifdef DEBUG
        std::cout << "propSetN - " << properties << " " << property << " \n";
#endif

        try {
          Set *thisSet = reinterpret_cast<Set*>(properties);
          PropertyTemplate<T> *prop = 0;
          OfxStatus stat = thisSet->getProperty(property, prop);
          if (stat != kOfxStatOK) {
            return stat;
          }
          prop->setValueN(value, count);
        } catch (Exception e) {
          return e.getStatus();
        }
        return kOfxStatOK;
      }
      
      /// static functions for the suite
      template<class T> OfxStatus Set::propGet(OfxPropertySetHandle properties,
                                                      const char *property,
                                                      int index,
                                                      typename T::OuterTypeConstless *value) {
#ifdef DEBUG
        std::cout << "propGet - " << properties << " " << property << " = ...";
#endif

        try {
          Set *thisSet = reinterpret_cast<Set*>(properties);
          PropertyTemplate<T> *prop = 0;
          OfxStatus stat = thisSet->getProperty(property, prop);
          if (stat != kOfxStatOK) {
            return stat;
          }
          *value = castAwayConst(prop->getValue(index));

#ifdef DEBUG
          std::cout << *value << "\n";
#endif
        } catch (Exception e) {

#ifdef DEBUG
          std::cout  << "\n";
#endif
          return e.getStatus();
        }
        return kOfxStatOK;
      }
      
      /// static functions for the suite
      template<class T> OfxStatus Set::propGetN(OfxPropertySetHandle properties,
                                                       const char *property,
                                                       int count,
                                                       typename T::OuterTypeConstless *value) {
        try {
          Set *thisSet = reinterpret_cast<Set*>(properties);
          PropertyTemplate<T> *prop = 0;
          OfxStatus stat = thisSet->getProperty(property, prop);
          if (stat != kOfxStatOK) {
            return stat;
          }
          prop->getValueN(castToConst(value), count);
        } catch (Exception e) {
          return e.getStatus();
        }
        return kOfxStatOK;
      }
      
      /// static functions for the suite
      OfxStatus Set::propReset(OfxPropertySetHandle properties, const char *property) {
        try {            
          Set *thisSet = reinterpret_cast<Set*>(properties);
          Property *prop = 0;
          OfxStatus stat = thisSet->underlyingGetProperty(property, prop);
          if (stat != kOfxStatOK) {
            return stat;
          }

          prop->reset();
        } catch (Exception e) {
          return e.getStatus();
        }
        return kOfxStatOK;
      }
      
      /// static functions for the suite
      OfxStatus Set::propGetDimension(OfxPropertySetHandle properties, const char *property, int *count) {
        Set *thisSet = reinterpret_cast<Set*>(properties);
        Property *prop = 0;
        OfxStatus stat = thisSet->underlyingGetProperty(property, prop);
        if (stat != kOfxStatOK) {
          return stat;
        }
        *count = prop->getDimension();
        return kOfxStatOK;
      }    

      struct OfxPropertySuiteV1 Set::suite = {
        Set::propSet<PointerValue>,
        Set::propSet<StringValue>,
        Set::propSet<DoubleValue>,
        Set::propSet<IntValue>,
        Set::propSetN<PointerValue>,
        Set::propSetN<StringValue>,
        Set::propSetN<DoubleValue>,
        Set::propSetN<IntValue>,
        Set::propGet<PointerValue>,
        Set::propGet<StringValue>,
        Set::propGet<DoubleValue>,
        Set::propGet<IntValue>,
        Set::propGetN<PointerValue>,
        Set::propGetN<StringValue>,
        Set::propGetN<DoubleValue>,
        Set::propGetN<IntValue>,
        Set::propReset,
        Set::propGetDimension
      };
    }
  }
}
