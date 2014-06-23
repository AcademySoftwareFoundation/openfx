#ifndef _ofxNatron_h_
#define _ofxNatron_h_

/** @brief The kOfxPropName property of the Natron Host */
#define kOfxNatronHostName          "fr.inria.Natron"

/** @brief The name of a string parameter that maps to a sublabel in the GUI,
 * e.g. the name of the file in a Reader node, or the name of the operation
 * in a Merge node.
 * This parameter should have the properties
 * - kOfxParamPropSecret=1,
 * - kOfxParamPropEnabled=0,
 * - kOfxParamPropPersistant=1,
 * - kOfxParamPropEvaluateOnChange=0.
 * You should set its default value during the kOfxImageEffectActionDescribeInContext action.
 */
#define kOfxParamStringSublabelName "OfxParamStringSublabelName"

/**
 * @brief The name of a string parameter that represents the label of the effect in the GUI.
 * The host can update the value of this parameter when the effect's name changes to inform the
 * plug-in of the name of the effect.
 * This is used for example in the tracker node to know what is the name of the track since each
 * plug-in instance is a separate effect.
 *
 * This parameter should have the properties:
 *
 * - kOfxParamPropSecret=1,
 * - kOfxParamPropEnabled=0,
 * - kOfxParamPropPersistant=0,
 * - kOfxParamPropEvaluateOnChange=0.
 **/
#define kOfxParamStringEffectInstanceLabel "OfxParamStringEffectInstanceLabel"

#endif // #ifndef _ofxNatron_h_
