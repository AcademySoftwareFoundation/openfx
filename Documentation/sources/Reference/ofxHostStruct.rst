.. SPDX-License-Identifier: CC-BY-4.0
.. _OfxHost:

The OfxHost Struct
==================

The OfxHost struct is how a host provides plug-ins with access to the
various suites that make up the API they implement, as well as a host
property set handle which a plug-in can ask questions of. The
``setHost`` function in the OfxPlugin struct is passed a pointer to an
OfxHost as the first thing to boot-strapping plug-in/host communication.

The OfxHost contains two elements,

-  host
   - a property set handle that holds a set of properties which describe
   the host for the plug-in's API
-  fetchSuite
   - a function handle used to fetch function suites from the host that
   implement the plug-in's API

The host property set handle in the OfxHost is not global across all
plug-ins defined in the binary. It is only applicable for the plug-in
whose 'setHost' function was called. Use this handle to fetch things
like host application names, host capabilities and so on. The set of
properties on an OFX Image Effect host is found in the section
`Properties on the Image Effect Host <#ImageEffectHostProperties>`__

The ``fetchSuite`` function is how a plug-in gets a suite from the host.
It asks for a suite by giving the C string corresponding to that suite
and the version of that suite. The host will return a pointer to that
suite, or NULL if it does not support it. Please note that a suite
cannot be fetched until the very first action is called on the plug-in,
which is the load action.

.. doxygenstruct:: OfxHost
