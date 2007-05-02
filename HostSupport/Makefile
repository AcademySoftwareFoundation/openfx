all : lib/libofxHost.a
clean :
	rm -f lib/libofxHost.a
	rm -f */*.o

OS = $(shell uname)

ifeq ($(OS),Darwin) 
CXX_OSFLAGS = -arch ppc -arch i386 
RANLIB = ranlib
endif

ifeq ($(OS),Linux) 
CXX_OSFLAGS = 
RANLIB = echo
endif

INCLUDES += -I../include -Iinclude $(EXPAT_INCLUDE)

CXXFLAGS = $(CXX_OSFLAGS) $(INCLUDES) -g -Wall

objects = src/ofxhBinary.o \
	src/ofxhClip.o \
	src/ofxhHost.o \
	src/ofxhImageEffect.o \
	src/ofxhImageEffectAPI.o \
	src/ofxhInteract.o \
	src/ofxhMemory.o \
	src/ofxhParam.o \
	src/ofxhPluginAPICache.o \
	src/ofxhPluginCache.o \
	src/ofxhPropertySuite.o 

lib/libofxHost.a:$(objects)
	mkdir -p lib
	rm -f lib/libofxHost.a
	ar -rc lib/libofxHost.a $(objects)
	$(RANLIB) lib/libofxHost.a
