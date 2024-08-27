# SPDX-License-Identifier: BSD-3-Clause

import os,sys,getopt

badlyNamedProperties = ["kOfxImageEffectFrameVarying", "kOfxImageEffectPluginRenderThreadSafety"]

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

def main(argv):

    sourcePath=''
    outputFile=''
    try:
        opts, args = getopt.getopt(argv,"i:o:r",["sourcePath=","outputFile"])
        for opt,value in opts:
            if opt == "-i":
                sourcePath = value
            elif opt == "-o":
                outputFile = value

    except getopt.GetoptError:
        sys.exit(1)

    props = getPropertiesFromDir(sourcePath)

    with open(outputFile, 'w') as f:
        f.write('.. _propertiesReference:\n')
        f.write('Properties Reference\n')
        f.write('=====================\n')
        for p in sorted(props):
            f.write('.. doxygendefine:: ' + p + '\n\n')

if __name__ == "__main__":
    main(sys.argv[1:])
