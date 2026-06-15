<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->

# OpenFX Security Policy

## Reporting a Vulnerability

If you think you've found a potential vulnerability in OpenFX, please
report it privately so that we can investigate and prepare a fix before
the issue is publicly disclosed. **Do not** open a public GitHub issue,
post to the mailing list, or otherwise disclose the problem publicly
before it has been addressed.

Please report vulnerabilities through GitHub's private security advisory
system, which lets you disclose the issue confidentially to the OpenFX
maintainers:

https://github.com/AcademySoftwareFoundation/openfx/security/advisories/new

Please include as much detail as you can:

* The version, branch, or commit of OpenFX affected.
* The component involved (API headers, the C++ support library, the
  example plug-ins, or the build/release tooling).
* A description of the vulnerability and its potential impact.
* Steps to reproduce, ideally with a minimal proof of concept.

We will acknowledge receipt of your report within 5 business days and
will work with you to understand and resolve the issue promptly. Once a
fix is available we will coordinate disclosure and credit you for the
report unless you prefer to remain anonymous.

## Supported Versions

OpenFX is a standard (a set of C API headers) accompanied by a C++
support library and example plug-ins. Fixes are applied as follows:

| Version       | Supported                                   |
|---------------|---------------------------------------------|
| `main` branch | Yes — all fixes land here first             |
| 1.4.x         | Yes — security fixes backported when feasible |
| 1.3.x and earlier | No longer receiving patches             |

## Security Model and Expectations

OpenFX defines a binary interface between *host* applications (such as
compositors and editors) and *plug-ins* that provide image-processing
effects. Understanding this trust boundary is important when reasoning
about security:

* **Plug-ins are native code that runs inside the host process.** A host
  loads OFX plug-ins as dynamic libraries/bundles, and a loaded plug-in
  has the same privileges as the host. Installing and loading a plug-in
  is therefore equivalent to running arbitrary native code; hosts and
  users should only load plug-ins from trusted sources. This is an
  inherent property of the plug-in model, not a defect in OpenFX.

* **Hosts and plug-ins should treat each other defensively.** Because
  the API is a boundary between independently developed components,
  both sides should validate the property values, image data, and
  pointers they receive across the suite interfaces rather than
  assuming the other side is well-behaved.

The code maintained in this repository that is most relevant to
security is:

* The **API header files** in `include/` — the definitions of the
  suites and properties.
* The **C++ support library** in `Support/` — helper code that hosts
  and plug-in authors may compile into their own products. Memory
  handling, bounds checking, and parsing of host-supplied values in
  this library are the primary in-repo attack surface.
* The **example plug-ins** — provided for illustration and not intended
  for production use.

Issues in third-party host applications or commercial plug-ins are out
of scope for this repository and should be reported to their respective
vendors.

## Known Vulnerabilities

There are no known unpatched security vulnerabilities in OpenFX at this
time.
