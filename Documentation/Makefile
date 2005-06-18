STYLESHEETDIR=http://docbook.sourceforge.net/release/xsl/1.68.1/

XSLARGS = --stringparam  funcsynopsis.style "ansi" \
	--stringparam  toc.section.depth "3" \
	--stringparam html.stylesheet ofxStyle.css


SOURCES = ofxProgrammingGuide.xml \
	ofxPropertiesReference.xml \
	ofxPropertiesByObject.xml

allInOne : ofxProgrammingGuide.html

chunks : index.html

ofxProgrammingGuide.html : $(SOURCES)
	xsltproc -o ofxProgrammingGuide.html $(XSLARGS) \
	$(STYLESHEETDIR)/xhtml/docbook.xsl  ofxProgrammingGuide.xml

index.html : $(SOURCES)
	xsltproc -o ofxProgrammingGuide.html $(XSLARGS)  \
	$(STYLESHEETDIR)/xhtml/chunk.xsl  ofxProgrammingGuide.xml

PROPERTY_HEADER_FILES = ../include/ofxParam.h \
	../include/ofxImageEffect.h \
	../include/ofxInteract.h \
	../include/ofxCore.h

ofxPropertiesReference.xml : extractPropertyDefinitions.perl $(PROPERTY_HEADER_FILES)
	mkdir -p props
	./extractPropertyDefinitions.perl
	cat props/* > ofxPropertiesReference.xml
	rm -rf props

clean :
	rm -rf *.html props
