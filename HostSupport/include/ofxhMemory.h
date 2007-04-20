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
        virtual void free();
        virtual void* getPtr();
        virtual void lock();
        virtual void unlock();

      protected:
        void*   _ptr;
        bool    _locked;
      };

    } // Memory
 
  } // Host

} // OFX

#endif // OFX_MEMORY_H
