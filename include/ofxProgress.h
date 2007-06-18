#ifndef _ofxProgressSuite_h_
#define _ofxProgressSuite_h_

/** @brief suite for displaying a progress bar */
#define kOfxProgressSuite "OfxProgressSuite"

/** @brief A suite that provides progress feedback from a plugin to an application */
typedef struct OfxProgressSuiteV1 {  

  /** @brief Initiate a progress bar display.
      
      Call this to initiate the display of a progress bar.
      
      \arg \e effectInstance - the instance of the plugin this progress bar is
                               associated with. It cannot be NULL.
      \arg \e label          - a text label to display in any message portion of the
                               progress object's user interface. A UTF8 string.

      \returns
      - ::kOfxStatOK - the handle is now valid for use
      - ::kOfxStatFailed - the progress object failed for some reason
      - ::kOfxStatErrBadHandle - effectInstance was invalid
   */
  OfxStatus (*progressStart)(void *effectInstance,
                             const char *label);

  /** @brief Indicate how much of the processing task has been completed and reports on any abort status.
      
      \arg \e effectInstance - the instance of the plugin this progress bar is
                                associated with. It cannot be NULL.
      \arg \e progress - a number between 0.0 and 1.0 indicating what proportion of the current task has been processed.
      
      \returns
      - ::kOfxStatOK - the progress object was successfully updated and the task should continue
      - ::kOfxStatReplyNo - the progress object was successfully updated and the task should abort
      - ::kOfxStatErrBadHandle - the progress handle was invalid,
  */
  OfxStatus (*progressUpdate)(void *effectInstance, double progress);

  /** @brief Signal that we are finished with the progress meter.
      
      Call this when you are done with the progress meter and no
      longer need it displayed.

      \arg \e effectInstance - the instance of the plugin this progress bar is
                                associated with. It cannot be NULL.
      
      \postcondition - you can no longer call progressUpdate on the instance                       

      \returns
      - ::kOfxStatOK - the progress object was successfully closed
      - ::kOfxStatErrBadHandle - the progress handle was invalid,
   */
  OfxStatus (*progressEnd)(void *effectInstance);



} OfxProgressSuiteV1 ;

#endif
