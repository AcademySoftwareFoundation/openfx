<!-- SPDX-License-Identifier: CC-BY-4.0 -->
![Build](https://github.com/AcademySoftwareFoundation/openfx/actions/workflows/build.yml/badge.svg)

# OpenFX image processing plug-in standard

The authoritative source for information about OFX is http://openeffects.org/

* [OpenFX Build Instructions](https://github.com/AcademySoftwareFoundation/openfx/blob/master/install.md)
* [OpenFX Documentation](https://openfx.readthedocs.io/en/latest) - start here
* [OpenFX Documentation: Reference](https://openfx.readthedocs.io/en/latest/Reference)
* [Programming Guide By Example](https://openfx.readthedocs.io/en/latest/Guide)
* [OpenFX Wiki](https://wiki.aswf.io/pages/viewpage.action?pageId=49844871) 

Here are some [Ways to get involved](https://tac.aswf.io/engagement/#OpenFX) with OpenFX.

## Why a Standard?
VFX plug-in vendors were frustrated for years because host application vendors created proprietary plug-in interfaces. As a result, each plug-in vendor had to port their plug-ins to all the different hosts and hosts couldn't use each other's plug-ins, limiting the selection of effects available to artists. The need for a standard interface was clear, so Bruno Nicoletti of The Foundry led the effort to develop a standard. That standard is OFX.

OFX is a win for artists because there is no waiting for plug-in vendors to port their cool effects to your application. Once a host compositing or editing application adopts OFX, all OFX plug-ins on the market instantly become available on that host.

And OFX is a win for plug-in vendors because they can concentrate on what they do best: making cool effects

## OFX Terminology
### Host
A video compositing or editing application, such as The Foundry Nuke, Assimilate Scratch, Sony Vegas, or FilmLight Baselight
### Plug-in
Video software, such as GenArts Sapphire or RE:Vison Effects which adds a wider variety of effects to a host application.
### Open Effects
A standardized software interface between VFX host applications and plug-ins (also known as OpenFX and OFX).
### Editor
An application which allows you to manipulate a video timeline by adding, removing, and changing the in and out points of video clips. Effects, Generators, Transition, Compositors and Retiming effects are commonly used in editors.
### Compositor
An application which allows you build a video clip by layering video clips, still images, and effects.

## Contributing

Please read the [Contribution Guidelines](https://github.com/ofxa/openfx/wiki/Extending-OpenFX-Guidelines) for how to submit pull requests for fixes and changes to the standard.

# Building Libs and Plugins

You can build the examples, support lib, and host support lib using Conan and CMake.

On all OSes (even Windows with Mingw), you should be able to use `scripts/build-cmake.sh`. For more details, see [install.md](install.md).

# Building Docs

See instructions in [Documentation/README.md](Documentation/README.md).

# Producing a Release

## Pre-release

* Update the release notes and documentation and version number
  * Update [release-notes.md](./release-notes.md) and [release-notes-next.md](./release-notes-next.md)
  * Update [include/ofx.doxy](include/ofx.doxy)
  * Update [Documentation/sources/conf.py](Documentation/sources/conf.py)
* Tag (locally) the desired version with e.g. `OFX_Release_1.x_pre_1`
   - use `git tag -a -s` to sign with the release gpg key
* Push that tag to github, and email everyone to test that tagged build.

## Release

* Tag (locally) the desired version with e.g. `OFX_Release_1.x`
   - use `git tag -a -s` to sign with the release gpg key
* Push that tag to github, then create the release on github from that tag.
* Publish the release on github; that will run the release publish workflow, creating and uploading the sigstore-signed artifacts.

# Releases

Release bundles are named like `openfx-<OS>-release-<REL>.tar.gz` and `openfx_plugins-<OS>-release-<REL>.tar.gz`.
The `openfx-*` bundles contain all the header files as well as the support libs. They look like this:

```
OpenFX
├── include
│  └── openfx
│     ├── ofxCore.h...
│     ├── HostSupport/*.h
│     └── Support/*.h
└── lib
   ├── lib*
```

so you can add compiler/linker options `-I.../OpenFX/include` `-LOpenFX/lib` and then in source files `#include "openfx/ofxCore.h"` etc.

The `openfx-plugins-*` bundles contain all the sample plugins for the OS. Copy these into your [plugin install dir](https://openfx.readthedocs.io/en/latest/Reference/ofxPackaging.html#installation-directory-hierarchy) and they should show up in your host application.

## Verifying Release Signatures

We use [`sigstore`](https://github.com/marketplace/actions/gh-action-sigstore-python) to sign our github releases. 
Release signatures are created using short-lived certificates, and audit trails are stored online using `rekor.sigstore.com`. 
To verify a release artifact (`.tar.gz` file), download its associated `.tar.gz.sigstore.json`, and then use [`cosign`](https://docs.sigstore.dev/cosign/system_config/installation/) to verify the signature like this:
```
cosign verify-blob \
  openfx-mac-release-x.y.tar.gz \
  --bundle openfx-mac-release-x.y.tar.gz.sigstore.json \
  --new-bundle-format \
  --certificate-identity-regexp='https://github.com/AcademySoftwareFoundation/openfx/.*' \ --certificate-oidc-issuer='https://token.actions.githubusercontent.com'
```
