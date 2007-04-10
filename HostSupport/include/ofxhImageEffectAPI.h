#ifndef OFXH_IMAGE_EFFECT_API_H
#define OFXH_IMAGE_EFFECT_API_H

#include <string>
#include <map>

#include "ofxhPluginCache.h"
#include "ofxhPropertySuite.h"
#include "ofxhHost.h"
#include "ofxhPluginAPICache.h"
#include "ofxhXml.h"

namespace OFX {
  namespace Host {
    namespace ImageEffect {

      class PluginCache;

      /// subclass of Plugin representing an ImageEffect plugin.  used to store API-specific
      /// data
      class ImageEffectPlugin : public Plugin {

        PluginCache &_pc;

        // this comes off ImageEffectDescriptor's property set after a describe
        ImageEffectDescriptor _ie;
        
        /// map to store contexts in
        std::map<std::string, ImageEffectDescriptor *> _contexts;
      public:
			  ImageEffectPlugin(PluginCache &pc, PluginBinary *pb, int pi, OfxPlugin *pl)
          : Plugin(pb, pi, pl)
          , _pc(pc)
          , _ie(this) {
        }

        ImageEffectPlugin(PluginCache &pc,
                          PluginBinary *pb,
                          int pi,
                          const std::string &api,
                          int apiVersion,
                          const std::string &pluginId,
                          int pluginMajorVersion,
                          int pluginMinorVersion)
          : Plugin(pb, pi, api, apiVersion, pluginId, pluginMajorVersion, pluginMinorVersion)
          , _pc(pc)
          , _ie(this) {
        }

        /// return the API handler this plugin was constructed by
        APICache::PluginAPICacheI &getApiHandler();

        /// get the properties
        Property::Set &getProps() {
          return _ie.getProps();
        }

        /// get the image effect descriptor
        ImageEffectDescriptor &getImageEffect() {
          return _ie;
        }

        void addContext(const std::string &context, ImageEffectDescriptor *ied)
        {
          _contexts[context] = ied;
        }

        virtual void saveXML(std::ostream &os);

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
        
        ImageEffectDescriptor *_currentContext;
        Param::Param *_currentParam;
        Clip::ClipDescriptor *_currentClip;

      public:      
        PluginCache() 
          : PluginAPICacheI("OfxImageEffectPluginAPI", 1, 1)
          , _currentPlugin(0)
          , _currentProp(0)
          , _currentContext(0)
          , _currentParam(0)
          , _currentClip(0)
        {
        }
        
        virtual ~PluginCache();
        
        /// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
        /// pick the highest found version.
        ImageEffectPlugin *getPluginById(const std::string &id, int vermaj=-1, int vermin=-1);

        /// get the plugin by label.  vermaj and vermin can be specified.  if they are not it will
        /// pick the highest found version.
        ImageEffectPlugin *getPluginByLabel(const std::string &label, int vermaj=-1, int vermin=-1);

        const std::vector<ImageEffectPlugin *>& getPlugins()
        {
          return _plugins;
        }

        const std::map<std::string, ImageEffectPlugin *>& getPluginsByID()
        {
          return _pluginsByID;
        }

        const std::map<MajorPlugin, ImageEffectPlugin *>& getPluginsByIDMajor()
        {
          return _pluginsByIDMajor;
        }

        /// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
        void loadFromPlugin(Plugin *p);
        
        /// handler for preparing to read in a chunk of XML from the cache, set up context to do this
        void beginXmlParsing(Plugin *p) {
          _currentPlugin = dynamic_cast<ImageEffectPlugin*>(p);
        }
        
        /// XML handler : element begins (everything is stored in elements and attributes)       
        virtual void xmlElementBegin(const std::string &el, std::map<std::string, std::string> map);

        virtual void xmlCharacterHandler(const std::string &) {
        }
        
        virtual void xmlElementEnd(const std::string &el) {
          if (el == "param") {
            _currentParam = 0;
          }

          if (el == "context") {
            _currentContext = 0;
          }
        }
        
        virtual void endXmlParsing() {
          _currentPlugin = 0;
        }
        
        virtual void saveXML(Plugin *ip, std::ostream &os) {
          ImageEffectPlugin *p = dynamic_cast<ImageEffectPlugin*>(ip);
          p->saveXML(os);
        }

        void confirmPlugin(Plugin *p);

        Plugin *newPlugin(PluginBinary *pb,
                          int pi,
                          OfxPlugin *pl) {
          ImageEffectPlugin *plugin = new ImageEffectPlugin(*this, pb, pi, pl);
          return plugin;
        }

        Plugin *newPlugin(PluginBinary *pb,
                          int pi,
                          const std::string &api,
                          int apiVersion,
                          const std::string &pluginId,
                          int pluginMajorVersion,
                          int pluginMinorVersion) 
        {
          ImageEffectPlugin *plugin = new ImageEffectPlugin(*this, pb, pi, api, apiVersion, pluginId, pluginMajorVersion, pluginMinorVersion);
          return plugin;
        }
      };
    }
  }
}

#endif
