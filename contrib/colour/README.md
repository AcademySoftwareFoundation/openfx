<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->
# ofxColourConvert — header-only colour conversion

`ofxColourConvert.h` is a small, dependency-free, header-only C++17 library
that converts RGB triplets between the OFX native colourspaces (see
[`include/ofxColour.h`](../../include/ofxColour.h)) and the ACES2065-1 (AP0)
reference colourspace.

It is a convenience for plug-in and host authors who want simple, exact colour
conversions without taking on a full [OpenColorIO](https://opencolorio.org/)
dependency. It is **not** a replacement for OCIO.

## Scope

Every supported space reduces to:

```
code value --(transfer function)--> linear RGB --(3x3 matrix)--> AP0
```

and back, so each conversion is colorimetrically exact and round-trips to
ACES2065-1. The gamut matrices are derived at compile time from the published
primaries using the Normalized Primaries Matrix plus a von Kries chromatic
adaptation, matching the OCIO ACES config the OFX native colourspaces are based
on.

Covered: the scene-referred working spaces (ACES2065-1, ACEScg, ACEScc/cct,
linear Rec.709/Rec.2020/P3-D65) and the camera/texture encodings (ARRI, Sony,
Canon, Panasonic, RED, Blackmagic, DaVinci, sRGB and pure-gamma texture
spaces).

Out of scope (these need OCIO): display rendering (the ACES Output Transform /
RRT+ODT), the display-referred `*_display` spaces, the ADX film-density
encodings, and the abstract "basic" `ofx_*` spaces.

## Usage

It is header-only — just include it (C++17 or later required):

```cpp
#include "ofxColourConvert.h"

using namespace ofx::colour;
RGB c    = { 0.2, 0.5, 0.8 };                      // an S-Log3/S-Gamut3 pixel
RGB aces = toACES2065_1(c, Colourspace::slog3_sgamut3);
RGB out  = fromACES2065_1(aces, Colourspace::lin_rec709_srgb);
// or directly:
RGB out2 = convert(c, Colourspace::slog3_sgamut3, Colourspace::lin_rec709_srgb);
```

`colourspaceFromName()` maps the OFX colourspace identifier strings (e.g.
`"slog3_sgamut3"`) to the `Colourspace` enum.

### CMake

The header is exposed as an `INTERFACE` target:

```cmake
target_link_libraries(my_plugin PRIVATE OpenFX::ColourConvert)
```

### Conan

The OpenFX package exposes it as the `ColourConvert` component
(`openfx::ColourConvert`), header-only.

## Tests

The [`tests/`](tests) directory validates the header against OpenColorIO in
both directions for every supported colourspace. They are off by default
(OCIO is heavy to build); enable with `-DOFX_BUILD_COLOUR_TESTS=ON`. See
[`tests/README.md`](tests/README.md).
