.. SPDX-License-Identifier: CC-BY-4.0
.. _OfxPlugin:

The OfxPlugin Struct
====================

This structure is returned by a plugin to identify itself to the host.

    ::

        typedef struct OfxPlugin {
           const char        *pluginApi;
           int                apiVersion;
           const char        *pluginIdentifier; 
           unsigned int       pluginVersionMajor;
           unsigned int       pluginVersionMinor;
           void               (*setHost)(OfxHost *host);
           OfxPluginEntryPoint *mainEntry;
        } OfxPlugin;

pluginApi
    This C string tells the host what API the plug-in implements.

apiVersion
    This integer tells the host which version of its API the plug-in
    implements.

pluginIdentifier
    This is the globally unique name for the plug-in.

pluginVersionMajor
    Major version of this plug-in, this gets incremented whenever
    software is changed and breaks backwards compatibility.

pluginVersionMinor
    Minor version of this plug-in, this gets incremented when software
    is changed, but does not break backwards compatibility.

setHost
    Function used to set the host pointer (see below) which allows the
    plug-in to fetch suites associated with the API it implements.

mainEntry
    The plug-in function that takes messages from the host telling it to
    do things.

Interpreting the OfxPlugin Struct
---------------------------------

When a host gets a pointer back from OfxGetPlugin, it examines the
string pluginApi. This identifies what kind of plug-in it is. Currently
there is only one publicly specified API that uses the OFX mechanism,
this is ``"OfxImageEffectPluginAPI"``, which is the image effect API
being discussed by this book. More APIs may be created at a future date,
for example "OfxImageImportPluginAPI". Knowing the type of plug-in, the
host then knows what suites and host handles are required for that
plug-in and what functions the plug-in itself will have. The host passes
a OfxHost structure appropriate to that plug-in via its ``setHost``
function. This allows for the same basic architecture to support
different plug-in types trivially.

OFX explicitly versions plug-in APIs. By examining the apiVersion, the
host knows exactly what set of functions the plug-in is going to supply
and what version of what suites it will need to provide. This also
allows plug-ins to implement several versions of themselves in the same
binary, so it can take advantages of new features in a V2 API, but
present a V1 plug-in to older hosts that only support V1.

If a host does not support the given plug-in type, or it does not
support the given version it should simply ignore that plug-in.

A plug-in needs to uniquely identify itself to a host. This is the job
of pluginIdentifier. This null terminated ASCII C string should be
unique among all plug-ins, it is not necessarily meant to convey a
sensible name to an end user. The recommended format is the reverse
domain name format of the developer, for example "uk.co.thefoundry",
followed by the developer's unique name for the plug-in. e.g.
"uk.co.thefoundry.F_Kronos".

A plug-in (as opposed to the API it implements) is versioned with two
separate integers in the OfxPlugin struct. They serve two separate
functions and are,

-  pluginVersionMajor
   flags the functionality contained within a plug-in. Incrementing this
   number means that you have broken backwards compatibility of the
   plug-in. More specifically, this means a setup from an earlier
   version, when loaded into this version, will not yield the same
   result.
-  pluginVersionMinor
   flags the release of a plug-in that does not break backwards
   compatibility, but otherwise enhances that plug-in. For example,
   increment this when you have fixed a bug or made it faster.

If a host encounters multiple versions of the same plug-in it should,

-  when creating a brand new instance, always use the version of a
   plug-in with the greatest major and minor version numbers,
-  when loading a setup, always use the plug-in with the major version
   that matches the setup, but has the greatest minor number.

As a more concrete example of versioning: the plug-in identified by
"org.wibble:Fred" is initially released as 1.0, However a few months
later, wibble.org figure out how to make it faster and release it as
1.1. A year later, Fred can now do automatically what a user once needed
to set up five parameters to do, thus making it much simpler to use.
However this breaks backwards compatibility as the effect can no longer
produce the same output as before, so wibble.org then release this as
v2.0.

A user's host might now have three versions of the Fred plug-in on it,
v1.0, v1.1 and v2.0. When creating a new instance of the plug-in, the
host should always use v2.0. When loading an old project which has a
setup from a v1.x plug-in, it should always use the latest, in this case
being v1.1.

Note that plug-ins can change the set of parameters between minor
version releases. If a plug-in does so, it should do so in a backwards
compatible manner, such that the default value of any new parameter
would yield the same results as previously. See the chapter below about
parameters.


.. doxygenstruct: OfxPlugin
