
// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef OFX_MEMORY_H
#define OFX_MEMORY_H

namespace OFX {

  namespace Host {

    namespace Memory {

      class Instance {
      public:
        Instance();

        virtual ~Instance();        
        virtual bool alloc(size_t nBytes);
        virtual OfxImageMemoryHandle getHandle();
        virtual void freeMem();
        virtual void* getPtr();
        virtual void lock();
        virtual void unlock();

        virtual bool verifyMagic() { return true; }

      protected:
        char*   _ptr;
        int     _locked;
      };

    } // Memory

  } // Host

} // OFX

#endif // OFX_MEMORY_H
