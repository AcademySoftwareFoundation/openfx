<!--
This is a temporary space to hold changes and release notes for the next release.
When preparing a release, copy this into the release-notes.md and reset this to empty.

-->

# Release Notes - NEXT (upcoming)

This is version NEXT of the OpenFX API.

## Key Features of OpenFX Version NEXT:

- **Unlicensed-render behaviour**: Added `kOfxImageEffectPropBehaviourWhenUnlicensed` so a host can tell plugins whether to fail a render or render anyway (e.g. watermarked) when unlicensed (issue #202).
- **Parameter interpolation types**: Added `kOfxParamInterpType` and related definitions documenting standard keyframe interpolation modes (issue #116).
- **Obsolete plugins**: Added `kOfxImageEffectPluginPropObsolete` so a plugin bundle can mark a plugin as obsolete: available for use in old projects but not offered to users for new use (issue #221).
- **Windows ARM64 packaging**: Defined plugin install locations for Windows on ARM, including the new normative `Win-arm64ec` folder for Arm64EC/Arm64X plug-ins, with most-specific-first DLL search order (issue #160).
- **Colour-Managed Colour Params**: Added `kOfxParamPropColourManagement`. Lets a plugin declare the colourspace of an RGB/RGBA parameter's values (including its default): `Managed` (ACES2065-1, scene-linear, unclamped) for processing colours, or `SRGB` (sRGB, [0..1]) for display-referred UI colours such as Draw Suite overlays, so a host can interpret and convert them correctly (issue #158).

## Behaviour Changes

Changes that existing hosts, plugins or SDK users may need to act on:

- **Project-load semantics**: Hosts are now required to send the `instanceChanged` action with `kOfxPropChangeReason` = `kOfxChangePluginEdited` when a clip or parameter was changed while loading a project (issue #184).
- **Windows ARM64 plugin search**: An Arm64EC host should load plug-ins from the new `Win-arm64ec` folder. An x64 host should detect when it is running under Arm64 emulation and then also check `Win-arm64ec`, preferring a native Arm64EC plug-in there over the x64 plug-in in `Win64`. Plug-in installers should put Arm64EC (or Arm64X) binaries in `Win-arm64ec` and keep x64 binaries in `Win64` (issue #160).
- **HostSupport descriptor inheritance**: An effect instance now inherits `kOfxImageEffectPropSupportsTiles` and the GPU `*RenderSupported` properties from the plugin descriptor instead of overriding them with a hard default, so values set only in describe are honoured (issue #177). The header docs now state this inheritance rule for hosts.
- **Conan package layout**: Restructured the recipe to the standard Conan Center Index layout (headers under `include/`, libs and CMake module under `lib/`, licenses under `licenses/`) (issues #238, #246), and example-only dependencies (OpenGL, CImg, spdlog, OpenCL) are no longer imposed on consumers — they're gated behind a new `build_examples` option (#253).

## Fixes in OpenFX Version NEXT:

- Fixed incorrect enum value names in property metadata (`@propdef`) for several properties, and made the generator reject unknown enum names so this can't regress (issue #247).
- Set proper RGBA colour defaults on the colour parameter in the Rectangle example (issue #240).
- Fixed the ColourSpace example to compile under `FMT_ENFORCE_COMPILE_STRING`, with a CI job to keep it that way (issue #236).
- CMake: use `target_compile_features(cxx_std_17)` instead of forcing `CMAKE_CXX_STANDARD`, so consumers can build with a later C++ standard (#234).
- `OfxExport` now marks a symbol visible on GCC and Clang as well as exporting it on Windows. The entry points in `ofxCore.h` are declared with it, so plugins built with hidden visibility export `OfxGetPlugin`, `OfxGetNumberOfPlugins` and `OfxSetHost` definitions properly.  The examples no longer need to define `EXPORT` macros.
- Fixed the ColourSpace example's `OfxSetHost` to have the proper signature so it actually gets called.
- Fixed the `@propdef` metadata of `kOfxParamPropChoiceEnum` (a string array, not a bool) and `kOfxParamPropDimensionLabel` (one label per dimension, not one).
- Added missing metadata: the `outArgs` of `kOfxImageEffectActionIsIdentity`, and a property set for the OpenGL texture handle returned by `clipLoadTexture` (#263, #276).
- Fixed the Invert example never releasing its output image (a shadowed handle variable).
- Fixed the Support Noise example giving a different image when a frame is rendered in tiles: it seeded its noise per render call, and now each pixel's noise depends only on its position, the time and the noise level.
- Fixed the Support GPUGain example failing to render when the host chose Alpha for its output: it declared Alpha output but processes only RGBA, so its output clip now supports only RGBA.
- Fixed the Support Gamma example producing NaNs from negative float input: it now applies the gamma to a value's magnitude and keeps its sign.
- Fixed the Support Retimer example asking for source frames around the output time rather than the retimed source time it then fetches.

## Documentation Improvements

- Removed the outdated "Properties by object reference" page; the generated Property Sets reference replaces it (issue #173).
- Moved the 1.1 to 1.2 API changes chapter from the Reference section to the Release Notes section of the documentation.
- Fixed broken links to the Programming Guide by Example (issue #229) and typos in the `ofxGPURender.h` and `ofxParam.h` comments (issues #42, #44).

## Deprecations

## Detailed List of Changes

- Property metadata now lives in inline `@propdef` blocks in the headers (previously a separate YAML file); `scripts/gen-props.py` generates the reference documentation and the `openfx-cpp` metadata headers from it (#233).
- Added `SECURITY.md` and fixed stale repository URLs (#242).
- CI: hardened workflows (actions pinned to SHAs, untrusted inputs via env) (#235); updated Conan and pre-authorized future compiler versions so new Xcode/compiler releases don't break builds (#252); pinned the Windows CUDA job to VS2022.
- CI: the CentOS 7 jobs (VFX CY2021 and CY2022) run GitHub's JavaScript actions on a glibc 2.17 build of Node 24, since GitHub's runners no longer have Node 20.

