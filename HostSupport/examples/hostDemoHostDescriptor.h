// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
#ifndef HOST_DEMO_HOST_DESCRIPTOR_H
#define HOST_DEMO_HOST_DESCRIPTOR_H

namespace MyHost {

  /// a host combines several things...
  ///    - a factory to create a new instance of your plugin
  ///      - it also gets to filter some calls during in the
  ///        API to check for validity and perform custom
  ///        operations (eg: add extra properties).
  ///    - it provides a description of the host application
  ///      which is passed back to the plugin.
  class Host : public OFX::Host::ImageEffect::Host
  {
  public:    
    Host();

    /// Create a new instance of an image effect plug-in.
    ///
    /// It is called by ImageEffectPlugin::createInstance which the
    /// client code calls when it wants to make a new instance.
    /// 
    ///   \arg clientData - the clientData passed into the ImageEffectPlugin::createInstance
    ///   \arg plugin - the plugin being created
    ///   \arg desc - the descriptor for that plugin
    ///   \arg context - the context to be created in
    virtual OFX::Host::ImageEffect::Instance* newInstance(void* clientData,
                                                          OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                                          OFX::Host::ImageEffect::Descriptor& desc,
                                                          const std::string& context);

    /// Override this to create a descriptor, this makes the 'root' descriptor
    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(OFX::Host::ImageEffect::ImageEffectPlugin* plugin);

    /// used to construct a context description, rootContext is the main context
    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(const OFX::Host::ImageEffect::Descriptor &rootContext, 
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plug);        

    /// used to construct populate the cache
    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(const std::string &bundlePath, 
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plug);

    /// vmessage
    virtual OfxStatus vmessage(const char* type,
                               const char* id,
                               const char* format,
                               va_list args);

    /// vmessage
    virtual OfxStatus setPersistentMessage(const char* type,
                                           const char* id,
                                           const char* format,
                                           va_list args);
    /// vmessage
    virtual OfxStatus clearPersistentMessage();

#ifdef OFX_SUPPORTS_OPENGLRENDER
    /// @see OfxImageEffectOpenGLRenderSuiteV1.flushResources()
    virtual OfxStatus flushOpenGLResources() const { return kOfxStatFailed; };
#endif
  };

  // my ofx host object
  extern Host gOfxHost;

}

#endif // HOST_DEMO_HOST_DESCRIPTOR_H
