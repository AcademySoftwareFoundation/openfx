/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004-2005 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  The Foundry Visionmongers Ltd
  35-36 Gt Marlborough St 
  London W1V 7FN
  England
*/

#include "ofxsSupportPrivate.H"

namespace OFX {

    namespace MultiThread {

        ////////////////////////////////////////////////////////////////////////////////
        // SMP class

        /** @brief ctor */
        Processor::Processor(void)
        {
        }

        /** @brief dtor */
        Processor::~Processor()
        {
        }
        
        /** @brief Function to pass to the multi thread suite */
        void Processor::staticMultiThreadFunction(unsigned int threadIndex, unsigned int threadMax, void *customArg)
        {
            // cast the custom arg to one of me
            Processor *me = (Processor *)  customArg;

            // and call my thread function
            me->multiThreadFunction(threadIndex, threadMax);
        }

        /** @brief Function to pass to the multi thread suite */
        void Processor::multiThread(unsigned int nCPUs)
        {
            // if 0, use all the CPUs we can
            if(nCPUs == 0)
                nCPUs = OFX::MultiThread::getNumCPUs();

            // if 1 cpu, don't bother with the threading
            if(nCPUs == 1) {
                multiThreadFunction(0, 1);
            }
            else {
                // OK do it
                OfxStatus stat = OFX::Private::gThreadSuite->multiThread(staticMultiThreadFunction, nCPUs, (void *)this);

                // did we do it?
                throwSuiteStatusException(stat);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // futility functions

        /** @brief Has the current thread been spawned from an MP */
        bool isSpawnedThread(void)
        {
            int v = OFX::Private::gThreadSuite->multiThreadIsSpawnedThread();
            return v != 0;
        }

        /** @brief The number of CPUs that can be used for MP-ing */
        unsigned  int getNumCPUs(void)
        {
            unsigned int n = 1;
            OfxStatus stat = OFX::Private::gThreadSuite->multiThreadNumCPUs(&n);
            if(stat != kOfxStatOK) n = 1;
            return n;
        }

        /** @brief The index of the current thread. From 0 to numCPUs() - 1 */
        unsigned int getThreadIndex(void)
        {
            unsigned int n = 0;
            OfxStatus stat = OFX::Private::gThreadSuite->multiThreadIndex(&n);
            if(stat != kOfxStatOK) n = 0;
            return n;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // MUTEX class
        
        /** @brief ctor */
        Mutex::Mutex(int lockCount)
          : _handle(0)
        {
            OfxStatus stat = OFX::Private::gThreadSuite->mutexCreate((const OfxMutexHandle *) &_handle, lockCount);
            throwSuiteStatusException(stat);
        }

        /** @brief dtor */
        Mutex::~Mutex(void)
        {
            OfxStatus stat = OFX::Private::gThreadSuite->mutexDestroy(_handle);
        }

        /** @brief lock it, blocks until lock is gained */
        void Mutex::lock()
        {
            OfxStatus stat = OFX::Private::gThreadSuite->mutexLock(_handle);
            throwSuiteStatusException(stat);
        }

        /** @brief unlock it */
        void Mutex::unlock()
        {
            OfxStatus stat = OFX::Private::gThreadSuite->mutexUnLock(_handle);
            throwSuiteStatusException(stat);
        }

        /** @brief attempt to lock, non-blocking */
        bool Mutex::tryLock()
        {
            OfxStatus stat = OFX::Private::gThreadSuite->mutexTryLock(_handle);
            return stat == kOfxStatOK;            
        }

    };
};
