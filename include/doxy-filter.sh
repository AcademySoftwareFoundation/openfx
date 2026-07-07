#!/bin/bash
# Doxygen input filter: strip @propdef and @actiondef YAML blocks
# from doxygen comments. These blocks are parsed by gen-props.py
# but should not appear in the generated API documentation.
sed '/@propdef/,/\*\//{ /@propdef/d; /\*\//!d; }; /@actiondef/,/\*\//{ /@actiondef/d; /\*\//!d; }' "$1"
