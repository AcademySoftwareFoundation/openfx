.. SPDX-License-Identifier: CC-BY-4.0
.. _OfxPropertySuiteV1:
OfxPropertySuiteV1
==================

The files ``ofxCore.h`` and ``ofxProperty.h`` contain the basic
definitions for the property suite.

The property suite is the most basic and important suite in OFX, it is
used to get and set the values of various objects defined by other
suites.

A property is a named value of a specific data type, such values can be
multi-dimensional, but is typically of one dimension. The name is a 'C'
string literal, typically #defined in one of the various OFX header
files. For example, the property labeled by the string literal
``"OfxPropName"`` is a 'C' string which holds the name of some object.

Properties are not accessed in isolation, but are grouped and accessed
through a property set handle. The number and types of properties on a
specific property set handle are currently strictly defined by the API
that the properties are being used for. There is no scope to add new
properties.

There is a naming convention for property labels and the macros #defined
to them. The scheme is,

-  generic properties names start with
   "OfxProp"
   + name of the property, e.g. "OfxPropTime".
-  properties pertaining to a specific object with
   "Ofx"
   + object name +
   "Prop"
   + name of the property, e.g. "OfxParamPropAnimates".
-  the C preprocessor #define used to define the string literal is the
   same as the string literal, but with
   "k"
   prepended to the name. For example,
   #define kOfxPropLabel "OfxPropLabel"

OfxPropertySetHandle
OfxPropertySetHandle
Blind data type used to hold sets of properties::

        #include "ofxCore.h"
        typedef struct OfxPropertySetStruct *OfxPropertySetHandle; 

Description
-----------

Properties are not accessed on their own, nor do they exist on their
own. They are grouped and manipulated via an OfxPropertySetHandle.

Any object that has properties can be made to return it's property set
handle via some call on the relevant suite. Individual properties are
then manipulated with the property suite through that handle.


.. doxygenstruct:: OfxPropertySuiteV1
