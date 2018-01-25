This directory tree contains a set of plugins and corresponding
guides which take you through the basics of the OFX
Image Effects Plugin API.

There are two sub-directories...
   - Code - which contains the example plugins source files,
   - Doc  - has a guide to each plugin.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BUILDING THE PLUGINS

For Windows instructions, see below.


To build the example plugins you will need,
   - a C++ compiler
   - gmake (or nmake on Windows)
   - the ofx header files.


Within Code there is a subdirectory per plugin.


The assumption is that you have checked out all the OFX
source code in one lump and so the
OFX header files will be in a standard relative path to
the plugin sources. If this is not the case you will
need to modify the file...

   Code/MakefileCommon

and change the line

   OFX_INC_DIR = -I../../../include

to point to the directory where you have put the headers.


To build all the examples simply go...

    cd Code
    make

this will compile all the plugins and place them in
a directory called 'built_plugins'.


You can build individual plugins by changing into the
relevant subdirectory and simply issuing a 'make' command.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BUILDING ON WINDOWS

NMakefiles are included for use with Windows' nmake utility.
This should build on any Visual Studio version (at least 2008 or newer).

Open a Visual Studio command-line window of the appropriate bitness
that you want (32 for a 32-bit OFX host, 64 for a 64-bit host).  From
the Start menu, go to Microsoft Visual Studio XXXX -> Visual Studio
Tools -> Visual Studio XXX Command Prompt (choose the appropriate
bitness here).

In that window, cd to the openfx/Guide/Code dir, and type:

  nmake /F nmakefile install

This will build and install the plugins into the standard OFX plugins
dir (c:\Program Files\Common Files\OFX\Plugins).

To clean up:

  nmake /F nmakefile clean

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BUILDING THE DOCUMENTATION

To build the documentation you will need...
   - gmake
   - asciidoctor


The documentation is written in asciidoctor markdown, which can be used to
generate HTML, docbook XML and more. You will need to download and install
asciidoctor to build the doc. Visit...

    http://asciidoctor.org/

for installation instructions.


There is a gnu Makefile currently configured to generate html files. To build
the documentation simply go...

   cd Doc
   make

this will generate a subdirectory called 'html' which will contain the
guides in html format.



Last Edit 11/11/14
