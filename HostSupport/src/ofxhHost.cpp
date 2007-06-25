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

#include <limits.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

// ofx
#include "ofxCore.h"
#include "ofxProperty.h"
#include "ofxMultiThread.h"
#include "ofxMemory.h"

#include "ofxhHost.h"

typedef OfxPlugin* (*OfxGetPluginType)(int);

namespace OFX {
  
  namespace Host {

    ////////////////////////////////////////////////////////////////////////////////
    /// simple memory suite 
    namespace Memory {
      static OfxStatus memoryAlloc(void *handle, size_t bytes, void **data)
      {
        *data = malloc(bytes);
        if (*data) {
          return kOfxStatOK;
        } else {
          return kOfxStatErrMemory;
        }
      }
      
      static OfxStatus memoryFree(void *data)
      {
        free(data);
        return kOfxStatOK;
      }
      
      static struct OfxMemorySuiteV1 gMallocSuite = {
        memoryAlloc,
        memoryFree
      };
    }

  }

}

namespace OFX {
  namespace Host {
    /// our own internal property for storing away our private pointer to our host descriptor
#define kOfxHostSupportHostPointer "sf.openfx.net.OfxHostSupportHostPointer"

    static Property::PropSpec hostStuffs[] = {
      { kOfxPropType, Property::eString, 1, false, "Host" },
      { kOfxPropName, Property::eString, 1, false, "UNKNOWN" },
      { kOfxPropLabel, Property::eString, 1, false, "UNKNOWN" },
      { kOfxHostSupportHostPointer,    Property::ePointer,    0,    false,    NULL },
      { 0 },
    };    

    static void *fetchSuite(OfxPropertySetHandle hostProps, const char *suiteName, int suiteVersion)
    {      
      Property::Set* properties = reinterpret_cast<Property::Set*>(hostProps);
      
      Host* host = (Host*)properties->getPointerProperty(kOfxHostSupportHostPointer);
      
      if(host)
        return host->fetchSuite(suiteName,suiteVersion);
      else
        return 0;
    }

    // Base Host
    Host::Host() : _properties(hostStuffs) 
    {
      _host.host = _properties.getHandle();
      _host.fetchSuite = OFX::Host::fetchSuite;

      // record the host descriptor in the propert set
      _properties.setPointerProperty(kOfxHostSupportHostPointer,this);
    }

    OfxHost *Host::getHandle() {
      return &_host;
    }

    void *Host::fetchSuite(const char *suiteName, int suiteVersion)
    {
      if (strcmp(suiteName, kOfxPropertySuite)==0  && suiteVersion == 1) {
        return Property::GetSuite(suiteVersion);
      }
      else if (strcmp(suiteName, kOfxMemorySuite)==0 && suiteVersion == 1) {
        return (void*)&Memory::gMallocSuite;
      }  
    
      ///printf("fetchSuite failed with host = %p, name = %s, version = %i\n", this, suiteName, suiteVersion);
      return NULL;
    }

  } // Host

} // OFX 
