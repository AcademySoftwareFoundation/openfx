# override these to change where it should look for the expat headers and libs
# These defaults are foundry internal bits.
EXPAT_INCLUDE ?= -I../../../common/include
EXPAT_LIB ?=  ../../../osx_universal/lib/libexpat.a

CXXFLAGS= -g -I../include -I../../include $(EXPAT_INCLUDE) 


HOST_DEMO_FILES = hostDemo.o \
	hostDemoClipInstance.o     \
	hostDemoEffectInstance.o   \
	hostDemoHostDescriptor.o   \
	hostDemoParamInstance.o    


all : cacheDemo hostDemo

clean :
	rm -f *.o cacheDemo hostDemo

cacheDemo : cacheDemo.cpp
	$(CXX) $(CXXFLAGS) cacheDemo.cpp -o cacheDemo -L../lib -lofxHost $(EXPAT_LIB) -ldl

hostDemo : $(HOST_DEMO_FILES)
	$(CXX) $(CXXFLAGS) $(HOST_DEMO_FILES) -o hostDemo -L../lib -lofxHost $(EXPAT_LIB) -ldl