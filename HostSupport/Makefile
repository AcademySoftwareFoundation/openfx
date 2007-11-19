# Variables to set to help you
#   EXPAT_INCLUDE - the directory to find the expat XML parser header files in
#   DST_DIR       - the directory to put the built library in
#   DEBUG         - whether to build the libraries with debug information, or optimise.
#   OPTIMISE      - optimisation flags override.

OS = $(shell uname)
DEBUG ?= false
EXPAT_INCLUDE ?= expat-2.0.1/lib
OBJSUF ?= .o
LIBSUF ?= .a
LIBPREFIX ?= lib
LIBNAME ?= ofxHost

LIBTARGET = $(LIBPREFIX)$(LIBNAME)$(LIBSUF)
EXPATLIB = $(LIBPREFIX)expat$(LIBSUF)

ifeq ($(DEBUG), true)
  DST_DIR = $(OS)-debug
  OPTIMISE ?= -g -Wall
  EXPATFLAG = --enable-debug
else ifeq ($(DEBUG), instrument)
  DST_DIR = $(OS)-instrument
  OPTIMISE ?= -g -O3 -Wall
else
  DST_DIR = $(OS)-release
  OPTIMISE ?= -O2 -Wall
  EXPATFLAGS = --disable-debug
endif

INT_DIR ?= $(DST_DIR)

CXX_OSFLAGS = 
RANLIB = echo

ifeq ($(OS),Darwin) 
  CXX_OSFLAGS = -arch ppc -arch i386 
  RANLIB = ranlib
endif

HEADERS = include/ofxhBinary.h                  \
   include/ofxhClip.h                           \
   include/ofxhHost.h                           \
   include/ofxhImageEffect.h                    \
   include/ofxhImageEffectAPI.h                 \
   include/ofxhInteract.h                       \
   include/ofxhMemory.h                         \
   include/ofxhParam.h                          \
   include/ofxhPluginAPICache.h                 \
   include/ofxhPluginCache.h                    \
   include/ofxhProgress.h                       \
   include/ofxhPropertySuite.h                  \
   include/ofxhTimeLine.h                       \
   include/ofxhUtilities.h                      \
   include/ofxhXml.h                            \
   ../include/ofxCore.h                         \
  ../include/ofxImageEffect.h                   \
  ../include/ofxInteract.h                      \
  ../include/ofxKeySyms.h                       \
  ../include/ofxMemory.h                        \
  ../include/ofxMessage.h                       \
  ../include/ofxMultiThread.h                   \
  ../include/ofxParam.h                         \
  ../include/ofxProgress.h                      \
  ../include/ofxProperty.h                      \
  ../include/ofxTimeLine.h


INCLUDES += -I../include -Iinclude -I$(EXPAT_INCLUDE) 

CXXFLAGS = $(CXX_OSFLAGS) $(INCLUDES) $(OPTIMISE)

objects = $(INT_DIR)/ofxhParam$(OBJSUF) \
	$(INT_DIR)/ofxhImageEffectAPI$(OBJSUF) \
	$(INT_DIR)/ofxhUtilities$(OBJSUF) \
	$(INT_DIR)/ofxhHost$(OBJSUF) \
	$(INT_DIR)/ofxhInteract$(OBJSUF) \
	$(INT_DIR)/ofxhBinary$(OBJSUF) \
	$(INT_DIR)/ofxhClip$(OBJSUF) \
	$(INT_DIR)/ofxhImageEffect$(OBJSUF) \
	$(INT_DIR)/ofxhMemory$(OBJSUF) \
	$(INT_DIR)/ofxhPluginAPICache$(OBJSUF) \
	$(INT_DIR)/ofxhPluginCache$(OBJSUF) \
	$(INT_DIR)/ofxhPropertySuite$(OBJSUF)

$(DST_DIR)/$(LIBTARGET): $(objects) $(DST_DIR)/$(EXPATLIB)
	rm -f $(DST_DIR)/$(LIBTARGET)
	ar -rc $(DST_DIR)/$(LIBTARGET) $(objects)
	$(RANLIB) $(DST_DIR)/$(LIBTARGET)

$(objects) : $(DST_DIR)/%.o : src/%.cpp
	mkdir -p $(INT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(objects) : $(HEADERS)

$(DST_DIR)/$(EXPATLIB):
	mkdir -p $(DST_DIR)
	cd expat-2.0.1;	pwd; ./configure --disable-shared $(EXPATFLAGS); make
	cp expat-2.0.1/.libs/libexpat.a $(DST_DIR)

all :	@echo "$(DST_DIR)/$(EXPATLIB)" $(DST_DIR)/$(EXPATLIB) $(DST_DIR)/$(LIBTARGET)

clean :
	rm -f $(DST_DIR)/$(EXPATLIB)
	rm -f $(DST_DIR)/$(LIBTARGET)
	rm -f $(INT_DIR)/*$(OBJSUF)


