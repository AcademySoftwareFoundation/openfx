STYLESHEETDIR=/usr/share/sgml/docbook/docbook-xsl-1.68.1/

XSLARGS = --stringparam  funcsynopsis.style "ansi" \
	--stringparam  toc.section.depth "3" \
	--stringparam html.stylesheet ofxStyle.css


allInOne : ofxProgrammingGuide.html

chunks : index.html

ofxProgrammingGuide.html : ofxProgrammingGuide.xml ofxPropertiesReference.xml  
	xsltproc -o ofxProgrammingGuide.html $(XSLARGS) \
	$(STYLESHEETDIR)/xhtml/docbook.xsl  ofxProgrammingGuide.xml

index.html : ofxProgrammingGuide.xml ofxPropertiesReference.xml  
	xsltproc -o ofxProgrammingGuide.html $(XSLARGS)  \
	$(STYLESHEETDIR)/xhtml/chunk.xsl  ofxProgrammingGuide.xml

ofxPropertiesReference.xml : extractPropertyDefs ../include/ofxParam.h
	mkdir -p props
	./extractPropertyDefs
	cat props/* > ofxPropertiesReference.xml
	rm -rf props

clean :
	rm -f ofxProgrammingGuide.html
