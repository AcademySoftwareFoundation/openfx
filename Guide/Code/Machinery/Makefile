INCDIR = -I../../../include
OPTIMISER = -g

ifeq ($(OSTYPE), darwin)
  OS_LDFLAGS = -bundle
endif


CXXFLAGS = $(INCDIR) $(OPTIMISER)

machinery.ofx : machinery.o
	$(CXX) $(OS_LDFLAGS) $< -o $@
	mkdir -p $@.bundle/Contents/MacOS/
	cp $@ $@.bundle/Contents/MacOS/

clean :
	rm -rf machinery.ofx.bundle
	rm -rf machinery.ofx
	rm -rf machinery.o
