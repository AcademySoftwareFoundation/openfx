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
    
    /// a host descriptor: used to hold the OfxHost for the C api, and a property set
    class Descriptor {
    protected:
      OfxHost       _host;
      Property::Set _properties;

    public:
      
      Descriptor();
      virtual ~Descriptor() { } 

      OfxHost *getHandle();

      virtual void* fetchOfxImageEffectSuite(int suiteVersion);
      virtual void* fetchOfxPropertySuite(int suiteVersion);
      virtual void* fetchOfxParameterSuite(int suiteVersion);
      virtual void* fetchOfxMemorySuite(int suiteVersion);

      /* Overriding Multi Threading Suite

      The multithreading suite API function calls do not use handles and are just simple
      C function calls. In an attempt to provide a basic default threading behavior for
      the host we have supplied MultiThread::suite that implements the nine functions
      below.

      However, should you wish to write you own multithreading function you can do so by
      implementing 9 multithreading function like those below and override the fetchOfxMultiThreading
      function on your implementation of the host to return you own multi threading suite.

      Example -

      namespace MyMultiThreading {

        OfxStatus multiThread(OfxThreadFunctionV1 func,unsigned int nThreads,void *customArg);
        OfxStatus multiThreadNumCPUs(unsigned int *nCPUs);
        OfxStatus multiThreadIndex(unsigned int *threadIndex);
        int       multiThreadIsSpawnedThread(void);
        OfxStatus mutexCreate(const OfxMutexHandle *mutex, int lockCount);
        OfxStatus mutexDestroy(const OfxMutexHandle mutex);
        OfxStatus mutexLock(const OfxMutexHandle mutex);
        OfxStatus mutexUnLock(const OfxMutexHandle mutex);
        OfxStatus mutexTryLock(const OfxMutexHandle mutex);
        
        struct OfxMultiThreadSuiteV1 myMultiThreadedSuite = {
          multiThread,
          multiThreadNumCPUs,
          multiThreadIndex,
          multiThreadIsSpawnedThread,
          mutexCreate,
          mutexDestroy,
          mutexLock,
          mutexUnLock,
          mutexTryLock
        };

      }

      virtual void* fetchOfxMultiThreadSuite(int suiteVersion){
        return (void*)&MyMultiThreading::myMultiThreadedSuite;
      }      

      */

      // standard multithreading implementation - to override see above
      virtual void* fetchOfxMultiThreadSuite(int suiteVersion);
      virtual void* fetchOfxMessageSuite(int suiteVersion);
      void* fetchOfxInteractSuite(int suiteVersion);
      virtual void *fetchSuite(const char *suiteName, int suiteVersion);

    };

  }
}

#endif
