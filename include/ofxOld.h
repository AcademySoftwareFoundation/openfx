#ifndef _ofxOld_h_
#define _ofxOld_h_

/*
Software License :

Copyright (c) 2003-2015, The Open Effects Association Ltd. All rights reserved.

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


/** @brief Flags whether the plugin will attempt to set the value of a parameter in some callback or analysis pass

    - Type - int x 1
    - Property Set - plugin parameter descriptor (read/write) and instance (read only)
    - Default - 0
    - Valid Values - 0 or 1

This is used to tell the host whether the plug-in is going to attempt to set the value of the parameter.
*/
#define kOfxParamPropPluginMayWrite "OfxParamPropPluginMayWrite"
/*
v1.4: removed
*/

/** @brief String to label images with YUVA components

Note, this has been deprecated.
*/
#define kOfxImageComponentYUVA "OfxImageComponentYUVA"
/*
removed in v1.4
 */

/** @brief Indicates whether an effect is performing an analysis pass.

   - Type - int X 1
   - Property Set -  plugin instance (read/write)
   - Default - to 0
   - Valid Values - This must be one of 0 or 1

This feature has been deprecated - officially commented out v1.4.
*/
#define kOfxImageEffectPropInAnalysis "OfxImageEffectPropInAnalysis"

/** @brief The size of an interact's openGL viewport

    - Type - int X 2 
    - Property Set - read only property on the interact instance and in argument to all the interact actions.

This property is the redundant and its use will be deprecated in future releases.
Note, this has been deprecated.
*/
#define kOfxInteractPropViewportSize "OfxInteractPropViewport"
/*
removed in v1.4
 */

#endif

