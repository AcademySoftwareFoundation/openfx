# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause

"""Shared utilities for extracting inline metadata from OFX C headers.

This module provides functions to parse inline YAML blocks embedded in
doxygen comments in the OFX C headers:

- @propdef: property metadata (type, dimension, etc.) before #define lines
- @propset / @propsetdef: property set definitions (standalone comment blocks)
- @actiondef: action argument definitions (in comments before #define or typedef)

Format examples:
    /** @brief Unique name of an object.
        @propdef
        type: string
        dimension: 1
    */
    #define kOfxPropName "OfxPropName"

    /** @propset EffectDescriptor
        write: plugin
        props:
          - OfxPropType
          - OfxPluginPropFilePath | write=host
    */

    /** @propsetdef ParamsCommon
        - OfxPropType
        - OfxPropName
    */

    /** @brief Description of the action...
        @actiondef
        inArgs:
          - OfxPropType
        outArgs:
    */
    #define kOfxActionInstanceChanged "OfxActionInstanceChanged"
"""

import re
import yaml
from pathlib import Path


def extract_inline_metadata(header_path: str | Path) -> dict:
    """Extract @propdef metadata from a C header file.

    Finds all /** ... */ comment blocks containing @propdef,
    extracts the YAML metadata, associates it with the next
    #define kOfx* line, and also extracts the doc text.

    Args:
        header_path: Path to the C header file.

    Returns:
        Dict mapping property string names (the quoted value in #define)
        to {"metadata": {...}, "doc": "...", "cname": "kOfx..."}.
        For properties whose YAML key starts with 'k' (like kOfxPropKeySym),
        the string name IS the cname.
    """
    header_path = Path(header_path)
    text = header_path.read_text()

    result = {}

    # Find all /** ... */ comment blocks
    # We need to find comments that contain @propdef, then find the next #define
    comment_pattern = re.compile(r'/\*\*(.*?)\*/', re.DOTALL)

    for match in comment_pattern.finditer(text):
        comment_body = match.group(1)

        # Check if this comment has @propdef
        if '@propdef' not in comment_body:
            continue

        # Split at @propdef
        parts = comment_body.split('@propdef', 1)
        doc_part = parts[0]
        yaml_part = parts[1]

        # Extract doc text: everything between @brief and @propdef
        doc_text = _clean_doc_text(doc_part)

        # Extract YAML: lines after @propdef until end of comment
        yaml_text = _clean_yaml_text(yaml_part)

        # Parse YAML
        try:
            metadata = yaml.safe_load(yaml_text)
        except yaml.YAMLError as e:
            raise ValueError(
                f"Failed to parse @propdef YAML in {header_path} "
                f"near offset {match.start()}: {e}\n"
                f"YAML text was:\n{yaml_text}"
            )

        if metadata is None:
            metadata = {}

        # Find the next #define after this comment
        after_comment = text[match.end():]
        define_match = re.search(
            r'#define\s+(k\w+)\s+"([^"]+)"', after_comment
        )
        if not define_match:
            continue

        # Only consider defines that are close (within a few lines)
        # to avoid matching unrelated defines
        between = after_comment[:define_match.start()]
        if between.count('\n') > 3:
            continue

        cname = define_match.group(1)
        string_name = define_match.group(2)

        result[string_name] = {
            'metadata': metadata,
            'doc': doc_text,
            'cname': cname,
        }

    return result


def extract_all_inline_metadata(include_dir: str | Path) -> dict:
    """Extract @propdef metadata from all OFX header files.

    Args:
        include_dir: Path to the include directory containing ofx*.h files.

    Returns:
        Dict mapping property string names to metadata dicts,
        aggregated from all headers.
    """
    include_dir = Path(include_dir)
    all_props = {}

    for header in sorted(include_dir.glob('ofx*.h')):
        props = extract_inline_metadata(header)
        for name, data in props.items():
            if name in all_props:
                raise ValueError(
                    f"Duplicate property {name} found in {header} "
                    f"and previous definition"
                )
            all_props[name] = data

    return all_props


def get_properties_from_headers(include_dir: str | Path) -> dict:
    """Extract property metadata in the same format as the YAML 'properties' section.

    This is the main entry point for gen-props.py and gen-props-doc.py.
    Returns a dict in the same format as props_data['properties'] from
    the YAML file, so it's a drop-in replacement.

    Args:
        include_dir: Path to the include directory.

    Returns:
        Dict mapping property names (YAML-style keys) to metadata dicts.
        Keys are typically like "OfxPropName" (without k prefix),
        except for the special cases where the YAML key starts with 'k'.
    """
    all_inline = extract_all_inline_metadata(include_dir)
    props = {}

    for string_name, data in all_inline.items():
        metadata = dict(data['metadata'])
        cname = data['cname']

        # Determine the YAML-style key for this property.
        # Normally, the YAML key is the string value (e.g., "OfxPropName")
        # and the cname is "k" + key.
        # Special cases where cname != "k" + string_name need a cname field.
        # Also, properties whose string value starts with "k" (like "kOfxPropKeySym")
        # have the YAML key = string_name and cname = string_name.

        expected_cname = "k" + string_name
        if cname != expected_cname:
            metadata['cname'] = cname

        # The YAML key is the string_name (the quoted value in the #define)
        yaml_key = string_name

        props[yaml_key] = metadata

    return props


def _clean_doc_text(doc_part: str) -> str:
    """Clean the doc text portion of a doxygen comment (before @propdef).

    Strips leading * characters and excessive whitespace.
    """
    lines = doc_part.split('\n')
    cleaned = []
    for line in lines:
        # Remove leading whitespace and * characters
        stripped = line.strip()
        if stripped.startswith('*'):
            stripped = stripped[1:].strip()
        # Also handle lines starting with just spaces then *
        cleaned.append(stripped)

    # Join and clean up
    text = '\n'.join(cleaned).strip()
    return text


def _clean_yaml_text(yaml_part: str) -> str:
    """Clean the YAML portion of a @propdef block.

    Strips leading * characters and comment formatting from each line,
    producing clean YAML.
    """
    lines = yaml_part.split('\n')
    cleaned = []
    for line in lines:
        # Remove leading whitespace, then optional * or leading spaces+*
        stripped = line.strip()
        if stripped.startswith('*'):
            stripped = stripped[1:]
            # Remove one leading space after * if present
            if stripped.startswith(' '):
                stripped = stripped[1:]
        # Skip empty lines at start
        if not cleaned and not stripped.strip():
            continue
        cleaned.append(stripped)

    # Remove trailing empty lines
    while cleaned and not cleaned[-1].strip():
        cleaned.pop()

    return '\n'.join(cleaned)


# ---------------------------------------------------------------------------
# Property set extraction (@propset / @propsetdef)
# ---------------------------------------------------------------------------

def extract_propsets(header_path: str | Path) -> dict:
    """Extract @propset and @propsetdef blocks from a C header file.

    @propset blocks define property sets with a write mode and list of props:
        /** @propset EffectDescriptor
            write: plugin
            props:
              - OfxPropType
        */

    @propsetdef blocks define reusable prop lists (expanded via _REF):
        /** @propsetdef ParamsCommon
            - OfxPropType
            - OfxPropName
        */

    Returns:
        Dict matching the YAML 'propertySets' format:
        - Regular sets: {"SetName": {"write": "host", "props": [...]}}
        - DEF lists: {"Name_DEF": ["prop1", "prop2", ...]}
    """
    header_path = Path(header_path)
    text = header_path.read_text()
    result = {}

    comment_pattern = re.compile(r'/\*\*(.*?)\*/', re.DOTALL)

    for match in comment_pattern.finditer(text):
        comment_body = match.group(1)

        # Check for @propsetdef first (more specific match)
        propsetdef_match = re.search(r'@propsetdef\s+(\w+)', comment_body)
        if propsetdef_match:
            base_name = propsetdef_match.group(1)
            yaml_text = comment_body.split('@propsetdef ' + base_name, 1)[1]
            yaml_text = _clean_yaml_text(yaml_text)
            try:
                data = yaml.safe_load(yaml_text)
            except yaml.YAMLError as e:
                raise ValueError(
                    f"Failed to parse @propsetdef YAML in {header_path} "
                    f"near offset {match.start()}: {e}\n"
                    f"YAML text was:\n{yaml_text}"
                )
            if data is None:
                data = []
            result[base_name + '_DEF'] = data
            continue

        # Check for @propset
        propset_match = re.search(r'@propset\s+(\w+)', comment_body)
        if propset_match:
            name = propset_match.group(1)
            yaml_text = comment_body.split('@propset ' + name, 1)[1]
            yaml_text = _clean_yaml_text(yaml_text)
            try:
                data = yaml.safe_load(yaml_text)
            except yaml.YAMLError as e:
                raise ValueError(
                    f"Failed to parse @propset YAML in {header_path} "
                    f"near offset {match.start()}: {e}\n"
                    f"YAML text was:\n{yaml_text}"
                )
            if data is None:
                data = {}
            result[name] = data

    return result


def get_propsets_from_headers(include_dir: str | Path) -> dict:
    """Extract all property set definitions from OFX header files.

    Scans all ofx*.h files and aggregates @propset/@propsetdef blocks.
    Returns a dict in the same format as props_data['propertySets'] from
    the YAML file.
    """
    include_dir = Path(include_dir)
    all_sets = {}

    for header in sorted(include_dir.glob('ofx*.h')):
        sets = extract_propsets(header)
        for name, data in sets.items():
            if name in all_sets:
                raise ValueError(
                    f"Duplicate property set {name} found in {header} "
                    f"and previous definition"
                )
            all_sets[name] = data

    return all_sets


# ---------------------------------------------------------------------------
# Action extraction (@actiondef)
# ---------------------------------------------------------------------------

def extract_actions(header_path: str | Path) -> dict:
    """Extract @actiondef blocks from a C header file.

    @actiondef blocks are embedded in doxygen comments before action
    #define lines (or the CustomParamInterpFunc typedef):

        /** @brief Description...
            @actiondef
            inArgs:
              - OfxPropType
            outArgs:
        */
        #define kOfxActionInstanceChanged "OfxActionInstanceChanged"

    Returns:
        Dict mapping action string names to {"inArgs": [...], "outArgs": [...]},
        matching the YAML 'Actions' format.
    """
    header_path = Path(header_path)
    text = header_path.read_text()
    result = {}

    comment_pattern = re.compile(r'/\*\*(.*?)\*/', re.DOTALL)

    for match in comment_pattern.finditer(text):
        comment_body = match.group(1)

        if '@actiondef' not in comment_body:
            continue

        # Split at @actiondef and parse the YAML portion
        yaml_text = comment_body.split('@actiondef', 1)[1]
        yaml_text = _clean_yaml_text(yaml_text)

        try:
            data = yaml.safe_load(yaml_text)
        except yaml.YAMLError as e:
            raise ValueError(
                f"Failed to parse @actiondef YAML in {header_path} "
                f"near offset {match.start()}: {e}\n"
                f"YAML text was:\n{yaml_text}"
            )

        if data is None:
            data = {}

        # Find the next #define or typedef after this comment
        after_comment = text[match.end():]
        action_name = None

        # Try #define first
        define_match = re.search(
            r'#define\s+\w+\s+"([^"]+)"', after_comment
        )
        if define_match:
            between = after_comment[:define_match.start()]
            if between.count('\n') <= 3:
                action_name = define_match.group(1)

        # Try typedef (for CustomParamInterpFunc)
        if action_name is None:
            typedef_match = re.search(
                r'typedef\s+\w+\s+\((\w+)\)', after_comment
            )
            if typedef_match:
                between = after_comment[:typedef_match.start()]
                if between.count('\n') <= 3:
                    # OfxCustomParamInterpFuncV1 -> CustomParamInterpFunc
                    typedef_name = typedef_match.group(1)
                    if typedef_name == 'OfxCustomParamInterpFuncV1':
                        action_name = 'CustomParamInterpFunc'
                    else:
                        action_name = typedef_name

        if action_name:
            result[action_name] = data

    return result


def get_actions_from_headers(include_dir: str | Path) -> dict:
    """Extract all action definitions from OFX header files.

    Scans all ofx*.h files and aggregates @actiondef blocks.
    Returns a dict in the same format as props_data['Actions'] from
    the YAML file.
    """
    include_dir = Path(include_dir)
    all_actions = {}

    for header in sorted(include_dir.glob('ofx*.h')):
        actions = extract_actions(header)
        for name, data in actions.items():
            if name in all_actions:
                raise ValueError(
                    f"Duplicate action {name} found in {header} "
                    f"and previous definition"
                )
            all_actions[name] = data

    return all_actions
