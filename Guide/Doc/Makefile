# figure out the OS and where to find asciidoctor
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
ASCIIDOCTOR = /usr/bin/asciidoctor
else ifeq ($(UNAME), Linux)
ASCIIDOCTOR = /usr/local/bin/asciidoctor
endif

# where to put the html files
OUTPUT_DIR = html

# html output from asciidoc format
$(OUTPUT_DIR)/%.html : %.adoc
	$(ASCIIDOCTOR) -a theme=volnitsky -b html5 $< -o $@

# all the sources to our programming guide
GUIDE_SRCS := $(wildcard *.adoc)

# the html to our programming guide
GUIDE_HTML = $(addprefix $(OUTPUT_DIR)/, $(GUIDE_SRCS:.adoc=.html) )

#default target
.PHONY: all
all : $(GUIDE_HTML)

# actual target, our HTMLs
$(GUIDE_HTML) : | $(OUTPUT_DIR)

# make our output dir
$(OUTPUT_DIR) :
	mkdir $(OUTPUT_DIR)

# clean it up
.PHONY: clean
clean :
	rm -rf html 
