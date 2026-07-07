# OFX Documentation

This is the documentation directory for OFX, the open visual effects standard.

## Documentation Architecture

The OpenFX documentation system combines several tools:

1. **Doxygen** - Parses C/C++ header files in the `include` directory to extract API documentation from comments
2. **Breathe** - A Sphinx extension that bridges Doxygen XML output with Sphinx
3. **Sphinx** - Processes ReStructured Text (.rst) files and Doxygen output to generate the final HTML documentation
4. **Python scripts** - `scripts/gen-props.py` generates C++ metadata headers; `scripts/gen-props-doc.py` generates RST reference pages from inline metadata in the headers

The documentation is organized into:
- **Reference manual** - API documentation generated from Doxygen comments in the header files
- **Guide** - Tutorial content with examples in ReStructured Text
- **Release notes** - Version-specific information

## Building Docs

Buildthedocs.io will auto-build whenever changes are pushed to master.
But to build the docs yourself, e.g. to check that your updates look
right, you can do your own doc build.

### Prerequisites

* Install doxygen (Linux: `sudo apt install doxygen`)
* Create a python3 virtualenv: `python -mvenv ofx-docgen` (may need to do `apt install python3.8-venv` first)
* Activate it: `source ofx-docgen/bin/activate`
* Install the python requirements in that virtualenv (e.g. Sphinx): `pip install -r Documentation/pipreq.txt`

(Virtualenv is recommended, but not required; you could install the reqs into your
system python if you like.)

### Build Process

The simplest way to build the documentation is to run the build script:

```bash
cd Documentation
./build.sh
```

This script performs the following steps:

1. Generates property and property-set RST reference pages from inline `@propdef`/`@propset` metadata using `scripts/gen-props-doc.py`
2. Builds Doxygen documentation from the header files (using `include/doxy-filter.sh` to strip metadata blocks)
3. Uses Breathe to collect Doxygen API docs
4. Builds the final HTML documentation with Sphinx

After building, you can view the documentation at:
`file:///path/to/your/ofx/openfx/Documentation/build/index.html`

If you want to run the steps manually, you can follow the steps in `build.sh`.

## Documentation Writing Guide

### Doxygen Documentation

Doxygen is used to document C/C++ code in the source and headers. The main Doxygen configuration is in `include/ofx.doxy`.

#### Key Doxygen Features Used in OpenFX

* **Groups** - Organize related items together
  ```c
  /** \defgroup PropertiesAll Property Suite */
  /** \ingroup PropertiesAll */
  /** \addtogroup PropertiesAll @{ ... @} */
  ```

* **Documentation Comments** - Document functions, structs, defines, etc.
  ```c
  /**
   * \brief Brief description
   *
   * Detailed description that can span
   * multiple lines
   *
   * \param paramName Description of parameter
   * \return Description of return value
   */
  ```

* **Cross-references** - Link to other documentation elements
  ```c
  /** See \ref kOfxImageEffectPropSupportedPixelDepths */
  ```

### ReStructured Text (RST) Documentation

RST files are used for the prose documentation in the `/Documentation/sources` directory.

#### Key RST Features Used in OpenFX

* **Section Headers** - Create hierarchy with underlines
  ```rst
  Section Title
  ============

  Subsection Title
  ---------------
  ```

* **Internal Links** - Create references between sections
  ```rst
  .. _target-name:

  Section Title
  ============

  See :ref:`target-name` for more information.
  ```

* **Code Blocks** - Display code examples
  ```rst
  .. code-block:: c

     #define kOfxImageEffectPluginRenderThreadSafety "OfxImageEffectPluginRenderThreadSafety"
  ```

* **Including Files** - Embed external file content
  ```rst
  .. literalinclude:: ../examples/basic.cpp
     :language: cpp
     :lines: 10-20
  ```

* **Doxygen Integration** - Include Doxygen-documented items
  ```rst
  .. doxygendefine:: kOfxImageEffectPropSupportsMultiResolution

  .. doxygenfunction:: OfxGetPropertySet

  .. doxygenstruct:: OfxRectD
     :members:
  ```

* **Cross-domain References** - Link to C/C++ elements
  ```rst
  :c:macro:`kOfxImageEffectPropSupportsOverlays`
  :cpp:class:`OfxImageEffectStruct`
  :c:struct:`OfxRectD`
  :c:func:`OfxGetPropertySet`
  ```

### Breathe Integration

The Breathe extension bridges Doxygen and Sphinx, enabling:

* Automatic generation of API documentation pages
* Full cross-referencing between RST and Doxygen content
* Consistent styling across all documentation

## Documentation Maintenance Tips

1. **Common Issues**
   - Missing cross-references - Check spelling of target names
   - Doxygen parse errors - Check comment formatting
   - Breathe integration issues - Check Doxygen XML output

2. **Adding New Documentation**
   - For new API elements: Add Doxygen comments to header files
   - For new concepts/guides: Create new RST files in `sources/Guide`
   - For property references: They're automatically generated from inline metadata in headers (see below)

3. **Testing Changes**
   - Always build documentation locally before submitting changes
   - Check for warning messages during the build process
   - Review the HTML output to ensure proper formatting and cross-references

## Property and Action Metadata

Properties, property sets, and actions are documented using inline YAML
metadata blocks embedded in doxygen comments in the C header files
(`include/ofx*.h`). These blocks are parsed by `scripts/gen-props.py`
(to generate C++ metadata headers) and `scripts/gen-props-doc.py` (to
generate RST reference pages). A doxygen input filter
(`include/doxy-filter.sh`) strips them from the doxygen output so they
don't appear in the API reference HTML.

### `@propdef` — Property Definitions

A `@propdef` block goes inside the `/** ... */` doxygen comment for a
property `#define`. It must appear at the **end** of the comment, just
before the closing `*/`. Everything before `@propdef` is treated as
the doxygen documentation; everything after it is parsed as YAML.

```c
/** @brief Unique name of the plug-in.

    - Type - C string X 1
    - Property Set - host descriptor (read only)
    - Valid Values - the unique name of the plug-in

    @propdef
    type: string
    dimension: 1
*/
#define kOfxPropName "OfxPropName"
```

Supported YAML fields:

| Field         | Description                                          | Example                              |
|---------------|------------------------------------------------------|--------------------------------------|
| `type`        | Property type                                        | `string`, `int`, `double`, `pointer`, `bool`, `enum` |
| `dimension`   | Number of values (integer or `N`)                    | `1`, `2`, `N`                        |
| `values`      | List of valid enum/string values                     | `["false", "true", "needed"]`        |
| `default`     | Default value                                        | `"false"`, `0`                       |
| `introduced`  | Version when this property was added                 | `"1.4"`                              |
| `deprecated`  | Version when this property was deprecated            | `"1.4"`                              |
| `cname`       | Override the C macro name (rare, for misnamed props) | `kOfxImageEffectFrameVarying`        |

### `@propset` — Property Set Definitions

A `@propset` block defines which properties belong to a named property
set (e.g., the set of properties on an image effect descriptor). These
are standalone `/** ... */` comments, not attached to a `#define`.

```c
/** @propset EffectDescriptor
    write: plugin
    props:
      - OfxPropLabel
      - OfxPropShortLabel
      - OfxPluginPropFilePath | write=host
      - OfxImageEffectPropCPURenderSupported | host_optional=true
*/
```

- `write:` sets the default write permission (`plugin` or `host`)
- Each property in `props:` can have pipe-separated modifiers:
  - `| write=host` — override the write permission for this property
  - `| host_optional=true` — mark as optional for hosts to support

### `@propsetdef` — Reusable Property Lists

When multiple property sets share a common subset of properties, use
`@propsetdef` to define the shared list, then reference it with `_REF`:

```c
/** @propsetdef ParamsCommon
    - OfxPropType
    - OfxPropName
    - OfxPropLabel
*/

/** @propset IntParam
    write: plugin
    props:
      - ParamsCommon_REF
      - OfxParamPropDefault
*/
```

`ParamsCommon_REF` expands to all the properties listed in the
`ParamsCommon` `@propsetdef` block.

### `@actiondef` — Action Argument Definitions

An `@actiondef` block lists the properties passed in `inArgs` and
`outArgs` for an action. Like `@propdef`, it goes at the **end** of
the doxygen comment, just before `*/`.

```c
/** @brief Called when something in the instance is changed.

    @param handle handle to the plug-in instance
    @param inArgs has the following properties
        - \ref kOfxPropType the type of the thing that changed
        - \ref kOfxPropName the name of the thing that changed
        - \ref kOfxPropChangeReason what triggered the change

    @actiondef
    inArgs:
      - OfxPropType
      - OfxPropName
      - OfxPropChangeReason
      - OfxPropTime
      - OfxImageEffectPropRenderScale
    outArgs:
*/
#define kOfxActionInstanceChanged "OfxActionInstanceChanged"
```

Property names in `inArgs`/`outArgs` use the string name (without the
`k` prefix), matching the `@propdef` key. Use an empty value or `[]`
for actions with no args on that side.

### Validation

Run `scripts/gen-props.py` to validate the metadata. It checks that:

- Every property with a `@propdef` is referenced in at least one
  `@propset` or `@actiondef`
- All property names referenced in `@propset` and `@actiondef` blocks
  have corresponding `@propdef` definitions
- No duplicate property or property set names exist
