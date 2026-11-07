<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->
# ofxColourConvert.h tests

Unit tests for [`../ofxColourConvert.h`](../ofxColourConvert.h),
validated against **OpenColorIO** (the reference implementation).

For every colourspace the header supports, the suite runs sample pixels
through both OCIO's built-in studio config and the header — in both
directions (to and from ACES2065-1) — and requires them to agree. It also
checks round-trip self-consistency and the OFX-name lookup helper.

The test is built and run by [pcons](https://github.com/garyo/pcons), which
fetches its two dependencies — OpenColorIO and GoogleTest — with Conan from
[`conanfile.txt`](conanfile.txt). The first run compiles OpenColorIO and its
dependencies from source, which takes a while; subsequent runs are cached.
Requirements: a C++17 compiler, `conan` 2.x on `PATH`, and `uv` (which runs
pcons with zero install via `uvx`).

## Running directly (pcons)

```sh
cd contrib/colour/tests
uvx pcons             # conan install (first run, slow) + build the test program
uvx pcons test        # build and run the tests
uvx pcons test --list # list the tests without running them
```

## Running via CTest

The main CMake build registers this suite when `-DOFX_BUILD_COLOUR_TESTS=ON`;
the CTest entry simply delegates to `uvx pcons test`. From the repo root, after
configuring the project:

```sh
cmake -B build -S . -DOFX_BUILD_COLOUR_TESTS=ON   # (with the usual Conan flow)
ctest --test-dir build -R colour_convert --output-on-failure
```

## What's covered

- **`ToACES2065_1MatchesOCIO`** / **`FromACES2065_1MatchesOCIO`** — every
  supported scene-referred and camera/texture colourspace, compared to OCIO
  in both directions.
- **`SelfConsistency`** — `space -> AP0 -> space` round-trips to 1e-9, and
  ACES2065-1 is the identity.
- **`NameLookup`** — `colourspaceFromName()` maps the OFX identifier strings.

### Tolerances

Spaces that reduce to a matrix plus an analytic curve match OCIO's analytic
built-in transforms to float round-off (combined absolute + relative
tolerance). A few are evaluated through interpolated LUTs or baked CLF
matrices in this OCIO config — ACEScc and the Canon logs are LUTs, and the
Blackmagic/DaVinci transforms come from CLF files rather than analytic
built-ins — so those use a looser tolerance. The header's analytic versions
are in fact more accurate; the test only asserts that they track OCIO.

The AP0 → space direction uses near-neutral, in-gamut samples: a saturated
AP0 colour can map to negative linear RGB in a smaller gamut (e.g. Rec.709),
and the *encode* of a negative value is undefined and handled differently by
every implementation. Saturated values are still exercised by the round-trip
test.

## Notes

- OCIO version is pinned to `2.3.2` in `conanfile.txt` to match the config the
  OFX native colourspaces are derived from (`ofx-native-v1.5_aces-v1.3_ocio-v2.3`).
- The test resolves the built-in config
  `studio-config-v2.1.0_aces-v1.3_ocio-v2.3`, falling back to
  `studio-config-latest`. A colourspace absent from the linked OCIO version is
  reported as skipped rather than failed.
