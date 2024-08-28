# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause

import os
import difflib
import argparse
import yaml
import logging
from pathlib import Path
from collections.abc import Iterable

# Set up basic configuration for logging
logging.basicConfig(
    level=logging.INFO,  # Log level: DEBUG, INFO, WARNING, ERROR, CRITICAL
    format='%(levelname)s: %(message)s',  # Format of the log messages
    datefmt='%Y-%m-%d %H:%M:%S'  # Date format
)

def getPropertiesFromFile(path):
    """Get all OpenFX property definitions from C header file.

    Uses a heuristic to identify property #define lines:
    anything starting with '#define' and containing 'Prop' in the name.
    """
    props = set()
    with open(path) as f:
        try:
            lines = f.readlines()
        except UnicodeDecodeError as e:
            logging.error(f'error reading {path}: {e}')
            raise e
        for l in lines:
            # Detect lines that correspond to a property definition, e.g:
            # #define kOfxPropLala "OfxPropLala"
            splits=l.split()
            if len(splits) < 3:
                continue
            if splits[0] != '#define':
                continue
            # ignore these
            nonProperties = ('kOfxPropertySuite',
                             # prop values, not props
                             'kOfxImageEffectPropColourManagementNone',
                             'kOfxImageEffectPropColourManagementBasic',
                             'kOfxImageEffectPropColourManagementCore',
                             'kOfxImageEffectPropColourManagementFull',
                             'kOfxImageEffectPropColourManagementOCIO',
                             )
            if splits[1] in nonProperties:
                continue
            # these are props, as well as anything with Prop in the name
            badlyNamedProperties = ("kOfxImageEffectFrameVarying",
                                    "kOfxImageEffectPluginRenderThreadSafety")
            if 'Prop' in splits[1] \
               or any(s in splits[1] for s in badlyNamedProperties):
                props.add(splits[1])
    return props

def getPropertiesFromDir(dir):
    """
    Recursively get all property definitions from source files in a dir.
    """

    extensions = {'.c', '.h', '.cxx', '.hxx', '.cpp', '.hpp'}

    props = set()
    for root, _dirs, files in os.walk(dir):
        for file in files:
            # Get the file extension
            file_extension = os.path.splitext(file)[1]

            if file_extension in extensions:
                file_path = os.path.join(root, file)
                props |= getPropertiesFromFile(file_path)
    return list(props)

def get_def(name: str, defs):
    if name.endswith('_REF'):
        defname = name.replace("_REF", "_DEF")
        return defs[defname]
    else:
        return [name]

def expand_set_props(props_by_set):
    """Expand refs in props_by_sets.
    YAML can't interpolate a list, so we do it here, using
    our own method:
    - A prop set may end with _DEF in which case it's just a definition
      containing a list of prop names.
    - A prop *name* may be <name>_REF which means to interpolate the
      corresponding DEF list into this set's props.
    Returns a new props_by_set with DEFs removed and all lists interpolated.
    """
    # First get all the list defs
    defs = {}
    sets = {}
    for key, value in props_by_set.items():
        if key.endswith('_DEF'):
            defs[key] = value   # should be a list to be interpolated
        else:
            sets[key] = value
    for key in sets:
        sets[key] = [item for element in sets[key] \
                     for item in get_def(element, defs)]
    return sets


def find_missing(all_props, props_metadata):
    """Find and print all mismatches between prop defs and metadata.

    Returns 0 if no errors.
    """
    errs = 0
    for p in sorted(all_props):
        if not props_metadata.get(p):
            logging.error(f"No YAML metadata found for {p}")
            errs += 1
    for p in sorted(props_metadata):
        if p not in all_props:
            logging.error(f"No prop definition found for '{p}' in source/include")
            matches = difflib.get_close_matches(p, all_props, 3, 0.9)
            if matches:
                logging.info(f" Did you mean: {matches}")
            errs += 1
    return errs

def check_props_by_set(props_by_set, props_metadata):
    """Find and print all mismatches between prop set specs, props, and metadata.

    * Each prop name in props_by_set should have a match in props_metadata
    Returns 0 if no errors.
    """
    errs = 0
    for pset in sorted(props_by_set):
        for prop in sorted(props_by_set[pset]):
            if not props_metadata.get(prop):
                logging.error(f"No props metadata found for {pset}.{prop}")
                errs += 1
    return errs

def check_props_used_by_set(props_by_set, props_metadata):
    """Find and print all mismatches between prop set specs, props, and metadata.

    * Each prop name in props_metadata should be used in at least one set.
    Returns 0 if no errors.
    """
    errs = 0
    for prop in props_metadata:
        found = 0
        for pset in props_by_set:
            for set_prop in props_by_set[pset]:
                if set_prop == prop:
                    found += 1
        if not found:
            logging.error(f"Prop {prop} not used in any prop set in YML file")
    return errs

def gen_props_metadata(props_metadata, outfile_path: Path):
    """Generate a header file with metadata for each prop"""
    with open(outfile_path, 'w') as outfile:
        outfile.write("""
#include <string>
#include <vector>
#include "ofxImageEffect.h"
#include "ofxGPURender.h"
#include "ofxColour.h"
#include "ofxDrawSuite.h"
#include "ofxParametricParam.h"
#include "ofxKeySyms.h"
#include "ofxOld.h"

namespace OpenFX {
enum class PropType {
   Int,
   Double,
   Enum,
   Bool,
   String,
   Bytes,
   Pointer
};

enum class Writable {
   Host,
   Plugin,
   All
};

struct PropsMetadata {
  std::string name;
  std::vector<PropType> types;
  int dimension;
  Writable writable;
  bool host_optional;
  std::vector<const char *> values; // for enums
};

""")
        outfile.write("const std::vector<struct PropsMetadata> props_metadata {\n")
        for p in sorted(props_metadata):
            try:
                md = props_metadata[p]
                types = md.get('type')
                if isinstance(types, str): # make it always a list
                    types = (types,)
                prop_type_defs = "{" + ",".join(f'PropType::{t.capitalize()}' for t in types) + "}"
                writable = "Writable::" + md.get('writable', "unknown").capitalize()
                host_opt = md.get('hostOptional', 'false')
                if host_opt in ('True', 'true', 1):
                    host_opt = 'true'
                if host_opt in ('False', 'false', 0):
                    host_opt = 'false'
                if md['type'] == 'enum':
                    assert isinstance(md['values'], list)
                    values = "{" + ",".join(f'{v}' for v in md['values']) + "}"
                else:
                    values = "{}"
                outfile.write(f"{{ {p}, {prop_type_defs}, {md['dimension']}, "
                              f"{writable}, {host_opt}, {values} }},\n")
            except Exception as e:
                logging.error(f"Error: {p} is missing metadata? {e}")
                raise(e)
        outfile.write("};\n} // namespace OpenFX\n")

def gen_props_by_set(props_by_set, outfile_path: Path):
    """Generate a header file with definitions of all prop sets, including their props"""
    with open(outfile_path, 'w') as outfile:
        outfile.write("""
#include <string>
#include <vector>
#include <map>
#include "ofxImageEffect.h"
#include "ofxGPURender.h"
#include "ofxColour.h"
#include "ofxDrawSuite.h"
#include "ofxParametricParam.h"
#include "ofxKeySyms.h"
#include "ofxOld.h"

namespace OpenFX {
""")
        outfile.write("const std::map<std::string, std::vector<const char *>> prop_sets {\n")
        for pset in sorted(props_by_set.keys()):
            propnames = ",\n   ".join(sorted(props_by_set[pset]))
            outfile.write(f"{{ \"{pset}\", {{ {propnames} }} }},\n")
        outfile.write("};\n} // namespace OpenFX\n")

def main(args):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    include_dir = Path(script_dir).parent / 'include'
    all_props = getPropertiesFromDir(include_dir)
    logging.info(f'Got {len(all_props)} props from "include" dir')

    with open(include_dir / 'ofx-props.yml', 'r') as props_file:
        props_data = yaml.safe_load(props_file)
    props_by_set = props_data['propertySets']
    props_by_set = expand_set_props(props_by_set)
    props_metadata = props_data['properties']

    if args.verbose:
        print("\n=== Checking ofx-props.yml: should map 1:1 to props found in source/header files")
    errs = find_missing(all_props, props_metadata)
    if not errs and args.verbose:
        print(" ✔️ ALL OK")

    if args.verbose:
        print("\n=== Checking ofx-props.yml: every prop in a set should have metadata in the YML file")
    errs = check_props_by_set(props_by_set, props_metadata)
    if not errs and args.verbose:
        print(" ✔️ ALL OK")

    if args.verbose:
        print("\n=== Checking ofx-props.yml: every prop should be used in in at least one set in the YML file")
    errs = check_props_used_by_set(props_by_set, props_metadata)
    if not errs and args.verbose:
        print(" ✔️ ALL OK")

    if args.verbose:
        print("=== Generating gen_props_metadata.hxx")
    gen_props_metadata(props_metadata, include_dir / 'gen_props_metadata.hxx')

    if args.verbose:
        print("=== Generating props by set header gen_props_by_set.hxx")
    gen_props_by_set(props_by_set, include_dir / 'gen_props_by_set.hxx')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Check OpenFX properties and generate ancillary data structures")

    # Define arguments here
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose mode')

    # Parse the arguments
    args = parser.parse_args()

    # Call the main function with parsed arguments
    main(args)
