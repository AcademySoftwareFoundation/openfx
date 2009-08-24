/*
Software License :

Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   * Neither the name The Open Effects Association Ltd, nor the names of its 
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
    
////////////////////////////////////////////////////////////////////////////////
/// This example shows basic plugin cache management.

#include <iostream>
#include <fstream>
    
#include "ofxhPluginCache.h"
#include "ofxhPropertySuite.h"
#include "ofxhImageEffectAPI.h"
   
    
/// our derived host, which provides a set of virtuals
/// to do things like check for plug-in support and
/// create instances etc...
/// For the purposes of this example, we don't actually
/// need it to do anything. In reality we do.
class CacheHost : public OFX::Host::ImageEffect::Host
{
public :
  /// This should really return a new plugin instance, however
  /// we don't need to for the purposes of this exam1ple.
  OFX::Host::ImageEffect::Instance* newInstance(void* clientData,
                                                OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                                OFX::Host::ImageEffect::Descriptor& desc,
                                                const std::string& context)
  {
    return NULL; 
  }
  
  /// Override this to create a descriptor, this makes the 'root' descriptor
  OFX::Host::ImageEffect::Descriptor *makeDescriptor(OFX::Host::ImageEffect::ImageEffectPlugin* plugin)
  {
    OFX::Host::ImageEffect::Descriptor *desc = new OFX::Host::ImageEffect::Descriptor(plugin);
    return desc;
  }

  /// used to construct a context description, rootContext is the main context
  OFX::Host::ImageEffect::Descriptor *makeDescriptor(const OFX::Host::ImageEffect::Descriptor &rootContext, 
                                                     OFX::Host::ImageEffect::ImageEffectPlugin *plugin)
  {
    OFX::Host::ImageEffect::Descriptor *desc = new OFX::Host::ImageEffect::Descriptor(rootContext, plugin);
    return desc;
  }  
  
  /// used to construct populate the cache
  OFX::Host::ImageEffect::Descriptor *makeDescriptor(const std::string &bundlePath, 
                                                     OFX::Host::ImageEffect::ImageEffectPlugin *plugin)
  {
    OFX::Host::ImageEffect::Descriptor *desc = new OFX::Host::ImageEffect::Descriptor(bundlePath, plugin);
    return desc;
  }
  
  /// vmessage
  OfxStatus vmessage(const char* type,
                     const char* id,
                     const char* format,
                     va_list args)
  {
    bool isQuestion = false;
    const char *prefix = "Message : ";
    if (strcmp(type, kOfxMessageLog) == 0) {
      prefix = "Log : ";
    }
    else if(strcmp(type, kOfxMessageFatal) == 0 ||
            strcmp(type, kOfxMessageError) == 0) {
      prefix = "Error : ";
    }
    else if(strcmp(type, kOfxMessageQuestion) == 0)  {
      prefix = "Question : ";
      isQuestion = true;
    }
    
    // Just dump our message to stdout, should be done with a proper
    // UI in a full ap, and post a dialogue for yes/no questions.
    fputs(prefix, stdout);
    vprintf(format, args);
    printf("\n");

    if(isQuestion) {
      /// cant do this properly inour example, as we need to raise a dialogue to ask a question, so just return yes
      return kOfxStatReplyYes;      
    }
    else {
      return kOfxStatOK;
    }
  }

};

int main(int argc, char **argv) 
{
#ifdef _WIN32
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  /// set the version label in the global cache
  OFX::Host::PluginCache::getPluginCache()->setCacheVersion("cacheDemoV1");

  /// create our derived image effect host
  CacheHost myHost;

  /// make an image effect plugin cache
  OFX::Host::ImageEffect::PluginCache imageEffectPluginCache(myHost);

  /// register the image effect cache with the global plugin cache
  imageEffectPluginCache.registerInCache(*OFX::Host::PluginCache::getPluginCache());

  /// now read an old cache cache
  std::ifstream ifs("oldcache.xml");
  OFX::Host::PluginCache::getPluginCache()->readCache(ifs);
  OFX::Host::PluginCache::getPluginCache()->scanPluginFiles();
  ifs.close();

  /// and write a new cache, long version with everything in there
  std::ofstream of("newCache.xml");
  OFX::Host::PluginCache::getPluginCache()->writePluginCache(of);
  of.close();

  imageEffectPluginCache.dumpToStdOut();
  //Clean up, to be polite.
  OFX::Host::PluginCache::clearPluginCache();
}
