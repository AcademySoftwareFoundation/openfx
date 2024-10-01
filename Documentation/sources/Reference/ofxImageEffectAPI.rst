.. SPDX-License-Identifier: CC-BY-4.0
The Image Effect API
====================

Introduction
------------

In general, image effects plug-ins take zero or more input clips and
produce an output clip. So far so simple, however there are many devils
hiding in the details. Several supporting suites are required from the
host and the plug-in needs to respond to a range of actions to work
correctly. How an effect is intended to be used also complicates the
issue, forcing sets of behaviours depending on the context of an effect.

Plug-ins that implement the image effect API set the ``pluginApi``
member of the :ref:`OfxPlugin struct<OfxPlugin>` returned by the global
:ref:`OfxGetPlugin<OfxGetPlugin>` to be:

.. doxygendefine:: kOfxImageEffectPluginApi

The current version of the API is 1. This is enough to label the plug-in as an image effect
plug-in.

Image Effect API Header Files
-----------------------------

The header files used to define the OFX Image Effect API are...

-  `ofxCore.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxCore.h>`__
   Provides the core definitions of the general OFX architecture that
   allow the bootstrapping of specific APIs, as well as several core actions,
-  `ofxProperty.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxProperty.h>`__
    Provides generic property fetching suite used to get and set values about objects in the API,
-  `ofxParam.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxParam.h>`__
   Provides the suite for defining user visible parameters to an
   effect
-  `ofxMultiThread.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxMultiThread.h>`__
   Provides the suite for basic multi-threading capabilities
-  `ofxInteract.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxInteract.h>`__
   Provides the suite that allows a plug-in to use OpenGL to draw their own interactive GUI tools
-  `ofxKeySyms.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxKeySyms.h>`__
   Provides key symbols used by 'Interacts' to represent keyboard events
-  `ofxMemory.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxMemory.h>`__
   Provides a simple memory allocation suite,
-  `ofxMessage.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxMessage.h>`__
   Provides a simple messaging suite to communicate with an end user
-  `ofxImageEffect.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxImageEffect.h>`__
   Defines a suite and set of actions that draws all the above together to create an visual effect plug-in.
-  `ofxDrawSuite.h <https://github.com/AcademySoftwareFoundation/openfx/blob/master/include/ofxDrawSuite.h>`__
   Provides an optional suite that allows a plug-in to draw their own interactive GUI tools without using OpenGL

These contain the suite definitions, property definitions and action
definitions that are used by the API.

Actions Used by the API
-----------------------

All image effect plug-ins have a main entry point. This is used to trap
all the standard actions used to drive the plug-in. They can also have
other optional entry points that allow the plug-in to create custom user
interface widgets. These *interact* entry points are specified during
the two description actions.

The following actions can be passed to a plug-in's main entry point...

-  The Generic Load Action
   called just after a plug-in is first loaded,
-  The Generic Unload Action
   called just before a plug-in is unloaded,
-  The Generic Describe Action
   called to describe a plug-in's behaviour to a host,
-  The Generic Create Instance Action
   called just after an instance is created,
-  The Generic Destroy Instance Action
   called just before an instance is destroyed,
-  The Generic Begin/End Instance Changed Actions
   , a pair of actions used to bracket a set of Instance Changed
   actions,
-  The Generic Instance Changed Action
   an action used to indicate that a value has changed in a plug-in
   instance,
-  The Generic Purge Caches Action
   called to have the plug-in delete any temporary private data caches
   it may have,
-  The Sync Private Data Action
   called to have the plug-in sync any private state data back to its
   data set,
-  The Generic Begin/End Instance Edit Actions
   a pair of calls that are used to bracket the fact that a user
   interface has been opened on an instance and it is being edited,
-  The Begin Sequence Render Action
   where a plug-in is told that it is about to render a sequence of
   images,
-  The Render Action
   where a plug-in is told that it is to render an output image,
-  The End Sequence Render Action
   where a plug-in is told it has finished rendering a sequence of
   images,
-  The Describe In Context Action
   used to have a plug-in describe itself in a specific context,
-  The Get Region of Definition Action
   where an instance gets to state how big an image it can create,
-  The Get Regions Of Interest Action
   where an instance gets to state how much of its input images it needs
   to create a give output image,
-  The Get Frames Needed Action
   where an instance gets to state how many frames of input it needs on
   a given clip to generate a single frame of output,
-  The Is Identity Action
   where an instance gets to state that its current state does not
   affect its inputs, so that the output can be directly copied from an
   input clip,
-  The Get Clip Preferences Action
   where an instance gets to state what data and pixel types it wants on
   its inputs and will generate on its outputs,
-  The Get Time Domain Action
   where a plug-in gets to state how many frames of data it can
   generate.

Main Objects Used by the API
----------------------------

The image effect API uses a variety of different objects. Some are
defined via blind data handles, others via property sets, and some by a
combination of the two. These objects are...

-  Host Descriptor
   - a descriptor object used by a host to describe its behaviour to a
   plug-in,
-  Image Effect Descriptor
   - a descriptor object used by a plug-in to describe its behaviour to
   a host,
-  Image Effect Instance
   - an instance object maintaining state about an image effect,
-  Clip Descriptor
   - a descriptor object for a sequence of images used as input or
   output a plug-in may use,
-  Clip Instance
   - a instance object maintaining state about a sequence of images used
   as input or output to an effect instance,
-  Parameter Descriptor
   - a descriptor object used to specify a user visible parameter in an
   effect descriptor,
-  Parameter Instance
   - an instance object that maintains state about a user visible
   parameter in an effect instance,
-  Parameter Set Descriptor
   - a descriptor object used to specify a set of user visible
   parameters in an effect descriptor,
-  Parameter Set Instance
   - an instance object that maintains state about a set of user visible
   parameters in an effect instance,
-  Image Instance
   - a instance object that maintains state about a 2D image being
   passed to an effect instance.
-  Interact Descriptor
   - which describes a custom openGL user interface, for example an
   overlay over the inputs to an image effect. These have a separate
   entry point to an image effect.
-  Interact Instance
   - which holds the state on a custom openGL user interface. These have
   a separate entry point to an image effect.

Host Descriptors
~~~~~~~~~~~~~~~~

The host descriptor is represented by the properties found on the host property set handle in the
:ref:`OfxHost struct<OfxHost>`. The complete set of read only
properties are found in the section :ref:`Properties on the Image Effect
Host <ImageEffectHostProperties>`.

These sets of properties are there to describe the capabilities of the
host to a plug-in, thus giving a plug-in the ability to modify its
behaviour depending on the capabilities of the host.

A host descriptor is valid while a plug-in is loaded.

Effects
~~~~~~~

An effect is an object in the OFX Image Effect API that represents an
image processing plug-in. It has associated with it a set of properties,
a set of image clips and a set of parameters. These component objects of
an effect are defined and used by an effect to do whatever processing it
needs to. A handle to an image effect (instance or descriptor) is passed
into a plug-in's :ref:`main entry point<mainEntryPoint>` *handle*
argument:

.. doxygentypedef:: OfxImageEffectHandle

The functions that directly manipulate an image effect handle are
specified in the :cpp:class:`OfxImageEffectSuiteV1` found
in the header file `ofxImageEffect.h <https://github.com/ofxa/openfx/blob/master/include/ofxImageEffect.h>`_.

Effect Descriptors
^^^^^^^^^^^^^^^^^^

An effect descriptor is an object of type :cpp:type:`OfxImageEffectHandle`
passed into an effect's :ref:`main entry point<mainEntryPoint>`
``handle`` argument. The two actions it is passed to are:

*  :c:macro:`kOfxActionDescribe`
*  :c:macro:`kOfxImageEffectActionDescribeInContext`

A effect descriptor does not refer to a 'live' effect, it is a handle
which the effect uses to describe itself back to the host. It does this
by setting a variety of properties on an associated property handle, and
specifying a variety of objects (such as clips and parameters) using
functions in the available suites.

Once described, a host should cache away the description in some manner
so that when an instance is made, it simply looks at the description and
creates the necessary objects needed by that instance. This stops the
overhead of having every instance be forced to describe itself over the
API.

Effect descriptors are only valid in a effect for the duration of the
instance they were passed into.

The properties on an effect descriptor can be found in the section
:ref:`Properties on an Effect Descriptor <EffectDescriptorProperties>`.

Effect Instances
^^^^^^^^^^^^^^^^

A effect instance is an object of type :cpp:type:`OfxImageEffectHandle` passed
into an effect's :ref:`main entry point <mainEntryPoint>` ``handle``
argument. The ``handle`` argument should be statically cast to this
type. It is passed into all actions of an image effect that a descriptor
is not passed into.

The effect instance represents a 'live' instance of an effect. Because
an effect has previously been described, via a effect descriptor, an
instance does not have to respecify the parameters, clips and properties
that it needs. These means, that when an instance is passed to an
effect, all the objects previously described will have been created.

Generally multiple instances of an effect can be in existence at the
same time, each with a different set of parameters, clips and
properties.

Effect instances are valid between the calls to
:c:macro:`kOfxActionCreateInstance` and
:c:macro:`kOfxActionDestroyInstance`, for which it
is passed as the ``handle`` argument.

The properties on an effect instance can be found in the section
:ref:`Properties on an Effect Instance <EffectInstanceProperties>`.

Clips
~~~~~

A clip is a sequential set of images attached to an effect. They are
used to fetch images from a host and to specify how a plug-in wishes to
manage the sequence.

Clip Descriptors
^^^^^^^^^^^^^^^^

Clip descriptors are returned by the
:cpp:func:`OfxImageEffectSuiteV1::clipDefine` function.
They are used during the :c:macro:`kOfxActionDescribe` action by
an effect to indicate the presence of an input or output clip and how
that clip behaves.

A clip descriptor is only valid for the duration of the action it was
created in.

The properties on a clip descriptor can be found in the section
:ref:`Properties on a Clip Descriptor <ClipDescriptorProperties>`.

Clip Instances
^^^^^^^^^^^^^^

.. doxygentypedef:: OfxImageClipHandle

Clip instances are returned by the
:cpp:func:`OfxImageEffectSuiteV1::clipGetHandle` function.
They are are used to access images and and manipulate properties on an effect instance's input and output clips.
A variety of functions in the :cpp:class:`OfxImageEffectSuiteV1` are used to manipulate them.

A clip instance is valid while the related effect instance is valid.

The properties on a clip instance can be found in the section
:ref:`Properties on a Clip Instance <ClipInstanceProperties>`.

Parameters
~~~~~~~~~~

Parameters are user visible objects that an effect uses to specify its
state, for example a floating point value used to control the blur size
in a blur effect. Parameters (both descriptors and instances) are
represented as blind data handles of type:

.. doxygentypedef:: OfxParamHandle

Parameter sets are the collection of parameters that an effect has
associated with it. They are represented by the type
:cpp:type:`OfxParamSetHandle`. The contents of an effect's parameter set are
defined during the
:c:macro:`kOfxImageEffectActionDescribeInContext` action.
Parameters cannot be dynamically added to, or deleted from an effect instance.

Parameters can be of a wide range of types, each of which have their own
unique capabilities and property sets. For example a colour parameter
differs from a boolean parameter.

Parameters and parameter sets are manipulated via the calls and
properties in the :cpp:class:`OfxParameterSuiteV1` specified
in `ofxParam.h <https://github.com/ofxa/openfx/blob/master/include/ofxParam.h>`_.
The properties on parameter instances and
descriptors can be found in the section :ref:`Properties on Parameter
Descriptors and Instances <ParameterProperties>`.

Parameter Set Descriptors
^^^^^^^^^^^^^^^^^^^^^^^^^

Parameter set descriptors are returned by the
:cpp:func`OfxImageEffectSuiteV1::getParamSet` function.
This returns a handle associated with an image effect
descriptor which can be used by the parameter suite routines to create
and describe parameters to a host.

A parameter set descriptor is valid for the duration of the
:c:macro:`kOfxImageEffectActionDescribeInContext`
action in which it is fetched.

Parameter Descriptors
^^^^^^^^^^^^^^^^^^^^^

Parameter descriptors are returned by the
:cpp:func:`OfxParameterSuiteV1::paramDefine` function.
They are used to define the existence of a parameter to the
host, and to set the various attributes of that parameter. Later, when
an effect instance is created, an instance of the described parameter
will also be created.

A parameter descriptor is valid for the duration of the
:c:macro:`kOfxImageEffectActionDescribeInContext`
action in which it is created.

Parameter Set Instances
^^^^^^^^^^^^^^^^^^^^^^^

Parameter set instances are returned by the
:cpp:func:`OfxImageEffectSuiteV1::getParamSet` function.
This returns a handle associated with an image effect instance
which can be used by the parameter suite routines to fetch and describe
parameters to a host.

A parameter set handle instance is valid while the associated effect
instance remains valid.

Parameter Instances
^^^^^^^^^^^^^^^^^^^

Parameter instances are returned by the
:cpp:func:`OfxParameterSuiteV1::paramGetHandle` function.
This function fetches a previously described parameter back
from the parameter set. The handle can then be passed back to the
various functions in the
:cpp:class:`OfxParameterSuite1V` to manipulate it.

A parameter instance handle remains valid while the associated effect
instance remains valid.

Image Instances
^^^^^^^^^^^^^^^

An image instance is an object returned by the
:cpp:func:`OfxImageEffectSuiteV1::clipGetImage` function.
This fetches an image out of a clip and returns it as a
property set to the plugin. The image can be accessed by looking up the
property values in that set, which includes the data pointer to the
image.

An image instance is valid until the effect calls
:cpp:func:`OfxImageEffectSuiteV1::clipReleaseImage`
on the property handle. The effect *must* release all fetched images
before it returns from the action.

The set of properties that make up an image can be found in the section
:ref:`Properties on an Image <ImageProperties>`.

Interacts
~~~~~~~~~

An interact is an OFX object that is used to draw custom user interface
elements, for example overlays on top of a host's image viewer or custom
parameter widgets. Interacts have their own :ref:`main entry
point <mainEntryPoint>`, which is separate to the effect's main entry
point. Typically an interact's main entry point is specified as a
pointer property on an OFX object, for example the
:c:macro:`kOfxImageEffectPluginPropOverlayInteractV1`
property on an effect descriptor.

The functions that directly manipulate interacts are in the :cpp:class:`OfxInteractSuiteV1` found in the header file
`ofxInteract.h <https://github.com/ofxa/openfx/blob/master/include/ofxInteract.h>`_ , as well as the properties and specific actions that
apply to interacts.

Interact Descriptors
^^^^^^^^^^^^^^^^^^^^

Interact descriptors are blind handles passed to the :c:macro:`kOfxActionDescribeInteract` sent to an interact's separate
main entry point. They should be cast to the type :cpp:type:`OfxInteractHandle`.

The properties found on a descriptor are found in section :ref:`Properties on
Interact Descriptors <InteractDescriptorProperties>`.

Interact Instances
^^^^^^^^^^^^^^^^^^


Interact instances are blind handles passed to all actions but the
:c:macro:`kOfxActionDescribe` sent to an interact's
separate main entry point. They should be cast to the type

.. doxygentypedef:: OfxInteractHandle

The properties found on an instance are found in section :ref:`Properties on
Interact Instance <InteractInstanceProperties>`.
