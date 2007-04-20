#include <iostream>
#include <fstream>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhImageEffect.h"
#include "ofxhPluginAPICache.h"
#include "ofxhPluginCache.h"
#include "ofxhHost.h"
#include "ofxhImageEffectAPI.h"

// my host
#include "hostDemoHostDescriptor.h"
#include "hostDemoEffectInstance.h"
#include "hostDemoClipInstance.h"

namespace MyHost
{ 
  MyHostDescriptor::MyHostDescriptor()
  {
    // set host properties... TODO
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxPropName, 0, "Avid OFX Host");
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxPropLabel, 0, "Avid OFX Host");
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectHostPropIsBackground, 0, false);
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSupportsOverlays, 0, false);
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSupportsMultiResolution, 0, true);
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSupportsTiles, 0, true);
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropTemporalClipAccess, 0, true);
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropSupportedComponents, 0, kOfxImageComponentRGBA);
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropSupportedComponents, 1, kOfxImageComponentAlpha);
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextGenerator );
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropSupportedContexts, 1, kOfxImageEffectContextFilter );
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropSupportedContexts, 2, kOfxImageEffectContextGeneral );
    _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropSupportedContexts, 3, kOfxImageEffectContextTransition );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSupportsMultipleClipDepths, 0, false);
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSupportsMultipleClipPARs, 0, false);
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSetableFrameRate, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImageEffectPropSetableFielding, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropSupportsCustomInteract, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropSupportsStringAnimation, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropSupportsChoiceAnimation, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropSupportsBooleanAnimation, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropSupportsCustomAnimation, 0, false );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropMaxParameters, 0, -1 );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropMaxPages, 0, 0 );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropPageRowColumnCount, 0, 0 );
    _properties.setProperty<OFX::Host::Property::IntValue>(kOfxParamHostPropPageRowColumnCount, 1, 0 );
  }

  MyHostDescriptor gOfxHost;

}