/*
Software License :

Copyright (c) 2004, The Foundry Visonmongers Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Foundry Visonmongers Ltd, nor the names of its 
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


/** @file testProperties.cpp Ofx host testing plug-in which logs all the needed properties
 */
#include <string> // stl strings
#include "ofxImageEffect.h"
#include "ofxMemory.h"
#include "ofxMultiThread.h"
#include "ofxMessage.h"

#include "ofxLog.H"

static OfxHost               *gHost;
static OfxImageEffectSuiteV1 *gEffectSuite;
static OfxPropertySuiteV1    *gPropSuite;
static OfxInteractSuiteV1    *gInteractSuite;
static OfxParameterSuiteV1   *gParamSuite;
static OfxMemorySuiteV1      *gMemorySuite;
static OfxMultiThreadSuiteV1 *gThreadSuite;
static OfxMessageSuiteV1 *gMessageSuite;

/** @brief wraps up a set of properties
 */
class PropertySet {
 protected :
  bool _propLogMessages;

  OfxPropertySetHandle _propHandle;

  void propLogMessages(bool v) {_propLogMessages = v;}

 public :
  PropertySet(OfxPropertySetHandle h = 0) 
    : _propHandle(h)
    , _propLogMessages(true)
  {}

  virtual ~PropertySet();
  void propSetHandle(OfxPropertySetHandle h) { _propHandle = h;}

  // these set
  OfxStatus propSetPointer(const char *property, void *value, int idx = 0);
  OfxStatus propSetString(const char *property, const std::string &value, int idx = 0);
  OfxStatus propSetDouble(const char *property, double value, int idx = 0);
  OfxStatus propSetInt(const char *property, int value, int idx = 0);
  OfxStatus propSetDoubleN(const char *property, double *value, int n);
  
  // these fetch
  OfxStatus propGetPointer(const char *property, void * &value, int idx = 0) const;
  OfxStatus propGetDouble(const char *property, double &value, int idx = 0) const;
  OfxStatus propGetInt(const char *property, int &value, int idx = 0) const;
  OfxStatus propGetIntN(const char *property, int *values, int N) const;
  OfxStatus propGetDoubleN(const char *property, double *value, int n) const;
  OfxStatus propGetString(const char *property, std::string  &value, int idx = 0) const;

  // these check for existance and dimensionality
  OfxStatus propGetDimension(const char *property, int &size);
};

////////////////////////////////////////////////////////////////////////////////
// POD structure that defines a property, name, dimension and type
struct PropertyDescription {
  enum TypeEnum {ePointer,
		 eInteger,
		 eString,
		 eDouble};
  char *name;
  int   dimension; // -1 implies variable dim
  TypeEnum ilk;
};

////////////////////////////////////////////////////////////////////////////////
// Describes a set of properties
class PropertySetDescription : PropertySet {
 protected :
  char                *_setName;
  PropertyDescription *_descriptions;
  int                  _nDescriptions;

 public :
  PropertySetDescription(char *setName, OfxPropertySetHandle handle, PropertyDescription *d, int nD)
    : PropertySet(handle)
    , _setName(setName)
    , _descriptions(d)
    , _nDescriptions(nD)
  {}

  void checkProperties(void);
};

////////////////////////////////////////////////////////////////////////////////
/** @brief describes a host applications capabilities so we can switch beets
 */
class HostDescription : public PropertySet {
 public :
  std::string			type;
  std::string			hostName;
  std::string			hostLabel;
  int                           hostIsBackground;
  int				supportsOverlays;
  int				supportsMultiResolution;
  int				supportsTiles;
  int				temporalClipAccess;
  int				supportsMultipleClipDepths;
  int				supportsMultipleClipPARs;
  int				supportsSetableFrameRate;
  int				supportsSetableFielding;
  int				supportsStringAnimation;
  int				supportsCustomInteract;
  int				supportsChoiceAnimation;
  int				supportsBooleanAnimation;
  int				supportsCustomAnimation;
  int				maxParameters;
  int				maxPages;
  int				pageRowCount;
  int				pageColumnCount;

  HostDescription(OfxPropertySetHandle handle);
    
};
HostDescription *gHostDescription = 0;


////////////////////////////////////////////////////////////////////////////////
// maps status to a string
static char *
mapStatus(OfxStatus stat)
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


////////////////////////////////////////////////////////////////////////////////
// property set code
PropertySet::~PropertySet()
{
}

OfxStatus 
PropertySet::propSetPointer(const char *property, void *value, int idx)
{
  OfxStatus stat = gPropSuite->propSetPointer(_propHandle, property, idx, value);
  return stat;
}

OfxStatus 
PropertySet::propSetString(const char *property, const std::string &value, int idx)
{
  OfxStatus stat = gPropSuite->propSetString(_propHandle, property, idx, value.c_str());
  return stat;
}

OfxStatus 
PropertySet::propSetDouble(const char *property, double value, int idx)
{
  OfxStatus stat = gPropSuite->propSetDouble(_propHandle, property, idx, value);
  return stat;
}

OfxStatus 
PropertySet::propSetDoubleN(const char *property, double *values, int N)
{
  OfxStatus stat = gPropSuite->propSetDoubleN(_propHandle, property, N, values);
  return stat;
}

OfxStatus 
PropertySet::propSetInt(const char *property, int value, int idx)
{
  OfxStatus stat = gPropSuite->propSetInt(_propHandle, property, idx, value);
  return stat;
}
  
OfxStatus 
PropertySet::propGetPointer(const char *property, void * &value, int idx) const
{
  OfxStatus stat = gPropSuite->propGetPointer(_propHandle, property, idx, &value);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching pointer property %s[%d], host returned status %s", property, idx, mapStatus(stat));
  if(stat == kOfxStatOK && _propLogMessages) 
    OFX::logPrint("Fetched pointer property %s[%d] = %p", property, idx, value);
  return stat;
}

OfxStatus 
PropertySet::propGetDouble(const char *property, double &value, int idx) const
{
  OfxStatus stat = gPropSuite->propGetDouble(_propHandle, property, idx, &value);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching double property %s[%d], host returned status %s", property, idx, mapStatus(stat));
  if(stat == kOfxStatOK && _propLogMessages) 
    OFX::logPrint("Fetched double property %s[%d] = %g", property, idx, value);
  return stat;
}

OfxStatus 
PropertySet::propGetDoubleN(const char *property, double *values, int N) const
{
  OfxStatus stat = gPropSuite->propGetDoubleN(_propHandle, property, N, values);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching multiple double property %s X %d, host returned status %s", property, N, mapStatus(stat));
  if(stat == kOfxStatOK && _propLogMessages) {
    OFX::logPrint("Fetched multiple double property %s X %d", property, N);
    for(int i = 0; i < N; i++) {      
      OFX::logPrint("  %s[%d] = %g", property, i, values[i]);
    }
  }
  return stat;
}

OfxStatus 
PropertySet::propGetIntN(const char *property, int *values, int N) const
{
  OfxStatus stat = gPropSuite->propGetIntN(_propHandle, property, N, values);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching multiple int property %s X %d, host returned status %s", property, N, mapStatus(stat));
  if(stat == kOfxStatOK && _propLogMessages) {
    OFX::logPrint("Fetched multiple int property %s X %d", property, N);
    for(int i = 0; i < N; i++) {      
      OFX::logPrint("  %s[%d] = %f", property, i, values[i]);
    }
  }
  return stat;
}


OfxStatus 
PropertySet::propGetInt(const char *property, int &value, int idx) const
{
  OfxStatus stat = gPropSuite->propGetInt(_propHandle, property, idx, &value);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching int property %s[%d], host returned status %s", property, idx, mapStatus(stat));
  if(stat == kOfxStatOK && _propLogMessages) 
    OFX::logPrint("Fetched int property %s[%d] = %d", property, idx, value);
  return stat;
}

OfxStatus 
PropertySet::propGetString(const char *property, std::string  &value, int idx) const
{
  char *str;
  OfxStatus stat = gPropSuite->propGetString(_propHandle, property, idx, &str);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching string property %s[%d], host returned status %s", property, idx, mapStatus(stat));
  if(kOfxStatOK == stat && _propLogMessages) {
    value = str;
    OFX::logPrint("Fetched string property %s[%d] = %s", property, idx, value.c_str());
  }
  else {
    value = "";
  }
  return stat;
}

OfxStatus 
PropertySet::propGetDimension(const char *property, int &size)
{
  OfxStatus stat = gPropSuite->propGetDimension(_propHandle, property, &size);
  OFX::logError(stat != kOfxStatOK, "Failed on fetching dimension for property %s, host returned status %s", property, mapStatus(stat));
}

////////////////////////////////////////////////////////////////////////////////
// PropertySetDescription code

void
PropertySetDescription::checkProperties(void)
{
  OFX::logPrint("PropertySetDescription::checkProperties - start, checking properties of %s", _setName);

  // don't print ordinary messages whilst we are checking them
  propLogMessages(false);

  // for each property in the description
  for(int i = 0; i < _nDescriptions; i++) {
    int dimension;

    // first check it's dimension and existance via the propGetDimension call
    OfxStatus stat = propGetDimension(_descriptions[i].name, dimension);

    if(stat == kOfxStatOK) {
      if(_descriptions[i].dimension != -1) // implies variable dimension
	OFX::logError(dimension != _descriptions[i].dimension, "Host reports property '%s' with dimension %d, should be %d", _descriptions[i].name, dimension, _descriptions[i].dimension);

      // check type by getting the first element, the property getting will print failure messages to the log
      if(dimension > 0) {
	void       *vP;
	int         vI;
	double      vD;
	std::string vS;
	
	switch(_descriptions[i].ilk) {
	case PropertyDescription::ePointer : propGetPointer(_descriptions[i].name, vP); break;
	case PropertyDescription::eInteger : propGetInt(_descriptions[i].name, vI); break;
	case PropertyDescription::eString  : propGetString(_descriptions[i].name, vS); break;
	case PropertyDescription::eDouble  : propGetDouble(_descriptions[i].name, vD); break;
	}
      }
    }
  }
  propLogMessages(true);

  OFX::logPrint("PropertySetDescription::checkProperties - stop");
}

////////////////////////////////////////////////////////////////////////////////
// host description stuff

// list of the properties needed on the host
static PropertyDescription gHostPropDescription[ ] = 
{
  kOfxPropType,                                  1, PropertyDescription::eString,
  kOfxPropName,                                  1, PropertyDescription::eString,
  kOfxPropLabel,                                 1, PropertyDescription::eString,
  kOfxImageEffectHostPropIsBackground,           1, PropertyDescription::eInteger,
  kOfxImageEffectPropSupportsOverlays,           1, PropertyDescription::eInteger,
  kOfxImageEffectPropSupportsMultiResolution,    1, PropertyDescription::eInteger,
  kOfxImageEffectPropSupportsTiles,              1, PropertyDescription::eInteger,
  kOfxImageEffectPropTemporalClipAccess,         1, PropertyDescription::eInteger,
  kOfxImageEffectPropSupportsMultipleClipDepths, 1, PropertyDescription::eInteger,
  kOfxImageEffectPropSupportsMultipleClipPARs,   1, PropertyDescription::eInteger,
  kOfxImageEffectPropSetableFrameRate,           1, PropertyDescription::eInteger,
  kOfxImageEffectPropSetableFielding,            1, PropertyDescription::eInteger,
  kOfxImageEffectPropSupportedComponents,       -1, PropertyDescription::eString,
  kOfxImageEffectPropSupportedContexts,         -1, PropertyDescription::eString,

  kOfxParamHostPropSupportsStringAnimation,      1, PropertyDescription::eInteger,
  kOfxParamHostPropSupportsCustomInteract,       1, PropertyDescription::eInteger,
  kOfxParamHostPropSupportsChoiceAnimation,      1, PropertyDescription::eInteger,
  kOfxParamHostPropSupportsBooleanAnimation,     1, PropertyDescription::eInteger,
  kOfxParamHostPropSupportsCustomAnimation,      1, PropertyDescription::eInteger,
  kOfxParamHostPropMaxParameters,                1, PropertyDescription::eInteger,
  kOfxParamHostPropMaxPages,                     1, PropertyDescription::eInteger,
  kOfxParamHostPropPageRowColumnCount,           2, PropertyDescription::eInteger,
}; 

// create a host description, checking properties on the way
HostDescription::HostDescription(OfxPropertySetHandle handle) :
  PropertySet(handle) ,
  hostName(""),
  supportsOverlays(false),
  supportsMultiResolution(false),
  supportsTiles(false),
  temporalClipAccess(false),
  supportsMultipleClipDepths(false),
  supportsMultipleClipPARs(false),
  supportsSetableFrameRate(false),
  supportsSetableFielding(false),
  supportsCustomAnimation(false),
  supportsStringAnimation(false),
  supportsCustomInteract(false),
  supportsChoiceAnimation(false),
  supportsBooleanAnimation(false),
  maxParameters(-1),
  maxPages(-1),
  pageRowCount(-1),
  pageColumnCount(-1)
{
  OFX::logPrint("HostDescription::HostDescription - start, fetching host description");

  // do basic existance checking with a PropertySetDescription
  PropertySetDescription hostPropSet("Host Properties", handle, gHostPropDescription, sizeof(gHostPropDescription)/sizeof(PropertyDescription));
  hostPropSet.checkProperties();

  // now go through and fill in the host description
  OfxStatus status;

  status = propGetString(kOfxPropType, type);
  OFX::logError(type != kOfxTypeImageEffectHost, "kOfxPropType should have value '%s' not '%s'", kOfxTypeImageEffectHost, type.c_str());

  propGetString(kOfxPropName, hostName);
  propGetString(kOfxPropLabel, hostLabel);

  propGetInt(kOfxImageEffectHostPropIsBackground, hostIsBackground);
  propGetInt(kOfxImageEffectPropSupportsOverlays, supportsOverlays);
  propGetInt(kOfxImageEffectPropSupportsMultiResolution, supportsMultiResolution);
  propGetInt(kOfxImageEffectPropSupportsTiles, supportsTiles);
  propGetInt(kOfxImageEffectPropTemporalClipAccess, temporalClipAccess);
  propGetInt(kOfxImageEffectPropSupportsMultipleClipDepths, supportsMultipleClipDepths);
  propGetInt(kOfxImageEffectPropSupportsMultipleClipPARs, supportsMultipleClipPARs);
  propGetInt(kOfxImageEffectPropSetableFrameRate, supportsSetableFrameRate);
  propGetInt(kOfxImageEffectPropSetableFielding, supportsSetableFielding);
  propGetInt(kOfxParamHostPropSupportsStringAnimation, supportsCustomAnimation);
  propGetInt(kOfxParamHostPropSupportsCustomInteract, supportsStringAnimation);
  propGetInt(kOfxParamHostPropSupportsChoiceAnimation, supportsCustomInteract);
  propGetInt(kOfxParamHostPropSupportsBooleanAnimation, supportsChoiceAnimation);
  propGetInt(kOfxParamHostPropSupportsCustomAnimation, supportsBooleanAnimation);
  propGetInt(kOfxParamHostPropMaxParameters, maxParameters);
  propGetInt(kOfxParamHostPropMaxPages, maxPages);
  
  int rcCount[2];
  propGetIntN(kOfxParamHostPropPageRowColumnCount, rcCount, 2);
  pageRowCount = rcCount[0];
  pageColumnCount = rcCount[1];
  
  OFX::logPrint("HostDescription::HostDescription - stop");
}


////////////////////////////////////////////////////////////////////////////////
// fetch a suite
static void *
fetchSuite(char *suiteName, int suiteVersion, bool optional = false)
{
  void *suite = gHost->fetchSuite(gHost->host, suiteName, suiteVersion);
  if(optional)
    OFX::logWarning(suite == 0, "Could not fetch the optional suite '%s' version %d.", suiteName, suiteVersion);
  else
    OFX::logError(suite == 0, "Could not fetch the mandatory suite '%s' version %d.", suiteName, suiteVersion);
  if(!optional && suite == 0) throw kOfxStatErrMissingHostFeature;
  return suite;
}


// how many times has actionLoad been called
static int gLoadCount = 0;

/** @brief Called at load */
static OfxStatus
actionLoad(void)
{
  OFX::logPrint("loadAction - start");
  OFX::logError(gLoadCount != 0, "Load action called more than once without unload being called.");
  gLoadCount++;


  OfxStatus status = kOfxStatOK;
  
  try {
    // fetch the hosts
    OFX::logError(gHost == 0, "Host pointer has not been set.");
    if(!gHost) throw kOfxStatErrBadHandle;
    
    if(gLoadCount == 1) {
      gEffectSuite    = (OfxImageEffectSuiteV1 *) fetchSuite(kOfxImageEffectSuite, 1);
      gPropSuite      = (OfxPropertySuiteV1 *)    fetchSuite(kOfxPropertySuite, 1);
      gParamSuite     = (OfxParameterSuiteV1 *)   fetchSuite(kOfxParameterSuite, 1);
      gMemorySuite    = (OfxMemorySuiteV1 *)      fetchSuite(kOfxMemorySuite, 1);
      gThreadSuite    = (OfxMultiThreadSuiteV1 *) fetchSuite(kOfxMultiThreadSuite, 1);
      gMessageSuite  = (OfxMessageSuiteV1 *)     fetchSuite(kOfxMessageSuite, 1);

      // OK check and fetch host information
      gHostDescription = new HostDescription(gHost->host);

      // fetch the interact suite if the host supports interaction
      if(gHostDescription->supportsOverlays || gHostDescription->supportsCustomInteract)
	gInteractSuite  = (OfxInteractSuiteV1 *)    fetchSuite(kOfxInteractSuite, 1);
    }
  }

  catch(int err) {
    status = err;
  }

  OFX::logPrint("loadAction - stop");
  return status;
}

/** @brief Called before unload */
static OfxStatus
unLoadAction(void)
{
  OFX::logError(gLoadCount != 0, "UnLoad action called without a corresponding load action having been called.");
  gLoadCount--;
  
  // force these to null
  gEffectSuite = 0;
  gPropSuite = 0;
  gParamSuite = 0;
  gMemorySuite = 0;
  gThreadSuite = 0;
  gMessageSuite = 0;
  gInteractSuite = 0;
  return kOfxStatOK;
}

//  instance construction
static OfxStatus
createInstance( OfxImageEffectHandle effect)
{
  return kOfxStatOK;
}

// instance destruction
static OfxStatus
destroyInstance( OfxImageEffectHandle  effect)
{
  return kOfxStatOK;
}

// tells the host what region we are capable of filling
OfxStatus 
getSpatialRoD( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  return kOfxStatOK;
}

// tells the host how much of the input we need to fill the given window
OfxStatus 
getSpatialRoI( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  return kOfxStatOK;
}

// Tells the host how many frames we can fill, only called in the general context.
// This is actually redundant as this is the default behaviour, but for illustrative
// purposes.
OfxStatus 
getTemporalDomain( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  return kOfxStatOK;
}


// Set our clip preferences 
static OfxStatus 
getClipPreferences( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs,  OfxPropertySetHandle outArgs)
{
  return kOfxStatOK;
}

// are the settings of the effect performing an identity operation
static OfxStatus
isIdentity( OfxImageEffectHandle effect,
	    OfxPropertySetHandle inArgs,
	    OfxPropertySetHandle outArgs)
{
  // In this case do the default, which in this case is to render 
  return kOfxStatReplyDefault;
}

////////////////////////////////////////////////////////////////////////////////
// function called when the instance has been changed by anything
static OfxStatus
instanceChanged( OfxImageEffectHandle  effect,
		 OfxPropertySetHandle inArgs,
		 OfxPropertySetHandle outArgs)
{
  // don't trap any others
  return kOfxStatReplyDefault;
}

// the process code  that the host sees
static OfxStatus render(OfxImageEffectHandle  instance,
			OfxPropertySetHandle inArgs,
			OfxPropertySetHandle outArgs)
{  
  return kOfxStatOK;
}

//  describe the plugin in context
static OfxStatus
describeInContext( OfxImageEffectHandle  effect,  OfxPropertySetHandle inArgs)
{
  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// the plugin's description routine
static OfxStatus
describe(OfxImageEffectHandle  effect)
{  
  return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////.
// check handles to the main function
static void
checkMainHandles(const char *action,  const void *handle, OfxPropertySetHandle inArgsHandle,  OfxPropertySetHandle outArgsHandle,
		 bool handleCanBeNull, bool inArgsCanBeNull, bool outArgsCanBeNull)
{
  if(handleCanBeNull)
      OFX::logWarning(handle != 0, "Handle passed to '%s' is not null.", action);
  else
      OFX::logError(handle != 0, "'Handle passed to '%s' is null.", action);

  if(inArgsCanBeNull)
      OFX::logWarning(inArgsHandle != 0, "'inArgs' Handle passed to '%s' is not null.", action);
  else
      OFX::logError(inArgsHandle != 0, "'inArgs' handle passed to '%s' is null.", action);

  if(outArgsCanBeNull)
      OFX::logWarning(outArgsHandle != 0, "'outArgs' Handle passed to '%s' is not null.", action);
  else
      OFX::logError(outArgsHandle != 0, "'outArgs' handle passed to '%s' is null.", action);

  if(!handleCanBeNull && !handle) throw kOfxStatErrBadHandle;
  if(!inArgsCanBeNull && !inArgsHandle) throw kOfxStatErrBadHandle;
  if(!outArgsCanBeNull && !outArgsHandle) throw kOfxStatErrBadHandle;
}

////////////////////////////////////////////////////////////////////////////////
// The main function
static OfxStatus
pluginMain(const char *action,  const void *handle, OfxPropertySetHandle inArgsHandle,  OfxPropertySetHandle outArgsHandle)
{
  OFX::logPrint("pluginMain - start");
  OFX::logPrint("  action is '%s'", action);
  OfxStatus stat = kOfxStatReplyDefault;

  try {
    // cast to handle appropriate type
    OfxImageEffectHandle effect = (OfxImageEffectHandle) handle;
  
    // construct two property set wrappers
    PropertySet inArgs(inArgsHandle);
    PropertySet outArgs(outArgsHandle);
  
    if(OFX::strEquals(action, kOfxActionLoad)) {    
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, true, true, true);
      stat = actionLoad();
    }
    else if(OFX::strEquals(action, kOfxActionUnload)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, true, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionDescribe)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionPurgeCaches)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionSyncPrivateData)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionCreateInstance)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionDestroyInstance)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else if(OFX::strEquals(action, kOfxActionBeginInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else if(OFX::strEquals(action, kOfxActionEndInstanceChanged)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else if(OFX::strEquals(action, kOfxActionBeginInstanceEdit)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxActionEndInstanceEdit)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, true);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionGetRegionOfDefinition)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, false);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionGetRegionsOfInterest)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, false);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionGetTimeDomain)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, true, false);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionGetFramesNeeded)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, false);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionGetClipPreferences)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, false);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionIsIdentity)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, false);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionRender)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionBeginSequenceRender)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionEndSequenceRender)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else if(OFX::strEquals(action, kOfxImageEffectActionDescribeInContext)) {
      checkMainHandles(action, handle, inArgsHandle, outArgsHandle, false, false, true);
    }
    else {
      OFX::logError("Unknown action '%s'", action);
    }
  }

  catch (int err)
  {
    // ho hum, gone wrong somehow
    stat = err;
  }

  OFX::logPrint("pluginMain - stop");
	  
  // other actions to take the default value
  return stat;
}

// function to set the host structure
static void
setHostFunc(OfxHost *hostStruct)
{
  OFX::logPrint("setHostFunc - start");
  OFX::logError(hostStruct == 0, "host is a null pointer");
  gHost = hostStruct;
  OFX::logPrint("setHostFunc - stop");
}

////////////////////////////////////////////////////////////////////////////////
// the plugin struct 
static OfxPlugin basicPlugin = 
{       
  kOfxImageEffectPluginApi,
  1,
  "net.sf.openfx:PropertyTestPlugin",
  1,
  0,
  setHostFunc,
  pluginMain
};
   
// the two mandated functions
OfxPlugin *
OfxGetPlugin(int nth)
{
  OFX::logPrint("OfxGetPlugin - start");
  OFX::logPrint("  asking for %dth plugin", nth);
  OFX::logError(nth != 0, "requested plugin %d is more than the number of plugins in the file", nth);
  OFX::logPrint("OfxGetPlugin - stop");
  
  if(nth == 0) return &basicPlugin;
  return 0;
}
 
int
OfxGetNumberOfPlugins(void)
{       
  OFX::logPrint("OfxGetNumberOfPlugins - start");
  OFX::logPrint("OfxGetNumberOfPlugins - stop");
  return 1;
}




////////////////////////////////////////////////////////////////////////////////
// globals desctructor, the destructor is called when the plugin is unloaded
class GlobalDestructor {
 public :
  ~GlobalDestructor();
};

GlobalDestructor::~GlobalDestructor()
{
  if(gHostDescription) 
    delete gHostDescription;
}

static GlobalDestructor globalDestructor;
