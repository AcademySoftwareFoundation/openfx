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
#include <map>

#include "ofxhPluginCache.h"
#include "ofxhPluginAPICache.h"
#include "ofxhHost.h"
#include "ofxhXml.h"

namespace OFX
{
  namespace Host
  {
    namespace APICache
    {
      void PluginAPICacheI::registerInCache(OFX::Host::PluginCache &pluginCache) {
        pluginCache.registerAPICache(_apiName, _apiVersionMin, _apiVersionMax, this);
      }      

      void propertySetXMLRead(const std::string &el,
                              std::map<std::string,
                              std::string> map,
                              Property::Set &set,
                              Property::Property *&_currentProp) {
        if (el == "property") {
          std::string propName = map["name"];
          std::string propType = map["type"];
          int dimension = atoi(map["dimension"].c_str());
          int valuesize = atoi(map["size"].c_str());
          
          if (propType == "int") {
            _currentProp = new Property::Int(propName, dimension, false, 0);
          } else if (propType == "string") {
            _currentProp = new Property::String(propName, dimension, false, "");
          } else if (propType == "double") {
            _currentProp = new Property::Double(propName, dimension, false, 0);
          } else if (propType == "pointer") {
            _currentProp = new Property::Pointer(propName, dimension, false, 0);
          }

          set.addProperty(_currentProp);
        }
        
        if (el == "value" && _currentProp) {
          int index = atoi(map["index"].c_str());
          std::string value = map["value"];
          
          switch (_currentProp->getType()) {
          case Property::eInt:
            set.setProperty<Property::IntValue>(_currentProp->getName(), index, atoi(value.c_str()));
            break;
          case Property::eString:
            set.setProperty<Property::StringValue>(_currentProp->getName(), index, value.c_str());
            break;
          case Property::eDouble:
            set.setProperty<Property::DoubleValue>(_currentProp->getName(), index, atof(value.c_str()));
            break;
          case Property::ePointer:
            break;
          default:
            break;
          }
        }
      }
      
      void propertySetXMLWrite(std::ostream &o, Property::Set &set) {
        for (std::map<const std::string, Property::Property*>::const_iterator i = set.getProperties().begin();
             i != set.getProperties().end();
             i++)
          {
            Property::Property *prop = i->second;
            
            if (prop->getType() == Property::ePointer) 
              continue;
            
            o << "<property "
              << XML::attribute("name", prop->getName())
              << XML::attribute("type", Property::typeNames[prop->getType()])
              << XML::attribute("dimension", prop->getDimension()) 
              << ">\n";
              							                                                     
            for (size_t i=0;i<prop->getDimension();i++) {
              o << "<value " 
                << XML::attribute("index", i)
                << XML::attribute("value", prop->getStringValue(i)) 
                << " />\n";
            }
            
            o << "</property>\n";
          }
      }

      ImageAPIHelper::~ImageAPIHelper() {
      }

      void ImageAPIHelper::loadFromPlugin(Plugin *p) {
        
        PluginHandle plug(p);
        
        OFX::Host::HostDescriptor host;
        plug->setHost(host.getHandle());
        int rval = plug->mainEntry(kOfxActionLoad, 0, 0, 0);

        ImageEffect::ImageEffectDescriptor *e = new ImageEffect::ImageEffectDescriptor(p);
        if (rval == 0 || rval == 14) {
          rval = plug->mainEntry(kOfxActionDescribe, e->getHandle(), 0, 0);
        }
        
        /*  int size = e->_properties.getDimension(kOfxImageEffectPropSupportedContexts);
            std::vector<std::string> contexts;
            
            for (int j=0;j<size;j++) {
            std::string context = e->_properties.getProperty<OFX::Host::Property::StringValue>("OfxImageEffectPropSupportedContexts", j);
            contexts.push_back(context);
        */
        /*
          OFX::Host::Property::PropSpec inargspec[] = {
          { kOfxImageEffectPropContext, OFX::Host::Property::eString, 1, true, context },
          { 0 }
          };
          
          OFX::Host::Property::Set inarg(inargspec);
          
          if (rval == 0 || rval == 14) {
          rval = plug->mainEntry(kOfxImageEffectActionDescribeInContext, e->getHandle(), inarg.getHandle(), 0);
          printf("describe in context: %i \n", rval);
          }
          
          for (std::map<std::string, OFX::Host::Param*>::iterator i=e->_params._params.begin();
          i != e->_params._params.end();
          i++) {
          std::cout << "<param name=\"" << i->first << "\">\n";
          i->second->_properties.dump(std::cout);
          std::cout << "</param>\n";
          }
        */
        /*  }
         */
        
        if (rval == 0) {
          rval = plug->mainEntry(kOfxActionUnload, 0, 0, 0);
        }
        
        _effectDescriptors[p] = e;
      }      
    }
  }
}
