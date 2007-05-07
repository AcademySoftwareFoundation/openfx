// ofx host

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhMemory.h"

namespace OFX {

  namespace Host {

    namespace Memory {

      Instance::Instance() : _ptr(0), _locked(false) {}

      Instance::~Instance() {
        delete [] _ptr;
      }

      bool Instance::alloc(size_t nBytes) {
        if(!_locked){
          if(_ptr)
            free();
          _ptr = new char[nBytes];
          return true;
        }
        else
          return false;
      }

      OfxImageMemoryHandle Instance::getHandle(){
        return (OfxImageMemoryHandle)this;
      }

      void Instance::free(){
        delete [] _ptr;
        _ptr = 0;
      }

      void* Instance::getPtr() {
        return _ptr;
      }

      void Instance::lock() {
        _locked = true;
      }

      void Instance::unlock() {
        _locked = false;
      }

    } // Memory

  } // Host

} // OFX

