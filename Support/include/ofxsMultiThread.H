#ifndef _ofxsMultiThread_H_
#define _ofxsMultiThread_H_
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

/** @file This file contains core code that wraps OFX 'objects' with C++ classes.

This file only holds code that is visible to a plugin implementation, and so hides much
of the direct OFX objects and any library side only functions.
*/

#include "ofxsCore.h"

namespace OFX {

    /** @brief Multi thread namespace */
    namespace MultiThread {

        /** @brief Class that wraps up SMP multi-processing */
        class Processor {
        private :
            /** @brief Function to pass to the multi thread suite */
            static void staticMultiThreadFunction(unsigned int threadIndex, unsigned int threadMax, void *customArg);

        public  :
            /** @brief ctor */
            Processor();

            /** @brief dtor */
            virtual ~Processor();            

            /** @brief function that will be called in each thread. ID is from 0..nThreads-1 nThreads are the number of threads it is being run over */
            virtual void multiThreadFunction(unsigned int threadID, unsigned int nThreads) = 0;

            /** @brief call this to kick off multi threading
            
            The nCPUs is 0, the maximum allowable number of CPUs will be used.
            */
            virtual void multiThread(unsigned int nCPUs = 0);
        };

        /** @brief Has the current thread been spawned from an MP */
        bool isSpawnedThread(void);

        /** @brief The number of CPUs that can be used for MP-ing */
        unsigned int numCPUs(void);

        /** @brief The index of the current thread. From 0 to numCPUs() - 1 */
        unsigned int threadIndex(void);
    
        /** @brief An OFX mutex */
        class Mutex {
        protected :
            OfxMutexHandle _handle; /**< @brief The handle */

        public :
            /** @brief ctor */
            Mutex(int lockCount = 0);

            /** @brief dtor */
            virtual ~Mutex(void);

            /** @brief lock it, blocks until lock is gained */
            void lock();

            /** @brief unlock it */
            void unlock();

            /** @brief attempt to lock, non-blocking
        
            \brief returns
            - true if the lock was achieved
            - false if it could not
            */
            bool tryLock();
        };
    };
};

#endif
