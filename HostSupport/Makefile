# Variables to set to help you
#   EXPAT_INCLUDE - the directory to find the expat XML parser header files in
#   DST_DIR       - the directory to put the built library in

DST_DIR ?= lib

OS = $(shell uname)

#CXX = icl

DEBUG ?= false

ifneq ($(OS),CYGWIN_NT-5.1)
  ifeq ($(DEBUG),true)
    OPTIMISE ?= -g -Wall
  else
    OPTIMISE ?= -O2 -Wall
  endif
else
  ifeq ($(DEBUG),true)
    OPTIMISE ?= /MDd /Od /Ob1 /DWIN32 /DXML_STATIC /GR /GX
    INTDIR ?= private/DEBUG/w2k_intel/
    OUTLIB ?= ofxhostd.lib
  else
    OPTIMISE ?= /MD /O2 /Ob1 /DWIN32 /DXML_STATIC /GR /GX
    INTDIR ?= private/OPTIMISE/w2k_intel/
    OUTLIB ?= ofxhost.lib
  endif
endif

ifeq ($(OS),Darwin) 
CXX_OSFLAGS = -arch ppc -arch i386 
RANLIB = ranlib
endif

ifeq ($(OS),Linux) 
CXX_OSFLAGS = 
RANLIB = echo
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


INCLUDES += -I../include -Iinclude $(EXPAT_INCLUDE) 

CXXFLAGS = $(CXX_OSFLAGS) $(INCLUDES) $(OPTIMISE)

ifeq ($(OS),CYGWIN_NT-5.1)
objects = 	 \
	src/ofxhUtilities.obj \
	src/ofxhHost.obj \
	src/ofxhInteract.obj \
	src/ofxhBinary.obj \
	src/ofxhClip.obj \
	src/ofxhImageEffect.obj \
	src/ofxhImageEffectAPI.obj \
	src/ofxhMemory.obj \
	src/ofxhParam.obj \
	src/ofxhPluginAPICache.obj \
	src/ofxhPluginCache.obj \
	src/ofxhPropertySuite.obj \
	../../Expat/lib/xmltok.obj \
	../../Expat/lib/xmlparse.obj \
	../../Expat/lib/xmlrole.obj
else
objects = 	 \
	src/ofxhParam.o \
	src/ofxhImageEffectAPI.o \
	src/ofxhUtilities.o \
	src/ofxhHost.o \
	src/ofxhInteract.o \
	src/ofxhBinary.o \
	src/ofxhClip.o \
	src/ofxhImageEffect.o \
	src/ofxhMemory.o \
	src/ofxhPluginAPICache.o \
	src/ofxhPluginCache.o \
	src/ofxhPropertySuite.o
endif

ifeq ($(OS),CYGWIN_NT-5.1)
.SUFFIXES : .obj

.cpp.obj:
	@mkdir -p $(INTDIR)
	$(CXX) $(CXXFLAGS) /c /Tp$(<) /Fo'$(INTDIR)$(notdir $(@))'

.c.obj:
	@mkdir -p $(INTDIR)
	$(CXX) $(CXXFLAGS) /D_LIB /DCOMPILED_FROM_DSP /c $(<) /Fo$(INTDIR)$(notdir $(@))

$(DST_DIR)/$(OUTLIB):$(objects)
	@mkdir -p $(DST_DIR)
	@rm -f $(DST_DIR)/$(OUTLIB)
	@xilib $(patsubst ../../Expat/lib/%,$(INTDIR)%,$(patsubst src/%,$(INTDIR)%,$(objects))) /OUT:$(DST_DIR)/$(OUTLIB)
else
$(DST_DIR)/libofxHost.a:$(objects)
	mkdir -p $(DST_DIR)
	rm -f $(DST_DIR)/libofxHost.a
	ar -rc $(DST_DIR)/libofxHost.a $(objects)
	$(RANLIB) $(DST_DIR)/libofxHost.a
endif

$(objects) : $(HEADERS)

ifeq ($(OS),CYGWIN_NT-5.1)
all : 
	$(DST_DIR)/$(OUTLIB)

clean : 
	rm -rf $(INTDIR)
	rm -rf $(DST_DIR)/$(OUTLIB)

else
all : $(DST_DIR)/libofxHost.a

clean :
	rm -f $(DST_DIR)/libofxHost.a
	rm -f */*.o
endif

