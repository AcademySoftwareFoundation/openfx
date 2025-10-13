#!/usr/bin/env python3
# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause

# /// script
# requires-python = ">=3.11"
# dependencies = [
#     "pyyaml>=1.0",
# ]
# ///

import os
import sys
import argparse
import yaml
import logging
from pathlib import Path
from collections import defaultdict

# Set up basic configuration for logging
logging.basicConfig(
    level=logging.INFO,  # Log level: DEBUG, INFO, WARNING, ERROR, CRITICAL
    format='%(levelname)s: %(message)s',  # Format of the log messages
    datefmt='%Y-%m-%d %H:%M:%S'  # Date format
)

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
    - A prop *name* may be <n>_REF which means to interpolate the
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
        if not sets[key].get('props'):
            pass # do nothing, no expansion needed in inArgs/outArgs for now
        else:
            sets[key]['props'] = [item for element in sets[key]['props'] \
                         for item in get_def(element, defs)]
    return sets

def props_for_set(pset, props_by_set, name_only=True):
    """Generator yielding all props for the given prop set (not used for actions).
    This implements the options override scheme, parsing the prop name etc.
    If not name_only, yields a dict of name and other options.
    """
    import re
    
    if not props_by_set[pset].get('props'):
        return
        
    # All the default options for this propset. Merged into each prop.
    propset_options = props_by_set[pset].copy()
    propset_options.pop('props', None)
    
    for p in props_by_set[pset]['props']:
        # Parse p, of form NAME | key=value,key=value
        if '|' in p:
            parts = p.split('|', 1)
            name = parts[0].strip()
            key_values_str = parts[1].strip()
            
            if name_only:
                yield name
            else:
                # Parse key/value pairs, apply defaults, and include name
                options = {}
                if key_values_str:
                    key_values = key_values_str.split(',')
                    for kv in key_values:
                        if '=' in kv:
                            k, v = kv.split('=', 1)
                            options[k.strip()] = v.strip()
                            
                yield {**propset_options, **options, **{"name": name}}
        else:
            # Simple property name with no options
            if name_only:
                yield p
            else:
                yield {**propset_options, **{"name": p}}

def get_cname(propname, props_metadata):
    """Get the C `#define` name for a property name.

    Look up the special cname in props_metadata, or in the normal
    case just prepend "k".
    """
    return props_metadata[propname].get('cname', "k" + propname)

def generate_property_documentation(props_metadata, props_by_set, outfile_path):
    """Generate RST documentation for all properties
    
    This creates a comprehensive property reference that includes:
    - Property name
    - C #define name
    - Type and dimension
    - Property sets where it's used
    - Description (if available)
    - Valid values (for enums)
    - Link to Doxygen documentation
    """
    with open(outfile_path, 'w') as outfile:
        outfile.write(".. _propertiesReferenceGenerated:\n")
        outfile.write("Properties Reference (Generated)\n")
        outfile.write("==============================\n\n")
        outfile.write("This reference is auto-generated from property definitions in the OpenFX source code.\n")
        outfile.write("It provides a structured view of properties with their types, dimensions, and where they are used.\n")
        outfile.write("For each property, a link to the detailed Doxygen documentation is provided when available.\n\n")

        # Create a mapping from property to the sets that use it
        prop_to_sets = defaultdict(list)
        for pset in props_by_set:
            for prop in props_for_set(pset, props_by_set):
                prop_to_sets[prop].append(pset)

        # Process properties by type
        types = {
            'int': 'Integer',
            'double': 'Double',
            'string': 'String',
            'bool': 'Boolean',
            'enum': 'Enumeration',
            'pointer': 'Pointer'
        }

        # Group properties by type
        for type_key, type_name in sorted(types.items()):
            type_props = [p for p in props_metadata if props_metadata[p].get('type') == type_key or 
                           (isinstance(props_metadata[p].get('type'), list) and type_key in props_metadata[p].get('type'))]
            
            if not type_props:
                continue
                
            outfile.write(f"\n{type_name} Properties\n{'-' * len(type_name) + '-----------'}\n\n")
            
            for prop in sorted(type_props):
                # Get metadata
                metadata = props_metadata[prop]
                cname = get_cname(prop, props_metadata)
                
                # Write property name and C define name
                outfile.write(f".. _prop_{prop}:\n\n")
                outfile.write(f"**{prop}**\n")
                outfile.write(f"{'^' * len(prop)}\n\n")
                
                # Link to the corresponding Doxygen documentation
                outfile.write(f"- **C #define**: :c:macro:`{cname}`\n")
                
                # Write type and dimension
                if isinstance(metadata.get('type'), list):
                    types_str = ', '.join(metadata.get('type'))
                    outfile.write(f"- **Type**: Multiple types: {types_str}\n")
                else:
                    outfile.write(f"- **Type**: {metadata.get('type', 'unknown')}\n")
                
                dim = metadata.get('dimension', 'unknown')
                if dim == 0:
                    outfile.write(f"- **Dimension**: Variable (0 or more)\n")
                else:
                    outfile.write(f"- **Dimension**: {dim}\n")
                
                # Write property sets where this is used
                if prop in prop_to_sets:
                    sets_str = ', '.join([f':ref:`{s} <propset_{s}>`' for s in sorted(prop_to_sets[prop])])
                    outfile.write(f"- **Used in Property Sets**: {sets_str}\n")
                
                # Write valid values for enums
                if metadata.get('type') == 'enum' and metadata.get('values'):
                    outfile.write("- **Valid Values**:\n")
                    for value in metadata.get('values'):
                        outfile.write(f"  - ``{value}``\n")
                
                # Write additional metadata
                if metadata.get('default'):
                    outfile.write(f"- **Default**: {metadata.get('default')}\n")
                
                if metadata.get('introduced'):
                    outfile.write(f"- **Introduced in**: version {metadata.get('introduced')}\n")
                
                if metadata.get('deprecated'):
                    outfile.write(f"- **Deprecated in**: version {metadata.get('deprecated')}\n")
                
                # Add direct link to Doxygen documentation for detailed info
                outfile.write(f"- **Doc**: For detailed doc, see :c:macro:`{cname}`.\n")
                
                outfile.write("\n")

def generate_action_args_documentation(action_data, props_metadata, props_by_set_doc):
    """Generate documentation for action arguments (inArgs/outArgs)."""
    action_section = []
    
    action_section.append("\nActions Property Sets\n-------------------\n\n")
    action_section.append("Actions in OFX have input and output property sets that are used to pass data between the host and plugin.\n")
    action_section.append("For each action, the required input properties (passed from host to plugin) and output properties ")
    action_section.append("(set by the plugin for the host to read) are documented.\n\n")
    
    # Add a quick reference table of contents for all actions
    action_section.append("**Actions Quick Reference**\n\n")
    for action_name in sorted(action_data.keys()):
        action_section.append(f"* :ref:`{action_name} <action_{action_name}>`\n")
    action_section.append("\n")
    
    for action_name in sorted(action_data.keys()):
        # Create a section for this action
        action_section.append(f".. _action_{action_name}:\n\n")
        action_section.append(f"**{action_name}**\n")
        action_section.append(f"{'^' * len(action_name)}\n\n")
        has_args = False
        
        # Document inArgs (input properties)
        if action_data[action_name].get('inArgs'):
            has_args = True
            action_section.append(f"**Input Arguments**\n\n")
            
            for prop in action_data[action_name]['inArgs']:
                metadata = props_metadata.get(prop)
                if not metadata:
                    action_section.append(f"- ``{prop}`` - (No metadata available)\n")
                    continue

                # Determine type display
                if isinstance(metadata.get('type'), list):
                    type_str = '/'.join(metadata.get('type'))
                else:
                    type_str = metadata.get('type', 'unknown')

                # Get dimension
                dim = metadata.get('dimension', 'unknown')
                if dim == 0:
                    dim_str = "Variable"
                else:
                    dim_str = str(dim)

                # Get C name
                cname = get_cname(prop, props_metadata)

                # Write property entry with link to full definition and Doxygen reference
                action_section.append(f"- :ref:`{prop} <prop_{prop}>` - Type: {type_str}, Dimension: {dim_str} (:c:macro:`{cname}`)\n")
                
                action_section.append("\n")
        
        # Document outArgs (output properties)
        if action_data[action_name].get('outArgs'):
            has_args = True
            action_section.append(f"**Output Arguments**\n\n")
            
            for prop in action_data[action_name]['outArgs']:
                metadata = props_metadata.get(prop)
                if not metadata:
                    action_section.append(f"- ``{prop}`` - (No metadata available)\n")
                    continue

                # Determine type display
                if isinstance(metadata.get('type'), list):
                    type_str = '/'.join(metadata.get('type'))
                else:
                    type_str = metadata.get('type', 'unknown')

                # Get dimension
                dim = metadata.get('dimension', 'unknown')
                if dim == 0:
                    dim_str = "Variable"
                else:
                    dim_str = str(dim)

                # Get C name
                cname = get_cname(prop, props_metadata)

                # Write property entry with link to full definition and Doxygen reference
                action_section.append(f"- :ref:`{prop} <prop_{prop}>` - Type: {type_str}, Dimension: {dim_str} (doc: :c:macro:`{cname}`)\n")
                
                action_section.append("\n")
        if not has_args:
            action_section.append(f"-- no in/out args --\n\n")
    return ''.join(action_section)

def generate_property_set_documentation(props_by_set, props_metadata, actions_data, outfile_path):
    """Generate RST documentation for property sets and action property sets
    
    This creates documentation on property sets and the properties they contain,
    as well as action property sets (inArgs/outArgs)
    """
    with open(outfile_path, 'w') as outfile:
        outfile.write(".. _propertySetReferenceGenerated:\n")
        outfile.write("Property Sets Reference (Generated)\n")
        outfile.write("==================================\n\n")
        outfile.write("This reference is auto-generated from property set definitions in the OpenFX source code.\n")
        outfile.write("It provides an overview of property sets and their associated properties.\n")
        outfile.write("For each property, a link to its detailed description in the :doc:`Properties Reference (Generated) <ofxPropertiesReferenceGenerated>` is provided.\n\n")

        # First document the regular property sets
        outfile.write("Regular Property Sets\n--------------------\n\n")
        outfile.write("These property sets represent collections of properties associated with various OpenFX objects.\n\n")
        
        # Build a table of contents for the property sets
        outfile.write("**Property Sets Quick Reference**\n\n")
        for pset in sorted(props_by_set.keys()):
            if not pset.endswith('_DEF'):
                outfile.write(f"* :ref:`{pset} <propset_{pset}>`\n")
        outfile.write("\n")
        
        for pset in sorted(props_by_set.keys()):
            # Skip DEF sets as they're just for internal organization
            if pset.endswith('_DEF'):
                continue
                
            outfile.write(f".. _propset_{pset}:\n\n")
            outfile.write(f"**{pset}**\n")
            outfile.write(f"{'^' * len(pset)}\n\n")
            
            # Write access information
            write_access = props_by_set[pset].get('write', 'unknown')
            outfile.write(f"- **Write Access**: {write_access}\n\n")

            # List all properties in this set
            outfile.write("**Properties**\n\n")
            
            props_list = list(props_for_set(pset, props_by_set))
            if not props_list:
                outfile.write("No properties defined for this set.\n\n")
                continue
                
            for prop in sorted(props_list):
                metadata = props_metadata.get(prop)
                if not metadata:
                    continue
                    
                # Determine type display
                if isinstance(metadata.get('type'), list):
                    type_str = '/'.join(metadata.get('type'))
                else:
                    type_str = metadata.get('type', 'unknown')
                    
                # Get dimension
                dim = metadata.get('dimension', 'unknown')
                if dim == 0:
                    dim_str = "Variable"
                else:
                    dim_str = str(dim)
                
                # Get C name
                cname = get_cname(prop, props_metadata)
                    
                # Write property entry with link to full definition and Doxygen doc reference
                outfile.write(f"- :ref:`{prop} <prop_{prop}>` - Type: {type_str}, Dimension: {dim_str} (doc: :c:macro:`{cname}`)\n")
                
            outfile.write("\n")
            
        # Now add the Action property sets section
        action_docs = generate_action_args_documentation(actions_data, props_metadata, props_by_set)
        outfile.write(action_docs)

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    include_dir = Path(script_dir).parent / 'include'
    doc_ref_dir = Path(script_dir).parent / 'Documentation/sources/Reference'
    
    # Default output paths
    props_doc_path = doc_ref_dir / 'ofxPropertiesReferenceGenerated.rst'
    propsets_doc_path = doc_ref_dir / 'ofxPropertySetsGenerated.rst'
    
    parser = argparse.ArgumentParser(description="Generate RST documentation from OpenFX properties YAML")
    parser.add_argument('--props-file', default=include_dir/'ofx-props.yml',
                        help="Path to the ofx-props.yml file")
    parser.add_argument('--props-doc', default=props_doc_path,
                        help="Output path for properties documentation")
    parser.add_argument('--propsets-doc', default=propsets_doc_path,
                        help="Output path for property sets documentation")
    parser.add_argument('-v', '--verbose', action='store_true',
                        help="Enable verbose output")
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.info(f"Reading properties from {args.props_file}")
        
    # Load property definitions
    with open(args.props_file, 'r') as props_file:
        props_data = yaml.safe_load(props_file)
    
    props_by_set = expand_set_props(props_data['propertySets'])
    props_metadata = props_data['properties']
    actions_data = props_data.get('Actions', {})
    
    if args.verbose:
        logging.info(f"Generating property documentation in {args.props_doc}")
    generate_property_documentation(props_metadata, props_by_set, args.props_doc)
    
    if args.verbose:
        logging.info(f"Generating property sets documentation in {args.propsets_doc}")
    generate_property_set_documentation(props_by_set, props_metadata, actions_data, args.propsets_doc)
    
    if args.verbose:
        logging.info("Documentation generation complete")

if __name__ == "__main__":
    main()
