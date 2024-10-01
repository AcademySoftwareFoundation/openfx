.. SPDX-License-Identifier: CC-BY-4.0
Sequences of Operations Required to Load a Plug-in
---------------------------------------------------

The following sequence of operations needs to be performed by a host
before it can start telling a plug-in what to do via its ``mainEntry``
function.

1. the binary containing the plug-in is loaded,
2. (if implemented by plugin and host): the host calls the plug-in's ``OfxSetHost`` function
3. the number of plug-ins is determined via the
   ``OfxGetNumberOfPlugins``
   function,
4. for each plug-in defined in the binary

   1. ``OfxGetPlugin`` is called,
   2. the pluginApi and apiVersion of the returned OfxPlugin struct are examined,
   3. if the plug-in's API or its version are not supported, the plug-in
      is ignored and we skip to the next one,
   4. the plug-in's pointer is recorded in a plug-in cache,
   5. an appropriate OfxHost struct is passed to the plug-in via setHost in the returned OfxPlugin struct.
