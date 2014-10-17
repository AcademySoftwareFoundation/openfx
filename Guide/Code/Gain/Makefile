INCDIR = -I../../../include
OPTIMISER = -g

ifeq ($(OSTYPE), darwin)
  OS_LDFLAGS = -bundle
endif

NAME = gain

CXXFLAGS = $(INCDIR) $(OPTIMISER)

.PHONY: clean install

$(NAME).ofx : $(NAME).o
	$(CXX) $(OS_LDFLAGS) $< -o $@
	mkdir -p $@.bundle/Contents/MacOS/
	cp $@ $@.bundle/Contents/MacOS/

install :
	mkdir -p ../built_plugins
	cp -r $(NAME).ofx.bundle ../built_plugins

clean :
	rm -rf $(NAME).ofx.bundle
	rm -rf $(NAME).ofx
	rm -rf $(NAME).o
