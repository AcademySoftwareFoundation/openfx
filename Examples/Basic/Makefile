CXXFLAGS = -I../../include
OPTIMIZER = -O3

basic.dso : basic.o
	$(CXX) -shared basic.o -o basic.dso
	strip -fhls basic.dso

