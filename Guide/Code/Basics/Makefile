INCDIR = -I../../../include
OPTIMISER = -g

ifeq ($(OSTYPE), darwin)
  OS_LDFLAGS = -bundle
endif


CXXFLAGS = $(INCDIR) $(OPTIMISER)

basics.ofx : basics.o
	$(CXX) $(OS_LDFLAGS) $< -o $@
	mkdir -p $@.bundle/Contents/MacOS/
	cp $@ $@.bundle/Contents/MacOS/

clean :
	rm -rf basics.ofx.bundle
	rm -rf basics.ofx
	rm -rf basics.o
