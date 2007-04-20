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

namespace OFX
{
  namespace Host {
    class Plugin;
    class PluginBinary;
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

        /// factory method, to create a new plugin (from binary)
        virtual Plugin *newPlugin(PluginBinary *, int pi, OfxPlugin *plug) = 0;

        /// factory method, to create a new plugin (from the 
        virtual Plugin *newPlugin(PluginBinary *pb, int pi, const std::string &api, int apiVersion, const std::string &pluginId,
                                  int pluginMajorVersion, int pluginMinorVersion) = 0;
        
        virtual void beginXmlParsing(Plugin *) = 0;
        virtual void xmlElementBegin(const std::string &, std::map<std::string, std::string>) = 0;
        virtual void xmlCharacterHandler(const std::string &) = 0;
        virtual void xmlElementEnd(const std::string &) = 0;
        virtual void endXmlParsing() = 0;
        
        virtual void saveXML(Plugin *, std::ostream &) = 0;

        virtual void confirmPlugin(Plugin *) = 0;

        void registerInCache(OFX::Host::PluginCache &pluginCache);
      };
      
      /// helper function to build a property set from XML
      void propertySetXMLRead(const std::string &el, std::map<std::string, std::string> map, Property::Set &set, Property::Property*&);

      /// helper function to write a property set to XML
      void propertySetXMLWrite(std::ostream &o, Property::Set &set, int indent=0);

    }
  }
}
#endif
