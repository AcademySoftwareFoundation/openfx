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

#include <vector>
#include <string>
#include <map>

/** @brief Nasty macro used to define empty protected copy ctors and assign ops */
#define mDeclareProtectedAssignAndCC(CLASS) \
  CLASS &operator=(const CLASS &v1) {assert(false); return *this;}	\
  CLASS(const CLASS &v) {assert(false); } 

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
 */
namespace OFX {
  /** forward class declarations */
  class Exception;
  class PropertySet;


  /** @brief Enumerates the different types a property can be */
  enum PropertyTypeEnum {ePointer,
			 eInt,
			 eString,
			 eDouble};
  /** @brief OFX base exception thrown by any function wrapping a base level ofx suite call that could return an error code;
   */
  class Exception {
  protected :
    OfxStatus _status;

  public :
    Exception(OfxStatus s) : _status(s) {}
    OfxStatus status(void) {return _status;}
    operator OfxStatus() {return _status;}
  };

  /** @brief Throws an @ref OFX::Exception depending on the status flag passed in */
  void throwStatusException(OfxStatus stat) throw(OFX::Exception);

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
    void propSetHandle(OfxPropertySetHandle h) { _propHandle = h;}

    /** @brief return the handle for this property set */
    OfxPropertySetHandle propSetHandle(void) {return _propHandle;}

    int  propGetDimension(const std::string &property) const throw(OFX::Exception);
    void propReset(const std::string &property) throw(OFX::Exception);

    // set single values
    void propSetPointer(const std::string &property, void *value, int idx = 0) throw(OFX::Exception);
    void propSetString(const std::string &property, const std::string &value, int idx = 0) throw(OFX::Exception);
    void propSetDouble(const std::string &property, double value, int idx = 0) throw(OFX::Exception);
    void propSetInt(const std::string &property, int value, int idx = 0) throw(OFX::Exception);

    // set multiple values
    void propSetPointerN(const std::string &property, const std::vector<void *> &values) throw(OFX::Exception);
    void propSetStringN(const std::string &property,  const std::vector<std::string> &values) throw(OFX::Exception);
    void propSetDoubleN(const std::string &property,  const std::vector<double> &values) throw(OFX::Exception);
    void propSetIntN(const std::string &property,  const std::vector<int> &values) throw(OFX::Exception);
  
    // get single values
    void       *propGetPointer(const std::string &property, int idx = 0) const throw(OFX::Exception);
    std::string propGetString(const std::string &property, int idx = 0) const throw(OFX::Exception);
    double      propGetDouble(const std::string &property, int idx = 0) const throw(OFX::Exception);
    int         propGetInt(const std::string &property, int idx = 0) const throw(OFX::Exception);
  };

  /** @brief maps a status to a string for debugging purposes, note a c-str for printf */
  char * mapStatusToString(OfxStatus stat);
};

// undeclare the protected assign and CC macro
#undef mDeclareProtectedAssignAndCC

#endif
