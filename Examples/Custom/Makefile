CXXFLAGS = -I../../include 
OPTIMIZER = -O3

custom.dso : custom.o
	$(CXX) -shared custom.o -o custom.dso
	strip -fhls custom.dso

