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
        typedef int APIType; ///< C type of the property that is passed across the raw API
        typedef int APITypeConstless;  ///< C type of the property that is passed across the raw API, without any const it
        typedef int Type; ///< Type we actually hold and deal with the propery in everything by the raw API
        typedef int ReturnType; ///< type to return from a function call
        static const TypeEnum typeCode = eInt;
        static int kEmpty;
      };

      /// type holder, for doubles
      struct DoubleValue { 
        typedef double APIType;
        typedef double APITypeConstless;
        typedef double Type;
        typedef double ReturnType; ///< type to return from a function call
        static const TypeEnum typeCode = eDouble;
        static double kEmpty;
      };

      /// type holder, for pointers
      struct PointerValue { 
        typedef void *APIType;
        typedef void *APITypeConstless;
        typedef void *Type;
        typedef void *ReturnType; ///< type to return from a function call
        static const TypeEnum typeCode = ePointer;
        static void *kEmpty;
      };

      /// type holder, for strings
      struct StringValue { 
        typedef const char *APIType;
        typedef char *APITypeConstless;
        typedef std::string Type;
        typedef const std::string &ReturnType; ///< type to return from a function call
        static const TypeEnum typeCode = eString;
        static std::string kEmpty;
      };

      /// array representing the names of the various types, in order of TypeEnum
      extern const char *typeNames[];

      /// Sits on a property and can override the local property value when a value is being fetched
      /// only one of these can be in any property (as the thing has only a single value).
      /// We deliberately dont have a getStringPropertyN as it is somewhat more painfull and never
      /// used in practice.
      class GetHook {
      public :
        /// dtor
        virtual ~GetHook()
        {
        }

        /// We specialise this to do some magic so that it calls get string/int/double/pointer appropriately
        /// this is what is called by the propertytemplate code to fetch values out of a hook.
        template<class T> typename T::ReturnType getProperty(const std::string &name, int index=0) OFX_EXCEPTION_SPEC;

        /// We specialise this to do some magic so that it calls get int/double/pointer appropriately
        /// this is what is called by the propertytemplate code to fetch values out of a hook.
        template<class T> void getPropertyN(const std::string &name, typename T::APIType *values, int count) OFX_EXCEPTION_SPEC;

        /// override this to fetch a single value at the given index.
        virtual const std::string &getStringProperty(const std::string &name, int index = 0) OFX_EXCEPTION_SPEC;

        /// override this to fetch a single value at the given index.
        virtual int getIntProperty(const std::string &name, int index = 0) OFX_EXCEPTION_SPEC;

        /// override this to fetch a multiple values in a multi-dimension property
        virtual void getIntPropertyN(const std::string &name, int *values, int count) OFX_EXCEPTION_SPEC;

        /// override this to fetch a single value at the given index.
        virtual double getDoubleProperty(const std::string &name, int index = 0) OFX_EXCEPTION_SPEC;

        /// override this to fetch a multiple values in a multi-dimension property
        virtual void getDoublePropertyN(const std::string &name, double *values, int count) OFX_EXCEPTION_SPEC;

        /// override this to fetch a single value at the given index.
        virtual void *getPointerProperty(const std::string &name, int index = 0) OFX_EXCEPTION_SPEC;
        
        /// override this to fetch a multiple values in a multi-dimension property
        virtual void getPointerPropertyN(const std::string &name, void **values, int count) OFX_EXCEPTION_SPEC;

        /// override this to fetch the dimension size.
        virtual int getDimension(const std::string &name) OFX_EXCEPTION_SPEC;

        /// override this to handle a reset(). 
        virtual void reset(const std::string &name) OFX_EXCEPTION_SPEC;
      };

      /// Sits on a property and is called when the local property is being set.
      /// It notify or notifyN is called whenever the plugin sets a property
      /// Many of these can sit on a property, as various objects will need to know when a property
      /// has been changed.
      class NotifyHook {
      public :
        /// dtor
        virtual ~NotifyHook() {}

        /// override this to be notified when a property changes
        /// \arg name is the name of the property just set
        /// \arg singleValue is whether setProperty on a single index was call, otherwise N properties were set
        /// \arg indexOrN is the index if single value is true, or the count if singleValue is false
        virtual void notify(const std::string &name, bool singleValue, int indexOrN) OFX_EXCEPTION_SPEC = 0;
      };

      /// base class for all properties
      class Property {
      protected :
        std::string  _name;                     ///< name of this property
        TypeEnum     _type;                     ///< type of this property
        int          _dimension;                ///< the fixed dimension of this property 
        bool         _pluginReadOnly;           ///< set is forbidden through suite: value may still change between get() calls
        std::vector<NotifyHook *> _notifyHooks; ///< hooks to call whenever the property is set
        GetHook                  *_getHook;     ///< if we are not storing props locally, they are stored via fetching from here

      public :
        /// ctor
        Property(const std::string &name,
                 TypeEnum type,
                 int dimension = 1,
                 bool pluginReadOnly=false);
            
        /// copy ctor
        Property(const Property &other);

        /// dtor
        virtual ~Property()
        {
        }
        
        /// is it read only?
        bool getPluginReadOnly() const {return _pluginReadOnly; }

        /// change the state of readonlyness
        void setPluginReadOnly(bool v) {_pluginReadOnly = v;}

        /// override this to return a clone of the property
        virtual Property *deepCopy() = 0;
        
        /// get the name of this property
        const std::string &getName()
        {
          return _name;
        }
        
        /// get the type of this property
        TypeEnum getType()
        {
          return _type;
        }

        /// add a notify hook
        void addNotifyHook(NotifyHook *hook)
        {
          _notifyHooks.push_back(hook);
        }
        
        /// add a notify hook
        void setGetHook(GetHook *hook)
        {
          _getHook = hook;
        }
        
        /// call notify on the contained notify hooks
        void notify(bool single, int indexOrN);

        // get the current dimension of this property
        virtual int getDimension() = 0;

        /// get the fixed dimension of this property
        int getFixedDimension() {
          return _dimension;
        }

        /// are we a fixed dim property
        bool isFixedSize() const 
        {
          return _dimension != 0;
        }

        /// reset this property to the default
        virtual void reset() = 0;

        // get a string representing the value of this property at element nth
        virtual std::string getStringValue(int nth) = 0;
      };
      
      /// this represents a generic property.
      /// template parameter T is the type descriptor of the
      /// type of property to model.  the class holds an internal _value vector which can be used
      /// to store the values.  if set and get hooks are installed, these will be called instead
      /// of using this variable.
      template<class T>
      class PropertyTemplate : public Property
      {
      public :
        typedef typename T::Type Type; 
        typedef typename T::ReturnType ReturnType; 
        typedef typename T::APIType APIType;

      protected :
        /// this is the present value of the property
        std::vector<Type> _value;

        /// this is the default value of the property
        std::vector<Type> _defaultValue;

      public :
        /// constructor
        PropertyTemplate(const std::string &name,
                         int dimension,
                         bool pluginReadOnly,
                         APIType defaultValue);

        PropertyTemplate(const PropertyTemplate<T> &pt);
          
        PropertyTemplate<T> *deepCopy() {
          return new PropertyTemplate(*this);
        }

        ~PropertyTemplate()
        {
        }

        /// get one value
        const ReturnType getValue(int index=0) OFX_EXCEPTION_SPEC;

        // get multiple values
        void getValueN(APIType *value, int count) OFX_EXCEPTION_SPEC;

        /// get one value, without going through the getHook
        const ReturnType getValueRaw(int index=0) OFX_EXCEPTION_SPEC;

        // get multiple values, without going through the getHook
        void getValueNRaw(APIType *value, int count) OFX_EXCEPTION_SPEC;

        /// set one value
        void setValue(const Type &value, int index=0) OFX_EXCEPTION_SPEC;

        /// set multiple values
        void setValueN(APIType *value, int count) OFX_EXCEPTION_SPEC;

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

      
      /// a map of properties
      typedef std::map<std::string, Property *> PropertyMap;

      /// represents a set of properties
      class Set {
      protected :
        PropertyMap _props; ///< what we is

        /// hide assignment
        void operator=(const Set &);

        /// get property with the particular name and type.  if the property is 
        /// missing or is of the wrong type, return an error status.  if this is a sloppy
        /// property set and the property is missing, a new one will be created of the right
        /// type
        template<class T> OfxStatus fetchProperty(const std::string &name, T *&prop) const;

        /// as getProperty(), but will not create new properties even when sloppy
        template<class T> OfxStatus fetchUnderlyingProperty(const std::string &name, T *&prop) const;

        /// static functions for the suite
        template<class T> static OfxStatus propSet(OfxPropertySetHandle properties,
                                                   const char *property,
                                                   int index,
                                                   typename T::APIType value);
        
        /// static functions for the suite
        template<class T> static OfxStatus propSetN(OfxPropertySetHandle properties,
                                                    const char *property,
                                                    int count,
                                                    typename T::APIType *value);

        /// static functions for the suite
        template<class T> static OfxStatus propGet(OfxPropertySetHandle properties,
                                                   const char *property,
                                                   int index,
                                                   typename T::APITypeConstless *value);

        /// static functions for the suite
        template<class T> static OfxStatus propGetN(OfxPropertySetHandle properties,
                                                    const char *property,
                                                    int count,
                                                    typename T::APITypeConstless *value);

        /// static functions for the suite
        static OfxStatus propReset(OfxPropertySetHandle properties, const char *property);

        /// static functions for the suite
        static OfxStatus propGetDimension(OfxPropertySetHandle properties, const char *property, int *count);

        /// set a particular property
        template<class T> void setProperty(const std::string &property, int index, typename T::Type &value) 
        {
          PropertyTemplate<T> *prop = 0;
          if(fetchUnderlyingProperty(property, prop) == kOfxStatOK) {
            prop->setValue(value, index);
          }
        }

        /// get a particular property
        template<class T> typename T::ReturnType getProperty(const std::string &property, int index)  const
        {
          PropertyTemplate<T> *prop;
          if(fetchUnderlyingProperty(property, prop) == kOfxStatOK) {
            return prop->getValue(index);
          }
          return T::kEmpty;
        }

        /// get a particular property
        template<class T> typename T::ReturnType getPropertyRaw(const std::string &property, int index)  const
        {
          PropertyTemplate<T> *prop;
          if(fetchUnderlyingProperty(property, prop) == kOfxStatOK) {
            return prop->getValueRaw(index);
          }
          return T::kEmpty;
        }

      public :
        /// the suite 
        static struct OfxPropertySuiteV1 suite;

        /// take an array of of PropSpecs (which must be terminated with an entry in which
        /// ->name is null), and turn these into a Set
        explicit Set(const PropSpec *);

        /// deep copies the property set
        explicit Set(const Set &);

        /// empty ctor
        explicit Set() {};

        /// destructor
        virtual ~Set();

        /// adds a bunch of properties from PropSpec
        void addProperties(const PropSpec *);
        
        /// add one new property
        void addProperty(Property *newProp) {
          _props[newProp->getName()] = newProp;
        }

        /// grab the internal properties map
        const PropertyMap &getProperties()
        {
          return _props;
        }

        /// set the get hook for a particular property.  users may need to call particular
        /// specialised versions of this.
        void setGetHook(const std::string &s, GetHook *ghook) const;

        /// add a set hook for a particular property.  users may need to call particular
        /// specialised versions of this.
        void addNotifyHook(const std::string &name, NotifyHook *hook) const;
                
        /// get a particular string property
        const std::string &getStringProperty(const std::string &property, int index = 0) const;
        
        /// get a particular int property
        int getIntProperty(const std::string &property, int index = 0) const;
        
        /// get a particular double property
        double getDoubleProperty(const std::string &property, int index = 0) const;

        /// get a particular double property
        void *getPointerProperty(const std::string &property, int index = 0) const;

        /// get a particular string property
        const std::string &getStringPropertyRaw(const std::string &property, int index = 0) const;
        
        /// get a particular int property
        int getIntPropertyRaw(const std::string &property, int index = 0) const;
        
        /// get a particular double property
        double getDoublePropertyRaw(const std::string &property, int index = 0) const;

        /// get a particular double property
        void *getPointerPropertyRaw(const std::string &property, int index = 0) const;

        /// set a particular string property
        void setStringProperty(const std::string &property, const std::string &value, int index = 0);
        
        /// get a particular int property
        void setIntProperty(const std::string &property, int v, int index = 0);
        
        /// get a particular double property
        void setDoubleProperty(const std::string &property, double v, int index = 0);

        /// get a particular double property
        void setPointerProperty(const std::string &property,  void *v, int index = 0);
        
        /// get the dimension of a particular property
        int getDimension(const std::string &property) const;

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
