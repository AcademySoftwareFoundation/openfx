CXXFLAGS = -I../../include
OPTIMIZER = -g

testProperties.bundle : testProperties.o ofxLog.o
	$(CXX) -shared testProperties.o ofxLog.o -o testProperties.dso