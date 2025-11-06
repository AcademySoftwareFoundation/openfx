/**************************************************************/
/*                                                            */
/*  Copyright 2025 Dark Star Systems, Inc.                    */
/*  All Rights Reserved.                                      */
/*                                                            */
/**************************************************************/

#pragma once

#include <ofxCore.h>
#include <ofxGPURender.h>

inline const char *ofxStatusToString(OfxStatus s) {
  switch (s) {
    case kOfxStatOK:
      return "kOfxStatOK";
    case kOfxStatFailed:
      return "kOfxStatFailed";
    case kOfxStatErrFatal:
      return "kOfxStatErrFatal";
    case kOfxStatErrUnknown:
      return "kOfxStatErrUnknown";
    case kOfxStatErrMissingHostFeature:
      return "kOfxStatErrMissingHostFeature";
    case kOfxStatErrUnsupported:
      return "kOfxStatErrUnsupported";
    case kOfxStatErrExists:
      return "kOfxStatErrExists";
    case kOfxStatErrFormat:
      return "kOfxStatErrFormat";
    case kOfxStatErrMemory:
      return "kOfxStatErrMemory";
    case kOfxStatErrBadHandle:
      return "kOfxStatErrBadHandle";
    case kOfxStatErrBadIndex:
      return "kOfxStatErrBadIndex";
    case kOfxStatErrValue:
      return "kOfxStatErrValue";
    case kOfxStatReplyYes:
      return "kOfxStatReplyYes";
    case kOfxStatReplyNo:
      return "kOfxStatReplyNo";
    case kOfxStatReplyDefault:
      return "kOfxStatReplyDefault";

    case kOfxStatErrImageFormat:
      return "kOfxStatErrImageFormat";

    case kOfxStatGPUOutOfMemory:
      return "kOfxStatGPUOutOfMemory";
    case kOfxStatGPURenderFailed:
      return "kOfxStatGPURenderFailed";
    default:
      return "Unknown OFX Status";
  }
}
