
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

#ifndef OFXH_PARAM_H
#define OFXH_PARAM_H

#include <string>
#include <map>
#include <list>

namespace OFX {

  namespace Host {

    namespace Param {

      /// fetch the param suite
      void *GetSuite(int version);

      /// base class for all params
      class Base {

      private:
        Base();
      protected:
        std::string   _paramName;
        std::string   _paramType;
        Property::Set _properties;        
      public:
        Base(const std::string &name, const std::string &type);
        Base(const std::string &name, const std::string &type, const Property::Set &properties);
        virtual ~Base();

        /// grab a handle on the parameter for passing to the C API
        OfxParamHandle getHandle() const;

        virtual bool verifyMagic() { return true; }

        /// grab a handle on the properties of this parameter for the C api
        OfxPropertySetHandle getPropHandle() const;

        const Property::Set &getProperties() const;
        Property::Set &getProperties();

        const std::string &getType() const;

        const std::string &getName() const;

        const std::string &getParentName() const;

        const std::string &getLabel() const;

        const std::string &getShortLabel() const;

        const std::string &getLongLabel() const;

        const std::string &getScriptName() const;

        const std::string &getDoubleType() const;

        const std::string &getHint() const;

        bool getEnabled() const;

        bool getCanUndo() const;

        bool getSecret() const;

        bool getEvaluateOnChange() const;
      };

      /// the Descriptor of a plugin parameter
      class Descriptor : public Base {
        Descriptor();

      public:
        /// make a parameter, with the given type and name
        explicit Descriptor(const std::string &type, const std::string &name);

      };

      /// base class to the param set instance and param set descriptor
      class BaseSet {
      public:
        virtual ~BaseSet();

        /// obtain a handle on this set for passing to the C api
        OfxParamSetHandle getParamSetHandle() const;

        /// get the property handle that lives with the set
        /// The plugin descriptor/instance that derives from
        /// this will provide this.
        virtual Property::Set &getParamSetProps() = 0;
      };

      /// a set of parameters
      class SetDescriptor : public BaseSet 
      {
        std::map<std::string, Descriptor*> _paramMap;
        std::list<Descriptor *> _paramList;

        /// doesn't exist
        SetDescriptor(const SetDescriptor &);

      public:
        SetDescriptor();

        virtual ~SetDescriptor();

        const std::map<std::string, Descriptor*>& getParams() const;

        const std::list<Descriptor*>& getParamList() const;

        void addParam(const std::string &name, Descriptor *p);
      };

      // forward declare
      class SetInstance;

      /// the description of a plugin parameter
      class Instance : public Base, private Property::NotifyHook {
        Instance();  
      protected:
        SetInstance*  _paramSetInstance;
        Instance*     _parentInstance;
      public:
        virtual ~Instance();

        /// make a parameter, with the given type and name
        explicit Instance(Descriptor& descriptor, Param::SetInstance* instance = 0);

        //        OfxStatus instanceChangedAction(std::string why,
        //                                        OfxTime     time,
        //                                        double      renderScaleX,
        //                                        double      renderScaleY);

        // get the param instance
        OFX::Host::Param::SetInstance* getParamSetInstance() { return _paramSetInstance; }

        // set/get parent instance
        void setParentInstance(Instance* instance);
        Instance* getParentInstance();

        // copy one parameter to another
        virtual OfxStatus copy(const Instance &instance, OfxTime offset);

        // copy one parameter to another, with a range
        virtual OfxStatus copy(const Instance &instance, OfxTime offset, OfxRangeD range);

        // callback which should set enabled state as appropriate
        virtual void setEnabled()
        {
        }

        /// overridden from Property::NotifyHook
        virtual void notify(const std::string &name, bool single, int num) OFX_EXCEPTION_SPEC {
          if (name == kOfxParamPropEnabled) {
            setEnabled();
          }
        }
      };

      class KeyframeParam {
      public:
        virtual OfxStatus getNumKeys(unsigned int &nKeys) const;
        virtual OfxStatus getKeyTime(int nth, OfxTime& time) const;
        virtual OfxStatus getKeyIndex(OfxTime time, int direction, int & index) const;
        virtual OfxStatus deleteKey(OfxTime time) ;
        virtual OfxStatus deleteAllKeys();
        virtual ~KeyframeParam() {}
      };

      class GroupInstance : public Instance 
      {
      protected:
        std::vector<Param::Instance*> _children;
      public:
        GroupInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}
        void setChildren(std::vector<Param::Instance*> children);
        const std::vector<Param::Instance*>& getChildren() const;
      };

      class PageInstance : public Instance {
      public:
        PageInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}
        const std::map<int,Param::Instance*>& getChildren() const;
      protected:
        mutable std::map<int,Param::Instance*> _children;
      };

      class IntegerInstance : public Instance, public KeyframeParam {
      public:
        IntegerInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        ChoiceInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

        // needed 
        virtual OfxStatus get(int&) = 0;
        virtual OfxStatus get(OfxTime time, int&) = 0;
        virtual OfxStatus set(int) = 0;
        virtual OfxStatus set(OfxTime time, int) = 0;
      };

      class DoubleInstance : public Instance, public KeyframeParam {
      public:
        DoubleInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        BooleanInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

        // needed
        virtual OfxStatus get(bool&) = 0;
        virtual OfxStatus get(OfxTime time, bool&) = 0;
        virtual OfxStatus set(bool) = 0;
        virtual OfxStatus set(OfxTime time, bool) = 0;
      };

      class RGBAInstance : public Instance, public KeyframeParam {
      public:
        RGBAInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        RGBInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        Double2DInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        Integer2DInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        Double3DInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        Integer3DInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

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
        StringInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}

        virtual OfxStatus get(std::string &) = 0;
        virtual OfxStatus get(OfxTime time, std::string &) = 0;
        virtual OfxStatus set(const char*) = 0;
        virtual OfxStatus set(OfxTime time, const char*) = 0;
      };

      class PushbuttonInstance : public Instance, public KeyframeParam {
      public:
        PushbuttonInstance(Descriptor& descriptor, Param::SetInstance* instance = 0) : Instance(descriptor,instance) {}
      };

      /// A set of parameters
      ///
      /// As we are the owning object we delete the params inside ourselves. It was tempting
      /// to make params autoref objects and have shared ownership with the client code
      /// but that adds complexity for no strong gain.
      class SetInstance : public BaseSet {
      protected:
        std::map<std::string, Instance*> _params;        ///< params by name
        std::list<Instance *>            _paramList;     ///< params list

      public :
        /// ctor
        ///
        /// The propery set being passed in belongs to the owning 
        /// plugin instance.
        explicit SetInstance();

        /// dtor. 
        virtual ~SetInstance();

        /// get the params
        const std::map<std::string, Instance*>& getParams() const;

        /// get the params
        const std::list<Instance*>& getParamList() const;

        // get the param
        Instance* getParam(std::string name) 
        {
          std::map<std::string,Instance*>::iterator it = _params.find(name);
          if(it!=_params.end())
            return it->second;
          else
            return 0;
        }

        /// The inheriting plugin instance needs to set this up to deal with 
        /// plug-ins changing their own values.
        virtual void paramChangedByPlugin(Param::Instance *param) = 0;

        /// add a param
        virtual OfxStatus addParam(const std::string& name, Instance* instance);

        /// make a parameter instance
        ///
        /// Client host code needs to implement this
        virtual Instance* newParam(const std::string& name, Descriptor& Descriptor) = 0;        

        /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
        ///
        /// Client host code needs to implement this
        virtual OfxStatus editBegin(const std::string& name) = 0;

        /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
        ///
        /// Client host code needs to implement this
        virtual OfxStatus editEnd() = 0;

      };
    }
  }
}

#endif // OFXH_PARAM_H
