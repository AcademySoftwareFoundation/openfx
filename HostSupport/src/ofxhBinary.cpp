/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
* Neither the name The Open Effects Association Ltd, nor the names of its 
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

#include "ofxhBinary.h"

using namespace OFX;

Binary::Binary(const std::string &binaryPath): _binaryPath(binaryPath), _invalid(false), _dlHandle(0), _users(0)
{
  struct stat sb;
  if (stat(binaryPath.c_str(), &sb) != 0) {
    _invalid = true;
    _time = 0;
    _size = 0;
  } 
  else {
    _time = sb.st_mtime;
    _size = sb.st_size;
  }
}


// actually open the binary.
void Binary::load() 
{
  if(_invalid)
    return;

#if defined (UNIX)
  _dlHandle = dlopen(_binaryPath.c_str(), RTLD_LAZY);
#else
  _dlHandle = LoadLibrary(_binaryPath.c_str());
#endif
  if (_dlHandle == 0) {
#if defined (UNIX)
    std::cerr << "couldn't open library " << _binaryPath << " because " << dlerror() << std::endl;
#else
    LPVOID lpMsgBuf = NULL;
    DWORD err = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      err,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL);

    std::cerr << "couldn't open library " << _binaryPath << " because " << (char*)lpMsgBuf << " was returned" << std::endl;
    if (lpMsgBuf != NULL) {
      LocalFree(lpMsgBuf);
    }
#endif
    _invalid = true;
  }
}

/// close the binary
void Binary::unload() {
  if (_dlHandle != 0) {
#if defined (UNIX)
    dlclose(_dlHandle);
#elif defined (WINDOWS)
    FreeLibrary(_dlHandle);
#endif
    _dlHandle = 0;
  }
}

/// look up a symbol in the binary file and return it as a pointer.
/// returns null pointer if not found, or if the library is not loaded.
void *Binary::findSymbol(const std::string &symbol) {
  if (_dlHandle != 0) {
#if defined(UNIX)
    return dlsym(_dlHandle, symbol.c_str());
#elif defined (WINDOWS)
    return GetProcAddress(_dlHandle, symbol.c_str());
#endif
  } else {
    return 0;
  }
}


void Binary::ref()
{
  if (_users == 0) {
    load();
  }
  _users++;
}

void Binary::unref()
{
  _users--;
  if (_users == 0) {
    unload();
  }
  if (_users < 0) {
    _users = 0;
  }
}


