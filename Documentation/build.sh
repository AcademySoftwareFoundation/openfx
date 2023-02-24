#! /bin/bash

# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause

#########################
# Build OFX documentation
#########################

set -e
trap 'catch $? $LINENO' err

catch() {
    echo "ERROR code $1 on line $2"
    exit 1
}

# Check for prereqs

if [ ! -f genPropertiesReference.py ] ; then
    echo "This script must be run in the Documentation directory."
    exit 1
fi

if ! command -v sphinx-build > /dev/null 2>&1 ; then
    echo "Python can't import required modules; did you set up the prereqs?"
    echo "Check the README.md."
    exit 1
fi

rm -rf build

# Generate references
EXPECTED_ERRS="unable to resolve reference|explicit link request|found in multiple"
python genPropertiesReference.py \
       -i ../include -o sources/Reference/ofxPropertiesReference.rst -r \
       > /tmp/ofx-doc-build.out 2>&1
egrep -v "$EXPECTED_ERRS" /tmp/ofx-doc-build.out || true

# Build the Doxygen docs
EXPECTED_ERRS="malformed hyperlink target|Duplicate explicit|Definition list ends|unable to resolve|could not be resolved"
cd ../include
doxygen ofx.doxy > /tmp/ofx-doc-build.out 2>&1
egrep -v "$EXPECTED_ERRS" /tmp/ofx-doc-build.out || true
cd -

# Build the Sphinx docs
EXPECTED_ERRS="Explicit markup ends without|Duplicate declaration|cpp:func targets a member|undefined label"
sphinx-build -b html sources build > /tmp/ofx-doc-build.out 2>&1
egrep -v "$EXPECTED_ERRS" /tmp/ofx-doc-build.out || true

echo "Documentation build complete."
echo "Open file:///$PWD/build/index.html in your browser"
echo "to check."

# end of build.sh
