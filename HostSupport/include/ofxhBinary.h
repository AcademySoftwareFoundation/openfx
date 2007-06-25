#ifndef OFX_BINARY_H
#define OFX_BINARY_H

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
#include <string>
#include <iostream>

// XXX: make the bodies be in a .cpp file

#if defined(WIN32) || defined(WIN64)
#define WINDOWS
#define I386
#elif defined(__linux__)
#define UNIX
#ifdef __i386__
#define I386
#elif defined(__amd64__)
#define AMD64
#else
#error cannot detect architecture
#endif
#elif defined( __APPLE__)
#define UNIX
#else
#error cannot detect operating system
#endif

#if defined(UNIX)
#include <dlfcn.h>
#elif defined (WINDOWS)
#include "windows.h"
#include <assert.h>
#endif

#include <sys/stat.h>

namespace OFX {

  /// class representing a DLL/Shared Object/etc
  class Binary {
  /// destruction will close the library and invalidate
  /// any function pointers returned by lookupSymbol()
  protected :
    std::string _binaryPath;

    bool _invalid;
#if defined(UNIX)
    void *_dlHandle;
#elif defined (WINDOWS)
    HINSTANCE _dlHandle;
#endif

    bool _exists;
    time_t _time;
    size_t _size;

    int _users;

  public :

    // create object representing the binary.  will stat() it, 
    // and this fails, will set binary to be invalid.
    Binary(const std::string &binaryPath) :
      _binaryPath(binaryPath),
      _invalid(false),
      _dlHandle(0),
      _users(0)
    {
      struct stat sb;
      if (stat(binaryPath.c_str(), &sb) != 0) {
        _invalid = true;
      } else {
        _time = sb.st_mtime;
        _size = sb.st_size;
      }
    }
    
    // is the binary loaded?
    bool isLoaded() {
      if (_dlHandle == 0) {
        return false;
      } else {
        return true;
      }
    }

    // is this binary invalid? (did the stat() or the load() fail?
    // or did something set it to be invalid?)  
    bool isInvalid() {
      return _invalid;
    }

    // set invalid status (e.g. called by user if a mandatory symbol was missing)
    void setInvalid(bool invalid) {
      _invalid = invalid;
    }

    time_t getTime() {
      return _time;
    }

    size_t getSize() {
      return _size;
    }

    const std::string &getBinaryPath() {
      return _binaryPath;
    }

    void ref()
    {
      if (_users == 0) {
        load();
      }
      _users++;
    }

    void unref()
    {
      _users--;
      if (_users == 0) {
        unload();
      }
      if (_users < 0) {
        _users = 0;
      }
    }

    // actually open the binary.
    void load();

    /// close the binary
    void unload();

    /// look up a symbol in the binary file and return it as a pointer.
    /// returns null pointer if not found, or if the library is not loaded.
    void *findSymbol(const std::string &symbol);

    // dtor.  will unload() the binary
    ~Binary() {
      unload();
    }
  };
};

#endif
