#ifndef OFX_HOST_H
#define OFX_HOST_H

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

#include "ofxCore.h"
#include "ofxImageEffect.h"

#include "ofxhPropertySuite.h"

namespace OFX {
  namespace Host {
    class Plugin;

    namespace Param {

      /// the description of a plugin parameter
      class Param {
        Param();

        std::string _paramType;
        Property::Set _properties;        
        
      public:
        /// make a parameter, with the given type and name
        explicit Param(const std::string &type, const std::string &name);
        
        /// grab a handle on the parameter for passing to the C API
        OfxParamHandle getHandle() {
          return (OfxParamHandle)this;
        }
        
        /// grab a handle on the properties of this parameter for the C api
        OfxPropertySetHandle getPropHandle() {
          return _properties.getHandle();
        }
      };
      
      /// a set of parameters
      class ParamSet {
        std::map<std::string, Param*> _params;
        
      public:
        std::map<std::string, Param*> &getParams()
        {
          return _params;
        }

        /// obtain a handle on this set for passing to the C api
        OfxParamSetHandle getHandle() {
          return (OfxParamSetHandle)this;
        }
      };
    }

    namespace Clip {
      
      /// a clip descriptor
      class ClipDescriptor {
        Property::Set _properties;        

      public:
        
        /// constructor
        ClipDescriptor();
        
        /// get a handle on the clip descriptor for the C api
        OfxImageClipHandle getHandle() {
          return (OfxImageClipHandle)this;
        }
        
        /// get a handle on the properties of the clip descriptor for the C api
        OfxPropertySetHandle getPropHandle() {
          return _properties.getHandle();
        }
      };

    }

    namespace ImageEffect {
      
      /// an image effect plugin descriptor
      class ImageEffectDescriptor {
        std::map<std::string, Clip::ClipDescriptor*> _clips;
        Param::ParamSet _params;
        Property::Set _properties;
        
      public:

        /// constructor
        ImageEffectDescriptor(Plugin *plug);
        
        /// obtain a handle on this for passing to the C api
        OfxImageEffectHandle getHandle() {
          return (OfxImageEffectHandle)this;
        }
        
        /// create a new clip and add this to the clip map
        Clip::ClipDescriptor *defineClip(const std::string &name) {
          Clip::ClipDescriptor *c = new Clip::ClipDescriptor();
          _clips[name] = c;
          return c;
        }

        /// get the properties set
        Property::Set &getProps() {
          return _properties;
        }

        /// get the parameters set
        Param::ParamSet &getParams() {
          return _params;
        }
      };
    }

    /// a host descriptor: used to hold the OfxHost for the C api, and a property set
    class HostDescriptor {
      OfxHost _host;
      Property::Set _properties;
     
    public:
      
      HostDescriptor();
      
      OfxHost *getHandle() {
        return &_host;
      }
    };

  }
}

#endif
