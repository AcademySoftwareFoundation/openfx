# -*- coding: utf-8 -*-
# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

# SPDX-License-Identifier: BSD-3-Clause

import subprocess, os, shutil
project = 'OpenFX'
copyright = '''2025, OpenFX a Series of LF Projects, LLC.
For web site terms of use, trademark policy and other project policies please see https://lfprojects.org/'''
author = 'Contributors to the OpenFX Project'
release = '1.5.1'

read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

if read_the_docs_build:
    # cwd here is Documentation/sources, i.e. the dir of this source file
    subprocess.call('python ../genPropertiesReference.py -r -i ../../include -o Reference/ofxPropertiesReference.rst', shell=True)
    subprocess.call('cd ../../include ; doxygen ofx.doxy', shell=True)
    print(f'Generating API doc')
    subprocess.call('python -m breathe.apidoc -p ofx_reference -o Reference/api ../doxygen_build/xml', shell=True)
    ps = subprocess.Popen("git rev-parse HEAD | git ls-remote --heads origin | grep $(git rev-parse HEAD) | cut -d / -f 3",shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    branch_name=ps.communicate()[0]
    branch_name=branch_name.rstrip().decode('utf-8')
    print(f'Current branch is: {branch_name}')
    # Modify index.rst
    with open('index.rst.tmp','w') as fo:
        with open('index.rst') as f:
            lines = f.readlines()
            for l in lines:
                if 'readthedocs.' in l and 'openfx' in l:
                    l = l.replace('master',branch_name)
                fo.write(l)

    # Copy back
    os.remove('index.rst')
    shutil.copyfile('index.rst.tmp','index.rst')
    os.remove('index.rst.tmp')

# -- General configuration ------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#
# needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [ "breathe", "sphinx_rtd_theme" ]

# breathe is an rst/sphinx extension to read and render doxygen xml output
breathe_projects = {
    "ofx_reference":"../doxygen_build/xml/",
}

breathe_default_project = "ofx_reference"

#breathe_projects_source = {
#    "ofx_reference" :
#        ( "../../include", [ "ofxCore.h","ofxDialog.h","ofxImageEffect.h","ofxInteract.h","ofxKeySyms.h", ] )
#        }

breathe_default_members = ('members', 'protected-members', 'private-members','undoc-members')

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']
exclude_patterns = []

# paths rel to html_static_path
html_css_files = [
    'css/custom.css'
]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_static_path = ['_static']
html_logo = "_static/openfx-horizontal-color.svg"
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'logo_only': True
}
