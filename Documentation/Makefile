
STYLESHEETDIR=/usr/share/sgml/docbook/docbook-xsl-1.68.1/

ofxProgrammingGuide.html : ofxProgrammingGuide.xml
	xsltproc -o ofxProgrammingGuide.html $(STYLESHEETDIR)/xhtml/docbook.xsl ofxProgrammingGuide.xml

clean :
	rm -f ofxProgrammingGuide.html
