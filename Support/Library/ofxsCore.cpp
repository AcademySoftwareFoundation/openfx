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

#include "./ofxsSupportPrivate.H"

namespace OFX {
    /** @brief the global host description */
    ImageEffectHostDescription *gHostDescription;
  
    /** @brief Throws an @ref OFX::Exception depending on the status flag passed in */
    void
    throwSuiteStatusException(OfxStatus stat) throw(OFX::Exception::Suite, std::bad_alloc)
    {
        switch (stat) {
        case kOfxStatOK :
        case kOfxStatReplyYes :
        case kOfxStatReplyNo :
        case kOfxStatReplyDefault :
            break;
    
        case kOfxStatErrMemory :
            throw std::bad_alloc();

        default :
            throw OFX::Exception::Suite(stat);
        }
    }


    /** @brief maps status to a string */
    char *
    mapStatusToString(OfxStatus stat)
    {
        switch(stat) {    
        case kOfxStatOK             : return "kOfxStatOK";
        case kOfxStatFailed         : return "kOfxStatFailed";
        case kOfxStatErrFatal       : return "kOfxStatErrFatal";
        case kOfxStatErrUnknown     : return "kOfxStatErrUnknown";
        case kOfxStatErrMissingHostFeature : return "kOfxStatErrMissingHostFeature";
        case kOfxStatErrUnsupported : return "kOfxStatErrUnsupported";
        case kOfxStatErrExists      : return "kOfxStatErrExists";
        case kOfxStatErrFormat      : return "kOfxStatErrFormat";
        case kOfxStatErrMemory      : return "kOfxStatErrMemory";
        case kOfxStatErrBadHandle   : return "kOfxStatErrBadHandle";
        case kOfxStatErrBadIndex    : return "kOfxStatErrBadIndex";
        case kOfxStatErrValue       : return "kOfxStatErrValue";
        case kOfxStatReplyYes       : return "kOfxStatReplyYes";
        case kOfxStatReplyNo        : return "kOfxStatReplyNo";
        case kOfxStatReplyDefault   : return "kOfxStatReplyDefault";
        case kOfxStatErrImageFormat : return "kOfxStatErrImageFormat";
        }
        return "UNKNOWN STATUS CODE";
    }
  

    /** @brief namespace for memory allocation that is done via wrapping the ofx memory suite */
    namespace Memory {
        /** @brief allocate n bytes, returns a pointer to it */
        void *allocate(size_t nBytes,
                       ImageEffect *effect = 0) throw(std::bad_alloc)
        {
            void *data = 0;
            OfxStatus stat = OFX::Private::gMemorySuite->memoryAlloc((void *)(effect ? effect->getHandle() : 0), nBytes, &data);
            if(stat != kOfxStatOK)
                throw std::bad_alloc();
            return data;
        }

        /** @brief free n previously allocated memory */
        void free(void *ptr) throw()
        {
            if(ptr)
                // note we are ignore errors, this could be bad, but we don't throw on a destruction
                OFX::Private::gMemorySuite->memoryFree(ptr);            
        }

    };

}; // namespace OFX

