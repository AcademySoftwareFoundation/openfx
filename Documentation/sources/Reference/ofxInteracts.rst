.. SPDX-License-Identifier: CC-BY-4.0
Interacts
=========

When a host presents a graphical user interface to an image effect, it
may optionally give it the chance to draw its own custom GUI tools and
to be able to interact with pen and keyboard input. In OFX this is done
via the OfxInteract suite, which is found in the file `ofxInteract.h <https://github.com/ofxa/openfx/blob/master/include/ofxInteract.h>`_.

OFX Interacts use openGL by default to perform all drawing, due to its
portabilty, robustness and wide implementation. As of 2022, some
systems are moving away from OpenGL support in favor of more modern
graphics drawing APIs. So as of OFX 1.5, effects may use the
:cpp:class:`OfxDrawSuiteV1` instead of OpenGL, if the host supports it,
by requesting :c:macro:`kOfxImageEffectPluginPropOverlayInteractV2`
instead of :c:macro:`kOfxImageEffectPluginPropOverlayInteractV1`.

Each plugin implementing interacts sets up a pointer property which
should point to a separate "InteractMain"" entry point. This entry
point is *not* the same as the one in the OfxPlugin struct, as it
needs to respond to a different set of actions to the effect.

There are two things an image effect can draw via its interact:

-  An overlay on the image being currently viewed in any image
   viewer, set via the effect descriptor's
   :c:macro:`kOfxImageEffectPluginPropOverlayInteractV1`
   property.
-  Replacement for any parameter's standard GUI object. set this
   via the parameter descriptor's
   :c:macro:`kOfxParamPropInteractV1`
   property.

Hosts might not be able to support interacts. To indicate this, two
properties exist on the host descriptor which an effect should examine
at description time so as to determine its own behaviour. These are:

-  :c:macro:`kOfxImageEffectPropSupportsOverlays`
-  :c:macro:`kOfxParamHostPropSupportsCustomInteract`

Interacts are separate objects from the effect they are associated with.
They have their own descriptor and instance handles passed into their
separate main entry point.

An interact instance cannot exist without a plugin instance. An
interact's instance, once created, is bound to a single instance of a
plugin until the interact instance is destroyed.

When using OpenGL, all interacts of the same type share openGL display
lists, even if they are in different openGL contexts.

All interacts of the same type will have the same pixel types (this is a
side effect of the last point), this will always be double buffered with
at least RGB components. Alpha and the exact bit depth is left to the
implementation.

So for example, all image effect overlays share the same display lists
and have the same pixel depth, and all custom parameter GUIs share the
same display list and have the same pixel depth, but overlays and custom
parameter GUIs do not necassarily share the same display list/pixel
depths.

An interact instance may be used in more than one view. Consider an
image effect overlay interact in a host that supports multiple viewers
to an effect instance. The same interact instance will be used in all
views, the relevant properties controlling the view being changed before
any action is passed to the interact. In this example, the draw action
would be called once for each view open on the instance, with the
projection, viewport and pixel scale being set appropriately for the
view before each action.

.. _ImageEffectOverlays:

Overlay Interacts
-----------------

Hosts will generally display images (both input and output) in user
their interfaces. A plugin can put an interact in this display by
setting the effect descriptor
:c:macro:`kOfxImageEffectPluginPropOverlayInteractV1`
or
:c:macro:`kOfxImageEffectPluginPropOverlayInteractV2`
property to point to a main entry.

If the plugin requests OverlayInteractV2 and the host supports it, then the plugin *must* do all drawing with the DrawSuite, not OpenGL.

OpenGL
^^^^^^^

In OpenGL, the viewport for such interacts will depend completely on the host.

The ``GL_PROJECTION`` matrix will be set up so that it maps openGL
coordinates to canonical image coordinates.

The ``GL_MODELVIEW`` matrix will be the identity matrix.

An overlay's interact draw action should assume that it is sharing the
openGL context and viewport with other objects that belong to the host.
It should not blank the background and it should never swap buffers,
that is for the host to do.

DrawSuite
^^^^^^^^^

For info on using DrawSuite, see :c:struct:`OfxDrawSuiteV1`.

.. _ParametersInteracts:

Parameter Interacts
-------------------

All parameters, except for custom parameters, have some default
interface that the host creates for them. Be it a numeric slider, colour
swatch etc... Effects can override the default interface (or set an
interface for a custom parameter) by setting the
:c:macro:`kOfxParamPropInteractV1`. This will
completely replace the parameters default user interface in the 'paged'
and *hierarchical* interfaces, but it will not replace the parameter's
interface in any animation sheet.

Note that not all hosts implement custom parameter interacts. See
:c:macro:`kOfxParamHostPropSupportsCustomInteract`.

Properties affecting custom interacts for parameters are:

-  :c:macro:`kOfxParamPropInteractSizeAspect`
-  :c:macro:`kOfxParamPropInteractMinimumSize`
-  :c:macro:`kOfxParamPropInteractPreferedSize`

The viewport for such interacts will be dependent upon the various
properties above, and possibly a per host override in any XML resource
file.

The ``GL_PROJECTION`` matrix will be an orthographic 2D view with -0.5,-0.5
at the bottom left and viewport width-0.5, viewport height-0.5 at the
top right.

The ``GL_MODELVIEW`` matrix will be the identity matrix.

The bit depth will be double buffered 24 bit RGB.

A parameter's interact draw function will have full responsibility for
drawing the interact, including clearing the background and swapping
buffers.


Interact Actions
----------------

The following actions may be passed to the interact entry point in an
image effect plug-in.

-  **The Generic Describe Action**, :c:macro:`KOfxActionDescribe` -
   called to describe the specific interact.
-  **The Create Instance Action**, :c:macro:`KOfxActionCreateInstance` -
   called just after an instance of the interact is created.
-  **The Generic Destroy Instance Action**, :c:macro:`KOfxActionDestroyInstance` -
   called just before the interact is destroyed.
-  **The Draw Action**, :c:macro:`kOfxInteractActionDraw` -
   called to have the interact draw itself.
-  :c:macro:`kOfxInteractActionPenMotion` -
   called whenever the interact has the input focus and the pen has
   moved, regardless of whether the pen is up or down.
-  :c:macro:`kOfxInteractActionPenDown` -
   called whenever the interact has the input focus and the pen has
   changed state to 'down'.
-  :c:macro:`kOfxInteractActionPenUp` -
   called whenever the interact has the input focus and the pen has
   changed state to 'up'.
-  :c:macro:`kOfxInteractActionKeyDown` -
   called whenever the interact has the input focus and a key has gone
   down.
-  :c:macro:`kOfxInteractActionKeyUp` -
   called whenever the interact has the input focus and a key has gone
   up.
-  :c:macro:`kOfxInteractActionKeyRepeat` -
   called whenever the interact has the input focus and a key has gone
   down and a repeat key sequence has been sent.
-  :c:macro:`kOfxInteractActionGainFocus` -
   called whenever the interact gains input focus.
-  :c:macro:`kOfxInteractActionLoseFocus` -
   called whenever the interact loses input focus.

The host must first call the effect's Describe action before calling its interact Describe action.

An interact instance must always be associated with an effect instance,
so it gets created after the effect and destroyed before the effect.

An interact instance should be issued a "gain focus" action before
any key or pen actions are issued, and a "lose focus" action when it
goes.
