#ifndef _ofxReadWrite_h_
#define _ofxReadWrite_h_

#ifdef __cplusplus
extern "C" {
#endif

/**
 @brief Indicates what file formats a decoder/encoder can handle.
  - Type - string X N
  - Property Set - image effect descriptor (read-write)
  - Valid Values - Any image or video file format extension
    e.g: "jpg" , "png", "avi", etc...
**/
#define kTuttleOfxImageEffectPropSupportedExtensions "TuttleOfxImageEffectPropSupportedExtensions"

#ifdef __cplusplus
}
#endif

#endif
