#ifndef _ofxCore_h_
#define _ofxCore_h_

/*
Software License :

Copyright (c) 2003-2004, The Foundry Visionmongers Ltd. All rights reserved.

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


#include "stddef.h" // for size_t

#ifdef __cplusplus
extern "C" {
#endif

/** @file ofxCore.h
Contains the core OFX architectural struct and function definitions. For more details on the basic OFX architecture, see \ref Architecture.
*/


/** @brief Platform independent export macro.
 *
 * This macro is to be used before any symbol that is to be
 * exported from a plug-in. This is OS/compiler dependant.
 */
#ifdef WIN32
	#define OfxExport extern __declspec(dllexport)
#else
	#define OfxExport extern
#endif

/** @brief Blind data structure to manipulate sets of properties through */
typedef struct OfxPropertySetStruct *OfxPropertySetHandle;

/** @brief OFX status return type */
typedef int OfxStatus;

/** @brief Generic host structure passed to OfxPlugin::setHost function

    This structure contains what is needed by a plug-in to bootstrap it's connection
    to the host.
*/
typedef struct OfxHost {
  /** @brief Global handle to the host. Extract relevant host properties from this.
      This pointer will be valid while the binary containing the plug-in is loaded.
   */
  OfxPropertySetHandle host;

  /** @brief The function which the plug-in uses to fetch suites from the host.

      \arg \e host          - the host the suite is being fetched from this \em must be the \e host member of the OfxHost struct containing fetchSuite.
      \arg \e suiteName     - ASCII string labelling the host supplied API
      \arg \e suiteVersion  - version of that suite to fetch

      Any API fetched will be valid while the binary containing the plug-in is loaded.

      Repeated calls to fetchSuite with the same parameters will return the same pointer.

      returns
         - NULL if the API is unknown (either the api or the version requested),
	 - pointer to the relevant API if it was found
  */
  void *(*fetchSuite)(OfxPropertySetHandle host, const char *suiteName, int suiteVersion);
} OfxHost;


/** @brief Entry point for plug-ins

  \arg \e action   - ASCII c string indicating which action to take 
  \arg \e instance - object to which action should be applied, this will need to be cast to the appropriate blind data type depending on the \e action
  \arg \e inData   - handle that contains action specific properties
  \arg \e outData  - handle where the plug-in should set various action specific properties

  This is how the host generally communicates with a plug-in. Entry points are used to pass messages
  to various objects used within OFX. The main use is within the OfxPlugin struct.

  The exact set of actions is determined by the plug-in API that is being implemented, however all plug-ins
  can perform several actions. For the list of actions consult \ref ActionsAll.
 */
typedef  OfxStatus (OfxPluginEntryPoint)(const char *action, const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs);

/** @brief The structure that defines a plug-in to a host.
 *
 * This structure is the first element in any plug-in structure
 * using the OFX plug-in architecture. By examining it's members
 * a host can determine the API that the plug-in implements,
 * the version of that API, it's name and version.
 *
 * For details see \ref Architecture.
 *
 */
typedef struct OfxPlugin {
  /** Defines the type of the plug-in, this will tell the host what the plug-in does. e.g.: an image 
      effects plug-in would be a "OfxImageEffectPlugin"
   */
  const char		*pluginApi;

  /** Defines the version of the pluginApi that this plug-in implements */
  int            apiVersion;

  /** String that uniquely labels the plug-in among all plug-ins that implement an API.
      It need not necessarily be human sensible, however the preference is to use reverse
      internet domain name of the developer, followed by a '.' then by a name that represents
      the plug-in.. It must be a legal ASCII string and have no whitespace in the
      name and no non printing chars.
      For example "uk.co.somesoftwarehouse.myPlugin"
  */
  const char 		*pluginIdentifier; 
  
  /** Major version of this plug-in, this gets incremented when backwards compatibility is broken. */
  unsigned int 	 pluginVersionMajor;
  
  /**  Major version of this plug-in, this gets incremented when software is changed,
       but does not break backwards compatibility. */
  unsigned int   pluginVersionMinor;

  /** @brief Function the host uses to connect the plug-in to the host's api fetcher
      
      \arg \e fetchApi - pointer to host's API fetcher

      Mandatory function. 

      The very first function called in a plug-in. The plug-in \em must \em not call any OFX functions within this, it must only set it's local copy of the host pointer.

      \pre
        - nothing else has been called

      \post
        - the pointer suite is valid until the plug-in is unloaded
  */
  void     (*setHost)(OfxHost *host);
 
  /** @brief Main entry point for plug-ins

  Mandatory function.

  The exact set of actions is determined by the plug-in API that is being implemented, however all plug-ins
  can perform several actions. For the list of actions consult \ref ActionsAll.

   Preconditions
      - setHost has been called
   */
  OfxPluginEntryPoint *mainEntry;
} OfxPlugin;

/**
   \defgroup ActionsAll OFX Actions

These are the actions passed to a plug-in's 'main' function
*/
/*@{*/

/** @brief Action called just after a plug-in has been loaded, for more details see \ref ArchitectureMainFunction and \ref ActionsGeneralLoad */
#define  kOfxActionLoad "OfxActionLoad"

/** @brief Action called to have a plug-in describe itself to the host, for more details see \ref ArchitectureMainFunction and \ref ActionsGeneralDescribe */
#define kOfxActionDescribe "OfxActionDescribe"

/** @brief Action called just before a plug-in is unloaded, for more details see \ref ArchitectureMainFunction and \ref ActionsGeneralUnload */
#define kOfxActionUnload "OfxActionUnload"

/** @brief Action called to have a plug-in purge any temporary caches it may have allocated \ref ArchitectureMainFunction and \ref ActionsGeneralPurgeCaches */
#define kOfxActionPurgeCaches                 "OfxActionPurgeCaches"

/** @brief Action called to have a plug-in sync any internal data structures into custom parameters */
#define kOfxActionSyncPrivateData                 "OfxActionSyncPrivateData"

/** @brief Action called just after an instance has been created \ref ArchitectureMainFunction and \ref ActionsGeneralCreateInstance  */
#define kOfxActionCreateInstance        "OfxActionCreateInstance"

/** @brief Action called just before an instance is destroyed and \ref ActionsGeneralDestroyInstance */
#define kOfxActionDestroyInstance       "OfxActionDestroyInstance"

/** @brief Action indicating something in the instance has been changed, see \ref ActionsGeneralInstanceChanged */
#define kOfxActionInstanceChanged "OfxActionInstanceChanged"

/** @brief Action called before the start of a set of kOfxActionEndInstanceChanged actions, used with ::kOfxActionEndInstanceChanged to bracket a grouped set of changes, see \ref ActionsGeneralInstanceChangedBeginEnd */
#define kOfxActionBeginInstanceChanged "OfxActionBeginInstanceChanged"

/** @brief Action called after the end of a set of kOfxActionEndInstanceChanged actions, used with ::kOfxActionBeginInstanceChanged to bracket a grouped set of changes,  see \ref ActionsGeneralInstanceChangedBeginEnd*/
#define kOfxActionEndInstanceChanged "OfxActionEndInstanceChanged"

/** @brief Action called when an instance has the first editor opened for it */
#define kOfxActionBeginInstanceEdit "OfxActionBeginInstanceEdit"

/** @brief Action called when an instance has the last editor closed */
#define kOfxActionEndInstanceEdit "OfxActionEndInstanceEdit"

/*@}*/

/** @brief Returns the 'nth' plug-in implemented inside a binary
 *
 * Returns a pointer to the 'nth' plug-in implemented in the binary. A function of this type
 * must be implemented in and exported from each plug-in binary.
 */
OfxExport OfxPlugin *OfxGetPlugin(int nth);

/** @brief Defines the number of plug-ins implemented inside a binary
 *
 * A host calls this to determine how many plug-ins there are inside
 * a binary it has loaded. A function of this type
 * must be implemented in and exported from each plug-in binary.
 */
OfxExport int OfxGetNumberOfPlugins(void);

/**
   \defgroup PropertiesAll Ofx Properties 

These strings are used to identify properties within OFX, they are broken up by the host suite or API they relate to.
*/
/*@{*/

/**
   \defgroup PropertiesGeneral General Properties 

These properties are general properties and  apply to may objects across OFX
*/
/*@{*/

/** @brief General property used to get/set the time of something.

    - Type - int X 1
    - Default - 0, if a setable property
    - Property Set - commonly used as an argument to actions, input and output.
*/
#define kOfxPropTime "OfxPropTime"

/** @brief Indicates if a host is actively editing the effect with some GUI.

    - Type - int X 1
    - Property Set - effect instance (read only)
    - Valid Values - 0 or 1

If false the effect currently has no interface, however this may be because the effect is loaded in a background render host, or it may be loaded on an interactive host that has not yet opened an editor for the effect.

The output of an effect should only ever depend on the state of it's parameters, not on the interactive flag. The interactive flag is more a courtesy flag to let a plugin know that it has an interace. If a plugin want's to have its behaviour dependant on the interactive flag, it can always make a secret parameter which shadows the state if the flag.
*/
#define kOfxPropIsInteractive "OfxPropIsInteractive"

/** @brief The file path to the plugin.

    - Type - C string X 1
    - Property Set - effect descriptor (read only)

This is a string that indicates the file path where the plug-in was found by the host. The path is in the native
path format for the host OS (eg: *NIX directory separators are '/', Windows ones are '`\').

The path is to the bundle location, see \ref InstallationLocation. 
eg:  '/usr/OFX/Plugins/AcmePlugins/AcmeFantasticPlugin.ofx.bundle'
*/
#define kOfxPluginPropFilePath "OfxPluginPropFilePath"

/** @brief  A private data pointer that the plug-in can store it's own data behind.

    - Type - pointer X 1
    - Property Set - plugin instance (read/write),
    - Default - NULL

This data pointer is unique to each plug-in instance, so two instances of the same plug-in do not share the same data pointer. Use it to hang any needed private data structures.
*/
#define kOfxPropInstanceData "OfxPropInstanceData"

/** @brief General property, used to identify the kind of an object behind a handle

    - Type - ASCII C string X 1
    - Property Set - any object handle (read only)
    - Valid Values - currently this can be...
       - ::kOfxTypeImageEffectHost
       - ::kOfxTypeImageEffect
       - ::kOfxTypeImageEffectInstance
       - ::kOfxTypeParameter
       - ::kOfxTypeParameterInstance
       - ::kOfxTypeClip
       - ::kOfxTypeImage
*/
#define kOfxPropType "OfxPropType"

/** @brief Unique name of an object.

    - Type - ASCII C string X 1
    - Property Set - on many objects (descriptors and instances), see \ref PropertiesByObject (read only)

This property is used to label objects uniquely amoung objects of that type. It is typically set when a plugin creates a new object with a function that takes a name.
*/
#define kOfxPropName "OfxPropName"

/** @brief User visible name of an object.

    - Type - UTF8 C string X 1
    - Property Set - on many objects (descriptors and instances), see \ref PropertiesByObject. Typically readable and writable in most cases.
    - Default - the ::kOfxPropName the object was created with.

The label is what a user sees on any interface in place of the object's name. 

Note that resetting this will also reset ::kOfxPropShortLabel and ::kOfxPropLongLabel.
*/
#define kOfxPropLabel "OfxPropLabel"


/** @brief Short user visible name of an object.

    - Type - UTF8 C string X 1
    - Property Set - on many objects (descriptors and instances), see \ref PropertiesByObject. Typically readable and writable in most cases.
    - Default - initially ::kOfxPropName, but will be reset if ::kOfxPropLabel is changed.

This is a shorter version of the label, typically 13 character glyphs or less. Hosts should use this if they have limitted display space for their object labels.
*/
#define kOfxPropShortLabel "OfxPropShortLabel"

/** @brief Long user visible name of an object.

    - Type - UTF8 C string X 1
    - Property Set - on many objects (descriptors and instances), see \ref PropertiesByObject. Typically readable and writable in most cases.
    - Default - initially ::kOfxPropName, but will be reset if ::kOfxPropLabel is changed.

This is a longer version of the label, typically 32 character glyphs or so. Hosts should use this if they have mucg display space for their object labels.
*/
#define kOfxPropLongLabel "OfxPropLongLabel"

/** @brief Indicates why a plug-in changed.

    - Type - ASCII C string X 1
    - Property Set - the inArgs parameter on the ::kOfxActionInstanceChanged action.
    - Valid Values - this can be...
       - ::kOfxChangeUserEdited - the user directly edited the instance somehow and caused a change to something, this includes undo/redos and resets
       - ::kOfxChangePluginEdited - the plug-in itself has changed the value of the object in some action
       - ::kOfxChangeTime - the time has changed and this has affected the value of the object because it varies over time

Argument property for the ::kOfxActionInstanceChanged action.
*/
#define kOfxPropChangeReason "OfxPropChangeReason"

/** @brief A pointer to an effect instance.

    - Type - pointer X 1
    - Property Set - on an interact instance (read only)

This property is used to link an object to the effect. For example if the plug-in supplies an openGL overlay for an image effect, 
the interact instance will have one of these so that the plug-in can connect back to the effect the GUI links to.
*/
#define kOfxPropEffectInstance "OfxPropEffectInstance"
/*@}*/

/*@}*/

/** @brief String used as a value to ::kOfxPropChangeReason to indicate a user has changed something */
#define kOfxChangeUserEdited "OfxChangeUserEdited"

/** @brief String used as a value to ::kOfxPropChangeReason to indicate the plug-in itself has changed something */
#define kOfxChangePluginEdited "OfxChangePluginEdited"

/** @brief String used as a value to ::kOfxPropChangeReason to a time varying object has changed due to a time change */
#define kOfxChangeTime "OfxChangeTime"

/** @brief How time is specified within the OFX API */
typedef double OfxTime;

/** @brief Defines one dimensional integer bounds */
typedef struct OfxRangeI {
  int min, max;
} OfxRangeI;

/** @brief Defines one dimensional double bounds */
typedef struct OfxRangeD {
  double min, max;
} OfxRangeD;

/** @brief Defines two dimensional integer point */
typedef struct OfxPointI {
  int x, y;
} OfxPointI;

/** @brief Defines two dimensional double point */
typedef struct OfxPointD {
  double x, y;
} OfxPointD;

/** @brief Used to flag infinite rects. Set minimums to this to indicate infinite

This is effectively INT_MAX. 
 */
#define kOfxFlagInfiniteMax ((int)((1 << (sizeof(int)*8 - 1)) - 1))

/** @brief Used to flag infinite rects. Set minimums to this to indicate infinite.

This is effectively INT_MIN
 */
#define kOfxFlagInfiniteMin ((int)(-kOfxFlagInfiniteMax - 1))

/** @brief Defines two dimensional integer region

Regions are x1 <= x < x2

Infinite regions are flagged by setting
- x1 = kOfxFlagInfiniteMin
- y1 = kOfxFlagInfiniteMin
- x2 = kOfxFlagInfiniteMax
- y2 = kOfxFlagInfiniteMax

 */
typedef struct OfxRectI {
  int x1, y1, x2, y2;
} OfxRectI;

/** @brief Defines two dimensional double region

Regions are x1 <= x < x2

Infinite regions are flagged by setting
- x1 = kOfxFlagInfiniteMin
- y1 = kOfxFlagInfiniteMin
- x2 = kOfxFlagInfiniteMax
- y2 = kOfxFlagInfiniteMax

 */
typedef struct OfxRectD {
  double x1, y1, x2, y2;
} OfxRectD;

/** @brief Defines an 8 bit per component RGBA pixel */
typedef struct OfxRGBAColourB {
  unsigned char r, g, b, a;
}OfxRGBAColourB;

/** @brief Defines a 16 bit per component RGBA pixel */
typedef struct OfxRGBAColourS {
  unsigned short r, g, b, a;
}OfxRGBAColourS;

/** @brief Defines a floating point component RGBA pixel */
typedef struct OfxRGBAColourF {
  float r, g, b, a;
}OfxRGBAColourF;


/** @brief Defines a double precision floating point component RGBA pixel */
typedef struct OfxRGBAColourD {
  double r, g, b, a;
}OfxRGBAColourD;


/** @brief Defines an 8 bit per component YUVA pixel */
typedef struct OfxYUVAColourB {
  unsigned char y, u, v, a;
}OfxYUVAColourB;

/** @brief Defines an 16 bit per component YUVA pixel */
typedef struct OfxYUVAColourS {
  unsigned short y, u, v, a;
}OfxYUVAColourS;

/** @brief Defines an floating point component YUVA pixel */
typedef struct OfxYUVAColourF {
  float y, u, v, a;
}OfxYUVAColourF;


/** @brief String used to label unset bitdepths */
#define kOfxBitDepthNone "OfxBitDepthNone"

/** @brief String used to label unsigned 8 bit integer samples */
#define kOfxBitDepthByte "OfxBitDepthByte"

/** @brief String used to label unsigned 16 bit integer samples */
#define kOfxBitDepthShort "OfxBitDepthShort"

/** @brief String used to label signed 32 bit floating point samples */
#define kOfxBitDepthFloat "OfxBitDepthFloat"

/**
   \defgroup StatusCodes Status Codes 

These strings are used to identify error states within ofx, they are returned
by various host suite functions, as well as plug-in functions. The valid return codes 
for each function are documented with that function.
*/
/*@{*/

/**
   \defgroup StatusCodesGeneral General Status Codes 

General status codes start at 1 and continue until 999

*/
/*@{*/

/** @brief Status code indicating all was fine */
#define kOfxStatOK 0

/** @brief Status error code for a failed operation */
#define kOfxStatFailed  ((int)1)

/** @brief Status error code for a fatal error

  Only returned in the case where the plug-in or host cannot continue to function and needs to be restarted. 
 */
#define kOfxStatErrFatal ((int)2)

/** @brief Status error code for an operation on or request for an unknown object */
#define kOfxStatErrUnknown ((int)3)

/** @brief Status error code returned by plug-ins when they are missing host functionality, either an API or some optional functionality (eg: custom params).

    Plug-Ins returning this should post an appropriate error message stating what they are missing.
 */
#define kOfxStatErrMissingHostFeature ((int) 4)

/** @brief Status error code for an unsupported feature/operation */
#define kOfxStatErrUnsupported ((int) 5)

/** @brief Status error code for an operation attempting to create something that exists */
#define kOfxStatErrExists  ((int) 6)

/** @brief Status error code for an incorrect format */
#define kOfxStatErrFormat ((int) 7)

/** @brief Status error code indicating that something failed due to memory shortage */
#define kOfxStatErrMemory  ((int) 8)

/** @brief Status error code for an operation on a bad handle */
#define kOfxStatErrBadHandle ((int) 9)

/** @brief Status error code indicating that a given index was invalid or unavailable */
#define kOfxStatErrBadIndex ((int)10)

/** @brief Status error code indicating that something failed due an illegal value */
#define kOfxStatErrValue ((int) 11)

/** @brief OfxStatus returned indicating a 'yes' */
#define kOfxStatReplyYes ((int) 12)

/** @brief OfxStatus returned indicating a 'no' */
#define kOfxStatReplyNo ((int) 13)

/** @brief OfxStatus returned indicating that a default action should be performed */
#define kOfxStatReplyDefault ((int) 14)

/*@}*/

/*@}*/

#ifdef __cplusplus
}
#endif

/*------------------------------------------------------------------------------
          No source code is below, it is purely Doxygen documentation
  ------------------------------------------------------------------------------*/

/** @mainpage OFX : Open Plug-Ins For Special Effects
 
@section mainpageIntro Introduction

OFX consists of several parts built around a core generic plug-in architecture which can host more than just visual effects plug-ins. However, the first plug-ins to be hosted under OFX are image effect plug-ins.

OFX is fragmented into smaller 'Suites' (see \ref ArchitecturePluginAPIs), which a host system provides to perform specific tasks. These suites are documented separately. The higher level OFX Image Effect Plug-In API consists of a set of suites and a set of functions the plug-in needs to provide.

<HR>

@section mainpageIndex Index

  - \ref Architecture 
    - \ref ActionsGeneral 
  - \ref ImageEffectsPage 
    - \ref ImageEffectsProcessingArchitecture 
    - \ref ImageEffectContexts 
    - \ref ImageEffectsThreadSafety
    - \ref ImageEffectsImagesAndClips 
    - \ref ImageEffectsImagesAndTime 
    - \ref ImageEffectsRendering 
    - \ref ImageEffectsCoordinates
    - \ref ImageEffectGUIs 
    - \ref ImageEffectsActions
    - \ref ImageEffectsProperties
  - Supporting Suites 
    - \ref PropertiesPage  
    - \ref ParametersPage 
    - \ref CustomInteractionPage 
    - \ref MemoryPage 
    - \ref MultiThreadPage 
    - \ref MessagePage 
  - \ref StatusCodes 
  - \ref ImageEffectsOptionalBeetsPage 
  - \ref ImageEffectsExtendingBeetsPage 
  - \ref ExternalResourcesPage
  - \ref LinkingPage
  - \ref ImageEffectsKnownProblems

<HR>
@section mainPageDocumentationNote A Note About The Documentation

The documentation for OFX is generated by doxygen, http://www.doxygen.org, which takes specially formated C and C++ comments and automatically generates a set of HTML pages (so if you look in ofxCore.h you'll see this exact line). It breaks documentation into sets of pages, be sure to use the links at the top of this page (marked 'Modules', 'Compound List' and especially 'File List' etcc) to browse through the OFX doco.

<HR>
@section mainpageTTD Things Still To Be Done

What still needs to be done, but with somewhat less urgency...
  - Clean up documentation and make sure it is consistant (so may revisions, so little time)
  - More Examples generally, but also...
     - Parameter (not overlay) interact,
     - a plug-in that plays with FPS and fielding on the output clip,
     - XML 
*/

/** @page Architecture OFX : Basic Architecture
 
<HR>
@section ArchitectureIntro Architecture
The basic OFX architecture is actually designed to support a range of plug-in types, not just image processing for visual effects. The basic architecture is expandable, versioned and encapsulated, all by forcing plug-ins and host to be implemented almost solely via function pointers, often encapsulated in structs (for example see the OfxImageEffectSuiteV1 struct).

OFX is aimed to be unambiguous and complete, efficiency is deliberately a \em secondary consideration. Thus the use of strings over enumerated types. As the OFX API's will be exercised relatively lightly and infrequently (compared with the cost of processing of images), this should have practically no impact on the overall performance of plug-ins and hosts using the API.

<HR>
@section ArchitectureLanguage Language
The OFX plug-in Architecture is implemented via prototyped 'C'.

<HR>
@section ArchitectureDefinition Definition and Implementation
The API is defined solely with prototyped C '.h' files. There are no subsidiary libraries that a plug-in or host need to link against, though support libraries that automate common functionality map be written.

<HR>
@section ArchitectureSymbolExports Required Exported Functions
There are a minimal set of functions that a plug-in needs to expose in their .dll/.dso/.so, these are simply.
 - int OfxGetNumberOfPlugins(void);
 - OfxPlugin * OfxGetPlugin(int nth);

The host does not need to export any functions at all to the plug-in.

<HR>
@section ArchitecturePluginAPIs Suites And Plugins

Within OFX a <B>Suite</B> specifically refers to set of function pointers inside a C struct designed to perform some task. All such suites are both versioned and labelled, for example the property setting/fetching suite is labelled with "OfxPropertySuite" and version 1 of which is implemented via the OfxPropertySuiteV1 C struct.

Suites are implemented in the host are are retrieved by a plug-in using the OfxHost::fetchSuite function, which is a function pointer within the OfxHost struct.

<HR>
@section ArchitecturePluginIdenification Identifiying A Plug-In
The OfxGetPlugin call in the plug-in's binary returns a pointer that, in all cases, points to memory that starts with the OfxPlugin C struct.

When a host gets a pointer back from OfxGetPlugin, it examines the string OfxPlugin::pluginApi. This identifies what kind of plugin it is. For the case of image processing effects, this is "OfxImageEffectPlugin", for sound processing plugins, this will most likely be "OfxSoundEffectPlugin" and so on. 

Knowing the type of plug-in, the host then knows what suites and host handles are required for that plugin and what functions the plugin itself will have. The host passes a OfxHost structure appropriate to that plugin via it's setHost function. 

This allows for the same basic architecture to support different plugin types trivially. If a host does not support the given plugin type, it can simply ignore that plugin.

<HR>
@section ArchitectureAPIVersioning Versioning APIs
OFX explicitly versions plugin APIs. By examining the OfxPlugin::apiVersion, the host knows exactly what set of functions the plugin is going to supply and what version of the Suites it will need to provide. The integer, OfxPlugin::apiVersion, tells the host what version of the API the plugin is supporting, if the host does not support that version, the plugin should be ignored.

This also allows plugins to implement several versions of themselves in the same binary, so it can take advantages of new features in a V2 API, but present a V1 plugin to hosts that only support V1.

<HR>
@section ArchitecturePluginIdentifcation Uniquely Identifying A Plug-In and Versioning It
A plug-in needs to uniquely identify itself to a host. This is the job of OfxPlugin::pluginIdentifier. This null terminated ASCII C string should be unique amoung all plug-ins, it is not necassarily meant to convey a sensible name to an end user. The recommended format is the reverse domain name format of the developer, for example "uk.co.thefoundry", followed by the developer's unique name for the plugin. eg "uk.co.thefoundry.F_Kronos".

A plugin (as opposed to the API it impliments) is versioned with two separate integers in the OfxPlugin struct. They serve two separate functions and are,

 - OfxPlugin::pluginVersionMajor flags the functionality contained within a plugin, incrementing this number means that you have broken backwards compatibility of the plugin. More specifically, this means a setup from an earlier version, when loaded into this version, will not yield the same result.
 - OfxPlugin::pluginVersionMinor flags the release of a plugin that does not break backwards compatibility, but otherwise enhances that plugin. For example, increment this when you have fixed a bug or made it faster.

If a host encounters multiple versions of the same plugin it should,

  - when creating a brand new instance, always use the version of a plugin with the greatest major and minor version numbers,
  - when loading a setup, always use the plugin with the major version that matches the setup, but has the greatest minor number.

A more concrete example of versioning, the plugin identified by "org.wibble:Fred" is initially released as 1.0, however a few months later, wibble.org figured out how to make it faster and release it as 1.1. A year later, Fred can now do automatically what a user once needed to set five parameters up to do, thus making it much simpler to use. However this breaks backwards compability as these five parameters have been deleted, wibble.org then release this as v2.0.

A user's host might now have three versions of the Fred plugin on it, v1.0, v1.1 and v2.0. When creating a new instance of the plugin, the host should always use v2.0. When loading an old project which has a setup from a v1.x plugin, it should always use the latest, in this case being v1.1
 
Note that plug-ins can change the set of parameters between minor version releases. If a plugin does so, it should do so in a backwards compatible manner, such that the default value of any new parameter would yield the same results as previously. See @ref ParametersPersistance.

<HR>
@section ArchitectureDefiningAPIs OFX APIs
In OFX an API is a named (ASCII C string) and versioned (32 bit integer) set of functions that a plugin and host need to provide to each other.

Specifically, for a given API and version
     - a host will have to provide a specified set of versioned suites 
     - a plugin will have to respond to a specific set of actions passed into it's main plugin

For example, under the kOfxImageEffectAPI, Version 1, the host will need to provide...
     - OfxImageEffectSuiteV1
     - OfxPropertySuiteV1
     - OfxParameterSuiteV1
     - OfxInteractSuiteV1
     - OfxMemorySuiteV1
     - OfxMultiThreadSuiteV1

<HR>
@section ArchitectureQuickProperties Handles, Properties and other Details
Within OFX objects are generally reference via generic handles, for example the host, parameters, images etc... Many of the functions in Suites work with these generic handles, for example getting the value of a parameter.

The most fundamental of these Suites is the OfxPropertySuite, see \ref PropertiesPage. All generic handles have a set of properties associated with them. Plug-ins use properties to both define their behaviour to the host and ask about the current state of affairs on various objects. This includes setting the functions needed to do what it is they do.

<HR>
@section ArchitectureHost The Generic Host

The OfxHost struct is a generic host that is passed to a plugin which allows the plugin to boot-strap communications between the two. It consists of a property handle to the host, OfxHost::host, and a pointer to a function which allows the plugin to fetch APIs which it needs to do what ever it does, the OfxHost::fetchSuite function.

All plugins \em must implement the OfxPlugin::setHost, which function is the first function ever called in a plugin, it is where a plugin is passed an OfxHost pointer appropriate for its API. With this the plug-in can fetch suites from the host to do what it needs to do.

<HR>
@section ArchitectureMainFunction The Main Function and Actions

All plugins have a OfxPlugin::mainEntry function, which is the main communications channel between the host and the plugin. The main function is called with four arguments, the most important one being an ASCII C string indicating the action to take. The other three arguments are property handles, one being the object to perform the action on, another holding a set of properties that are arguments to the action, the other being a handle where the plugin should set properties that act as return values.

Within OFX a plugin \b action is a specific request passed to the plugin's main function, for each action, the three property pointers will have a specific set of properties in them. The page \ref ActionsGeneral contains a list of generic, API independant actions that can be passed to any type of plug-in.

In general, if the OfxPlugin::mainEntry does not encounter an error, it returns one of two values...
  - ::kOfxStatOK - which means that the effect actively trapped the action and has done something appropriate for it
  - ::kOfxStatReplyDefault - which means that the effect did not trap the action and the host should take the default response

There are a certain actions which require the effect actively process it, for these (for example the ::kOfxActionDescribe action), it is an error to return ::kOfxStatReplyDefault.

<HR>
@section ArchitectureLoadingDescribingAndInstancing Loading and Describing

As described above, all plugins must define themselves via the OfxPlugin struct. The non function variables identify the plugin to the host, the functions being the way the host and plug-in communicate. The sequences of actions that a host calls on a plugin when intially loading and later unloading a plugin are...

The calling sequence for these are...
    - binary containing the plugin is loaded into the host
    - ::OfxGetNumberOfPlugins is called to deterimine the number of plugins in the binary
    - ::OfxGetPlugin is called for each plugin, if the plugin is supported by the host
        - OfxPlugin::setHost is called for 
        - OfxPlugin::mainEntry is called with the action ::kOfxActionLoad \em once \em only 
        - OfxPlugin::mainEntry is called with the action ::kOfxActionDescribe \em once \em only 
        - other functions called to do what it needs to do, generally instances are made and destroyed
          and the like. 
        - OfxPlugin::mainEntry is called with the action ::kOfxActionUnload \em once \em only, all instances of the plugin are destroyed by this point
    - binary containing the plugin is unloaded from the host

An instance of a plugin is a working version of the plugin. Multiple instances of plugins are supported by all the APIs so far developed for OFX. Specically, within OFX a \b plugin defines the behaviour of an effect, an \b instance of an effect is one in operation. A plugin has it's a single unique handle which is distinct to an instance's. Plugins have a different set of properties to instances, though some may overlap. The plugin's handle's scope is from the plugin's describe action and until it's unload action.

Note that the plugin's describe function is called once only by the host, it is \em not called once each time an instance is created.

<HR>
@section ArchitectureInstances Instances, Creating, Loading and Copying

An instance of a plugin is a 'running' plug-in. Each time a new copy of an effect is needed, a new instance is created.

A host creates an instance by looking at the values the plugin set inside it's describe action. Before any futher actions are called, the host must make all the relevant objects the plugin asked for in it's describe action (eg parameters, input clips etc...).

If making a brand new instance, the host must set the objects in the instance to their default value before any other the action is called. 

When loading a previously saved setup of a plugin, a host must \em always create a new instance and set the objects in the instance to the values from the setup. This means an existing instance \em cannot have data loaded from a setup (as for example Discreet 'SPARK' plugins can).

Copying an instance has the same behaviour as if the old instance was saved to a setup and a new instance was created from that setup. Plugins do not have a 'copy' action as they all state information for the plugin must be encoded in its set of parameters.

When the host creates a new instance of a plugin, the first action passed to it is ::kOfxActionCreateInstance. The plugin can then create any private data it may need for that instance. 

<HR>
@section ArchitectureInstalling Distributing, Naming and Installing Plugins

Where a host application chooses to search for OFX plugins, what binary format they are in and any directory hierarchy is entirely up to it. However, it is strongly recommended that the following scheme be followed.

@subsection ArchitectureInstallingBinary Binary Types

Plug-ins should be distributed in the following formats, depending on the host operating system....
-# Microsoft Windows, as ".dll" dynamically linked libraries,
-# Apple OSX, as <a href="http://developer.apple.com/documentation/DeveloperTools/Conceptual/MachORuntime/2rt_mach-o_overview/chapter_8_section_2.html"> Mach-O binary bundles</a>,
-# IRIX and LINUX, as ELF dynamic shared objects.

@subsection ArchitectureInstallingHierarchy Installation Directory Hierarchy

Each plugin binary is distributed as a Mac OS X package style directory hierarchy. Note that the there are two distinct meanings of 'bundle', one referring to a binary file format, the other to a directory hierarchy used to distribute software. We are distribution binaries in a bundle package, and in the case of OSX, the binary is a binary bundle.

The directory hierarchy is as follows.....

    - NAME.ofx.bundle
        - Contents
	    - Info.plist
	    - Resources
	        - NAME.xml
	        - PLUGIN_A.png
	        - PLUGIN_B.png
	        - ...
	        - PLUGIN_N.png
	    - ARCHITECTURE_A
	        - NAME.ofx
	    - ARCHITECTURE_B
	        - NAME.ofx
	    ....
	    - ARCHITECTURE_N
	        - NAME.ofx
 
Where...
  - Info.plist is relevant for OSX only and needs to be filled in appropriately,
  - \b NAME is the file name you want the installed plugin to be identified by,
  - \b PLUGIN.png - is the image to use as an icon for the plug-in in the binary which has a matching OfxPlugin::pluginIdentifier field,
  - \b ARCHITECTURE is the specific operating system architecture the plugin was built for, these are currently...
      - MacOS     - for Apple Macintosh OS X (compiled 32 bit)
      - Win32     - for Microsoft Windows (compiled 32 bit)
      - IRIX      - for SGI IRIX plug-ins (compiled 32 bit)
      - IRIX64    - for SGI IRIX plug-ins (compiled 64 bit)
      - Linux-x86 - for Linux on intel x86 CPUs (compiled 32 bit)
      - Linux-x86-64 - for Linux on intel x86 CPUs running AMD's 64 bit extensions

Note that not all the above architectures need be supported, only at least one.

This structure is necessary on OS X, but it also gives a nice skeleton to hang all other operating systems from in a single install, as well as a clean place to put resources.

The Info.plist should contain the following keys...
     - CFBundleExecutable - the name of the binary bundle in the MacOS directory
     - CFBundlePackageType - to be 'BNDL'
     - CFBundleInfoDictionaryVersion
     - CFBundleVersion
     - CFBundleDevelopmentRegion

For more information on the bundle keys and more, go to <a href="http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFBundles/Concepts/infoplist.html#//apple_ref/doc/uid/20001121/CJBEJBHH">the apple website</a>.

@subsection InstallationLocation Installation Location

Plugins are searched for in a variety of locations, both default and user specified. All such directories are examined for plugin bundles and sub directories are also recursively examined. 

A list of directories is supplied in the "OFX_PLUGIN_PATH" environment variable, these are examined, first to last, for plugins, then the default location is examined.

On Microsoft Windows machines, the plugins are searched for in,
-# the ';' separated directory list specified by the environment variable "OFX_PLUGIN_PATH"
-# the directory "C:\Program Files\Common Files\OFX\Plugins"

On Apple OSX machines, the plugins are searched for in,
-# the ';' separated directory list specified by the environment variable "OFX_PLUGIN_PATH"
-# the directory "/Library/OFX/Plugins"

On UNIX, Linux and other UNIX like operating systems,
-# the ':' separated directory specified by the environment variable "OFX_PLUGIN_PATH"
-# the directory "/usr/OFX/Plugins"

Any bundle or sub-directory name starting with the character '@' is to be ignored. Such directories or bundles must be skipped.

Different versions of the same plugin may be encountered in multiple locations, in which case, only the greatest minor version of each major version should be taken note of. For example: 3.2, 1.4 and 2.3would be noted, but not 3.1, 1.3 or 2.0.

If two copies of a plugin with the same major and minor version are encountered, it can be assumed they are duplicates and all but the first one ignored.

@subsection ArchitectureInstallingIcons Plugin Icons

Some hosts may wish to display an icon associated with a plug-in on their interfaces. Any such icon must be in the Portable Network Graphics format (see http://www.libpng.org/) and must contain 32 bits of colour, including an alpha channel. Ideally it should be at least 128x128 pixels. 

Host applications should dynamically resize the icon to fit their prefered icon size. The icon should not have it's aspect changed, rather the host should fill with some appropriate colour any blank areas due to aspect mis matches.

Ideally plug-in developers should not render the plug-in's name into the icon, as this may be changed by the resource file, especially for internationalisation purposes. Hosts should thus present the plug-in's name next to the icon in some way. 

The icon file must be named as the corresponding OfxPlugin::pluginIdentifier field, post pended with '.png' and be placed in the resources sub-directory.

@subsection ArchitectureInstallingCachingPlugins Finding and Caching Plugins

It is suggested that host applications implement a search and cache scheme when looking for plug-ins. This is to stop hosts having to load all plug-ins at start up to create interfaces and so on. 

Hosts should recursively descend the plugin installation directory noting any new files or ones that have had their timestamp changed. Such new plugins should then be loaded (including any resource file) and information needed to describe that plugin cached away. The plugin should then be unloaded.

If a plugin has not changed since it was cached, the plugin is \em not loaded at start up, rather it's description is fetched from the cache with which interfaces etc... can be constructed.

Plug-ins are then only ever loaded on demand, either from use in a setup, or from a user requesting one be used.


<HR>
@section ArchitectureReserverNames Reserved Names

Within OFX, many things need to be declared with a name, for example a parameter. All names begining with "Ofx" (upper or lower case, any combination) are reserved for the API, neither a host nor a plugin should name any object with such a name.

<HR>
@section ArchitectureStrings A Brief Note on Who Owns Strings

Strings are considerred to be of two types, \b value strings and \b label strings. A label string is any string used by OFX to name a property or type. A value string is generally a string value of a property.  

More specifically, a \b label string is a string passed across the API as one of the following...
- a property label (ie: the 'char *property' argument in the property suites)
- a string parameter which has a predefined set of values (eg: \e paramType argument to OfxParameterSuiteV1::paramDefine, but not the \e name argument)

A \b value string is a string passed across the API as one of the following...
- all \e value arguments to any of the property suite calls
- any other string

@subsection ArchitectureStringsLabel

\em Label strings are considerred to be static constant strings, when passed across the API the host/plug-in recieving the string neither needs to duplicate or free the string, it can simply retain the orginal pointer passed over and use that in future, as it will not change. A host must be aware that when it unloads a plug-in all such pointers will be invalid, and be prepared to cope with such a situation.

@subsection ArchitectureStringsValue

\em Value strings have no assumptions made about them, when one is passed across the API, the thing that passed the string retains ownership of it. The thing getting the string is not responsible for freeing that string. The scope of the string's validity is until the next OFX API function is called. For example, within a plugin

@verbatim
   // pointer to store the returned value of the host name
   char *returnedHostName;

   // get the host name
   propHost->propGetString(hostHandle, kOfxPropName, 0, &returnedHostName);

   // now make a copy of that before the next API call, as it may not be valid after it
   char *hostName = strdup(returnedHostName);

   ofxHost->getParamValue(instance, "myParam", &value);
   
@endverbatim

Similarly for the host side.

*/

/** @page StatusCodes Status Codes

OFX reports it's status and errors by returning an integer from various functions. This is typed as OfxStatus.

In all cases the returned integer will always be a #defined value in one of the OFX header files.

The valid return status codes for each function are documented, however note that the following codes are very commonly used,
   - ::kOfxStatOK - the operation was succesful
   - ::kOfxStatErrMemory - the operation failed due to insufficient memory. A plugin function or action that returns this code may be called again after a host attempts a memory purge.
   - ::kOfxStatFailed - the operation failed in a way that no error code encapsulates cleanly. An error message is generally posted.


*/

/**
   @page ActionsGeneral OFX : Generic Actions

<HR>
   @section ActionsGeneralIntro Introduction
   
   This page lists the actions that can be passed to a plug-in of any kind, and the meaning of the property handles passed as parameters to the actions.

<HR>
@section ActionsGeneralLoad The Load Action

::kOfxActionLoad is the first action passed to the plug-in after the binary containing the plug-in has been loaded.

It is there to allow a plug-in to create any global data structures it may need.

The three property handles are redundant and should be set to NULL.

\pre
  - the plugin's OfxPlugin::setHost function has been called

\post
  - the ::kOfxActionLoad action will not be called again while the binary containing the plug-in is loaded

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatFailed, the load action failed, no further actions will be passed to the plug-in
- ::kOfxStatErrFatal, in which case we the program will be forced to quit

\par Default		
- If not trapped, the default action is for the host to do nothing.

<HR>

@section ActionsGeneralUnload The Unload Action

::kOfxActionUnload is the last action passed to the plug-in before the binary containing the plug-in is unloaded.

It is there to allow a plug-in to destroy any global data structures it may have created.

The three property handles are redundant and should be set to NULL.

\pre
- the ::kOfxActionLoad action has been called
- all instances of a plugin have been destroyed

\post
- the ::kOfxActionUnload action will not be called again while the binary containing the plug-in is loaded

\return
- ::kOfxStatOK, the action was trapped all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit

\par Default		
- If not trapped, the default action is for the host to do nothing.

<HR>

@section ActionsGeneralDescribe The Describe Action

The ::kOfxActionDescribe is the second action passed to a plug-in. It is where a plugin defines how it behaves and the resources it needs to function.

The \e handle parameter is the global, unique handle to the plugin itself. All description operations take place via this handle (eg: parameter definition).

The \e inArgs and \e outArgs parameters are redundant and set to NULL.

\pre
- ::kOfxActionLoad has been called

\post
- ::kOfxActionDescribe will not be called again, unless it fails and returns one of the error codes where the host is allowed to attempt the action again
- the \e handle argument, being the global plug-in handle, is a valid handle from the end of a sucessful describe action until the end of the ::kOfxActionUnload action (ie: the plug-in can cache it away without worrying about it changing between actions).

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatErrFatal, in which case the program will be forced to quit
- ::kOfxStatErrMissingHostFeature, in which the plugin will be unloaded and ignored, plugin may post message
- ::kOfxStatErrMemory, in which case describe may be called again after a memory purge
- ::kOfxStatFailed, something wrong, but no error code appropriate, plugin to post message

\par Default		
- The describe action cannot be ignored

<HR>

@section ActionsGeneralCreateInstance The Create Instance Action

The ::kOfxActionCreateInstance is the first action passed to a plug-in's instance after its creation. It is there to allow a plugin to create
any per-instance data structures it may need.

The \e handle parameter is the instance's handle.

The \e inArgs and \e outArgs parameters are redundant for image effects and set to NULL.

\pre
- ::kOfxActionDescribe has been called
- all objects requested in the describe action have been constructed and have had their values set

\post
- the instance pointer will be valid until the ::kOfxActionDestroyInstance action is passed to the plug-in

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatErrMemory, in which case ::kOfxActionCreateInstance may be called again after a memory purge
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the default action is for the host to do nothing.

<HR>

@section ActionsGeneralDestroyInstance The Destroy Instance Action

The ::kOfxActionDestroyInstance is the last action passed to a plug-in's instance before its destruction. It is there to allow a plugin to destroy
any per-instance data structures it may have created.

The  \e handle parameter is the instance's handle.

The  \e inArgs and  \e outArgs parameters are redundant and set to NULL.

\pre
- ::kOfxActionCreateInstance has been called on the handle,
- the instance has not had any of its members destroyed yet, 

\post
- the instance pointer is no longer valid and any operation on it will be undefined

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the default action is for the host to do nothing.

<HR>

@section ActionsGeneralPurgeCaches The Purge Caches Action

The ::kOfxActionPurgeCaches is an action that may be passed to a plug-in instance from time to time in low memory situations. Instances recieving this action should destroy any temporary data structures they may have and return the memory to free store.

The  \e handle parameter is a plugin instance

The  \e inArgs and  \e outArgs parameters are redundant and set to NULL.

\pre
- ::kOfxActionCreateInstance has been called on the handle,

\return
- ::kOfxStatOK, the action was trapped and all was well, purging may or may not have occurred
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the default action is for the host to do nothing.

<HR>

@section ActionsGeneralInstanceChanged The Instance Changed Action

The ::kOfxActionInstanceChanged signals that something has changed in a plugin's instance, either by user action, the host or the plugin itself. All change actions are bracketted by a pair of ::kOfxActionBeginInstanceChanged and ::kOfxActionEndInstanceChanged actions.

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropType         - the kind of thing that was changed,
   - ::kOfxTypeParameter - a parameter value has changed in some way
   - ::kOfxTypeClip      - a clip to an image effect has changed in some way
- ::kOfxPropName         - the name of the thing that was changed,
- ::kOfxPropChangeReason - what triggered the change,
   - ::kOfxChangeUserEdited    - the user direclty edited the instance somehow and caused a change to something, this includes undo/redos and resets
   - ::kOfxChangePluginEdited  - the plugin itself has changed the value of the object in some action
   - ::kOfxChangeTime          - the time has changed and this has affected the value of the object because it varies over time

If the plugin is an image effect, the inArgs also has the following properties
   - ::kOfxPropTime         - the effect time at which changed occured
   - ::kOfxImageEffectPropRenderScale     - the render scale applied to any image fetched

The \e outArgs handle is redundant and set to null.

\pre
- ::kOfxActionBeginInstanceChanged has been called on the handle,
- ::kOfxActionCreateInstance has been called on the handle,

\post
- ::kOfxActionEndInstanceChanged will be called on the handle.

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the default action is for the host to do nothing.

<HR>

@section ActionsGeneralInstanceChangedBeginEnd The Begin and End Instance Changed Actions

::kOfxActionBeginInstanceChanged and ::kOfxActionEndInstanceChanged are used to bracket all ::kOfxActionInstanceChanged actions, whether a single change or multiple changes.

The \e inArgs parameter contains the following read only properties...
- ::kOfxPropChangeReason - what triggered the change,
   - ::kOfxChangeUserEdited    - the user direclty edited the instance somehow and caused a change to something, this includes undo/redos and resets
   - ::kOfxChangePluginEdited - the plugin itself has changed the value of the object in some action
   - ::kOfxChangeTime          - the time has changed and this has affected the value of the object because it varies over time

The \e outArgs handle is redundant and set to null.

Some changes to a plugin instance can be grouped logically (eg: a 'reset all' button resetting all the instance's parameters), the begin/end instance changed actions allow a plugin to respond appropriately to a large set of changes,
for example, allowing a plugin that maintains a complex internal state to delay any changes to that state until all parameter changes have completed.

Nested instance change blocks may not occur. for example, if inside an instance change block, the plugin sets the value of some object, the host \em must wait for the block that changed occured in to complete before the host calls the instance change block for that value change. The sequence of actions would be as follows...

\verbatim
    Host issues ::kOfxActionBeginInstanceChanged, for a user edit on a parameter
        Host issues ::kOfxActionInstanceChanged, for the user changed parameter
        Plugin traps that and as a result, changes some other parameter
    Host issues ::kOfxActionEndInstanceChanged,
    Host issues ::kOfxActionBeginInstanceChanged for the plugin changing the parameter
        Host issues ::kOfxActionInstanceChanged, for the plugin changed parameter
    Host issues ::kOfxActionEndInstanceChanged
\endverbatim

\pre
- ::kOfxActionCreateInstance has been called on the handle,

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the host will assume no action needed to be taken and continue as normal.


<HR>

@section ActionsGeneralInstanceEditBeginEnd The Begin and End Instance Edit Actions

::kOfxActionBeginInstanceEdit and ::kOfxActionEndInstanceEdit are called when the first user interface is opened on an instance and when the last user interface is closed on it. Multiple user interfaces may exist to the same instance. For more information on user interfaces see \ref ImageEffectGUIs and \ref ParametersInterfaces.

The \e inArgs parameter is redundant and is null
The \e outArgs handle is redundant and is null.

\pre
- ::kOfxActionCreateInstance has been called on the handle,

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the host will assume no action needed to be taken and continue as normal.

<HR>

@section ActionsGeneralSync The Sync Private Data Action

::kOfxActionSyncPrivateData is called when a plugin should synchronise any private data structures to its set of arameters. This
generally occurs when an effect is about to be saved or copied, but it could occur in other situations as well.

The \e inArgs parameter is redundant and is null,
The \e outArgs handle is redundant and is null.

\pre
- ::kOfxActionCreateInstance has been called on the handle,

\return
- ::kOfxStatOK, the action was trapped and all was well
- ::kOfxStatReplyDefault, the action was ignored
- ::kOfxStatErrFatal, in which case we the program will be forced to quit
- ::kOfxStatFailed, something went wrong, but no error code appropriate, the plugin should to post a message

\par Default		
- If not trapped, the host will assume no action needed to be taken and continue as normal.

*/


/** @page ExternalResourcesPage OFX : Externally Specified Resources.

@section ExternalResourcesIntro Introduction

Plug-ins need to be internationalised, or they may need to have their interface tweaked slightly to appear cleanly on specific hosts. Rather than have huge if/elseif/elseif statements inside a binary to switch on host and locale, OFX allows you to supply values for certain properties in a separate resource file.

Each ofx binary may have a single xml file associated with it that is contains resource overrides for various properties of the plugin. Typically, the properties being changed are to do with user interface labels and layouts, as well as a few other things such as default values for parameters and so on. The DTD for the XML is found in <b> ofx.dtd </b> in the openfx include directory.

The XML file contains one of more sets of property respecifications, these are broken down into individual sets via
a host/locale paring. For example you can re-specify for host the "com.acme::compositor" in the locale "Japan". 

Note that there is a special host and locale, this is "default", the "default" host / locale set matches if no other
host or local does.

If you are simply using your resource file for internationalisation, you can specify a property set for all hosts by
setting the host to "default" and the locale to your needed local.

In this way a plugin can respecify the labels for all hosts in the "Japan" local and but still be able to layout (but not necassarilly relabel) params on a per host basis separately.

@section ExternalResourcesImageEffects Override-able Properties for Image Effects

Note that parameter and clip properties for an image effect are per image effect context. Again, there is a 'default' catch-all context you can use in the XML resource file.

The following Image Effect Plugin properties can be overridden,
 - ::kOfxPropLabel 
 - ::kOfxPropShortLabel 
 - ::kOfxPropLongLabel 
 - ::kOfxImageEffectPluginPropGrouping

The following Image Effect Plugin clip properties can be overridden,
 - ::kOfxPropLabel 
 - ::kOfxPropShortLabel 
 - ::kOfxPropLongLabel 
 - ::kOfxImageClipPropOptional
 
The following parameter properties can be overridden,
- ::kOfxPropLabel
- ::kOfxPropShortLabel
- ::kOfxPropLongLabel
- ::kOfxParamPropInteractSizeAspect 
- ::kOfxParamPropInteractMinimumSize 
- ::kOfxParamPropInteractPreferedSize 
- ::kOfxParamPropAnimates
- ::kOfxParamPropEvaluateOnChange
- ::kOfxParamPropPersistant
- ::kOfxParamPropSecret 
- ::kOfxParamPropScriptName 
- ::kOfxParamPropHint 
- ::kOfxParamPropDefault 
- ::kOfxParamPropDoubleType
- ::kOfxParamPropEnabled 
- ::kOfxParamPropChoiceOption 
- ::kOfxParamPropShowTimeMarker 
- ::kOfxParamPropMin 
- ::kOfxParamPropMax 
- ::kOfxParamPropDisplayMin 
- ::kOfxParamPropDisplayMax 
- ::kOfxParamPropIncrement 
- ::kOfxParamPropDigits 
- ::kOfxParamPropDimensionLabel 
- ::kOfxParamPropStringMode 

<HR>
@section ExternalResourcesLayout Layout

You can redefine the hierarchical grouping and the page layout of parameters in the resource file. However, this cannot be done in the default context, it can only be done in actual context definitions (see the examples and the DTD for clarification).

Note that some hosts may allow the same parameter to appear on multiple pages, but in no case can a parameter appear in more than one hierarchical group.

<HR>
@section ExternalResourcesMessages Messages

The message suite function OfxMessageSuiteV1::message takes a 'messageId' argument, this argument is used to uniquely label each message the plugin posts. The message string posted can be overridden in the XML by associating a new string with this ID.

The message function takes a printf style var-args list. The string that overrides the one in the binary can reposition these arguments via use of positionall params. For example...
 -  gMessageHost->message(NULL, kOfxMessageMessage, "myID", "The day is %s and the time is %d:%d", dayStr, hour, minute);

to produce something like 

-   "The day is Friday and the time is 10:24"

In the resource file, positional parameters are labelled with '$1' "$2" etc... So we could override the above string with...
  - "Time is $2:$3 on $1"

to produce

  - "Time is 10:35 on Friday"

The '$' symbol not followed by a digit is passed through. To escape the '$' symbol, use '$$". For example
  - original gMessageHost->message(NULL, kOfxMessageMessage, "priceID", "Cost is %d.%.2d", dollars, cents);
  - override "Price is $$$1:$2'
  - producing "Price $12.05"

Also note that the formatting of the positional argument does not change.

<HR>
@section ExternalResourcesCustomProperties Custom Properties

A host can define it's own GUI related properties which a plugin can use, either in an XML file or in the binary. For example a host that allows 
free form layout of params in pages could define a property that specifies the location and size of the parameter's gui element in pixels.

<HR>
@section ExternalResourcesMatchingProperties Property Matching Rules

Because of the ability to 'default' hosts, locales and contexts, there is potentially ambiguity as to what property applies in which case, as the same property may be set in more that one valid location. In order of precedence, a property has its value set from the resource file if a match is found according to...
   - exact locale,   exact host,   exact context
   - exact locale,   exact host,   default context
   - exact locale,   default host, exact context
   - exact locale,   default host, default context
   - default locale, exact host,   exact context
   - default locale, exact host,   default context
   - default locale, default host, exact context
   - default locale, default host, default context

@section ExternalResourcesQuestions Questions
- we need a list of locales

*/

/** @page LinkingPage OFX : Building and Linking Plugins

@section LinkingIntroduction Introduction

Linking object files to create a plugin is fairly straight forward, and is greatly simplified by the almost absolute lack of symbolic dependancies between a plugin and a host. The only caveats here are finding openGL symbols for plugins that do overlays.

The discussion below is for 32 bit plugins.

@section LinkingWindows Linking On Microsoft Windows (c)

Plugins source should all be compiled as a multithreaded DLL.

Plugins should be linked as a DLL, and if you are using openGL calls in your plugin, you should link to Opengl32.lib.

@section LinkingLinux Linking On Linux

There are no special compiler flags required for linux.

To link with gcc/g++ you should use the options 
 - \b -fPIC to create position independant code,
 - \b -Xlinker \b -Bsymbolic to force the plugin to look for symbols inside itself first, rather than on the host.
 
You should \em not link explicitly to an openGL library. This will force the plugin to look for any openGL symbols through the host, rather than in an explicit one. If the plugin and host don't share the same openGL library, chaos will reign and program execution will all most certainly end abruptly.

You can limit the number of symbols exported from the plugin on linux with \b -Xlinker \b --version-script=FILENAME. This can reduce file size quite significantly. See the gnu manual on \b ld for details, but a typical version script file for OFX would look something like.

@verbatim
OFX_1.0 {
         global:
		OfxGetNumberOfPlugins;
		OfxGetPlugin;
         local:
                *;
};
@endverbatim

@section LinkingOSX Linking On Apple OSX (c)

There are no special compiler flags required for OSX.

To link with gcc/g++ you should use the options..
 - \b -bundle to create a binary OSX bundle,
 - \b -framework \b opengl if using any openGL symbols in the plugin.

*/

#endif
