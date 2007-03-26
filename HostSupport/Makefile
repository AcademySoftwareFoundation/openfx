all:lib/libofxHost.a

CXXFLAGS=-I../include -Iinclude -g -Wall

objects=src/ofxhHost.o src/ofxhPluginAPICache.o src/ofxhPluginCache.o src/ofxhPropertySuite.o

lib/libofxHost.a:$(objects)
	ar -rc lib/libofxHost.a $(objects)
