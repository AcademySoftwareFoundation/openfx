# OFX Documentation

This is the documentation directory for OFX, the open visual effects standard.

# Building Docs

Buildthedocs.io will auto-build whenever changes are pushed to master.
But to build the docs yourself, e.g. to check that your updates look
right, you can do your own doc build.

Right now building the docs is somewhat manual. The below assumes
Linux, but Mac is similar.

## Prerequisites

* Install doxygen (Linux: `sudo apt install doxygen`)
* Create a python3 virtualenv: `python -mvenv ofx-docgen` (may need to do `apt install python3.8-venv` first)
* Activate it: `source ofx-docgen/bin/activate`
* Install the python requirements in that virtualenv (e.g. Sphinx): `pip install -r Documentation/pipreq.txt`

(Virtualenv is recommended, but not required; you could install the reqs into your
system python if you like.)

## Build:

* Make sure your virtualenv above is activated: `source ofx-docgen/bin/activate`
* Generate references:
  `python Documentation/genPropertiesReference.py -i include -o Documentation/sources/Reference/ofxPropertiesReference.rst -r`
* Build the doxygen docs: `cd include; doxygen ofx.doxy; cd -` (you'll see
  some warnings)
* Build the sphinx docs:
  `cd Documentation; sphinx-build -b html sources build`
* Note that you can do all the above using the build script in `Documentation/build.sh`.
* Now open
  file:///path/to/your/ofx/openfx/Documentation/build/index.html in
  your browser; your changes should be there.

# Doxygen notes:

Doxygen is used in the source and headers. The doc build process
parses the doxygen comments to build docs, then breathe to merge them
with the `.rst` sphinx docs. See the [Doxygen docs](https://www.doxygen.nl/manual/docblocks.html)

* Params/Actions/etc. should be added to groups using `\defgroup`, `\ingroup` and `\addtogroup`. Use `@{` / `@}` to add multiple items.
* Use `\ref` to refer to entities in doxygen.


# RST Notes:

RST (ReStructured Text) is used for the prose documentation in the `/Documentation` subtree, using Sphinx and Breathe.

* Internal links:
  - Define: `.. _target-name:` (must be *globally unique*!)
  - Reference: ``:ref:`target-name` ``
  - Good to put these just before section headers; the link will refer to the header in that case.
  - See [Sphinx Docs](https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html#role-ref)
  - Links to structs/etc.:
    - ``:ref:`OfxHost<OfxHost>` ``
  - Links to documents:
    - ``:doc:`docname` `` (if docname starts with `/` it's absolute, otherwise rel to current file)
  - C macros: `` :c:macro:`kOfxParamPropChoiceOrder` `` (struct/var/func/member/enum/type/... work too), 
    see the [Doc](https://www.sphinx-doc.org/en/master/usage/domains/c.html#c-roles)
  - You can also reference `#define`s using this syntax: ``.. doxygendefine:: kOfxImageEffectRenderUnsafe`` on its own line,
    which pulls in the whole doxygen block for that `#define`.
* Useful macros:
  - `` .. literalinclude:: README.txt ``

