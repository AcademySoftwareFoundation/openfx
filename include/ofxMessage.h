#ifndef _ofxMessage_h_
#define _ofxMessage_h_

#include "ofxCore.h"

/*
Software License :

Copyright (c) 2003-2009, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Open Effects Association Ltd, nor the names of its 
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


#ifdef __cplusplus
extern "C" {
#endif

#define kOfxMessageSuite "OfxMessageSuite"


/** @brief String used to type fatal error messages

    Fatal error messages should only be posted by a plugin when it can no longer continue operation.
 */
#define kOfxMessageFatal "OfxMessageFatal"

/** @brief String used to type error messages 

    Ordinary error messages should be posted when there is an error in operation that is recoverable by
    user intervention.
*/
#define kOfxMessageError "OfxMessageError"

/** @brief String used to type simple ordinary messages 

    Ordinary messages simply convey information from the plugin directly to the user.
*/
#define kOfxMessageMessage "OfxMessageMessage"

/** @brief String used to type log messages 

    Log messages are written out to a log and not to the end user.
*/
#define kOfxMessageLog "OfxMessageLog"

/** @brief String used to type yes/no messages

    The host is to enter a modal state which waits for the user to respond yes or no. 
The OfxMessageSuiteV1::message function which posted the message will only return after 
the user responds. When asking a question, the OfxStatus code returned by the message function will be,
    - kOfxStatReplyYes - if the user replied 'yes' to the question
    - kOfxStatReplyNo - if the user replied 'no' to the question
    - some error code if an error was encounterred

    It is an error to post a question message if the plugin is not in an interactive session.
 */
#define kOfxMessageQuestion "OfxMessageQuestion"

/** @brief The OFX suite that allows a plug-in to pass messages back to a user.
 */
typedef struct OfxMessageSuiteV1 {

  /** @brief Post a message on the host, using printf style varargs

      \arg handle     - effect handle (descriptor or instance) the message should be associated with, may be null
      \arg messageType - string describing the kind of message to post, one of the kOfxMessageType* constants
      \arg messageId - plugin specified id to associate with this message. If overriding the message in XML resource, the message is identified with this, this may be NULL, or "", in which case no override will occur,
      \arg format    - printf style format string
      \arg ...       - printf style varargs list to print

\returns 
  - ::kOfxStatOK - if the message was sucessfully posted 
  - ::kOfxStatReplyYes - if the message was of type  ::kOfxMessageQuestion and the user reply yes
  - ::kOfxStatReplyNo - if the message was of type kOfxMessageQuestion and the user reply no
  - ::kOfxStatFailed - if the message could not be posted for some reason

   */
  OfxStatus (*message)(void *handle,
		       const char *messageType,
		       const char *messageId,
		       const char *format,
		       ...);

} OfxMessageSuiteV1;

/** @file ofxMessage.h
    This file contains the Host API for end user message communication.
*/

#ifdef __cplusplus
}
#endif

#endif
