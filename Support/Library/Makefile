CXXFLAGS = -g -I../../include -I../include 

default : libOfxSupport.a

libOfxSupport.a : libOfxSupport.a(ofxsMultiThread.o) \
	libOfxSupport.a(ofxsInteract.o) \
	libOfxSupport.a(ofxsProperty.o) \
	libOfxSupport.a(ofxsLog.o) \
	libOfxSupport.a(ofxsCore.o) \
	libOfxSupport.a(ofxsPropertyValidation.o) \
	libOfxSupport.a(ofxsImageEffect.o) \
	libOfxSupport.a(ofxsParams.o)
	ranlib libOfxSupport.a
