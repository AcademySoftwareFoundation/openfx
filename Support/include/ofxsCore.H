#ifndef _ofxsCore_H_
#define _ofxsCore_H_
/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004 The Foundry Visionmongers Ltd
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

#include <ofxCore.h>
#include <ofxImageEffect.h>
#include <ofxInteract.h>
#include <ofxKeySyms.h>
#include <ofxMemory.h>
#include <ofxMessage.h>
#include <ofxMultiThread.h>
#include <ofxParam.h>
#include <ofxProperty.h>

#include <assert.h>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <stdexcept>

/** @brief Nasty macro used to define empty protected copy ctors and assign ops */
#define mDeclareProtectedAssignAndCC(CLASS) \
  CLASS &operator=(const CLASS &v1) {assert(false); return *this;}	\
  CLASS(const CLASS &v) {assert(false); } 

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
 */
namespace OFX {
    /** forward class declarations */
    class PropertySet;


    /** @brief Enumerates the different types a property can be */
    enum PropertyTypeEnum {
        ePointer,
        eInt,
        eString,
        eDouble};

    /** @brief Enumerates the reasons a plug-in instance may have had one of its values changed */
    enum InstanceChangeReason {
        eChangeUserEdit,    /**< @brief A user actively editted something in the plugin, eg: changed the value of an integer param on an interface */
        eChangePluginEdit,  /**< @brief The plugin's own code changed something in the instance, eg: a callback on on param settting the value of another */
        eChangeTime         /**< @brief The current value of a parameter has changed because the param animates and the current time has changed */
    };

    /** @brief maps a status to a string for debugging purposes, note a c-str for printf */
    char * mapStatusToString(OfxStatus stat);

    /** @brief namespace for OFX support lib exceptions, all derive from std::exception, calling it */
    namespace Exception {

        /** @brief thrown when a suite returns a dud status code 
        */
        class Suite : public std::exception {
        protected :
            OfxStatus _status;
        public :
            Suite(OfxStatus s) : _status(s) {}
            OfxStatus status(void) {return _status;}
            operator OfxStatus() {return _status;}

            /** @brief reimplemented from std::exception */
            virtual const char * what () const throw () {return mapStatusToString(_status);}

        };

        /** @brief Exception indicating that a host doesn't know about a property that is should do */
        class PropertyUnknownToHost : public std::exception {
        protected :
            const char *_what;
        public :
            PropertyUnknownToHost(const char *what) : _what(what) {}

            /** @brief reimplemented from std::exception */
            virtual const char * what () const throw ()
            {
                return _what;
            }
        };

        /** @brief exception indicating that the host thinks a property has an illegal value */
        class PropertyValueIllegalToHost : public std::exception {
        protected :
            const char *_what;
        public :
            PropertyValueIllegalToHost(const char *what) : _what(what) {}

            /** @brief reimplemented from std::exception */
            virtual const char * what () const throw ()
            {
                return _what;
            }
        };

        /** @brief exception indicating a request for a named thing exists (eg: a param), but is of the wrong type, should never make it back to the main entry
        indicates a logical error in the code. Asserts are raised in debug code in these situations.
        */
        class TypeRequest : public std::exception {
        protected :
            const char *_what;
        public :
            TypeRequest(const char *what) : _what(what) {}

            /** @brief reimplemented from std::exception */
            virtual const char * what () const throw ()
            {
                return _what;
            }
        };

        ////////////////////////////////////////////////////////////////////////////////
        // These execeptions are to be thrown by the plugin if it hits a problem, the
        // code managing the main entry will trap the exception and return a suitable 
        // status code to the host.

        /** @brief exception indicating a required host feature is missing */
        class HostInadequate : public std::exception {
        protected :
            char *_what;
        public :
            HostInadequate(char *what) : _what(what) {}

            /** @brief reimplemented from std::exception */
            virtual const char * what () const throw ()
            {
                return _what;
            }
        };

    }; // end of Exception namespace

    /** @brief Throws an @ref OFX::Exception::Suite depending on the status flag passed in */
    void 
    throwSuiteStatusException(OfxStatus stat) 
        throw(OFX::Exception::Suite, std::bad_alloc);

    /** @brief This struct is used to return an identifier for the plugin by the function @ref OFX:Plugin::getPlugin. 
    The members correspond to those in the OfxPlugin struct defined in ofxCore.h.
    */
    struct PluginID {
        std::string  pluginIdentifier;
        unsigned int pluginVersionMajor;
        unsigned int pluginVersionMinor;
    };

    /** @brief This class wraps up an OFX property set */
    class PropertySet {
    protected :
        /** @brief The raw property handle */
        OfxPropertySetHandle _propHandle;

        /** @brief Class static, whether we are logging each property action */
        static int _gPropLogging;

    public :
        /** @brief turns on logging of property access functions */
        static void propEnableLogging(void)  {++_gPropLogging;}
    
        /** @brief turns off logging of property access functions */
        static void propDisableLogging(void) {--_gPropLogging;}

        /** @brief construct a property set */
        PropertySet(OfxPropertySetHandle h = 0) : _propHandle(h) {}
        virtual ~PropertySet();

        /** @brief set the handle to use for this set */
        void propSetHandle(OfxPropertySetHandle h) { _propHandle = h;}

        /** @brief return the handle for this property set */
        OfxPropertySetHandle propSetHandle(void) {return _propHandle;}

        int  propGetDimension(const std::string &property) const throw(std::bad_alloc, 
                                                                       OFX::Exception::PropertyUnknownToHost, 
                                                                       OFX::Exception::PropertyValueIllegalToHost);
        void propReset(const std::string &property) throw(std::bad_alloc, 
                                                          OFX::Exception::PropertyUnknownToHost, 
                                                          OFX::Exception::PropertyValueIllegalToHost);

        // set single values
        void propSetPointer(const std::string &property, void *value, int idx = 0) throw(std::bad_alloc, 
                                                                                         OFX::Exception::PropertyUnknownToHost, 
                                                                                         OFX::Exception::PropertyValueIllegalToHost);
        void propSetString(const std::string &property, const std::string &value, int idx = 0) throw(std::bad_alloc, 
                                                                                                     OFX::Exception::PropertyUnknownToHost, 
                                                                                                     OFX::Exception::PropertyValueIllegalToHost);
        void propSetDouble(const std::string &property, double value, int idx = 0) throw(std::bad_alloc, 
                                                                                         OFX::Exception::PropertyUnknownToHost, 
                                                                                         OFX::Exception::PropertyValueIllegalToHost);
        void propSetInt(const std::string &property, int value, int idx = 0) throw(std::bad_alloc, 
                                                                                   OFX::Exception::PropertyUnknownToHost, 
                                                                                   OFX::Exception::PropertyValueIllegalToHost);
  
        // get single values
        void       *propGetPointer(const std::string &property, int idx = 0) const throw(std::bad_alloc, 
                                                                                         OFX::Exception::PropertyUnknownToHost, 
                                                                                         OFX::Exception::PropertyValueIllegalToHost);
        std::string propGetString(const std::string &property, int idx = 0) const throw(std::bad_alloc, 
                                                                                        OFX::Exception::PropertyUnknownToHost, 
                                                                                        OFX::Exception::PropertyValueIllegalToHost);
        double      propGetDouble(const std::string &property, int idx = 0) const throw(std::bad_alloc, 
                                                                                        OFX::Exception::PropertyUnknownToHost, 
                                                                                        OFX::Exception::PropertyValueIllegalToHost);
        int         propGetInt(const std::string &property, int idx = 0) const throw(std::bad_alloc, 
                                                                                     OFX::Exception::PropertyUnknownToHost, 
                                                                                     OFX::Exception::PropertyValueIllegalToHost);
    };

    ////////////////////////////////////////////////////////////////////////////////

    /** @brief namespace for memory allocation that is done via wrapping the ofx memory suite */
    namespace Memory {
        /** @brief allocate n bytes, returns a pointer to it */
        void *alloc(size_t nBytes, void *handleToAssociateWithTheAllocation = 0) throw(std::bad_alloc);

        /** @brief free n previously allocated memory */
        void free(void *ptr) throw();

    };

};

// undeclare the protected assign and CC macro
#undef mDeclareProtectedAssignAndCC

#endif
