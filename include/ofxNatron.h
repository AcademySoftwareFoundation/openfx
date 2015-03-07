#ifndef _ofxNatron_h_
#define _ofxNatron_h_

/** @brief The kOfxPropName property of the Natron Host */
#define kOfxNatronHostName          "fr.inria.Natron"

/** @brief The name of a string parameter that maps to a sublabel in the GUI,
    e.g. the name of the file in a Reader node, or the name of the operation
    in a Merge node, or the name of the track in a Tracker node.

    Whenever the host sets the value of this parameter, 
    kOfxActionInstanceChanged is called (as with any other parameter).
    It may be kOfxParamPropSecret=1 if it should not appear in the instance UI.
    It may be kOfxParamPropEnabled=0 if only the host should set its value via
    a special GUI.

    This parameter should have the properties
    - kOfxParamPropPersistant=1,
    - kOfxParamPropEvaluateOnChange=0.
    You should set its default value during the kOfxImageEffectActionDescribeInContext action.
 */
#define kOfxParamStringSublabelName "OfxParamStringSublabelName"


/** @brief string property to indicate the presence of custom components on a clip in or out.
  The string should be formed as such:
  "OfxImageComponentsPlane_" + planeName + "_OfxImageChannelName_" + channel1Name + "_OfxImageChannelName_" + channel2Name + "_OfxImageChannelName_" + channel3Name
 
  e.g: "OfxImageComponentsPlane_" + "Position" + "_OfxImageChannelName_" + "X" + "_OfxImageChannelName_" + "Y" + "_OfxImageChannelName_" + "Z"
 
  This indicates to the host in which plane should the given components appear and how many pixel channels it contains.
  It can be used at any place where kOfxImageComponentAlpha, kOfxImageComponentRGB, kOfxImageComponentRGBA, kOfxImageComponentNone (etc...) is
  given.
 */
#define kNatronOfxImageComponentsPlane  "OfxImageComponentsPlane_"
#define kNatronOfxImageChannelName   "_OfxImageChannelName_"

#endif // #ifndef _ofxNatron_h_
