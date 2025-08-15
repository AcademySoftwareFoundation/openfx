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

if command -v uvx > /dev/null 2>&1; then
    USE_UV=1
    SPHINX_BUILD="uv run --with-requirements pipreq.txt sphinx-build"
    UV_RUN="uv run"
else
    USE_UV=
    SPHINX_BUILD="sphinx-build"
    UV_RUN=""
fi

rm -rf build

# Generate references
EXPECTED_ERRS="unable to resolve reference|explicit link request|found in multiple"
$UV_RUN python genPropertiesReference.py \
       -i ../include -o sources/Reference/ofxPropertiesReference.rst -r \
       > /tmp/ofx-doc-build.out 2>&1
grep -v -E "$EXPECTED_ERRS" /tmp/ofx-doc-build.out || true

# Build the Doxygen docs
EXPECTED_ERRS="malformed hyperlink target|Duplicate explicit|Definition list ends|unable to resolve|could not be resolved"
cd ../include
doxygen ofx.doxy > /tmp/ofx-doc-build.out 2>&1
grep -v -E "$EXPECTED_ERRS" /tmp/ofx-doc-build.out || true
cd -

# Use breathe.apidoc to collect the Doxygen API docs
rm -rf sources/Reference/api
if [[ $USE_UV ]]; then
    $UV_RUN --with breathe python -m breathe.apidoc -p 'ofx_reference' -m --force -g class,interface,struct,union,file,namespace,group -o sources/Reference/api doxygen_build/xml
else
    python -m breathe.apidoc -p 'ofx_reference' -m --force -g class,interface,struct,union,file,namespace,group -o sources/Reference/api doxygen_build/xml
fi

# Build the Sphinx docs
EXPECTED_ERRS='Explicit markup ends without|Duplicate C.*declaration|Declaration is|cpp:func targets a member|undefined label'
$SPHINX_BUILD -b html sources build > /tmp/ofx-doc-build.out 2>&1
grep -v -E "$EXPECTED_ERRS" /tmp/ofx-doc-build.out || true

echo "Documentation build complete."
echo "Open file:///$PWD/build/index.html in your browser"
echo "to check."

# end of build.sh
