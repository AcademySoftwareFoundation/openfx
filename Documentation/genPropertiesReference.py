# SPDX-License-Identifier: BSD-3-Clause

import os,sys,getopt

badlyNamedProperties = ["kOfxImageEffectFrameVarying", "kOfxImageEffectPluginRenderThreadSafety"]

def getPropertiesFromDir(sourcePath, recursive, props):

    if os.path.isdir(sourcePath):
        files=sorted(os.listdir(sourcePath))
        for f in files:
            absF = sourcePath + '/' + f
            if not recursive and os.path.isdir(absF):
                continue
            else:
                getPropertiesFromDir(absF,recursive,props)
    elif os.path.isfile(sourcePath):
        ext = os.path.splitext(sourcePath)[1]
        if ext.lower() in ('.c', '.cxx', '.cpp', '.h', '.hxx', '.hpp'):
            with open(sourcePath) as f:
                try:
                    lines = f.readlines()
                except UnicodeDecodeError as e:
                    print('WARNING: error in', sourcePath, ':')
                    raise e
                for l in lines:
                    # Detect lines that correspond to a property definition, e.g:
                    # #define kOfxPropLala "OfxPropLala"
                    splits=l.split(' ')
                    if len(splits) != 3:
                        continue
                    if splits[0] != '#define':
                        continue
                    if 'Prop' in splits[1] and splits[1] != 'kOfxPropertySuite':
                        #if l.startswith('#define kOfx') and 'Prop' in l:
                        props.append(splits[1])
    else:
        raise ValueError('No such file or directory: %s' % sourcePath)

def main(argv):

    recursive=False
    sourcePath=''
    outputFile=''
    try:
        opts, args = getopt.getopt(argv,"i:o:r",["sourcePath=","outputFile","recursive"])
        for opt,value in opts:
            if opt == "-i":
                sourcePath = value
            elif opt == "-o":
                outputFile = value
            elif opt == "-r":
                    recursive=True

    except getopt.GetoptError:
        sys.exit(1)

    props=badlyNamedProperties
    getPropertiesFromDir(sourcePath, recursive, props)

    re='/^#define k[\w_]*Ofx[\w_]*Prop/'
    with open(outputFile, 'w') as f:
        f.write('.. _propertiesReference:\n')
        f.write('Properties Reference\n')
        f.write('=====================\n')
        props.sort()
        for p in props:
            f.write('.. doxygendefine:: ' + p + '\n\n')

if __name__ == "__main__":
    main(sys.argv[1:])
