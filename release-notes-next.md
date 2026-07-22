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
- **Project-load semantics**: Hosts are now required to send the `instanceChanged` action with `kOfxPropChangeReason` = `kOfxChangePluginEdited` when a clip or parameter was changed while loading a project (issue #184).

## Fixes in OpenFX Version NEXT:

- Fixed incorrect enum value names in property metadata (`@propdef`) for several properties, and made the generator reject unknown enum names so this can't regress (issue #247).
- Set proper RGBA colour defaults on the colour parameter in the Rectangle example (issue #240).
- Fixed the ColourSpace example to compile under `FMT_ENFORCE_COMPILE_STRING`, with a CI job to keep it that way (issue #236).
- CMake: use `target_compile_features(cxx_std_17)` instead of forcing `CMAKE_CXX_STANDARD`, so consumers can build with a later C++ standard (issue #208).

## Deprecations

## Detailed List of Changes

- Property metadata now lives in inline `@propdef` blocks in the headers (previously a separate YAML file); `scripts/gen-props.py` generates the reference documentation and the `openfx-cpp` metadata headers from it (#233).
- Conan packaging: restructured the recipe to the standard Conan Center Index layout (headers under `include/`, libs and CMake module under `lib/`, licenses under `licenses/`) (issues #238, #246), and example-only dependencies (OpenGL, CImg, spdlog, OpenCL) are no longer imposed on consumers — they're gated behind a new `build_examples` option (#253).
- Added `SECURITY.md` and fixed stale repository URLs (#242).
- CI: hardened workflows (actions pinned to SHAs, untrusted inputs via env) (#235); updated Conan and pre-authorized future compiler versions so new Xcode/compiler releases don't break builds (#252); pinned the Windows CUDA job to VS2022.

