#ifndef _ofxNatron_h_
#define _ofxNatron_h_

/** @brief The kOfxPropName property of the Natron Host */
 #define kOfxNatronHostName          "fr.inria.Natron"

/** @brief The name of a string parameter that maps to a sublabel in the GUI,
    e.g. the name of the file in a Reader node, or the name of the operation
    in a Merge node.
    This parameter should have the properties
    - kOfxParamPropSecret=1,
    - kOfxParamPropEnabled=0,
    - kOfxParamPropPersistant=1,
    - kOfxParamPropEvaluateOnChange=0.
    You should set its default value during the kOfxImageEffectActionDescribeInContext action.
 */
#define kOfxParamStringSublabelName "OfxParamStringSublabelName"

#endif // #ifndef _ofxNatron_h_
