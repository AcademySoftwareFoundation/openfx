CXXFLAGS = -I../../include -g
OPTIMIZER = -g

basic.dso : basic.o
	$(CXX) -shared basic.o -o basic.dso
	strip -fhls basic.dso

