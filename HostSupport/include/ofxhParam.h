#ifndef OFXH_PARAM_H
#define OFXH_PARAM_H

#include <string>

namespace OFX {

  namespace Host {

    namespace Param {

      class Base {
      protected:
        std::string   _paramType;
        Property::Set _properties;        
      public:
        Base(const std::string &type);
        Base(const std::string &type, const Property::Set &properties);
        ~Base();

        /// grab a handle on the parameter for passing to the C API
        OfxParamHandle getHandle();
        
        /// grab a handle on the properties of this parameter for the C api
        OfxPropertySetHandle getPropHandle();

        Property::Set &getProperties();

        std::string &getType();
      };

      /// the Descriptor of a plugin parameter
      class Descriptor : public Base {
        Descriptor();
       
      public:
        /// make a parameter, with the given type and name
        explicit Descriptor(const std::string &type, const std::string &name);
 
      };
      
      class BaseSet {
      public:
        virtual ~BaseSet();

        /// obtain a handle on this set for passing to the C api
        OfxParamSetHandle getHandle();
      };

      /// a set of parameters
      class SetDescriptor : public BaseSet {
        std::map<std::string, Descriptor*> _params;
        
      public:
        std::map<std::string, Descriptor*> &getParams();
        void addParam(const std::string &name, Descriptor *p);
      };

      /// the description of a plugin parameter
      class Instance : public Base {
        Instance();                
      public:
        virtual ~Instance();

        /// make a parameter, with the given type and name
        explicit Instance(Descriptor& descriptor);
        
        // copy one parameter to another
        virtual OfxStatus copy(const Instance &instance, OfxTime offset);

        // copy one parameter to another, with a range
        virtual OfxStatus copy(const Instance &instance, OfxTime offset, OfxRangeD range);
      };

      class KeyframeParam {
      public:
        virtual OfxStatus getNumKeys(unsigned int &nKeys) ;
        virtual OfxStatus getKeyTime(int nth, OfxTime& time) ;
        virtual OfxStatus getKeyIndex(OfxTime time, int direction, int & index) ;
        virtual OfxStatus deleteKey(OfxTime time) ;
        virtual OfxStatus deleteAllKeys() ;
      };

      class IntegerInstance : public Instance, public KeyframeParam {
      public:
        // needed 
        virtual OfxStatus get(int&) = 0;
        virtual OfxStatus get(OfxTime time, int&) = 0;
        virtual OfxStatus set(int) = 0;
        virtual OfxStatus set(OfxTime time, int) = 0;

        // probably not needed
        virtual OfxStatus derive(OfxTime time, int&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, int&) ;
      };

      class ChoiceInstance : public Instance, public KeyframeParam {
      public:
        // needed 
        virtual OfxStatus get(int&) = 0;
        virtual OfxStatus get(OfxTime time, int&) = 0;
        virtual OfxStatus set(int) = 0;
        virtual OfxStatus set(OfxTime time, int) = 0;
      };

      class DoubleInstance : public Instance, public KeyframeParam {
      public:
        // needed 
        virtual OfxStatus get(double&) = 0;
        virtual OfxStatus get(OfxTime time, double&) = 0;
        virtual OfxStatus set(double) = 0;
        virtual OfxStatus set(OfxTime time, double) = 0;
        virtual OfxStatus derive(OfxTime time, double&) = 0;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, double&) = 0;
      };

      class BooleanInstance : public Instance, public KeyframeParam {
      public:
        // needed
        virtual OfxStatus get(bool&) = 0;
        virtual OfxStatus get(OfxTime time, bool&) = 0;
        virtual OfxStatus set(bool) = 0;
        virtual OfxStatus set(OfxTime time, bool) = 0;
      };

      class RGBAInstance : public Instance, public KeyframeParam {
      public:
        // needed
        virtual OfxStatus get(double&,double&,double&,double&) = 0;
        virtual OfxStatus get(OfxTime time, double&,double&,double&,double&) = 0;
        virtual OfxStatus set(double,double,double,double) = 0;
        virtual OfxStatus set(OfxTime time, double,double,double,double) = 0;

        // needed
        virtual OfxStatus derive(OfxTime time, double&,double&,double&,double&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, double&,double&,double&,double&) ;
      };

      class RGBInstance : public Instance, public KeyframeParam {
      public:
        // needed
        virtual OfxStatus get(double&,double&,double&) = 0;
        virtual OfxStatus get(OfxTime time, double&,double&,double&) = 0;
        virtual OfxStatus set(double,double,double) = 0;
        virtual OfxStatus set(OfxTime time, double,double,double) = 0;

        // not needed
        virtual OfxStatus derive(OfxTime time, double&,double&,double&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, double&,double&,double&) ;
      };
        
      class Double2DInstance : public Instance, public KeyframeParam {
      public:
        // needed
        virtual OfxStatus get(double&,double&) = 0;
        virtual OfxStatus get(OfxTime time, double&,double&) = 0;
        virtual OfxStatus set(double,double) = 0;
        virtual OfxStatus set(OfxTime time, double,double) = 0;

        // not needed
        virtual OfxStatus derive(OfxTime time, double&,double&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, double&,double&) ;
      };

      class Integer2DInstance : public Instance, public KeyframeParam {
      public:
        // needed
        virtual OfxStatus get(int&,int&) = 0;
        virtual OfxStatus get(OfxTime time, int&,int&) = 0;
        virtual OfxStatus set(int,int) = 0;
        virtual OfxStatus set(OfxTime time, int,int) = 0;

        // not needed
        virtual OfxStatus derive(OfxTime time, int&,int&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, int&,int&) ;
      };

      class Double3DInstance : public Instance , public KeyframeParam{
      public:
        // needed
        virtual OfxStatus get(double&,double&,double&)  = 0;
        virtual OfxStatus get(OfxTime time, double&,double&,double&)  = 0;
        virtual OfxStatus set(double,double,double)  = 0;
        virtual OfxStatus set(OfxTime time, double,double,double)  = 0;

        // not needed
        virtual OfxStatus derive(OfxTime time, double&,double&,double&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, double&,double&,double&) ;
      };

      class Integer3DInstance : public Instance, public KeyframeParam {
      public:
        virtual OfxStatus get(int&,int&,int&) = 0;
        virtual OfxStatus get(OfxTime time, int&,int&,int&) = 0;
        virtual OfxStatus set(int,int,int) = 0;
        virtual OfxStatus set(OfxTime time, int,int,int) = 0;

        // not needed
        virtual OfxStatus derive(OfxTime time, int&,int&,int&) ;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, int&,int&,int&) ;
      };

      class StringInstance : public Instance, public KeyframeParam {
      public:
        virtual OfxStatus get(char*&) = 0;
        virtual OfxStatus get(OfxTime time, char&) = 0;
        virtual OfxStatus set(const char*) = 0;
        virtual OfxStatus set(OfxTime time, const char*) = 0;
      };

      /// a set of parameters
      class SetInstance : public BaseSet {
      protected:
        std::map<std::string, Instance*> _params;
        
      public:
        SetInstance(SetDescriptor& descriptor);

        // get the params
        std::map<std::string, Instance*> &getParams();

        // add a param
        virtual OfxStatus addParam(const std::string& name, Instance* instance);

        // make a parameter instance
        virtual Instance* newParam(const Descriptor& Descriptor) = 0;        

        virtual OfxStatus editBegin(const std::string& name) = 0;
        virtual OfxStatus editEnd() = 0;

      };
    }
  }
}

#endif // OFXH_PARAM_H
