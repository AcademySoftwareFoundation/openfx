#include "./ofxsSupportPrivate.H"

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

using namespace OFX::Private;

namespace OFX {

  /** @brief are we logging properties */
  int PropertySet::_gPropLogging = 0;

  /** @brief Virtual destructor */
  PropertySet::~PropertySet() {}

  /** @brief, returns the dimension of the given property from this property set */
  int 
  PropertySet::propGetDimension(const std::string &property) const throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    int dimension;
    OfxStatus stat = gPropSuite->propGetDimension(_propHandle, property.c_str(), &dimension);
    Log::error(stat != kOfxStatOK, "Failed on fetching dimension for property %s, host returned status %s.", property.c_str(), mapStatusToString(stat));

    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) 
      Log::print("Fetched dimension of property %s, returned %d.",  property.c_str(), dimension);

    return dimension;
  }

  /** @brief, resets the property to it's default value */
  void
  PropertySet::propReset(const std::string &property) throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    OfxStatus stat = gPropSuite->propReset(_propHandle, property.c_str());
    Log::error(stat != kOfxStatOK, "Failed on reseting property %s to its defaults, host returned status %s.", property.c_str(), mapStatusToString(stat));
    throwSuiteStatusException(stat); 
    
    if(_gPropLogging > 0) Log::print("Reset property %s.",  property.c_str());
  }

  /** @brief, Set a single dimension pointer property */
  void 
  PropertySet::propSetPointer(const std::string &property, void *value, int idx) throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    OfxStatus stat = gPropSuite->propSetPointer(_propHandle, property.c_str(), idx, value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on setting pointer property %s[%d] to %p, host returned status %s;", property.c_str(), idx, value, mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Set pointer property %s[%d] to be %p.",  property.c_str(), idx, value);
  }

  /** @brief, Set a single dimension string property */
  void 
  PropertySet::propSetString(const std::string &property, const std::string &value, int idx) throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    OfxStatus stat = gPropSuite->propSetString(_propHandle, property.c_str(), idx, value.c_str());
    OFX::Log::error(stat != kOfxStatOK, "Failed on setting string property %s[%d] to %s, host returned status %s;", 
		    property.c_str(), idx, value.c_str(), mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Set string property %s[%d] to be %s.",  property.c_str(), idx, value.c_str());
  }

  /** @brief, Set a single dimension double property */
  void 
  PropertySet::propSetDouble(const std::string &property, double value, int idx) throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    OfxStatus stat = gPropSuite->propSetDouble(_propHandle, property.c_str(), idx, value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on setting double property %s[%d] to %lf, host returned status %s;", 
		    property.c_str(), idx, value, mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Set double property %s[%d] to be %lf.",  property.c_str(), idx, value);
  }

  /** @brief, Set a single dimension int property */
  void 
  PropertySet::propSetInt(const std::string &property, int value, int idx) throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    OfxStatus stat = gPropSuite->propSetInt(_propHandle, property.c_str(), idx, value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on setting int property %s[%d] to %lf, host returned status %s;", 
		    property.c_str(), idx, value, mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Set int property %s[%d] to be %d.",  property.c_str(), idx, value);
  }

  /** @brief Get single pointer property */
  void  *
  PropertySet::propGetPointer(const std::string &property, int idx) const throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    void *value;
    OfxStatus stat = gPropSuite->propGetPointer(_propHandle, property.c_str(), idx, &value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on getting pointer property %s[%d], host returned status %s;", 
		    property.c_str(), idx, mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Retrieved pointer property %s[%d], was given %p.",  property.c_str(), idx, value);

    return value;
  }

  /** @brief Get single string property */
  std::string 
  PropertySet::propGetString(const std::string &property, int idx) const throw(OFX::Exception::Suite &, std::bad_alloc)
  {
    assert(_propHandle != 0);
    char *value;
    OfxStatus stat = gPropSuite->propGetString(_propHandle, property.c_str(), idx, &value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on getting string property %s[%d], host returned status %s;", 
		    property.c_str(), idx, mapStatusToString(stat));
    throwSuiteStatusException(stat);
    
    if(_gPropLogging > 0) Log::print("Retrieved string property %s[%d], was given %s.",  property.c_str(), idx, value);
 
    return std::string(value);
  }

  /** @brief Get single double property */
  double      
  PropertySet::propGetDouble(const std::string &property, int idx) const throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    double value;
    OfxStatus stat = gPropSuite->propGetDouble(_propHandle, property.c_str(), idx, &value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on getting double property %s[%d], host returned status %s;", 
		    property.c_str(), idx, mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Retrieved double property %s[%d], was given %lf.",  property.c_str(), idx, value);
    return value;
  }

  /** @brief Get single int property */
  int         
  PropertySet::propGetInt(const std::string &property, int idx) const throw(OFX::Exception::Suite)
  {
    assert(_propHandle != 0);
    int value;
    OfxStatus stat = gPropSuite->propGetInt(_propHandle, property.c_str(), idx, &value);
    OFX::Log::error(stat != kOfxStatOK, "Failed on getting int property %s[%d], host returned status %s;", 
		    property.c_str(), idx, mapStatusToString(stat));
    throwSuiteStatusException(stat); 

    if(_gPropLogging > 0) Log::print("Retrieved int property %s[%d], was given %d.",  property.c_str(), idx, value);
    return value;
  }

#if 0
  // set multiple values
  void propSetPointerN(const std::string &property, const std::vector<void *> &values) throw(OFX::Exception::Suite);
  void propSetStringN(const std::string &property,  const std::vector<std::string> &values) throw(OFX::Exception::Suite);
  void propSetDoubleN(const std::string &property,  const std::vector<double> &values) throw(OFX::Exception::Suite);
  void propSetIntN(const std::string &property,     const std::vector<int> &values) throw(OFX::Exception::Suite);
  
  // get multiple values
  void propGetPointerN(const std::string &property, std::vector<void *> &values,      int N) const throw(OFX::Exception::Suite);;
  void propGetStringN(const std::string &property,  std::vector<std::string> &values, int N) const throw(OFX::Exception::Suite);;
  void propGetDoubleN(const std::string &property,  std::vector<double> &values,      int N) const throw(OFX::Exception::Suite);;
  void propGetIntN(const std::string &property,     std::vector<int> &values,         int N) const throw(OFX::Exception::Suite);;
#endif
  
};

