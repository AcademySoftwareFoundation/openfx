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
                nCPUs = OFX::MultiThread::numCPUs();

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
        unsigned  int numCPUs(void)
        {
            unsigned int n = 1;
            OfxStatus stat = OFX::Private::gThreadSuite->multiThreadNumCPUs(&n);
            if(stat != kOfxStatOK) n = 1;
            return n;
        }

        /** @brief The index of the current thread. From 0 to numCPUs() - 1 */
        unsigned int threadIndex(void)
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
