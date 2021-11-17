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
* Create a python3 virtualenv: `python -mvenv ofx-docgen`
* Activate it: `source ofx-docgen/bin/activate`
* Install the python requirements in that virtualenv: `pip install -r Documentation/pipreq.txt`

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
* Now open
  file:///path/to/your/ofx/openfx/Documentation/build/index.html in
  your browser; your changes should be there.
