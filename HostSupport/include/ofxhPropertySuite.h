#ifndef OFX_PROPERTY_SUITE_H
#define OFX_PROPERTY_SUITE_H

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

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

#ifndef WINDOWS
#define OFX_EXCEPTION_SPEC throw (OFX::Host::Property::Exception)
#else
#define OFX_EXCEPTION_SPEC 
#endif

namespace OFX {
  namespace Host {
    namespace Property {
      template<class T> inline std::string castToString(T i) {
        std::ostringstream o;
        o << i;
        return o.str();
      }

      inline int stringToInt(const std::string &s) {
        std::istringstream is(s);
        int number;
        is >> number;
        return number;
      }

      inline double stringToDouble(const std::string &s) {
        std::istringstream is(s);
        double number;
        is >> number;
        return number;
      }
      
      class Property; 
      class Set;

      /// exception, representing an OfxStatus
      class Exception {
        OfxStatus _stat;

      public:
        /// ctor
        Exception(OfxStatus stat) : _stat(stat)
        {
        }

        /// get the status
        OfxStatus getStatus()
        {
          return _stat;
        }
      };

      /// type of a property
      enum TypeEnum {
        eNone = -1,
        eInt = 0,
        eDouble = 1,
        eString = 2,
        ePointer = 3
      };

      /// type holder, for integers
      struct IntValue { 
        typedef int OuterType;
        typedef int OuterTypeConstless;
        typedef int InnerType;
        static const TypeEnum typeCode = eInt;
      };

      /// type holder, for doubles
      struct DoubleValue { 
        typedef double OuterType;
        typedef double OuterTypeConstless;
        typedef double InnerType;
        static const TypeEnum typeCode = eDouble;
      };

      /// type holder, for pointers
      struct PointerValue { 
        typedef void *OuterType;
        typedef void *OuterTypeConstless;
        typedef void *InnerType;
        static const TypeEnum typeCode = ePointer;
      };

      /// type holder, for strings
      struct StringValue { 
        typedef const char *OuterType;
        typedef char *OuterTypeConstless;
        typedef std::string InnerType;
        static const TypeEnum typeCode = eString;
      };

      /// array representing the names of the various types, in order of TypeEnum
      static const char *typeNames[] = {
        "int", "double", "string", "pointer"
      };

      /// Sits on a property and can override the local property value when a value is being fetched
      template<class T>
      class GetHook {
      /// only one of these can be in any property (as the thing has only a single value).
      public :
        typedef typename T::OuterType OuterType;

        /// dtor
        virtual ~GetHook()
        {
        }

        /// override this to fetch a single value at the given index.
        virtual void getProperty(const std::string &name, OuterType &ret, int index) OFX_EXCEPTION_SPEC = 0;

        /// override this to get n values and put them into memory starting at first.
        virtual void getPropertyN(const std::string &name, OuterType *first, int n) OFX_EXCEPTION_SPEC = 0;

        /// override this to fetch the dimension size.
        virtual int getDimension(const std::string &name) OFX_EXCEPTION_SPEC = 0;

        /// override this to handle a reset().  this is on get() not set() deliberately, as the hook
        /// needs to know what the default value is in this case.
        virtual void reset(const std::string &name) OFX_EXCEPTION_SPEC = 0;
      };

      typedef GetHook<IntValue> IntGetHook;
      typedef GetHook<DoubleValue> DoubleGetHook;
      typedef GetHook<StringValue> StringGetHook;
      typedef GetHook<PointerValue> PointerGetHook;

      /// Sits on a property and is called when the local property is being set.
      template<class T>
      class SetHook {
      /// Many of these can sit on a property, as various objects will need to know when a property
      /// has been changed.
      public :
        typedef typename T::OuterType OuterType;

        /// dtor
        virtual ~SetHook() {
        }

        /// override this to set a single value at the given index
        virtual void setProperty(const std::string &name, OuterType value, int index) OFX_EXCEPTION_SPEC = 0;

        /// override this to get n values and put them into memory starting at first.
        virtual void setPropertyN(const std::string &name, OuterType *first, int n) OFX_EXCEPTION_SPEC = 0;
      };

      typedef SetHook<IntValue> IntSetHook;
      typedef SetHook<DoubleValue> DoubleSetHook;
      typedef SetHook<StringValue> StringSetHook;
      typedef SetHook<PointerValue> PointerSetHook;

      /// base class for all properties
      class Property {
      protected :
        std::string  _name; ///< name of this property
        TypeEnum     _type; ///< type of this property
        int          _dimension; ///< the fixed dimension of this property, 0 implies no fixed dimension
        bool         _pluginReadOnly; ///< set is forbidden through suite: value may still change between get() calls

      public :
        /// ctor
        Property(const std::string &name,
                 TypeEnum type,
                 int dimension = 1,
                 bool pluginReadOnly=false)
          : _name(name)
          , _type(type)
          , _dimension(dimension)
          , _pluginReadOnly(pluginReadOnly) {
        }

        virtual Property *deepCopy() = 0;
        
        /// dtor
        virtual ~Property()
        {
        }

        // get the name of this property
        const std::string &getName()
        {
          return _name;
        }
        
        // get the type of this property
        TypeEnum getType()
        {
          return _type;
        }

        // get the current dimension of this property
        virtual int getDimension()
        {
          return _dimension;
        }

        // reset this property to the default
        virtual void reset() = 0;

        // get a string representing the value of this property at element nth
        virtual std::string getStringValue(int nth) = 0;
      };
      
      /// this represents a generic property.
      template<class T>
      /// template parameter T is the type descriptor of the
      /// type of property to model.  the class holds an internal _value vector which can be used
      /// to store the values.  if set and get hooks are installed, these will be called instead
      /// of using this variable.
      class PropertyTemplate : public Property
      {
      public :
        typedef typename T::InnerType InnerType; 
        typedef typename T::OuterType OuterType;

      protected :
        /// this is the present value of the property
        std::vector<InnerType> _value;

        /// this is the default value of the property
        std::vector<InnerType> _defaultValue;

        /// the hook to use for getting (or null)
        GetHook<T> *_getHook;

        /// the hooks to use for setting
        std::vector<SetHook<T> *> _setHooks;
        
      public :
        /// constructor
        PropertyTemplate(const std::string &name,
                         int dimension,
                         bool pluginReadOnly,
                         OuterType defaultValue);

        PropertyTemplate(const PropertyTemplate<T> &pt)
          : Property(pt._name, pt._type, pt._dimension, pt._pluginReadOnly)
          , _value(pt._value)
          , _defaultValue(pt._defaultValue)
          , _getHook(pt._getHook)
          , _setHooks(pt._setHooks)
        {
        }
          
        PropertyTemplate<T> *deepCopy() {
          return new PropertyTemplate(*this);
        }

        ~PropertyTemplate()
        {
        }

        /// set the getting hook.  usually called via PropertySet.  a setHook must be specified at 
        /// the same time.
        void setGetHook(GetHook<T> *getHook, SetHook<T> *setHook)
        {
          _getHook = getHook;
          _setHooks.push_back(setHook);
        }

        /// add an additional setting hook
        void addSetHook(SetHook<T> *setHook)
        {
          _setHooks.push_back(setHook);
        }

        /// get one value
        const OuterType getValue(int index=0) OFX_EXCEPTION_SPEC;

        // get multiple values
        void getValueN(OuterType *value, int count) OFX_EXCEPTION_SPEC;

        /// set one value
        void setValue(const OuterType &value, int index=0) OFX_EXCEPTION_SPEC;

        /// set multiple values
        void setValueN(OuterType *value, int count) OFX_EXCEPTION_SPEC;

        /// reset 
        void reset() OFX_EXCEPTION_SPEC;
        
        /// get the size of the vector
        int getDimension() OFX_EXCEPTION_SPEC;

        std::string getStringValue(int idx) {
          return castToString(_value[idx]);
        }
      };

      typedef PropertyTemplate<IntValue> Int;
      typedef PropertyTemplate<DoubleValue> Double;
      typedef PropertyTemplate<StringValue> String;
      typedef PropertyTemplate<PointerValue> Pointer;

      /// used in creating initialised arrays to pass to set::buildFromPropSpec()
      struct PropSpec {
        const char *name;
        TypeEnum type;
        int dimension;
        bool readonly;
        const char *defaultValue;
      };

      /// represents a set of properties
      class Set {
      protected :
        bool _sloppy;

        std::map<std::string, Property *> _props;

        /// hide assignment
        void operator=(const Set &);

        /// get property with the particular name and type.  if the property is 
        /// missing or is of the wrong type, return an error status.  if this is a sloppy
        /// property set and the property is missing, a new one will be created of the right
        /// type
        template<class T> OfxStatus getProperty(const std::string &name, T *&prop);

        /// as getProperty(), but will not create new properties even when sloppy
        template<class T> OfxStatus underlyingGetProperty(const std::string &name, T *&prop);

        /// static functions for the suite
        template<class T> static OfxStatus propSet(OfxPropertySetHandle properties,
                                                   const char *property,
                                                   int index,
                                                   typename T::OuterType value);

        /// static functions for the suite
        template<class T> static OfxStatus propSetN(OfxPropertySetHandle properties,
                                                   const char *property,
                                                   int count,
                                                    typename T::OuterType *value);

        /// static functions for the suite
        template<class T> static OfxStatus propGet(OfxPropertySetHandle properties,
                                                   const char *property,
                                                   int index,
                                                   typename T::OuterTypeConstless *value);

        /// static functions for the suite
        template<class T> static OfxStatus propGetN(OfxPropertySetHandle properties,
                                                   const char *property,
                                                   int count,
                                                    typename T::OuterTypeConstless *value);

        /// static functions for the suite
        static OfxStatus propReset(OfxPropertySetHandle properties, const char *property);

        /// static functions for the suite
        static OfxStatus propGetDimension(OfxPropertySetHandle properties, const char *property, int *count);

      public :
        /// take an array of of PropSpecs (which must be terminated with an entry in which
        /// ->name is null), and turn these into a Set
        Set(const PropSpec *);

        /// create a sloppy property set (new properties can be added just by adding thing)
        Set(bool sloppy) : _sloppy(sloppy) {
        }
        
        /// deep copies the property set
        Set(const Set &);

        /// destructor
        virtual ~Set();

        /// adds a bunch of properties from PropSpec
        void addProperties(const PropSpec *);
        
        /// add one new property
        void addProperty(Property *newProp) {
          _props[newProp->getName()] = newProp;
        }

        /// grab the internal properties map
        const std::map<std::string, Property *> &getProperties()
        {
          return _props;
        }

        /// set the get hook for a particular property.  users may need to call particular
        /// specialised versions of this.
        template<class T>
        void setGetHook(const std::string &s, GetHook<T> *ghook, SetHook<T> *shook)
        {
          PropertyTemplate<T> *prop;
          if (getProperty(s, prop) != kOfxStatOK) {
            return;
          }

          prop->setGetHook(ghook,shook);
        }

        /// add a set hook for a particular property.  users may need to call particular
        /// specialised versions of this.
        template<class T>
        void addSetHook(const std::string &s, SetHook<T> *hook)
        {
          PropertyTemplate<T> *prop;
          if (getProperty(s, prop) != kOfxStatOK) {
            return;
          }

          prop->addSetHook(hook);
        }

        /// the suite 
        static struct OfxPropertySuiteV1 suite;
        
        /// set a particular property
        template<class T> void setProperty(const std::string &property, int index, typename T::OuterType value) {
          propSet<T>(this->getHandle(), property.c_str(), index, value);
        }

        /// get a particular property
        template<class T> typename T::InnerType getProperty(const std::string &property, int index) {
          typename T::OuterTypeConstless value;
          propGet<T>(this->getHandle(), property.c_str(), index, &value);
          return value;
        }

        /// get the dimension of a particular property
        int getDimension(const std::string &property)
        {
          int dim;
          propGetDimension(this->getHandle(), property.c_str(), &dim);
          return dim;
        }

        /// get a handle on this object for passing to the C API
        OfxPropertySetHandle getHandle() 
        {
          return (OfxPropertySetHandle)this;
        }
      };
    }
  }
}

#endif
