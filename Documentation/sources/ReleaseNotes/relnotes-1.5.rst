.. SPDX-License-Identifier: CC-BY-4.0
OpenFX Release Notes for V1.5
=============================

Release Date: TBD

This will be version 1.5 of the OpenFX API. Significant additions
support for Metal, CUDA and OpenCL rendering (including half-float bit
depths), and an OfxDrawSuite for drawing overlays without requiring
OpenGL. It also includes a new PropChoiceValue string-valued param
type and the ability to reorder PropChoice options in new versions of
a plugin while retaining back compatibility.

This is the first release produced by new OpenFX project within the
`Academy Software Foundation <https://www.aswf.io/>`_.

The build process is updated to use CMake and Conan, docs are
available now on `ReadTheDocs <https://openfx.readthedocs.io/en/main/>`_, and we have a
new website at https://openeffects.org. Builds of the headers and
support libs and plugins are now automatically produced on all changes
to `main` on our
`Github <https://github.com/AcademySoftwareFoundation/openfx>`_.

GPU Rendering
-------------

The GPU Rendering Suite now supports Metal, CUDA, and OpenCL (Images and Buffers).
See :ref:`gpu-rendering` and source file `include/ofxGPURender.h` for details.

New Suites
----------

- :c:struct:`OfxDrawSuiteV1`


New Suite Versions
------------------

None

New Actions
-----------

- :c:func:`OfxSetHost` -- called first by the host, if it exists

New Properties
--------------

String Choice params:

- :c:macro:`kOfxParamTypeStrChoice` -- string-valued choice (menu) parameter
- :c:macro:`kOfxParamPropChoiceOrder` -- specifies the order in which ChoiceParam options are presented

GPU rendering:

- :c:macro:`kOfxImageEffectPropMetalRenderSupported`
- :c:macro:`kOfxImageEffectPropCudaRenderSupported`
- :c:macro:`kOfxImageEffectPropOpenCLRenderSupported`
- :c:macro:`kOfxImageEffectPropMetalEnabled`
- :c:macro:`kOfxImageEffectPropCudaEnabled`
- :c:macro:`kOfxImageEffectPropOpenCLEnabled`

  etc.

Deprecations
------------

None

Removals
--------

None
