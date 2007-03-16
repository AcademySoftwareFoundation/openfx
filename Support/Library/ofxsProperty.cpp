/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004-2005 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

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

The Foundry Visionmongers Ltd
1 Wardour St
London W1D 6PA
England


*/

#include "./ofxsSupportPrivate.H"

using namespace OFX::Private;

namespace OFX {

    void throwPropertyException(OfxStatus stat,
                                const std::string &propName) throw(std::bad_alloc,
                                                                   OFX::Exception::PropertyUnknownToHost,
                                                                   OFX::Exception::PropertyValueIllegalToHost,
                                                                   OFX::Exception::Suite)
    {
        switch (stat) {
        case kOfxStatOK :
        case kOfxStatReplyYes :
        case kOfxStatReplyNo :
        case kOfxStatReplyDefault :
            // Throw nothing!
            break;

        case kOfxStatErrUnknown :
        case kOfxStatErrUnsupported : // unsupported implies unknow here
            if(OFX::PropertySet::getThrowOnUnsupportedProperties()) // are we suppressing this?
                throw OFX::Exception::PropertyUnknownToHost(propName.c_str());
            break;
          
        case kOfxStatErrMemory :
            throw std::bad_alloc();
            break;

        case kOfxStatErrValue :
            throw  OFX::Exception::PropertyValueIllegalToHost(propName.c_str());
            break;
            
        case kOfxStatErrBadHandle :
        case kOfxStatErrBadIndex :
        default :
          throwSuiteStatusException(stat);
            break;
        }
    }

    /** @brief are we logging property get/set */
    int PropertySet::_gPropLogging = 0;

    /** @brief Do we throw an exception if a host returns 'unsupported' when setting a property */
    bool PropertySet::_gThrowOnUnsupported = true;

    /** @brief Virtual destructor */
    PropertySet::~PropertySet() {}

    /** @brief, returns the dimension of the given property from this property set */
    int 
    PropertySet::propGetDimension(const std::string &property) const throw(std::bad_alloc, 
                                                                           OFX::Exception::PropertyUnknownToHost, 
                                                                           OFX::Exception::PropertyValueIllegalToHost,
                                                                           OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        int dimension;
        OfxStatus stat = gPropSuite->propGetDimension(_propHandle, property.c_str(), &dimension);
        Log::error(stat != kOfxStatOK, "Failed on fetching dimension for property %s, host returned status %s.", property.c_str(), mapStatusToString(stat));

        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) 
            Log::print("Fetched dimension of property %s, returned %d.",  property.c_str(), dimension);

        return dimension;
    }

    /** @brief, resets the property to it's default value */
    void
    PropertySet::propReset(const std::string &property) throw(std::bad_alloc, 
                                                              OFX::Exception::PropertyUnknownToHost,
                                                              OFX::Exception::PropertyValueIllegalToHost,
                                                              OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        OfxStatus stat = gPropSuite->propReset(_propHandle, property.c_str());
        Log::error(stat != kOfxStatOK, "Failed on reseting property %s to its defaults, host returned status %s.", property.c_str(), mapStatusToString(stat));
        throwPropertyException(stat, property); 
    
        if(_gPropLogging > 0) Log::print("Reset property %s.",  property.c_str());
    }

    /** @brief, Set a single dimension pointer property */
    void 
    PropertySet::propSetPointer(const std::string &property, void *value, int idx) throw(std::bad_alloc, 
                                                                                         OFX::Exception::PropertyUnknownToHost, 
                                                                                         OFX::Exception::PropertyValueIllegalToHost,
                                                                                         OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        OfxStatus stat = gPropSuite->propSetPointer(_propHandle, property.c_str(), idx, value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on setting pointer property %s[%d] to %p, host returned status %s;", 
                        property.c_str(), idx, value, mapStatusToString(stat));
        throwPropertyException(stat, property);  

        if(_gPropLogging > 0) Log::print("Set pointer property %s[%d] to be %p.",  property.c_str(), idx, value);
    }

    /** @brief, Set a single dimension string property */
    void 
    PropertySet::propSetString(const std::string &property, const std::string &value, int idx) throw(std::bad_alloc, 
                                                                                                     OFX::Exception::PropertyUnknownToHost, 
                                                                                                     OFX::Exception::PropertyValueIllegalToHost,
                                                                                                     OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        OfxStatus stat = gPropSuite->propSetString(_propHandle, property.c_str(), idx, value.c_str());
        OFX::Log::error(stat != kOfxStatOK, "Failed on setting string property %s[%d] to %s, host returned status %s;", 
                        property.c_str(), idx, value.c_str(), mapStatusToString(stat));
        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) Log::print("Set string property %s[%d] to be %s.",  property.c_str(), idx, value.c_str());
    }

    /** @brief, Set a single dimension double property */
    void 
    PropertySet::propSetDouble(const std::string &property, double value, int idx) throw(std::bad_alloc, 
                                                                                         OFX::Exception::PropertyUnknownToHost, 
                                                                                         OFX::Exception::PropertyValueIllegalToHost,
                                                                                         OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        OfxStatus stat = gPropSuite->propSetDouble(_propHandle, property.c_str(), idx, value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on setting double property %s[%d] to %lf, host returned status %s;", 
                        property.c_str(), idx, value, mapStatusToString(stat));
        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) Log::print("Set double property %s[%d] to be %lf.",  property.c_str(), idx, value);
    }

    /** @brief, Set a single dimension int property */
    void 
    PropertySet::propSetInt(const std::string &property, int value, int idx) throw(std::bad_alloc, 
                                                                                   OFX::Exception::PropertyUnknownToHost, 
                                                                                   OFX::Exception::PropertyValueIllegalToHost,
                                                                                   OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        OfxStatus stat = gPropSuite->propSetInt(_propHandle, property.c_str(), idx, value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on setting int property %s[%d] to %d, host returned status %s (%d);", 
                        property.c_str(), idx, value, mapStatusToString(stat), stat);
        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) Log::print("Set int property %s[%d] to be %d.",  property.c_str(), idx, value);
    }

    /** @brief Get single pointer property */
    void  *
    PropertySet::propGetPointer(const std::string &property, int idx) const throw(std::bad_alloc, 
                                                                                  OFX::Exception::PropertyUnknownToHost, 
                                                                                  OFX::Exception::PropertyValueIllegalToHost,
                                                                                  OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        void *value = 0;
        OfxStatus stat = gPropSuite->propGetPointer(_propHandle, property.c_str(), idx, &value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on getting pointer property %s[%d], host returned status %s;", 
                        property.c_str(), idx, mapStatusToString(stat));
        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) Log::print("Retrieved pointer property %s[%d], was given %p.",  property.c_str(), idx, value);

        return value;
    }

    /** @brief Get single string property */
    std::string 
    PropertySet::propGetString(const std::string &property, int idx) const throw(std::bad_alloc, 
                                                                                 OFX::Exception::PropertyUnknownToHost, 
                                                                                 OFX::Exception::PropertyValueIllegalToHost,
                                                                                 OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        char *value = "";
        OfxStatus stat = gPropSuite->propGetString(_propHandle, property.c_str(), idx, &value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on getting string property %s[%d], host returned status %s;", 
                        property.c_str(), idx, mapStatusToString(stat));
        throwPropertyException(stat, property);
    
        if(_gPropLogging > 0) Log::print("Retrieved string property %s[%d], was given %s.",  property.c_str(), idx, value);
 
        return std::string(value);
    }

    /** @brief Get single double property */
    double      
    PropertySet::propGetDouble(const std::string &property, int idx) const throw(std::bad_alloc, 
                                                                                 OFX::Exception::PropertyUnknownToHost, 
                                                                                 OFX::Exception::PropertyValueIllegalToHost,
                                                                                 OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        double value;
        OfxStatus stat = gPropSuite->propGetDouble(_propHandle, property.c_str(), idx, &value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on getting double property %s[%d], host returned status %s;", 
                        property.c_str(), idx, mapStatusToString(stat));
        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) Log::print("Retrieved double property %s[%d], was given %lf.",  property.c_str(), idx, value);
        return value;
    }

    /** @brief Get single int property */
    int         
    PropertySet::propGetInt(const std::string &property, int idx) const throw(std::bad_alloc, 
                                                                              OFX::Exception::PropertyUnknownToHost, 
                                                                              OFX::Exception::PropertyValueIllegalToHost,
                                                                              OFX::Exception::Suite)
    {
        assert(_propHandle != 0);
        int value;
        OfxStatus stat = gPropSuite->propGetInt(_propHandle, property.c_str(), idx, &value);
        OFX::Log::error(stat != kOfxStatOK, "Failed on getting int property %s[%d], host returned status %s;", 
                        property.c_str(), idx, mapStatusToString(stat));
        throwPropertyException(stat, property); 

        if(_gPropLogging > 0) Log::print("Retrieved int property %s[%d], was given %d.",  property.c_str(), idx, value);
        return value;
    }

};

