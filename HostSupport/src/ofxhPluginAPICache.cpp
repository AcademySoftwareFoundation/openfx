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

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhImageEffect.h"
#include "ofxhPluginAPICache.h"
#include "ofxhPluginCache.h"
#include "ofxhHost.h"
#include "ofxhImageEffectAPI.h"
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
          return;
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

          return;
        }

        std::cout << "got unrecognised key " << el << "\n";

        assert(false);
      }
      
      void propertySetXMLWrite(std::ostream &o, Property::Set &set, int indent) {
        std::string indent_prefix(indent, ' ');

        for (std::map<std::string, Property::Property*>::const_iterator i = set.getProperties().begin();
             i != set.getProperties().end();
             i++)
          {
            Property::Property *prop = i->second;
            
            if (prop->getType() == Property::ePointer) 
              continue;
            
            o << indent_prefix << "<property "
              << XML::attribute("name", prop->getName())
              << XML::attribute("type", Property::typeNames[prop->getType()])
              << XML::attribute("dimension", prop->getFixedDimension()) 
              << ">\n";
              							                                                     
            for (int i=0;i<prop->getDimension();i++) {
              o << indent_prefix << "  <value " 
                << XML::attribute("index", i)
                << XML::attribute("value", prop->getStringValue(i)) 
                << "/>\n";
            }
            
            o << indent_prefix << "</property>\n";
          }
      }
    }
  }
}
