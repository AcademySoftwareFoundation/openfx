# where to get our style sheet from
STYLESHEETARCHIVE = http://downloads.sourceforge.net/project/docbook/docbook-xsl/1.77.1/docbook-xsl-1.77.1.zip

# local stylesheet dir as it takes ages over a network connection
STYLESHEETDIR = docbook-xsl-1.77.1/xhtml/

# stylesheet to use
STYLESHEET = docbook.xsl 

ASCIIDOCTOR = /usr/bin/asciidoctor

#  args for xsltproc
XSLARGS = --stringparam  funcsynopsis.style "ansi" \
	--stringparam  toc.section.depth "3" \
  --stringparam generate.toc "book toc,title,figure,table,example,equation" \
	--stringparam html.stylesheet ofxStyle.css

# docbook output from asciidoc format
%.xml : %.adoc
	$(ASCIIDOCTOR) -b docbook $< -o $@

# html output from asciidoc format
%.html : %.adoc
	$(ASCIIDOCTOR) -a theme=volnitsky -b html5 $< -o $@

all : ofxProgrammingGuide.html ofxMachineryExample.html

# programming guide, all in one
GUIDE_SRCS = ofxProgrammingGuide.adoc

ofxProgrammingGuide.html : $(GUIDE_SRCS)

# fetch the style sheet for docbook xslt
.PHONY: docbook-xsl
docbook-xsl :
	wget --output-document=docbook-xsl-download.zip '$(STYLESHEETARCHIVE)'; \
	unzip -q -o docbook-xsl-download.zip; \
	rm -f docbook-xsl-download.zip

clean :
	rm -rf *.html Guide docbook-xsl
