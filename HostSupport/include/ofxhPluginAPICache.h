#ifndef OFX_PLUGIN_API_CACHE
#define OFX_PLUGIN_API_CACHE

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
#include <iostream>
#include <ostream>
#include <map>

#include "ofxhPluginCache.h"
#include "ofxhHost.h"

namespace OFX
{
  namespace Host {
    class Plugin;
    class PluginCache;

    namespace ImageEffect {
      class ImageEffectDescriptor;
    }
  }
}

namespace OFX
{
  namespace Host
  {
    namespace APICache {

      /// this acts as an interface for the Plugin Cache, handling api-specific cacheing
      class PluginAPICacheI
      {
      protected:
        std::string _apiName;
        int _apiVersionMin, _apiVersionMax;
      public:
        PluginAPICacheI(std::string apiName, int verMin, int verMax)
          : _apiName(apiName)
            , _apiVersionMin(verMin)
            , _apiVersionMax(verMax)
        {
        }
        
        virtual ~PluginAPICacheI() {
        }
        
        virtual void loadFromPlugin(Plugin *) = 0;
        
        virtual void beginXmlParsing(Plugin *) = 0;
        virtual void xmlElementBegin(const std::string &, std::map<std::string, std::string>) = 0;
        virtual void xmlCharacterHandler(const std::string &) = 0;
        virtual void xmlElementEnd(const std::string &) = 0;
        virtual void endXmlParsing() = 0;
        
        virtual void saveXML(Plugin *, std::ostream &) = 0;

        void registerInCache(OFX::Host::PluginCache &pluginCache);
      };
      
      /// helper function to build a property set from XML
      void propertySetXMLRead(const std::string &el, std::map<std::string, std::string> map, Property::Set &set, Property::Property*&);

      /// helper function to write a property set to XML
      void propertySetXMLWrite(std::ostream &o, Property::Set &set);

      /// implementation of the specific Image Effect handler API cache.
      class ImageAPIHelper : public PluginAPICacheI {
        
      private:
        std::map<Plugin *, ImageEffect::ImageEffectDescriptor*> _effectDescriptors;
        
        Plugin *_currentPlugin;
        Property::Property *_currentProp;
        ImageEffect::ImageEffectDescriptor *_currentEffect;
        
      public:      
        ImageAPIHelper() : PluginAPICacheI("OfxImageEffectPluginAPI", 1, 1), _currentPlugin(0), _currentProp(0) { }
        
        virtual ~ImageAPIHelper();
        
        /// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
        void loadFromPlugin(Plugin *p);
        
        /// handler for preparing to read in a chunk of XML from the cache, set up context to do this
        void beginXmlParsing(Plugin *p) {
          _currentPlugin = p;
          _currentEffect = new ImageEffect::ImageEffectDescriptor(p);
          _effectDescriptors[_currentPlugin] = _currentEffect;
        }
        
        /// XML handler : element begins (everything is stored in elements and attributes)
        virtual void xmlElementBegin(const std::string &el, std::map<std::string, std::string> map) {
          propertySetXMLRead(el, map, _currentEffect->getProps(), _currentProp);
        }
        
        virtual void xmlCharacterHandler(const std::string &) {
        }
        
        virtual void xmlElementEnd(const std::string &) {
        }
        
        virtual void endXmlParsing() {
          _currentPlugin = 0;
        }
        
        virtual void saveXML(Plugin *p, std::ostream &os) {
          propertySetXMLWrite(os, _effectDescriptors[p]->getProps());
        }
      };
    }
  }
}

#endif
