.. SPDX-License-Identifier: CC-BY-4.0
.. _threadRecursionSafety:
Thread and Recursion Safety
===========================

Hosts are generally multi-threaded, those with a GUI will most likely
have an interactive thread and a rendering thread, while any host
running on a multi-CPU machine may have a render thread per CPU. Host
may batch effects off to a render farm, where the same effect has
separate frames rendered on completely different machines. OFX needs to
address all these situations.

Threads in the host application can be broken into two categories...

-  main theaads
   , where any action may be called
-  render threads
   where only a subset of actions may be called.

For a given effect instance, there can be only one main thread and zero
or more render threads. An instance must be able to handle simultaneous
actions called on the main and render threads. A plugin can control the
number of simultaneous render threads via the
:c:macro:`kOfxImageEffectPluginRenderThreadSafety`
effect descriptor property.

The only actions that can be called on a render thread are...

-  :c:macro:`kOfxImageEffectActionBeginSequenceRender`
-  :c:macro:`kOfxImageEffectActionRender`
-  :c:macro:`kOfxImageEffectActionEndSequenceRender`
-  :c:macro:`kOfxImageEffectActionIsIdentity`
-  :c:macro:`kOfxImageEffectActionGetFramesNeeded`
-  :c:macro:`kOfxImageEffectActionGetRegionOfDefinition`
-  :c:macro:`kOfxImageEffectActionGetRegionsOfInterest`

If a plugin cannot support this multi-threading behaviour, it will need
to perform explicit locking itself, using the locking mechanisms in the
suites defined in `ofxMultiThread.h <https://github.com/ofxa/openfx/blob/master/include/ofxMultiThread.h>`_.

This will also mean that the host may need to perform locking on the
various function calls over the API. For example, a main and render
thread may both simultaneously attempt to access a parameter from a
single effect instance. The locking should...

-  block write/read access
-  not block on read/read access
-  be fine grained at the level of individual function calls,
-  be transparent to the plugin, so it will block until the call
   succeeds.

For example, a render thread will only cause a parameter to lock out
writes only for the duration of the call that reads the parameter, not
for the duration of the whole render action. This will allow a main
thread to continue writing to the parameter during a render. This is
especially important if you have a custom interactive GUI that you want
to keep working during a render call.

Note that a main thread should generally issue an abort to any linked
render thread when a parameter or other value affecting the effect (eg:
time) has been changed by the user. A re-render should then be issued so
that a correct frame is created.

How an effect handles simulanteous calls to render is dealt with in
:ref:`the multi-thread rendering section<ImageEffectsMultiThreadingRendering>`.

Many hosts get around the problem of sharing a single instance in a UI
thread and a render thread by having two instances, one for the user to
interact with and a render only one that shadows the UI instance.

Recursive Actions
-----------------

When running on a main thread, some actions may end up being called
recursively. A plug-in must be able to deal with this. For example
consider the following sequence of events in a plugin...

1. user sets parameter A in a GUI
2. host issues
   :c:macro:`kOfxActionInstanceChanged`
   action
3. plugin traps that and sets parameter B

   1. host issues a new
      :c:macro:`kOfxActionInstanceChanged`
      action for parameter B
   2. plugin traps that and changes some internal private state and
      requests the overlay redraw itself

      1. :c:macro:`kOfxInteractActionDraw`
         issued to the effect's overlay
      2. plugin draws overlay
      3. :c:macro:`kOfxInteractActionDraw`
         returns

   3. :c:macro:`kOfxActionInstanceChanged`
      action for parameter B returns

4. :c:macro:`kOfxActionInstanceChanged`
   action returns

The image effect actions which may trigger a recursive action call on a
single instance are...

-  :c:macro:`kOfxActionBeginInstanceChanged`
-  :c:macro:`kOfxActionInstanceChanged`
-  :c:macro:`kOfxActionEndInstanceChanged`
-  :c:macro:`kOfxActionSyncPrivateData`

The interact actions which may trigger a recursive action to be called
on the associated plugin instance are...

-  :c:macro:`kOfxInteractActionGainFocus`
-  :c:macro:`kOfxInteractActionKeyDown`
-  :c:macro:`kOfxInteractActionKeyRepeat`
-  :c:macro:`kOfxInteractActionKeyUp`
-  :c:macro:`kOfxInteractActionLoseFocus`
-  :c:macro:`kOfxInteractActionPenDown`
-  :c:macro:`kOfxInteractActionPenMotion`
-  :c:macro:`kOfxInteractActionPenUp`

The image effect actions which may be called recursively are...

-  :c:macro:`kOfxActionBeginInstanceChanged`
-  :c:macro:`kOfxActionInstanceChanged`
-  :c:macro:`kOfxActionEndInstanceChanged`
-  :c:macro:`kOfxImageEffectActionGetClipPreferences`
-  :c:macro:`kOfxImageEffectActionGetRegionOfDefinition`
   (as a result of calling
   :cpp:func:`OfxImageEffectSuiteV1::clipGetImage`
   from
   :c:macro:`kOfxActionInstanceChanged`
   )
-  :c:macro:`kOfxImageEffectActionGetRegionsOfInterest`
   (as a result of calling
   :cpp:func:`OfxImageEffectSuiteV1::clipGetImage`
   from
   :c:macro:`kOfxActionInstanceChanged`
   )

The interact actions which may be called recursively are...

-  :c:macro:`kOfxInteractActionDraw`
