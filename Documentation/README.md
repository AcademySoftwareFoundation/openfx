# OFX Documentation

This is the documentation directory for OFX, the open visual effects standard.

## Documentation Architecture

The OpenFX documentation system combines several tools:

1. **Doxygen** - Parses C/C++ header files in the `include` directory to extract API documentation from comments
2. **Breathe** - A Sphinx extension that bridges Doxygen XML output with Sphinx
3. **Sphinx** - Processes ReStructured Text (.rst) files and Doxygen output to generate the final HTML documentation
4. **Python scripts** - Custom scripts like `genPropertiesReference.py` extract property definitions from source files

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

1. Generates property references using `genPropertiesReference.py`
2. Builds Doxygen documentation from the header files
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
   - For property references: They're automatically generated from headers

3. **Property Documentation**
   - Property definitions are extracted automatically by `genPropertiesReference.py`
   - Format should be: `#define kOfxPropName "OfxPropName"`
   - Add Doxygen comments above property definitions

4. **Testing Changes**
   - Always build documentation locally before submitting changes
   - Check for warning messages during the build process
   - Review the HTML output to ensure proper formatting and cross-references