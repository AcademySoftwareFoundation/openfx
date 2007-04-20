#include <string>
#include <map>

// ofx
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

namespace OFX {

  namespace Host {

    namespace ImageEffect {

      ImageEffectPlugin::ImageEffectPlugin(PluginCache &pc, PluginBinary *pb, int pi, OfxPlugin *pl)
        : Plugin(pb, pi, pl)
        , _pc(pc)
        , _ie(this) 
      {}

      ImageEffectPlugin::ImageEffectPlugin(PluginCache &pc,
        PluginBinary *pb,
        int pi,
        const std::string &api,
        int apiVersion,
        const std::string &pluginId,
        int pluginMajorVersion,
        int pluginMinorVersion)
        : Plugin(pb, pi, api, apiVersion, pluginId, pluginMajorVersion, pluginMinorVersion)
        , _pc(pc)
        , _ie(this) 
      {}

      APICache::PluginAPICacheI &ImageEffectPlugin::getApiHandler()
      {
        return _pc;
      }

      /// get the properties
      Property::Set &ImageEffectPlugin::getProps() {
        return _ie.getProps();
      }

      /// get the image effect descriptor
      Descriptor &ImageEffectPlugin::getDescriptor() {
        return _ie;
      }

      void ImageEffectPlugin::addContext(const std::string &context, Descriptor *ied)
      {
        _contexts[context] = ied;
      }

      void ImageEffectPlugin::saveXML(std::ostream &os) {
        APICache::propertySetXMLWrite(os, getProps(), 6);

        for (std::map<std::string, ImageEffect::Descriptor*>::iterator j=_contexts.begin();
          j != _contexts.end();
          j++) {

            os << "      <context " << XML::attribute("name", j->first) << ">\n";

            Descriptor *ed = j->second;

            for (std::map<std::string, OFX::Host::Param::Descriptor*>::const_iterator i=ed->getParams().getParams().begin();
              i != ed->getParams().getParams().end();
              i++) {
                os << "        <param " 
                  << XML::attribute("name", i->first) 
                  << XML::attribute("type", i->second->getType()) 
                  << ">\n";
                APICache::propertySetXMLWrite(os, i->second->getProperties(), 10);
                os << "        </param>\n";
            }

            for (std::map<std::string, OFX::Host::Clip::Descriptor*>::iterator i=ed->getClips().begin();
              i != ed->getClips().end();
              i++) {
                os << "        <clip " 
                  << XML::attribute("name", i->first) 
                  << ">\n";
                APICache::propertySetXMLWrite(os, i->second->getProps(), 10);
                os << "        </clip>\n";
            }

            os << "      </context>\n";
        }
      }

      const std::map<std::string, Descriptor *> ImageEffectPlugin::getContexts() {
        return _contexts;
      }        

      PluginHandle *ImageEffectPlugin::getPluginHandle() {
        if(!_pluginHandle) _pluginHandle = new OFX::Host::PluginHandle(this); 
        return _pluginHandle;
      }

      ImageEffect::Instance* ImageEffectPlugin::createInstance(const std::string &context){          
        /// todo - we need to make sure action:load is called, then action:describe again
        /// (not because we are expecting the results to change, but because plugin
        /// might get confused otherwise), then a describe_in_context
        getPluginHandle();

        std::map<std::string,Descriptor*>::iterator it = _contexts.find(context);
        if(it!=_contexts.end()){
          ImageEffect::Descriptor* desc = it->second;

          ImageEffect::Instance *instance = newInstance(gNewInstancePtr,
                                                        this,
                                                        *desc,
                                                        context);

          /// at this point we need the createinstance action to run.
          if(instance){
            instance->createInstanceAction(); 
          }

          return instance;
        }
        return 0;
      }

      PluginCache::PluginCache(OFX::Host::Descriptor* descriptor) 
        : PluginAPICacheI(kOfxImageEffectPluginApi, 1, 1)
        , _currentPlugin(0)
        , _currentProp(0)
        , _currentContext(0)
        , _currentParam(0)
        , _currentClip(0)
        , _descriptor(descriptor)
      {
      }

      PluginCache::~PluginCache() {}       

      /// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
      /// pick the highest found version.
      ImageEffectPlugin *PluginCache::getPluginById(const std::string &id, int vermaj, int vermin)
      {
        // return the highest version one, which fits the pattern provided
        ImageEffectPlugin *sofar = 0;

        for (std::vector<ImageEffectPlugin *>::iterator i=_plugins.begin();i!=_plugins.end();i++) {
          ImageEffectPlugin *p = *i;

          if (p->getIdentifier() != id) {
            continue;
          }

          if (vermaj != -1 && p->getVersionMajor() != vermaj) {
            continue;
          }

          if (vermin != -1 && p->getVersionMinor() != vermin) {
            continue;
          }

          if (!sofar || p->trumps(sofar)) {
            sofar = p;
          }
        }

        return sofar;
      }

      /// get the plugin by label.  vermaj and vermin can be specified.  if they are not it will
      /// pick the highest found version.
      ImageEffectPlugin *PluginCache::getPluginByLabel(const std::string &label, int vermaj, int vermin)
      {
        // return the highest version one, which fits the pattern provided
        ImageEffectPlugin *sofar = 0;

        for (std::vector<ImageEffectPlugin *>::iterator i=_plugins.begin();i!=_plugins.end();i++) {
          ImageEffectPlugin *p = *i;

          if (p->getProps().getProperty<Property::StringValue>(kOfxPropLabel, 0) != label) {
            continue;
          }

          if (vermaj != -1 && p->getVersionMajor() != vermaj) {
            continue;
          }

          if (vermin != -1 && p->getVersionMinor() != vermin) {
            continue;
          }

          if (!sofar || p->trumps(sofar)) {
            sofar = p;
          }
        }

        return sofar;
      }

      const std::vector<ImageEffectPlugin *>& PluginCache::getPlugins()
      {
        return _plugins;
      }

      const std::map<std::string, ImageEffectPlugin *>& PluginCache::getPluginsByID()
      {
        return _pluginsByID;
      }

      /// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
      void PluginCache::loadFromPlugin(Plugin *op) {

        ImageEffectPlugin *p = dynamic_cast<ImageEffectPlugin*>(op);
        assert(p);

        PluginHandle plug(p);

        plug->setHost(_descriptor->getHandle());

        int rval = plug->mainEntry(kOfxActionLoad, 0, 0, 0);

        if (rval == 0 || rval == 14) {
          rval = plug->mainEntry(kOfxActionDescribe, p->getDescriptor().getHandle(), 0, 0);
        }

        ImageEffect::Descriptor &e = p->getDescriptor();
        Property::Set &eProps = e.getProps();

        int size = eProps.getDimension(kOfxImageEffectPropSupportedContexts);
        std::vector<std::string> contexts;

        for (int j=0;j<size;j++) {
          std::string context = eProps.getProperty<OFX::Host::Property::StringValue>("OfxImageEffectPropSupportedContexts", j);
          contexts.push_back(context);

          OFX::Host::Property::PropSpec inargspec[] = {
            { kOfxImageEffectPropContext, OFX::Host::Property::eString, 1, true, context.c_str() },
            { 0 }
          };

          OFX::Host::Property::Set inarg(inargspec);

          if (rval == 0 || rval == 14) {
            ImageEffect::Descriptor *newContext = new ImageEffect::Descriptor(e);
            rval = plug->mainEntry(kOfxImageEffectActionDescribeInContext, newContext->getHandle(), inarg.getHandle(), 0);
            if (rval == 0 || rval == 14) {
              p->addContext(context, newContext);
            }
          }
        }

        if (rval == 0) {
          rval = plug->mainEntry(kOfxActionUnload, 0, 0, 0);
        }
      }      


      /// handler for preparing to read in a chunk of XML from the cache, set up context to do this
      void PluginCache::beginXmlParsing(Plugin *p) {
        _currentPlugin = dynamic_cast<ImageEffectPlugin*>(p);
      }

      /// XML handler : element begins (everything is stored in elements and attributes)       
      void PluginCache::xmlElementBegin(const std::string &el, std::map<std::string, std::string> map) 
      {
        if (el == "apiproperties") {
          return;
        }

        if (el == "context") {
          _currentContext = new ImageEffect::Descriptor(_currentPlugin->getBinary()->getBundlePath());
          _currentPlugin->addContext(map["name"], _currentContext);
          return;
        }

        if (el == "param" && _currentContext) {
          std::string pname = map["name"];
          std::string ptype = map["type"];

          _currentParam = new Param::Descriptor(ptype, pname);
          _currentContext->getParams().addParam(pname, _currentParam);
          return;
        }

        if (el == "clip" && _currentContext) {
          std::string cname = map["name"];

          _currentClip = new Clip::Descriptor();
          _currentContext->addClip(cname, _currentClip);
          return;
        }

        if (_currentContext && _currentParam) {
          APICache::propertySetXMLRead(el, map, _currentParam->getProperties(), _currentProp);
          return;
        }

        if (_currentContext && _currentClip) {
          APICache::propertySetXMLRead(el, map, _currentClip->getProps(), _currentProp);
          return;
        }

        if (!_currentContext && !_currentParam) {
          APICache::propertySetXMLRead(el, map, _currentPlugin->getProps(), _currentProp);
          return;
        }

        std::cout << "element " << el << "\n";
        assert(false);
      }

      void PluginCache::xmlCharacterHandler(const std::string &) {
      }

      void PluginCache::xmlElementEnd(const std::string &el) {
        if (el == "param") {
          _currentParam = 0;
        }

        if (el == "context") {
          _currentContext = 0;
        }
      }

      void PluginCache::endXmlParsing() {
        _currentPlugin = 0;
      }

      void PluginCache::saveXML(Plugin *ip, std::ostream &os) {
        ImageEffectPlugin *p = dynamic_cast<ImageEffectPlugin*>(ip);
        p->saveXML(os);
      }

      void PluginCache::confirmPlugin(Plugin *p) {
        ImageEffectPlugin *plugin = dynamic_cast<ImageEffectPlugin*>(p);
        _plugins.push_back(plugin);

        if (_pluginsByID.find(plugin->getIdentifier()) != _pluginsByID.end()) {
          ImageEffectPlugin *otherPlugin = _pluginsByID[plugin->getIdentifier()];
          if (plugin->trumps(otherPlugin)) {
            _pluginsByID[plugin->getIdentifier()] = plugin;
          }
        } else {
          _pluginsByID[plugin->getIdentifier()] = plugin;
        }
      }

      Plugin *PluginCache::newPlugin(PluginBinary *pb,
        int pi,
        OfxPlugin *pl) {
          ImageEffectPlugin *plugin = new ImageEffectPlugin(*this, pb, pi, pl);
          return plugin;
      }

      Plugin *PluginCache::newPlugin(PluginBinary *pb,
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

    } // ImageEffect

  } // Host

} // OFX

