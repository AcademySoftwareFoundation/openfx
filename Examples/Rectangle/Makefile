CXXFLAGS = -I../../include
OPTIMIZER = -O3

rectangle.dso : rectangle.o
	$(CXX) -shared rectangle.o -o rectangle.dso
	strip -fhls rectangle.dso

