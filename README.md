OpenFX effects API [![Build Status](https://api.travis-ci.org/devernay/openfx.png?branch=master)](https://travis-ci.org/devernay/openfx) [![Coverage Status](https://coveralls.io/repos/devernay/openfx/badge.png?branch=master)](https://coveralls.io/r/devernay/openfx?branch=master) [![Coverity Scan Build Status](https://scan.coverity.com/projects/2941/badge.svg)](https://scan.coverity.com/projects/2941 "Coverity Badge")
==================

A fork from the official openfx repository https://github.com/ofxa/openfx with bug fixes and enhancements.

Things that need to be clarified in the OpenFX 1.4 spec
-------------------------------------------------------

### Default value for output components?

When a plugin accepts different components for input and output, e.g.:
```
    ClipDescriptor *srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(ePixelComponentRGBA);
    srcClip->addSupportedComponent(ePixelComponentAlpha);

    ClipDescriptor *dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(ePixelComponentRGBA);
    dstClip->addSupportedComponent(ePixelComponentAlpha);
```
Does the host have to set the dstClip components by default to the components of the srcClip, or does the plug-in have to explicitely set these, using for example:
```
void
MyPlugin::getClipPreferences(OFX::ClipPreferencesSetter &clipPreferences)
{
    clipPreferences.setClipComponents(*_dstClip, _srcClip->getPixelComponents());
}
```

### OpenGL render support missing documentation

- when entering the render action, is the output framebuffer bound so that drawing is done on output image ?
- may OpenGL rendering also support tiles ? In this case what coordinate system should be used to draw - relative to bottomleft of renderWindow, or canonical ?

Things that need to be added to OpenFX
-------------------------------------------------------

### Plug-in and host license properties

The plugin and the host license may be incompatible, e.g. if one is GPL and the other is not GPL-compatible:
- http://www.gnu.org/licenses/gpl-faq.en.html#GPLAndPlugins
- http://www.gnu.org/licenses/gpl-faq.en.html#GPLPluginsInNF

We should have a host and instance descriptor property (kOfxPropLicense ?) that holds a string containing a standardized acronym of the license, e.g. "GPL-2.0", "BSD-3-Clause". For example, all plugins in the official OpenFX examples are "BSD-3-Clause", which is GPL-compatible. The default value should be "Commercial".

For a list of license acronyms, see http://opensource.org/licenses/alphabetical

### Passing host data as an opaque pointer to all suite functions

All suite functions should take a void* as first argument, so that the suite function, as implemented in the host, may figure out in which context the action was called (time, view, plane, etc.). This pointer should be passed in the inArgs for all actions. This requires upgrading all suites.

Possible OpenFX extensions
--------------------------

The following OpenFX extensions are not mentioned in the draft proposals on http://openeffects.org

### Ability to fetch images at a given render scale

### Sound effects suite
