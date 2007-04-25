#ifndef OFXH_IMAGE_EFFECT_API_H
#define OFXH_IMAGE_EFFECT_API_H

#include <string>
#include <map>

#include "ofxCore.h"
#include "ofxImageEffect.h"
#include "ofxhImageEffect.h"
#include "ofxhHost.h"

namespace OFX {
  
  namespace Host {
    
    namespace ImageEffect {

      class PluginCache;

      /// subclass of Plugin representing an ImageEffect plugin.  used to store API-specific
      /// data
      class ImageEffectPlugin : public Plugin {

        PluginCache &_pc;

        // this comes off Descriptor's property set after a describe
        Descriptor _ie;
        
        /// map to store contexts in
        std::map<std::string, Descriptor *> _contexts;

        PluginHandle* _pluginHandle;

      public:
			  ImageEffectPlugin(PluginCache &pc, PluginBinary *pb, int pi, OfxPlugin *pl);

        ImageEffectPlugin(PluginCache &pc,
                          PluginBinary *pb,
                          int pi,
                          const std::string &api,
                          int apiVersion,
                          const std::string &pluginId,
                          int pluginMajorVersion,
                          int pluginMinorVersion);

        /// return the API handler this plugin was constructed by
        APICache::PluginAPICacheI &getApiHandler();

        /// get the properties
        Property::Set &getProps();

        /// get the image effect descriptor
        Descriptor &getDescriptor();

        void addContext(const std::string &context, Descriptor *ied);

        virtual void saveXML(std::ostream &os);

        const std::map<std::string, Descriptor *> getContexts();

        PluginHandle *getPluginHandle();

        ImageEffect::Instance* createInstance(const std::string &context);

      };

      class MajorPlugin {
        std::string _id;
        int _major;

      public:
        MajorPlugin(const std::string &id, int major) : _id(id), _major(major) {
        }

        MajorPlugin(ImageEffectPlugin *iep) : _id(iep->getIdentifier()), _major(iep->getVersionMajor()) {
        }

        const std::string &getId() {
          return _id;
        }

        int getMajor() {
          return _major;
        }

        bool operator<(const MajorPlugin &other) const {
          if (_id < other._id)
            return true;

          if (_id > other._id)
            return false;
          
          if (_major < other._major)
            return true;

          return false;
        }
      };

      /// implementation of the specific Image Effect handler API cache.
      class PluginCache : public APICache::PluginAPICacheI {
        
      private:
        /// all plugins
        std::vector<ImageEffectPlugin *> _plugins;

        /// latest version of each plugin by ID
        std::map<std::string, ImageEffectPlugin *> _pluginsByID;

        /// latest minor version of each plugin by (ID,major)
        std::map<MajorPlugin, ImageEffectPlugin *> _pluginsByIDMajor;

        /// xml parsing state
        ImageEffectPlugin *_currentPlugin;
        /// xml parsing state
        Property::Property *_currentProp;
        
        Descriptor *_currentContext;
        Param::Descriptor *_currentParam;
        Clip::Descriptor *_currentClip;

        OFX::Host::Descriptor* _descriptor;

      public:      
        PluginCache(OFX::Host::Descriptor* descriptor = 0);
        
        virtual ~PluginCache();        

        /// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
        /// pick the highest found version.
        ImageEffectPlugin *getPluginById(const std::string &id, int vermaj=-1, int vermin=-1);

        /// get the plugin by label.  vermaj and vermin can be specified.  if they are not it will
        /// pick the highest found version.
        ImageEffectPlugin *getPluginByLabel(const std::string &label, int vermaj=-1, int vermin=-1);

        OFX::Host::Descriptor *getHostDescriptor() {
          return _descriptor;
        }

        const std::vector<ImageEffectPlugin *>& getPlugins();

        const std::map<std::string, ImageEffectPlugin *>& getPluginsByID();

        const std::map<MajorPlugin, ImageEffectPlugin *>& getPluginsByIDMajor()
        {
          return _pluginsByIDMajor;
        }

        /// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
        void loadFromPlugin(Plugin *p);
        
        /// handler for preparing to read in a chunk of XML from the cache, set up context to do this
        void beginXmlParsing(Plugin *p);

        /// XML handler : element begins (everything is stored in elements and attributes)       
        virtual void xmlElementBegin(const std::string &el, std::map<std::string, std::string> map);

        virtual void xmlCharacterHandler(const std::string &);
        
        virtual void xmlElementEnd(const std::string &el);
        
        virtual void endXmlParsing();
        
        virtual void saveXML(Plugin *ip, std::ostream &os);

        void confirmPlugin(Plugin *p);

        Plugin *newPlugin(PluginBinary *pb,
                          int pi,
                          OfxPlugin *pl);

        Plugin *newPlugin(PluginBinary *pb,
                          int pi,
                          const std::string &api,
                          int apiVersion,
                          const std::string &pluginId,
                          int pluginMajorVersion,
                          int pluginMinorVersion);
      };
    
    } // ImageEffect

  } // Host

} // OFX

#endif
