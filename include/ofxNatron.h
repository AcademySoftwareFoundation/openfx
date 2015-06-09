#ifndef _ofxNatron_h_
#define _ofxNatron_h_

/** @brief The kOfxPropName property of the Natron Host */
#define kNatronOfxHostName          "fr.inria.Natron"

/** @brief int x 1 property used on the image effect host descriptor. 
 It indicates whether the host is Natron as another mean than just the host name.
 This was added because it may be required to activate certain plug-ins by changing the host-name 
 (since some plug-ins don't allow to be loaded on any host)
 but to keep some other functionnalities specific to the Natron extensions working.
 
Valid values:
 - 0: Indicates that the host is not Natron
 - 1: Indicates that the host is Natron and can support all extensions provided by this header
 
 Default value:
 - 0
 */
#define kNatronOfxHostIsNatron "NatronOfxHostIsNatron"

/** @brief int x 1 property used on the image effect host descriptor.
 It indicates whether the host supports dynamic choice param entries changes or not.
 
 Reasoning:
 By default most of hosts expect the entries of a choice param to be statically defined in the describeInContext action.
 However this might often be a need to re-populate the entries depending on other dynamic parameters, such as the pixel
 components. 
 You would then have to call propReset on the kOfxParamPropChoiceOption property via the param suite and then call
 propSetString on the kOfxParamPropChoiceOption property to re-build the entries list.
 
 Valid values:
 - 0: Indicates that the host does not support dynamic choices
 - 1: Indicates that the host supports dynamic choices
 
 Default value:
 - 0
 */
#define kNatronOfxParamHostPropSupportsDynamicChoices "NatronOfxParamHostPropSupportsDynamicChoices"

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
#define kNatronOfxParamStringSublabelName "NatronOfxParamStringSublabelName"


/** @brief string property to indicate the presence of custom components on a clip in or out.
  The string should be formed as such:
  kNatronOfxImageComponentsPlane + planeName + kNatronOfxImageComponentsPlaneChannel + channel1Name + kNatronOfxImageComponentsPlaneChannel + channel2Name + kNatronOfxImageComponentsPlaneChannel + channel3Name
 
  e.g: kNatronOfxImageComponentsPlane + "Position" + kNatronOfxImageComponentsPlaneChannel + "X" + kNatronOfxImageComponentsPlaneChannel + "Y" + kNatronOfxImageComponentsPlaneChannel + "Z"
 
  This indicates to the host in which plane should the given components appear and how many pixel channels it contains.
  It can be used at any place where kOfxImageComponentAlpha, kOfxImageComponentRGB, kOfxImageComponentRGBA, kOfxImageComponentNone (etc...) is
  given.
 */
#define kNatronOfxImageComponentsPlane  "NatronOfxImageComponentsPlane_"
#define kNatronOfxImageComponentsPlaneChannel   "_Channel_"


/** @brief String to label images with 2 components. If the plug-in supports this then the host can attempt to send to the plug-in
 planes with 2 components.*/
#define kNatronOfxImageComponentXY "NatronOfxImageComponentXY"

/** @brief Whether the menu should be hierarchical/cascading, and each option contains a slash-separated path to the item.

    - Type - int X 1
    - Property Set - plugin parameter descriptor (read/write) and instance (read only), and on the image effect host descriptor (read only)
    - Default - 0
    - Valid Values - 0 or 1

This is a property on parameters of type ::kOfxParamTypeChoice, and tells the choice whether menu should be hierarchical/cascading, and each option contains a slash-separated path to the item.

*/
#define kNatronOfxParamPropChoiceCascading "NatronOfxParamPropChoiceCascading"

/** @brief int x1 property on a choice parameter descriptor (read/write) and choice parameter instance (read-only) to indicate whether
 the host can add a new choice on its own (probably via a GUI specific to this parameter).
 The plugin may then retrieve the option name whenever a choice value is out of its initial range.

 This property primarily targets image plane choices, where the host should be able to create a new plane and add it to the menu.
 
 Valid values:
 - 0: Indicates that the parameter does not support/want that the host adds new options
 - 1: Indicates that the parameter wants to have the choice "New" so that the host can create a new option
 Default value:
 - 0
 */
#define kNatronOfxParamPropChoiceHostCanAddOptions "NatronOfxParamPropChoiceHostCanAddOptions"

/** @brief The standard parameter for setting output channels of a plugin (used by Shuffle).
 
 This parameter may have the property kNatronOfxParamPropChoiceHostCanAddOptions set.
 */
#define kNatronOfxParamOutputChannels "outputChannels"

/** @brief Indicates if the host may add a channel selector, and which components should be selected by default.

 - Type - string X 1
 - Property Set - image effect descriptor (read/write), host descriptor (read only)
 - Valid Values - This must be one of
 - kOfxImageComponentNone (channel selector is disabled)
 - kOfxImageComponentRGBA (enabled, with RGBA selected by default)
 - kOfxImageComponentRGB (enabled, with RGB selected by default)
 - kOfxImageComponentAlpha (enabled, with Alpha selected by default)

 This string indicates if the host may add a channel selector, and which components should be selected by default.

 If the property is not present the the host descriptor, or its value is not kOfxImageComponentRGBA, then the host does not have a channel selector, and the plugin may propose its own solution (e.g. a checkbox for each channel).
 
 The default for an image effect descriptor is kOfxImageComponentRGBA.
 */
#define kNatronOfxImageEffectPropChannelSelector "NatronOfxImageEffectPropChannelSelector"


/** @brief Generic parameter name for a channel selector. If the plugin doesn't define these, and
    kNatronOfxImageEffectPropChannelSelector is not set by the plugin, the host may add its own channel selector.
 **/
#define kNatronOfxParamProcessR      "NatronOfxParamProcessR"
#define kNatronOfxParamProcessRLabel "R"
#define kNatronOfxParamProcessRHint  "Process red component."
#define kNatronOfxParamProcessG      "NatronOfxParamProcessG"
#define kNatronOfxParamProcessGLabel "G"
#define kNatronOfxParamProcessGHint  "Process green component."
#define kNatronOfxParamProcessB      "NatronOfxParamProcessB"
#define kNatronOfxParamProcessBLabel "B"
#define kNatronOfxParamProcessBHint  "Process blue component."
#define kNatronOfxParamProcessA      "NatronOfxParamProcessA"
#define kNatronOfxParamProcessALabel "A"
#define kNatronOfxParamProcessAHint  "Process alpha component."

/** @brief Used to define the tracker effect context. 
 In this context the effect instance will be exactly 1 track. 
 It  will define 4 buttons parameters, namely:
- kNatronParamTrackingPrevious
- kNatronParamTrackingNext
- kNatronParamTrackingBackward
- kNatronParamTrackingForward
 and 1 string parameter containing the name of the track:
 - kNatronOfxParamStringSublabelName
 
 The instance changed action on the 4 push-buttons parameters can be called on a thread different than the main-thread, allowing multiple instance changed action
 to be called on the same parameter concurrently but with a different 'time' parameter. It is up to the Host application to schedule correctly the 
 multi-threading of the tracks.
 
 In this context, typically the host would provide a general interface under which multiple instances of the plug-in in this context would co-exist. 
 The name parameter is here to identity the track.
 This could be a table in which each instance would have a separate row on its own.
 Such instances would probably have shared parameters, such as parameters of the tracking algorithm. On the other hand the instances have "specific" parameters
 that could not be shared among instances, e.g: the resulting position of a point tracking would be unique for each separate track instance.
 The host could propose in its user interface to display instance-specific parameters in each row of the table, but could display the shared parameter as a global
 parameter for all instances. To flag that a parameter is instance-specific, a new property on the parameter descriptor has been introduced, kNatronOfxImageEffectContextTracker. This property should be set to 1 for the kNatronOfxParamStringSublabelName parameter.
 */
#define kNatronOfxImageEffectContextTracker "NatronOfxImageEffectContextTracker"

/** @brief Button param that must be present on a plug-in in the kNatronOfxImageEffectContextTracker context. When the instance changed action
 is called on this parameter, it should apply the analysis on the frame preceding the current frame on the timeline.*/
#define kNatronParamTrackingPrevious "trackPrevious"

/** @brief Button param that must be present on a plug-in in the kNatronOfxImageEffectContextTracker context. When the instance changed action
 is called on this parameter, it should apply the analysis on the frame following the current frame on the timeline.*/
#define kNatronParamTrackingNext "trackNext"

/** @brief Button param that must be present on a plug-in in the kNatronOfxImageEffectContextTracker context. When the instance changed action
 is called on this parameter, it should apply the analysis on all the frames from the preceding frame until the left
 bound of the timeline..*/
#define kNatronParamTrackingBackward "trackBackward"

/** @brief Button param that must be present on a plug-in in the kNatronOfxImageEffectContextTracker context. When the instance changed action
 is called on this parameter, it should apply the analysis on all the frames from the following frame until the right
 bound of the timeline..*/
#define kNatronParamTrackingForward "trackForward"

/** @brief int  property to indicate whether a parameter is instance-specific or not.
 - Type - int x 1
 - Property Set - plugin parameter descriptor (read/write) and instance (read/write only)
 - Default - 0
 - Valid Values - 0 or 1
 When set to 1, the parameter is specific to an effect instance of the plug-in and should have a
 unique representation for each instance. See descripton of kNatronOfxImageEffectContextTracker for more details
 on multiple instances and difference between shared and specific parameters.
 */
#define kNatronOfxParamPropIsInstanceSpecific "NatronOfxParamPropIsInstanceSpecific"

#endif // #ifndef _ofxNatron_h_
