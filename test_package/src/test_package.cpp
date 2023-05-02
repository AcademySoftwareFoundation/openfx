// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#include <cstring>

#include "ofxhPluginCache.h"
#include "ofxhPropertySuite.h"
#include "ofxhImageEffectAPI.h"

class MyHost : public OFX::Host::ImageEffect::Host
{
public :
  OFX::Host::ImageEffect::Instance* newInstance(void* clientData,
                                                OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                                OFX::Host::ImageEffect::Descriptor& desc,
                                                const std::string& context) override
  {
    return nullptr; 
  }
  
  OFX::Host::ImageEffect::Descriptor *makeDescriptor(OFX::Host::ImageEffect::ImageEffectPlugin* plugin) override
  {
    return new OFX::Host::ImageEffect::Descriptor(plugin);
  }

  OFX::Host::ImageEffect::Descriptor *makeDescriptor(const OFX::Host::ImageEffect::Descriptor &rootContext, 
                                                     OFX::Host::ImageEffect::ImageEffectPlugin *plugin) override
  {
    return new OFX::Host::ImageEffect::Descriptor(rootContext, plugin);
  }  
  
  OFX::Host::ImageEffect::Descriptor *makeDescriptor(const std::string &bundlePath, 
                                                     OFX::Host::ImageEffect::ImageEffectPlugin *plugin) override
  {
    return new OFX::Host::ImageEffect::Descriptor(bundlePath, plugin);
  }
  
  /// vmessage
  OfxStatus vmessage(const char* type,
                     const char* id,
                     const char* format,
                     va_list args) override
  {
    OfxStatus status = kOfxStatOK;

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
      status = kOfxStatReplyYes;
    }

    fputs(prefix, stdout);
    vprintf(format, args);
    printf("\n");

    return status;
  }

  OfxStatus setPersistentMessage(const char* type,
                                 const char* id,
                                 const char* format,
                                 va_list args) override
  {
    return vmessage(type, id, format, args);
  }

  OfxStatus clearPersistentMessage() override
  {
    return kOfxStatOK;
  }

#ifdef OFX_SUPPORTS_OPENGLRENDER
  /// @see OfxImageEffectOpenGLRenderSuiteV1.flushResources()
  OfxStatus flushOpenGLResources() const override { return kOfxStatFailed; };
#endif
};

int main(int argc, char **argv) 
{
  OFX::Host::PluginCache::getPluginCache()->setCacheVersion("testPackageV1");
  MyHost myHost;
  OFX::Host::ImageEffect::PluginCache imageEffectPluginCache(myHost);
  imageEffectPluginCache.registerInCache(*OFX::Host::PluginCache::getPluginCache());

  OFX::Host::PluginCache::getPluginCache()->scanPluginFiles();

  // Search for the invert plugin to make sure we can successfully load a plugin built with
  // this package.
  bool found_invert_plugin = false;
  for (const auto* plugin : OFX::Host::PluginCache::getPluginCache()->getPlugins()) {
    if (plugin->getIdentifier() == "net.sf.openfx.invertPlugin") {
      found_invert_plugin = true;
      break;
    }
  }

  imageEffectPluginCache.dumpToStdOut();

  OFX::Host::PluginCache::clearPluginCache();

  if (!found_invert_plugin) {
    printf("Failed to find plugin.\n");
    return 1;
  }

  return 0;
}
