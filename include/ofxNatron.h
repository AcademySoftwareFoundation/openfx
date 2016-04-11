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

/*
  ----------------------------------------------- -----------------------------------------------------------------
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 Natron multi-plane definitions and extensions brought to Nuke multi-plane extensions defined in fnOfxExtensions.h:
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ----------------------------------------------- -----------------------------------------------------------------
 
 Definitions:
 ------------
 
 - Layer: Corresponds to 1 image plane and has a unique name
 
 - Components: The "type" of data (i.e: the number of channels) contained by a Layer. This may be equal to any of the default
 components defined by OpenFX (e.g: kOfxImageComponentRGBA) but also to the one added by fnOfxExtensions.h (namely 
 kFnOfxImagePlaneForwardMotionVector or kFnOfxImagePlaneBackwardMotionVector) and finally to the custom planes extension
 defined by Natron.
 
 ----------------------------------------------- -----------------------------------------------------------------

The Foundry multi-plane suite:
------------------------------
 
 - In The Foundry specification, some layers are paired and can be requested at the same time:
 this is the (kFnOfxImagePlaneBackwardMotionVector, kFnOfxImagePlaneForwardMotionVector) and (kFnOfxImagePlaneStereoDisparityLeft, kFnOfxImagePlaneStereoDisparityRight) layers. A pair means both layers have the same components type and are generally rendered together.
 These layers are the only one added by this extension.
 
 - The color layer (kFnOfxImagePlaneColour) can have the default OpenFX types.
 
 - The only plug-ins to support The Foundry multi-plane suite are the Furnace ones (among potentially others) and the only (known) hosts to support it are Natron and Nuke.
 
 - In The Foundry multi-plane suite, the plug-in specify that it wants to operate on a motion vector plane or disparity plane by setting kFnOfxImageComponentMotionVectors or kFnOfxImageComponentStereoDisparity on the clip components during the getClipPreferences action. They do not support other planes.

 - The getClipComponents action is unused.
 
 - If the clip components are set to kFnOfxImageComponentMotionVectors or kFnOfxImageComponentStereoDisparity is is expected that the following render actions are called on both paired planes (the plug-in will attempt to fetch both from the render action).
 
Natron modifications:
---------------------
 
 - Some file formats (OpenEXR, TIFF) allow multiple arbitrary image layers (= planes) to be embedded in a single file.
 In the same way, a host application may want/need to use multiple arbitrary image layers into the same image stream
 coming from a clip.
 The multi-plane extension defined in fnOfxExtensions.h by The Foundry has nothing set in this regard and we had to come-up
 with one.
 
 A custom Layer (or plane) is defined as follows:
 
 - A unique name, e.g: "RenderLayer"
 - A set of 1 to 4 channels represented by strings, e.g: ["R","G","B","A"]
 
 Typically it would be presented like this to the user in a choice parameter:
 
 RenderLayer.RGBA
 
 Internally instead of passing this string and parsing it, we encode the layer as such:
 
 kNatronOfxImageComponentsPlane + layerName + kNatronOfxImageComponentsPlaneChannel + channel1Name + kNatronOfxImageComponentsPlaneChannel + channel2Name + kNatronOfxImageComponentsPlaneChannel + channel3Name
 
 e.g: kNatronOfxImageComponentsPlane + "Position" + kNatronOfxImageComponentsPlaneChannel + "X" + kNatronOfxImageComponentsPlaneChannel + "Y" + kNatronOfxImageComponentsPlaneChannel + "Z"
 

 Natron custom layers can be passed wherever layers are used (clipGetImage,render action) or components are used: getClipComponents. They may not be used
 in getClipPreferences.
 
 - Multi-plane effects (kFnOfxImageEffectPropMultiPlanar=1) request their layers via the getClipComponents action
 
 - getClipPreferences has not changed and may only be used to specify the type of components onto which the color layer (kFnOfxImagePlaneColour) will be mapped
 
 - Multi-plane effects (kFnOfxImageEffectPropMultiPlanar=1) are expected to support arbitrary component types and should not rely on the components set during getClipPreferences except for the kFnOfxImagePlaneColour layer.

 - OpenFX didn't allow to pass 2-channel image natively through a clip, even for plug-ins that are not multi-plane.
 The color layer (kFnOfxImagePlaneColour) can now have the default OpenFX types of components as well as kNatronOfxImageComponentXY to specify 2-channel images.
 kNatronOfxImageComponentXY may be specified wherever default OpenFX components types are used. 
 

 
 
Notes:
------
 
 - Layers are what is passed to the render action and clipGetImage function whereas components are what is used for getClipComponents and getClipPreferences
 
 - In the getClipComponents action, the plug-in passes OpenFX components.
 
 */


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


/** @brief Indicates if the host may add a mask that will be handled automatically.
 
 - Type - int X 1
 - Property Set - image effect descriptor (read/write), host descriptor (read only)
 - Valid Values - 0 or 1
 - Default value: 0
 
 When set to 1, the plug-in should be able to call clipGetHandle on the clip created by the host.
 */

#define kNatronOfxImageEffectPropHostMasking "kNatronOfxImageEffectPropHostMasking"

/** @brief Indicates if the host may add a "Mix" double parameter that will dissolve
  between the source image at 0 and the full effect at 1.
 
 - Type - int X 1
 - Property Set - image effect descriptor (read/write), host descriptor (read only)
 - Valid Values - 0 or 1
 - Default value: 0
*/
#define kNatronOfxImageEffectPropHostMixing "kNatronOfxImageEffectPropHostMixing"


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


/** @brief int  property to indicate whether a double 3D parameter is associated to a matrix 3x3
 - Type - int x 1
 - Property Set - plugin parameter descriptor (read/write) and instance (read/write only)
 - Valid values - 0, 1, 2, 3
 0: The parameter is a regular Double3D and the host should use it as any other parameter
 1: The parameter is the first row of the 3x3 matrix
 2: The parameter is the second row of the 3x3 matrix
 3: The parameter is the third row of the 3x3 matrix
 
 The 3 parameters are identified by the host by their name: the share the same name, but they end with "_rowX", with X being replaced
 by the 1-based index row of the matrix, e.g:
 
 transform_row1
 transform_row2
 transform_row3
 
 would be recognized by the host as a parameter of type Matrix3x3 with the name "transform".
 In this case the properties of the host rectangle parameter are taken from the row1 parameter, except
 for the kOfxParamPropDimensionLabel, kOfxParamPropDefault
 */
#define kNatronOfxParamPropDoubleTypeMatrix3x3 "NatronOfxParamTypeMatrix3x3"


/** @brief int  property to indicate whether a double or int 2D parameter is associated to a rectangle [(x,y),(w,h)]
 - Type - int x 1
 - Property Set - plugin parameter descriptor (read/write) and instance (read/write only)
 - Valid values - 0, 1, or 2
  0: The parameter is a regular Int2D/Double2D and the host should use its double type property
  1: The parameter is the position component of the rectangle (x,y)
  2: The parameter is the size component of the rectangle (w,h)
 
 The 2 parameters are identified by the host by their name: the share both the same name, but one end with "_position"
 and the other with "_size", e.g:
 
 rectangle_position
 rectangle_size
 
 would be recognized by the host as a parameter of type rectangle with the name "rectangle".
 In this case the properties of the host rectangle parameter are taken from the position component parameter, except
 for the kOfxParamPropMin, kOfxParamPropMax, kOfxParamPropDisplayMin, kOfxParamPropDisplayMax, kOfxParamPropDimensionLabel, 
 kOfxParamPropDefault
 */
#define kNatronOfxParamPropTypeRectangle  "NatronOfxParamPropTypeRectangle"



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

/** @brief bool property to indicate that an effect should be considered deprecated
 - Type - int x 1
 - Property Set - image effect descriptor (read/write) image effect (read only)
 - Default - 0
 - Valid Values - 0 or 1
 When set to 1, the host may disable using this plugin in new projects.
*/
#define kNatronOfxImageEffectPropDeprecated "NatronOfxImageEffectPropDeprecated"

/** @brief This extension is to allow the Host application to add extra formats 
  (which are a set of 3 values: width, height, pixel aspect ratio) to a choice parameter.
  If the host application finds the following 3 parameters:
  - kNatronParamFormatChoice
  - kNatronParamFormatSize
  - kNatronParamFormatPar
  The host will then control the choice parameter itself, including:
  - Refreshing the list of available formats in the choice parameter
  - Handling the instanceChanged action calls instead of the plug-in on the parameter kNatronParamFormatChoice by setting
  the values of kNatronParamFormatSize and kNatronParamFormatPar accordingly. The handler will not be called on the plug-in.
  
  The plug-in should still handle the case where the instanceChanged action is called for kNatronParamFormatChoice
  which would indicate that the host does not handle the parameter itself.

 */

/** @brief Choice parameter used to choose the target format*/
#define kNatronParamFormatChoice "NatronParamFormatChoice"

/** @brief Int2D parameter used to store the width,height of the format. Should be secret and evaluateOnChange=false
 The values returned are in pixel coordinates.
 */
#define kNatronParamFormatSize "NatronParamFormatSize"

/** @brief Double parameter used to store the pixel aspect ratio of the format. Should be secret and evaluateOnChange=false*/
#define kNatronParamFormatPar "NatronParamFormatPar"

/** @brief string property that uniquely identifies the project which holds the instance within the host
 - Type - string x 1
 - Property Set - image effect descriptor (read only) image effect (read only)
 - Default - ""
 - Valid Values - any
 */
#define kNatronOfxImageEffectPropProjectId "NatronOfxImageEffectPropProjectId"

/** @brief string property that uniquely identifies the group (if any) containing the instance within the current project
 If the instance os within nested groups, this should be something like "Group1.Group2" with each group level separated by a dot.
 - Type - string x 1
 - Property Set - image effect descriptor (read only) image effect (read only)
 - Default - ""
 - Valid Values - any
 */
#define kNatronOfxImageEffectPropGroupId "NatronOfxImageEffectPropGroupId"

/** @brief string property that uniquely identifies the instance within the group (if any) or within the current project
 - Type - string x 1
 - Property Set - image effect descriptor (read only) image effect (read only)
 - Default - ""
 - Valid Values - any
 */
#define kNatronOfxImageEffectPropInstanceId "NatronOfxImageEffectPropInstanceId"

#endif // #ifndef _ofxNatron_h_
