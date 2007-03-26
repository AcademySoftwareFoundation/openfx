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

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>

#include "ofxhPluginCache.h"
#include "ofxhPluginAPICache.h"
#include "ofxhPropertySuite.h"
#include "ofxhHost.h"
#include "ofxhXml.h"

#include "expat.h"
#include "ofxImageEffect.h"

namespace OFX {

  namespace Host {

    /// try to open the plugin bundle object and query it for plugins
    void PluginBinary::loadPluginInfo() {      
      _fileModificationTime = _binary.getTime();
      _fileSize = _binary.getSize();
      _binaryChanged = false;
      
      _binary.load();
      
      int (*getNo)(void) = (int(*)()) _binary.findSymbol("OfxGetNumberOfPlugins");
      OfxPlugin* (*getPlug)(int) = (OfxPlugin*(*)(int)) _binary.findSymbol("OfxGetPlugin");
      
      if (getNo == 0 || getPlug == 0) {
        
        _binary.setInvalid(true);
        
      } else {
        int pluginCount = (*getNo)();
        
        _plugins.reserve(pluginCount);
        
        for (int i=0;i<pluginCount;i++) {
          OfxPlugin *plug = (*getPlug)(i);	    
          _plugins.push_back(new Plugin(this, i, plug));
        }
      }
      
      _binary.unload();
    }

    PluginBinary::~PluginBinary() {
      std::vector<Plugin*>::iterator i = _plugins.begin();
      while (i != _plugins.end()) {
        delete *i;
        i++;
      }
    }

    PluginHandle::PluginHandle(Plugin *p) : _p(p) {
      _b = p->getBinary();
      _b->_binary.ref();
      OfxPlugin* (*getPlug)(int) = (OfxPlugin*(*)(int)) _b->_binary.findSymbol("OfxGetPlugin");
      _op = getPlug(p->getIndex());
    }

    PluginHandle::~PluginHandle() {
      _b->_binary.unref();
    }
    
    PluginCache gPluginCache;
    
    PluginCache::PluginCache() : _xmlCurrentBinary(0), _xmlCurrentPlugin(0) {
      
      const char *envpath = getenv("OFX_PLUGIN_PATH");

      if (envpath) {
        std::string s = envpath;
  
        while (s.length()) {
          int spos = s.find(":");
      
          std::string path;
      
          if (spos != -1) {
            path = s.substr(0, spos);
            s = s.substr(spos+1);
          } else {
            path = s;
            s = "";
          }

          _pluginPath.push_back(path);
        }
      }
    
      _pluginPath.push_back("/usr/OFX/Plugins");
    }

    void PluginCache::scanDirectory(std::set<std::string> &foundBinFiles, const std::string &dir)
    {
        DIR *d = opendir(dir.c_str());
        if (!d) {
          return;
        }
    
        while (dirent *de = readdir(d)) {
          std::string name = de->d_name;
      
          if (name[0] != '@' && name != "." && name != "..") {
            scanDirectory(foundBinFiles, dir + "/" + name);
          }

          if (name.find(".ofx.bundle") != -1) {
            std::string barename = name.substr(0, name.length() - strlen(".bundle"));
            std::string bundlename = dir + "/" + name;
            std::string binpath = dir + "/" + name + "/Contents/Linux-x86/" + barename;
            
            foundBinFiles.insert(binpath);
            
            if (_knownBinFiles.find(binpath) == _knownBinFiles.end()) {
              
              // the binary was not in the cache
              
              PluginBinary *pb = new PluginBinary(binpath, bundlename);
              _binaries.push_back(pb);
              _knownBinFiles.insert(binpath);
              
              for (int j=0;j<pb->getNPlugins();j++) {
                Plugin *plug = &pb->getPlugin(j);
                APICache::PluginAPICacheI *api = findApiHandler(plug);
                if (api) {
                  api->loadFromPlugin(plug);
                }
              }
            }
          }
        }
        closedir(d);
    }
    
    void PluginCache::scanPluginFiles()
    {
  
      std::set<std::string> foundBinFiles;

      for (std::list<std::string>::iterator paths= _pluginPath.begin();
           paths != _pluginPath.end();
           paths++) {
        scanDirectory(foundBinFiles, *paths);
      }

      std::list<PluginBinary *>::iterator i=_binaries.begin();
      while (i!=_binaries.end()) {
        PluginBinary *pb = *i;

        if (foundBinFiles.find(pb->getFilePath()) == foundBinFiles.end()) {

          // the binary was in the cache, but was not on the path

          i = _binaries.erase(i);
          delete pb;

        } else {

          bool binChanged = pb->hasBinaryChanged();

          // the binary was in the cache, but the binary has changed and thus we need to reload
          if (binChanged) {
            pb->loadPluginInfo();
          }

          for (int j=0;j<pb->getNPlugins();j++) {
            Plugin *plug = &pb->getPlugin(j);
            _plugins.push_back(plug);
            APICache::PluginAPICacheI *api = findApiHandler(plug);
            if (binChanged && api) {
              api->loadFromPlugin(plug);
            }
          }

          i++;
        }
      }
    }


    /// callback for XML parser
    static void elementBeginHandler(void *userData, const XML_Char *name, const XML_Char **atts) {
      gPluginCache.elementBeginCallback(userData, name, atts);
    }

    /// callback for XML parser
    static void elementCharHandler(void *userData, const XML_Char *data, int len) {
      gPluginCache.elementCharCallback(userData, data, len);
    }

    /// callback for XML parser
    static void elementEndHandler(void *userData, const XML_Char *name) {
      gPluginCache.elementEndCallback(userData, name);
    }

    static bool mapHasAll(const std::map<std::string, std::string> &attmap, const char **atts) {
      while (*atts) {
        if (attmap.find(*atts) == attmap.end()) {
          return false;
          }
        atts++;
      }
      return true;
    }

    void PluginCache::elementBeginCallback(void *userData, const XML_Char *name, const XML_Char **atts) {
      std::string ename = name;
      std::map<std::string, std::string> attmap;

      while (*atts) {
        attmap[atts[0]] = atts[1];
        atts += 2;
      }

      /// XXX: validate in general

      if (ename == "binary") {
        const char *binAtts[] = {"path", "bundlepath", "mtime", "size", NULL};

        if (!mapHasAll(attmap, binAtts)) {
          // no path: bad XML
        }

        std::string fname = attmap["path"];
        std::string bname = attmap["bundle_path"];
        time_t mtime = OFX::Host::Property::stringToInt(attmap["mtime"]);
        size_t size = OFX::Host::Property::stringToInt(attmap["size"]);

        _xmlCurrentBinary = new PluginBinary(fname, bname, mtime, size);
        _binaries.push_back(_xmlCurrentBinary);
        _knownBinFiles.insert(fname);
        return;
      }
 
      if (ename == "plugin" && _xmlCurrentBinary && !_xmlCurrentBinary->hasBinaryChanged()) {
        const char *plugAtts[] = {"api", "name", "index", "api_version", "major_version", "minor_version", NULL};

        if (!mapHasAll(attmap, plugAtts)) {
          // no path: bad XML
        }

        std::string api = attmap["api"];
        std::string identifier = attmap["name"];
        int idx = OFX::Host::Property::stringToInt(attmap["index"]);
        int api_version = OFX::Host::Property::stringToInt(attmap["api_version"]);
        int major_version = OFX::Host::Property::stringToInt(attmap["major_version"]);
        int minor_version = OFX::Host::Property::stringToInt(attmap["minor_version"]);

        Plugin *pe = new Plugin(_xmlCurrentBinary, idx, api, api_version, identifier, major_version, minor_version); 
        _xmlCurrentBinary->addPlugin(pe);
        _xmlCurrentPlugin = pe;
        
        APICache::PluginAPICacheI *apiCache = findApiHandler(pe);
        if (apiCache) {
          apiCache->beginXmlParsing(pe);
        }
        return;
      }

      if (_xmlCurrentPlugin) {
        APICache::PluginAPICacheI *api = findApiHandler(_xmlCurrentPlugin);
        if (api) {
          api->xmlElementBegin(name, attmap);
        }
      }

    }

    void PluginCache::elementCharCallback(void *userData, const XML_Char *data, int size) 
    {
      std::string s(data, size);
      if (_xmlCurrentPlugin) {
        APICache::PluginAPICacheI *api = findApiHandler(_xmlCurrentPlugin);
        if (api) {
          api->xmlCharacterHandler(s);
        }
      } else {
        /// XXX: we only want whitespace
      }
    }

    void PluginCache::elementEndCallback(void *userData, const XML_Char *name) {
      std::string ename = name;

      /// XXX: validation?

      if (ename == "plugin") {
        if (_xmlCurrentPlugin) {
          APICache::PluginAPICacheI *api = findApiHandler(_xmlCurrentPlugin);
          if (api) {
            api->endXmlParsing();
          }
        }
        _xmlCurrentPlugin = 0;
        return;
      }

      if (ename == "bundle") {
        _xmlCurrentBinary = 0;
        return;
      }

      if (_xmlCurrentPlugin) {
        APICache::PluginAPICacheI *api = findApiHandler(_xmlCurrentPlugin);
        if (api) {
          api->xmlElementEnd(name);
        }
      }
    }

    void PluginCache::readCache(std::istream &ifs) {
  
      XML_Parser xP = XML_ParserCreate(NULL);
      XML_SetElementHandler(xP, elementBeginHandler, elementEndHandler);
      XML_SetCharacterDataHandler(xP, elementCharHandler);

      while (ifs.good()) {
        char buf[1001] = {0};
        ifs.read(buf, 1000);

        if (buf[0] == 0) {
          XML_Parse(xP, "", 0, XML_TRUE);
          break;
        }

        int p = XML_Parse(xP, buf, strlen(buf), XML_FALSE);

        if (p == XML_STATUS_ERROR) {
          std::cout << "xml error : " << XML_GetErrorCode(xP) << std::endl;
          /// XXX: do something here
          break;
        }
      }

      XML_ParserFree(xP);
    }

    void PluginCache::writePluginCache(std::ostream &os) {
      os << "<cache>\n";
      for (std::list<PluginBinary *>::iterator i=_binaries.begin();i!=_binaries.end();i++) {
        PluginBinary *b = *i;
        os << "<bundle>\n";
        os << "  <binary " 
           << XML::attribute("bundle_path", b->getBundlePath()) 
           << XML::attribute("path", b->getFilePath())
           << XML::attribute("mtime", b->getFileModificationTime())
           << XML::attribute("size", b->getFileSize()) << "/>\n";

        for (int j=0;j<b->getNPlugins();j++) {
          Plugin *p = &b->getPlugin(j);
      

          os << "  <plugin " 
             << XML::attribute("name", p->getIdentifier()) 
             << XML::attribute("api", p->getPluginApi())
             << XML::attribute("api_version", p->getApiVersion())
             << XML::attribute("major_version", p->getVersionMajor())
             << XML::attribute("minor_version", p->getVersionMinor())
             << ">\n";

          APICache::PluginAPICacheI *api = findApiHandler(p);
          if (api) {
            os << "    <apiproperties>\n";
            api->saveXML(p, os);
            os << "    </apiproperties>\n";
          }

          os << "  </plugin>\n";
        }
        os << "</bundle>\n";
      }
      os << "</cache>\n";
    }

    APICache::PluginAPICacheI *PluginCache::findApiHandler(Plugin *plug) {
        std::string api = plug->getPluginApi();
        int version = plug->getApiVersion();
        std::list<PluginCacheSupportedApi>::iterator i = _apiHandlers.begin();
        while (i != _apiHandlers.end()) {
          if (i->matches(api, version)) {
            return i->handler;
          }
          i++;
        }
        return 0;
    }
  }
}
