# local stylesheet dir as it takes ages over a network connection
STYLESHEETDIR = http://docbook.sourceforge.net/release/xsl/1.69.1/xhtml/

STYLESHEET = docbook.xsl 

#  args for our style sheet 
XSLARGS = --stringparam  funcsynopsis.style "ansi" \
	--stringparam  toc.section.depth "3" \
	--stringparam html.stylesheet ofxStyle.css

# .h files we are scanning for specially formated property definitions
# We do this so that the documentation in the source agrees with the 
# documentation in the XML, always.
PROPERTY_HEADER_FILES = ../include/ofxParam.h \
	../include/ofxImageEffect.h \
	../include/ofxInteract.h \
	../include/ofxCore.h

banana : ofxProgrammingReference.html

all : ofxProgrammingGuide.html ofxProgrammingReference.html

# programming guide, all in one
GUIDE_SRCS = header.xml ofxProgrammingGuide.xml
ofxProgrammingGuide.html : $(GUIDE_SRCS)
	cat $^ > tmp.xml
	xsltproc -o $@ $(XSLARGS) $(STYLESHEETDIR)docbook.xsl tmp.xml
	rm -f tmp.xml

# programming reference, all in one
REF_SRCS = header.xml ofxProgrammingReference.xml
ofxProgrammingReference.html : $(REF_SRCS)
	cat $^ > tmp.xml
	xsltproc -o $@ $(XSLARGS) $(STYLESHEETDIR)docbook.xsl tmp.xml
	rm -f tmp.xml

# the ref depends on these other two xml files
ofxProgrammingReference.xml  : ofxPropertiesReference.xml \
	ofxPropertiesByObject.xml \
	ofxPropertySuiteReference.xml \
	ofxMemorySuiteReference.xml \
	ofxParametersSuiteReference.xml

# generate property definitions from the property headers
ofxPropertiesReference.xml : extractPropertyDefinitions.perl $(PROPERTY_HEADER_FILES)
	mkdir -p props
	./extractPropertyDefinitions.perl
	cat props/* > ofxPropertiesReference.xml
	rm -rf props

clean :
	rm -rf *.html props

# Extract references on the property suite from the .h file
ofxPropertySuiteReference.xml : ../include/ofxProperty.h
	./getSuiteReference.perl OfxPropertySuite $^ > $@

# Extract references on the memory suite from the .h file
ofxMemorySuiteReference.xml : ../include/ofxMemory.h
	./getSuiteReference.perl OfxMemorySuite $^ > $@

# Extract references on the parameters suite from the .h file
ofxParametersSuiteReference.xml : ../include/ofxParam.h
	./getSuiteReference.perl OfxParameterSuite $^ > $@
