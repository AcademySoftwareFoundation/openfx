#ifndef _fnOfxExtensions_h_
#define _fnOfxExtensions_h_

/*
Software License :

Copyright (c) 2007-2008, The Foundry Visionmongers Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Foundry Visionmongers Ltd, nor the names of its 
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

#include "ofxImageEffect.h"

/*******************************************************************************
 * I don't  like the confusion between 'components' and 'planes', but there
 * was no real way around it. The general idea is that 'component' strings 
 * are useable in the places where the standard component strings are in OFX.
 * eg: kOfxImageComponentRGBA. These are basically used to indicate the
 * kind of an image.
 *
 * For 'components' that have more than one image attached (eg: movecs are have a 
 * forward image and backward image), we needed to differentiate between the two
 * so we did so via a 'plane', which is effectively a single image from a 
 * mult-image component. 
 *
 * Thus the 'component' #defines are used to indicate what exists in the regular
 * way, whilst the 'planes' are used to fetch actual images via our private suite.
 ******************************************************************************/
 
/** @brief string to indicate the presence of motion vector components on a clip, in or out. 

    Set as one of many values in the kOfxImageEffectPropSupportedComponents on a clip.
*/
#define kFnOfxImageComponentMotionVectors "uk.co.thefoundry.OfxImageComponentMotionVectors"

/** @brief string to indicate the presence of stereo disparity components on a clip, in or out. 

    Set as one of many values in the kOfxImageEffectPropSupportedComponents on a clip.
*/
#define kFnOfxImageComponentStereoDisparity "uk.co.thefoundry.OfxImageComponentStereoDisparity"
 
/** @brief string to indicate a 2D forward motion vector image plane be fetched.

    Passed to FnOfxImageEffectPlaneSuiteV1::clipGetImagePlane \e plane argument.
*/
#define kFnOfxImagePlaneForwardMotionVector "uk.co.thefoundry.OfxImagePlaneForwardMotionVector"

/** @brief string to indicate a 2D backward motion vector image plane 

    Passed to FnOfxImageEffectPlaneSuiteV1::clipGetImagePlane \e plane argument.

*/
#define kFnOfxImagePlaneBackwardMotionVector "uk.co.thefoundry.OfxImagePlaneBackMotionVector"

/** @brief string to indicate a colour image plane (RGB, RGBA or A) 

    Passed to FnOfxImageEffectPlaneSuiteV1::clipGetImagePlane \e plane argument.

    This indicates that the 'normal' colour planes be fetched as per the standard OFX
    API.
*/
#define kFnOfxImagePlaneColour "uk.co.thefoundry.OfxImagePlaneColour"

/** @brief string to indicate a 2D left stereo disparity image plane be fetched.

    Passed to FnOfxImageEffectPlaneSuiteV1::clipGetImagePlane \e plane argument.
*/
#define kFnOfxImagePlaneStereoDisparityLeft "uk.co.thefoundry.OfxImagePlaneStereoDisparityLeft"

/** @brief string to indicate a 2D right stereo disparity image plane be fetched.

    Passed to FnOfxImageEffectPlaneSuiteV1::clipGetImagePlane \e plane argument.
*/
#define kFnOfxImagePlaneStereoDisparityRight "uk.co.thefoundry.OfxImagePlaneStereoDisparityRight"

/** @brief Indicates that a host or plugin can fetch more than a type of image from a clip

   - Type - int X 1
   - Property Set - image effect descriptor passed to kOfxActionDescribe (read/write) and the host descriptor (read only)
   - Default - 0
   - Valid Values
      - 0 - host/plugin is not multiplanar
      - 1 - host/plugin is multiplanar
*/
#define kFnOfxImageEffectPropMultiPlanar "uk.co.thefoundry.OfxImageEffectPropMultiPlanar"

/** @brief Plugin indicates to the host that it should pass through any planes not modified by the plugin

   - Type - int X 1
   - Property Set - image effect descriptor passed to kOfxActionDescribe (read/write) and the host descriptor (read only)
   - Default - 0
   - Valid Values
      - 0 - plugin is asking the host to block all planes not processed on output
      - 1 - plugin is asking the host to pass through all planes not processed from the clip specified 
            by the kFnOfxImageEffectActionGetClipComponents action

The plugin must have flagged kFnOfxImageEffectPropMultiPlanar as true, if so tghe 
*/
#define kFnOfxImageEffectPropPassThroughComponents "uk.co.thefoundry.OfxImageEffectPropPassThroughComponents"

/** @brief Property set indicating the components present on something

   - Type - string X N
   - Property Set - image effect clip instance (read only)
   - Default - all the components specifed as supported on the given clip that exist on the clip
   - Valid Values - one or more of the following...
         - kOfxImageComponentNone
         - kOfxImageComponentRGBA OR kOfxImageComponentAlpha
         - kFnOfxImageComponentMotionVectors
         - kFnOfxImageComponentStereoDisparity

*/
#define kFnOfxImageEffectPropComponentsPresent "uk.co.thefoundry.OfxImageEffectClipPropPlanesPresent"

/** @brief Action called on multiplanar effect

    Called to enquire which components are needed on input and produced on output

    This action has the following properties....
       inargs - 
           - kOfxPropTime
           - kFnOfxImageEffectPropView (only if view aware)
       outargs
           - for each clip (in and out), a property that is starts with  "uk.co.thefoundry.OfxNeededComp" 
             post peneded by the clip's name (e.g. "uk.co.thefoundry.OfxNeededComp_Output") which represents,
             if an input, the components needed by the effect, if an output, the components produced by
             the effect.
             These are all char * X N properties, which must be one of ...
                - kOfxImageComponentRGBA OR kOfxImageComponentAlpha
                - kFnOfxImageComponentMotionVectors
                - kFnOfxImageComponentStereoDisparity
             Subsequent calls to render will say what components to actually fill in on the output, which
             will be a subset of the ones reported here.
           - kFnOfxImageEffectPropPassThroughClip - clip to use as a pass through for all non rendered planes
           - kFnOfxImageEffectPropPassThroughTime - time on that clip to pass through,
                                                    MUST HAVE BEEN A FRAME SPECIFIED IN FRAMES NEEDED
           - kFnOfxImageEffectPropPassThroughView - view on that clip to pass through,
                                                    MUST HAVE BEEN A FRAME SPECIFIED IN FRAMES NEEDED
                                                    
*/
#define kFnOfxImageEffectActionGetClipComponents "uk.co.thefoundry.OfxImageEffectActionGetClipComponents"

/** @brief Out arg to kFnOfxImageEffectActionGetClipComponents that indicates which clip to take pass through
           planes from

   - Type - string X 1
   - Property Set - out args on kFnOfxImageEffectActionGetClipComponents
   - Default - ""
   - Valid Values
      - name of a clip - pass through planes will be taken from this clip
      - "" - no planes will be passed through
*/
#define kFnOfxImageEffectPropPassThroughClip "uk.co.thefoundry.ImageEffectPropPassThroughClip"

/** @brief Out arg to kFnOfxImageEffectActionGetClipComponents that indicates the time at which the planes on
    the plass through clip should be taken.

   - Type - double X 1
   - Property Set - out args on kFnOfxImageEffectActionGetClipComponents
   - Default - the 'render' time
   - Valid Values
      - if the ::kFnOfxImageEffectPropPassThroughClip, any frame specified in the get frames needed action
*/
#define kFnOfxImageEffectPropPassThroughTime "uk.co.thefoundry.ImageEffectPropPassThroughTime"

/** @brief Out arg to kFnOfxImageEffectActionGetClipComponents that indicates the view from which the planes on
    the plass through clip should be taken.

   - Type - int X 1
   - Property Set - out args on kFnOfxImageEffectActionGetClipComponents
   - Default - the 'render' view
   - Valid Values
      - if the ::kFnOfxImageEffectPropPassThroughClip, any view specified in the get views needed action
*/
#define kFnOfxImageEffectPropPassThroughView "uk.co.thefoundry.ImageEffectPropPassThroughView"

/**  string to pre-pend to the name of props in the ::kFnOfxImageEffectActionGetClipComponents action */
#define kFnOfxImageEffectActionGetClipComponentsPropString "uk.co.thefoundry.OfxNeededComp_" 

/** @brief Indicates to the host that the plugin is view aware, in which case it will have to use the view calls

   - Type - int X 1
   - Property Set - image effect descriptor passed to kOfxActionDescribe (read/write)
   - Default - 0
   - Valid Values
      - 0 - plugin is not view aware
      - 1 - plugin is view aware
*/
#define kFnOfxImageEffectPropViewAware "uk.co.thefoundry.OfxImageEffectPropViewAware"

/** @brief Indicates to the host that a view aware plugin produces the same image independent of the view being rendered

This property includes pass-throughs.

   - Type - int X 1
   - Property Set - image effect descriptor passed to kOfxActionDescribe (read/write)
   - Default - 0
   - Valid Values
      - 0 - plugin will produce different output images depenedent on the view being rendered
      - 1 - plugin will produced exactly the same image for all output views only on the rendered planes, pass through are view variant
      - 2 - plugin will produced exactly the same image for all output views for all planes (even pass through)
*/
#define kFnOfxImageEffectPropViewInvariance "uk.co.thefoundry.OfxImageEffectPropViewInvariance"

/** @brief Action called to get the views needed on an input clip to render an output view 

For view aware plugins, this action is called instead of the standard getframesneeded action

    The views needed action has the following properties....
       inargs - 
           - kOfxPropTime
           - kFnOfxImageEffectPropView
       outargs
           - for each input clip a multidimensional double prop named
              "OfxImageClipPropFrameRangeView_" + clip name
             Set this to be a triple [start frame, stop frame, view] for each view/frame range needed
*/
#define kFnOfxImageEffectActionGetFrameViewsNeeded "uk.co.thefoundry.OfxImageEffectActionGetFrameViewsNeeded"

/** @brief Property indicating a view

   - Type - int X 1
   - Property Set - all actions that take a frame/renderscale
   - Default - 0
   - Valid Values
      - any valid view number
 */
#define kFnOfxImageEffectPropView "uk.co.thefoundry.OfxImageEffectPropView"

/** @brief String to identify a param as view chooser, which returns an int indicating the view chosen 

    1D int param in effect.
*/
#define kFnOfxParamTypeViewChooser "uk.co.thefoundry.OfxParamTypeViewChooser"

/** @brief String to identify a param as view chooser, which returns an int indicating a pair of views

    2D int param in effect
 */
#define kFnOfxParamTypeViewPair "uk.co.thefoundry.OfxParamTypeViewPair"

/// name of the Foundry's custom plane suite
#define kFnOfxImageEffectPlaneSuite "uk.co.thefoundry.FnOfxImageEffectPlaneSuite"

/// @brief Provides an extra set of functions for OFX 2 expansion
typedef struct FnOfxImageEffectPlaneSuiteV1 {  
  /** @brief Get a handle for an image plane from a clip at the indicated time and indicated region

      \arg clip  - the clip to extract the image from
      \arg time        - time to fetch the image at
      \arg region      - region to fetch the image from (optional, set to NULL to get a 'default' region)
                            this is in the \ref CannonicalCoordinates. 
      \arg plane       - a C string indicating what image plane should be fetched, 'official' ones are currently...  
                            - kOfxImagePlaneColour
                            - kOfxImagePlaneForwardMotionVector
                            - kOfxImagePlaneBackwardMotionVector
      \arg planeHandle - property set containing the image's data, this has the same properties as those returned by clipGetImage

  An image plane is fetched from a clip at the indicated time for the given region and returned in the planeHandle.

 If the \e region parameter is not set to NULL, then it will be clipped to the clip's Region of Definition for the given time. The returned image will be \em at \em least as big as this region. If the region parameter is not set, then the region fetched will be at least the Region of Interest the effect has previously specified, clipped the clip's Region of Definition.

If clipGetImagePlane is called twice with the same parameters, then two separate image handles will be returned, each of which must be release. The underlying implementation could share image data pointers and use reference counting to maintain them.

\pre
 - clip was returned by clipGetHandle

\post
 - image handle is only valid for the duration of the action clipGetImagePlane is called in
 - image handle to be disposed of by clipReleaseImage before the action returns

@returns
- ::kOfxStatOK - the image was successfully fetched and returned in the handle,
- ::kOfxStatFailed - the image could not be fetched because it does not exist in the clip at the indicated time and/or region, the plugin
                     should continue operation, but assume the image was black and transparent.
- ::kOfxStatErrBadHandle - the clip handle was invalid,
- ::kOfxStatErrMemory - the host had not enough memory to complete the operation, plugin should abort whatever it was doing.

\note
  - this replaces clipGetImage from OfxImageEffectSuiteV1
  */
  OfxStatus (*clipGetImagePlane)(OfxImageClipHandle clip,
                                 OfxTime       time,
                                 const char   *plane,
                                 OfxRectD     *region,
                                 OfxPropertySetHandle   *imageHandle);
} FnOfxImageEffectPlaneSuiteV1 ;

/// version two of the suite, adds support for views
typedef struct FnOfxImageEffectPlaneSuiteV2 {  
  /** @brief Get a handle for an image plane from a clip at the indicated time and indicated region

      \arg clip  - the clip to extract the image from
      \arg time        - time to fetch the image at
      \arg region      - region to fetch the image from (optional, set to NULL to get a 'default' region)
                            this is in the \ref CannonicalCoordinates. 
      \arg plane       - a C string indicating what image plane should be fetched, 'official' ones are currently...  
                            - kOfxImagePlaneColour
                            - kOfxImagePlaneForwardMotionVector
                            - kOfxImagePlaneBackwardMotionVector
      \arg planeHandle - property set containing the image's data, this has the same properties as those returned by clipGetImage

  An image plane is fetched from a clip at the indicated time for the given region and returned in the planeHandle.

 If the \e region parameter is not set to NULL, then it will be clipped to the clip's Region of Definition for the given time. The returned image will be \em at \em least as big as this region. If the region parameter is not set, then the region fetched will be at least the Region of Interest the effect has previously specified, clipped the clip's Region of Definition.

If clipGetImagePlane is called twice with the same parameters, then two separate image handles will be returned, each of which must be release. The underlying implementation could share image data pointers and use reference counting to maintain them.

\pre
 - clip was returned by clipGetHandle

\post
 - image handle is only valid for the duration of the action clipGetImagePlane is called in
 - image handle to be disposed of by clipReleaseImage before the action returns

@returns
- ::kOfxStatOK - the image was successfully fetched and returned in the handle,
- ::kOfxStatFailed - the image could not be fetched because it does not exist in the clip at the indicated time and/or region, the plugin
                     should continue operation, but assume the image was black and transparent.
- ::kOfxStatErrBadHandle - the clip handle was invalid,
- ::kOfxStatErrMemory - the host had not enough memory to complete the operation, plugin should abort whatever it was doing.

\note
  - this replaces clipGetImage from OfxImageEffectSuiteV1
  */
  OfxStatus (*clipGetImagePlane)(OfxImageClipHandle clip,
                                 OfxTime       time,
                                 int           view,
                                 const char   *plane,
                                 OfxRectD     *region,
                                 OfxPropertySetHandle   *imageHandle);

  /// get the rod on the given clip at the given time for the given view
  OfxStatus (*clipGetRegionOfDefinition)(OfxImageClipHandle clip,
                                         OfxTime            time,
                                         int                view,
                                         OfxRectD           *bounds);

  /// get the textual representation of the view
  OfxStatus (*getViewName)(OfxImageEffectHandle effect,
                           int                  view,
                           char               **viewName);
 
  /// get the number of views
  OfxStatus (*getViewCount)(OfxImageEffectHandle effect,
                            int                 *nViews);


} FnOfxImageEffectPlaneSuiteV2 ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Extensions for image effects that can return a transform rather than an image

/** @brief Property to indicate that a plugin or host can handle transform effects

- Type - int X 1
- Property Set - host descriptor (read only), plugin instance (read/write)
- Default - 0
- Valid Values - This must be one of
   - 0 if the host or plugin cannot make use of the kOfxImageEffectGetTransformAction
   - 1 if the host or plugin can use the kOfxImageEffectGetTransformAction

This is a property on the descriptor.
*/
#define kFnOfxImageEffectCanTransform "FnOfxImageEffectCanTransform"
 
/** @brief Action called in place of a render to recover a transform matrix from an effect.

    Some effects do a simple matrix transform on their images, the cleverness in the effect
    is how that transform is arrived at. For example a stabilisation effect which analyses 
    it's input clip in user interface and writes key frames to a set of params which represent
    a transform. The render action of such an effect is just a transform by a matrix.
    
    Often, such effects are chained together, and you are incurring both an extra compute cost
    and a degradation in the quality of the output image by having the effects in question
    perform multiple image transforms and filtering actions. 

    In such a situation it would be much better if you could recover the transforms from each
    of the effects and compose them together and transform the image once. This improves
    both performance and quality. This action allows you to do such a thing and serves in 
    place of a standard render call.

    To be able to successfully reproduce the same result as the render action with this 
    action, any effects that implement it must adhere to several conditions...
      - the effect does not need the following called to determine the correct transform,
          - The Get Region of Definition Action
          - The Get Regions Of Interest Action
          - The Get Frames Needed Action
          - The Is Identity Action
          - The Render Action
          - The Begin Sequence Render Action
          - The End Sequence Render Action
       - the RoD and RoI of the effect are implicitly determined by the transform returned
         by this action,
       - the effect only needs a single image frome the clip named in the out args of this action,
       - for view aware effects, it would have transformed the image from the view as passed into the action,
       - the effect instance must have set kFnOfxImageEffectCanTransform to 1.
  
    For maximum flexibility, a plug-in must be allowed to fetch images inside this action, so as to be
    able to perform on the fly analysis to calculate the required transform. Ideally this should be
    discouraged in favour of simply returning pre-analysed values.

    Note that the render action can still be called if a host so chooses, this action does not
    completely replace the render action of such an effect.

    This action has the following properties on its arguments....
       inargs - 
           - kOfxPropTime - the time at which to test for identity
           - kOfxImageEffectPropFieldToRender - the field being transformed
           - kOfxImageEffectPropRenderScale - the scale factor being applied to the images being transformed
           - kFnOfxImageEffectPropView (only if view aware)
       outargs -
           - kOfxPropName - this to the name of the input clip that would be transformed by the effect during render
             defaults to "Source".
           - kFnOfxPropMatrix4x4 - a 4x4 matrix representing the transform. This is in pixel coordinate space, 
             going from the source imaage to the destination, defaults to the identity matrix.

@returns
- ::kOfxStatDefault - don't attempt to use the transform matrix, but render the image as per normal,
- ::kOfxStatOK - the transfrom and clip name were set and can be used to modify the named image appropriately,
                                                    
*/

#define kFnOfxImageEffectActionGetTransform "uk.co.thefoundry.FnOfxImageEffectActionGetTransform"

/** @brief Property that represents a 2D matrix
/** this was originally a 4 by 4 matrix but as Phil said, we cant guess the depth z therefore we can't produce a correct 3D matrix, the host has to do it

- Type - double X 9
- Property Set - varies, but on the out args of kFnOfxImageEffectActionGetTransform
- Default - the identity matrix
- Valid Values - any matrix value 

The 9 values of this property represent a 2D 3 by 3 matrix. The matrix is in row/column format.**/

#define kFnOfxPropMatrix2D "FnOfxPropMatrix2D"
 


#endif

