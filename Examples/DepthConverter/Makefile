CXXFLAGS = -I../../include
OPTIMIZER = -O3

depthConverter.dso : depthConverter.o
	$(CXX) -shared depthConverter.o -o depthConverter.dso
	strip -fhls depthConverter.dso

