INCDIR = -I../../../include
OPTIMISER = -g

ifeq ($(OSTYPE), darwin)
  OS_LDFLAGS = -bundle
endif

NAME = invert

CXXFLAGS = $(INCDIR) $(OPTIMISER)

$(NAME).ofx : $(NAME).o
	$(CXX) $(OS_LDFLAGS) $< -o $@
	mkdir -p $@.bundle/Contents/MacOS/
	cp $@ $@.bundle/Contents/MacOS/

clean :
	rm -rf $(NAME).ofx.bundle
	rm -rf $(NAME).ofx
	rm -rf $(NAME).o
