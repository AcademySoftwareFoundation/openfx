// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
    
////////////////////////////////////////////////////////////////////////////////
/// This example shows basic plugin cache management.

#include <cstring>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <crtdbg.h>
#endif

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
      /// can't do this properly in our example, as we need to raise a dialogue to ask a question, so just return yes
      return kOfxStatReplyYes;      
    }
    else {
      return kOfxStatOK;
    }
  }

  OfxStatus setPersistentMessage(const char* type,
                                 const char* id,
                                 const char* format,
                                 va_list args)
  {
    return vmessage(type, id, format, args);
  }

  OfxStatus clearPersistentMessage()
  {
    return kOfxStatOK;
  }

#ifdef OFX_SUPPORTS_OPENGLRENDER
  /// @see OfxImageEffectOpenGLRenderSuiteV1.flushResources()
  virtual OfxStatus flushOpenGLResources() const { return kOfxStatFailed; };
#endif
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
